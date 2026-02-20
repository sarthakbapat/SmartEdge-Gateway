#ifndef MQTT_PUBLISH_H
#define MQTT_PUBLISH_H

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"
#include "mqtt/async_client.h"
#include "Sensor.hpp"

class MQTTPublisher: public virtual mqtt::callback {
    private:
        std::string serverURI;
        std::string clientID;
        std::string topic;
        int QoS;
        std::unique_ptr<mqtt::async_client> client;
        std::atomic<bool> is_connected;
        std::atomic<int> connection_count;

    public:
        MQTTPublisher(std::string serverURI, std::string clientID, std::string topic, int QoS);

        ~MQTTPublisher();

        bool client_connect();

        void publish_data(const std::string& jsonPayload);

        bool cient_disconnect();

        // Callback methods override

        void connection_lost(const std::string& cause) override;

        void connected(const std::string& cause) override;

        bool get_is_connected() const;

        int get_connection_count() const;

        void decrement_connection_count();

};

#endif
