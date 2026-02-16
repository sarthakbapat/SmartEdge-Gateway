#include "Sensor.hpp"

TempSensor::TempSensor(uint32_t id, std::string name): id(id), sensorName(std::move(name)) {}

SensorData TempSensor::read() {
    float tempData = 25.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/5.0f));
    return SensorData{
        id,
        std::chrono::system_clock::now(),
        tempData,
        "Celsius"
    };
}

std::string TempSensor::getSensorName() const {
    return sensorName;
}

uint32_t TempSensor::getSensorId() const {
    return id;
}
