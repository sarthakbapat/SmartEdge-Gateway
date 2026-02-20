#include "SmartEdgeGateway.hpp"

SmartEdgeGateway::SmartEdgeGateway(std::string serverURI, std::string topic) :
    tempSensor(std::make_unique<TempSensor>(1, "TempSensor")),
    dbManager(std::make_unique<DataBaseManager>("smartedge_sqlite3.db")),
    ringBuffer(std::make_unique<RingBuffer<SensorData>>(100)),
    mqttHandler(std::make_unique<MQTTPublisher>(serverURI, "test_client", topic, 0)),
    is_running(false)
    {}


SmartEdgeGateway::~SmartEdgeGateway()
{
    stop();
}


std::string SmartEdgeGateway::serialize_sensorData(const SensorData& data) {
    nlohmann::json j;
    j["id"] = data.sensorId;
    j["unit"] = data.dataUnit;

    // Extract value from variant safely for the SensorData.data 
    std::visit([&j](auto&& val) { j["data"] = val; }, data.data);

    // std::chrono::system_clock::time_point type is high precision time object. Cannot convert to std::string directly.
    // Instead convert it to Unix timestamp (epoch in sec) and send to MQTT or store to DB.
    auto duration = data.timestamp.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    j["timeStamp"] = seconds;

    std::string payload = j.dump();
    return payload;
}   

void SmartEdgeGateway::create_data_and_write_to_ringbuffer() {
    SensorData tempData;
    while(is_running) {
        tempData = tempSensor->read();
        if (ringBuffer->writeToBuffer(tempData)) {
            SPDLOG_DEBUG("SmartEdgeGateway: Data written to ringbuffer.");
        }
        else {
            spdlog::warn("SmartEdgeGateway: Failed to write data to ringbuffer.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::memset(&tempData, 0, sizeof(SensorData));
    }
}

std::optional<std::string> SmartEdgeGateway::read_and_serialize_data() {
    SensorData data;
    std::string payload;
    if (ringBuffer->readFromBuffer(data)) {
        SPDLOG_DEBUG("SmartEdgeGateway: Data read from ringbuffer.");
        payload = serialize_sensorData(data);
        return payload;
    }
    else {
        spdlog::warn("SmartEdgeGateway: Failed to read data from ringbuffer.");
        return std::nullopt;
    }
    
}

void SmartEdgeGateway::sync_offline_data_from_db() {
    spdlog::info("Starting background sync of offline data...");
    
    auto messages = dbManager->fetchAllFromDb();
    
    // Access the vector elements and publish the data to MQTT.
    for (const auto& msg : messages) {
        // Double check connection before each publish.
        if (mqttHandler->get_is_connected()) {
                mqttHandler->publish_data(msg.json);
                // Now remove the published data so we don't send it again in later syncs. 
                dbManager->deleteById(msg.id);
            }
         else {
            spdlog::info("Connection lost during sync. Stopping.");
            break; 
        }
    }
    spdlog::info("SmareEdgeGateway: Sync complete. Database is clear.");
}


void SmartEdgeGateway::run() {
    is_running = true;
    std::optional<std::string> payload_data;
    bool was_connected = false;

    // Connect to the MQTT server.
    if (mqttHandler->client_connect()) {
        spdlog::info("SmartEdgeGateway: Connected to the MQTT Server.");
    }
    else {
        spdlog::error("SmartEdgeGateway: Failed to connect to the MQTT server. Exiting ...");
        return;
    }

    // Calling member func of this class in a thread using &ClassName::member_func, this
    data_creation_thread = std::thread(&SmartEdgeGateway::create_data_and_write_to_ringbuffer, this);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    while(is_running) {
        // Read tempSensor data from ringbuffer and convert to json string. -> this thread
        // Publish the readings to MQTT server. -> this Thread
        // Handle the storage to db in case of network failure. -> this Thread

        // Check if mqtt connection is active and was_connected flag is true.
        bool currently_connected = mqttHandler->get_is_connected();
        // This gets executed on reconnect of mqtt connection.
        if (currently_connected && was_connected) {
            // Reconnected after mqtt connection break.
            // Start a thread to upload the data from db to mqtt. 
            std::thread sync_thread(&SmartEdgeGateway::sync_offline_data_from_db, this);
            // Detach this thread so that it runs in background.
            sync_thread.detach();
        }
        was_connected = currently_connected;

        // Check the MQTT connection status and decide whether to publish to cloud or store to database.
        // Read the ringBuffer data.
        payload_data = read_and_serialize_data();

        if (mqttHandler->get_is_connected()) {
            if (payload_data) {
                SPDLOG_DEBUG("SmartEdgeGateway: Json payload: {}", *payload_data);
                mqttHandler->publish_data(*payload_data);
            }
            else {
                spdlog::warn("SmartEdgeGateway: Failed to get JSON payload.");
            }
        }
        else {
            spdlog::info("SmartEdgeGateway: MQTT connection lost, storing data to database.");
            // Store to database function.
            if (payload_data) {
                dbManager->storeToDb(*payload_data);
            }
            else {
                spdlog::warn("SmartEdgeGateway: Failed to get JSON payload while storing to database.");
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    // Join data_creation thread before exiting this function. Safe thread exit.
    if (data_creation_thread.joinable()) {
        data_creation_thread.join();
    }
}

void SmartEdgeGateway::stop() {
    if (is_running) {
        is_running = false;
        spdlog::info("SmartEdgeGateway: Stopped.");
    }

}