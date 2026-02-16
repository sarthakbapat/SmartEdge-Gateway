#ifndef SMARTEDGE_GATEWAY_H
#define SMARTEDGE_GATEWAY_H

#include <iostream>
#include <chrono>
#include <thread>
#include <optional>
#include "RingBuffer.hpp"
#include "Sensor.hpp"
#include "Mqtt_publish.hpp"
#include "DatabaseManager.hpp"

class SmartEdgeGateway {
    private:
        std::unique_ptr<SensorInterface> tempSensor;
        std::unique_ptr<DataBaseManager> dbManager;
        std::unique_ptr<RingBuffer<SensorData>> ringBuffer;
        std::unique_ptr<MQTTPublisher> mqttHandler;
        std::atomic<bool> is_running;
        std::thread data_creation_thread;

        std::string serialize_sensorData(const SensorData& data);
        void create_data_and_write_to_ringbuffer();
        std::optional<std::string> read_and_serialize_data();

    public:
        SmartEdgeGateway(std::string serverURI, std::string topic);

        ~SmartEdgeGateway();

        void run();

        void stop();

};


#endif

