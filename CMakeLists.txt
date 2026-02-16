cmake_minimum_required(VERSION 3.19)
project(SmartEdge_Gateway)

SET(CMAKE_C_COMPILER "/usr/bin/clang")
SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET (CMAKE_CXX_STANDARD 17)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)

# PahoMqttCpp is built and installed locally. find_package should be able to find it.
find_package(PahoMqttCpp REQUIRED)

set(SRC
    src/Sensor.cpp
    src/Mqtt_publish.cpp
    src/DatabaseManager.cpp
    src/SmartEdgeGateway.cpp
    src/main.cpp)

# Need to do this else error - no LC_RPATH's found. libpaho-mqttpp3.1.dylib not loaded.
set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

add_executable(smartedge_gateway ${SRC})

target_include_directories(smartedge_gateway PRIVATE ${CMAKE_SOURCE_DIR}/inc)

target_link_libraries(smartedge_gateway 
    PRIVATE 
    PahoMqttCpp::paho-mqttpp3
    sqlite3
)
