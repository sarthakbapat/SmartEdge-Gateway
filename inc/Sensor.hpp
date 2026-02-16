#ifndef SENSOR_HPP
#define SENSOR_HPP

#include <iostream>
#include <string>
#include <variant>
#include <chrono>

using SensorValue = std::variant<int, float, bool, double>;

struct SensorData {
    uint32_t sensorId;
    std::chrono::system_clock::time_point timestamp;
    SensorValue data;
    std::string dataUnit;
};

class SensorInterface {
public:
    virtual ~SensorInterface() = default;

    virtual SensorData read() = 0;

    virtual std::string getSensorName() const = 0;
    virtual uint32_t getSensorId() const = 0;

};

class TempSensor: public SensorInterface {
    private:
        uint32_t id;
        std::string sensorName;
    public:
        TempSensor(uint32_t id, std::string name);

        SensorData read() override;

        std::string getSensorName() const override;

        uint32_t getSensorId() const override;
};

#endif