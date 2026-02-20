#include "Mqtt_publish.hpp"

MQTTPublisher::MQTTPublisher(std::string serverURI, std::string clientID, std::string topic, int QoS)
    : serverURI(serverURI), clientID(clientID), topic(topic), QoS(QoS)
{
    client = std::make_unique<mqtt::async_client>(serverURI, clientID);

    // Set the callback here using *this (callback object).
    client->set_callback(*this);

    // Set the connection related flags to false, callbacks should set them to true.
    is_connected = false;
}

MQTTPublisher::~MQTTPublisher() {
    cient_disconnect();
}

bool MQTTPublisher::client_connect() {
    bool success = false;
    spdlog::info("MQTT: Initializing client for the server {} ...", serverURI);

    try {
        auto connOpts = mqtt::connect_options_builder()
                            .mqtt_version(MQTTVERSION_3_1_1)
                            .connect_timeout(std::chrono::seconds(5))
                            .automatic_reconnect(true)
                            .clean_session(false)           // Keeps session when connection lost.
                            .finalize();

        spdlog::info("MQTT: Connecting ...");
        mqtt::token_ptr conntok = client->connect(connOpts);
        conntok->wait();
        success = true;
        spdlog::info("MQTT: Connected OK");
    }
    catch(const mqtt::exception& exc) {
        spdlog::error("MQTT: Client failed to connect: {}", exc.what());
    }
    return success;
}

void MQTTPublisher::publish_data(const std::string& jsonPayload) {
    try {
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, jsonPayload);
        pubmsg->set_qos(QoS);
        client->publish(pubmsg)->wait_for(5);
        spdlog::info("MQTT: Message published successfully.");
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("MQTT: Error while sending message: {}", exc.what());
    }
}

bool MQTTPublisher::cient_disconnect() {
    bool success = false;
    try {
        client->disconnect()->wait();
        success = true;
        spdlog::info("MQTT: Client disconnected.");
    }
    catch (const mqtt::exception& exc) {
        spdlog::error("MQTT: Client failed to disconnect: {}", exc.what());
    }
    return success;
}

bool MQTTPublisher::get_is_connected() const {
    return is_connected;
}

// connection_lost callback function, just setting connection_lost to true.
void MQTTPublisher::connection_lost(const std::string& cause) {
    is_connected = false;
    spdlog::info("MQTT: Connection lost: {}", cause);
}

// connected callback function, just setting is_connected to true.
void MQTTPublisher::connected(const std::string& cause) {
    is_connected = true;
    spdlog::info("MQTT: Connected.");
}