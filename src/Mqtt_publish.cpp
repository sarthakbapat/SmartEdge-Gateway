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

bool MQTTPublisher::client_connect() {
    bool success = false;
    std::cout << "Initializing client for the server '" << serverURI << "'..." << std::endl;

    try {
        auto connOpts = mqtt::connect_options_builder()
                            .mqtt_version(MQTTVERSION_3_1_1)
                            .connect_timeout(std::chrono::seconds(5))
                            .automatic_reconnect(true)
                            .clean_session(false)           // Keeps session when connection lost.
                            .finalize();

        std::cout << "\nConnecting..." << std::endl;
        mqtt::token_ptr conntok = client->connect(connOpts);
        std::cout << "Waiting for the connection..." << std::endl;
        conntok->wait();
        std::cout << "  ...OK" << std::endl;
        success = true;
    }
    catch(const mqtt::exception& exc) {
        std::cout << "Client failed to connect: " << exc.what() << std::endl;
    }
    return success;
}

void MQTTPublisher::publish_data(const std::string& jsonPayload) {
    try {
        std::cout << "\nSending message..." << std::endl;
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, jsonPayload);
        pubmsg->set_qos(QoS);
        client->publish(pubmsg)->wait_for(5);
        std::cout << " ... OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cout << "MQTT error while sending message: " << exc.what() << std::endl;
    }
}

bool MQTTPublisher::cient_disconnect() {
    bool success = false;
    try {
        std::cout << "\nDisconnecting..." << std::endl;
        client->disconnect()->wait();
        std::cout << "  ...OK" << std::endl;
        success = true;
    }
    catch (const mqtt::exception& exc) {
        std::cout << "Client failed to disconnect: " << exc.what() << std::endl;
    }
    return success;
}

bool MQTTPublisher::get_is_connected() const {
    return is_connected;
}

// connection_lost callback function, just setting connection_lost to true.
void MQTTPublisher::connection_lost(const std::string& cause) {
    is_connected = false;
    std::cout << "Connection Lost: " << cause << std::endl;
}

// connected callback function, just setting is_connected to true.
void MQTTPublisher::connected(const std::string& cause) {
    is_connected = true;
    std::cout << "MQTT connected: " << cause << std::endl;
}