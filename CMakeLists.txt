cmake_minimum_required(VERSION 3.19)
project(SmartEdge_Gateway)

SET(CMAKE_C_COMPILER "/usr/bin/clang")
SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET (CMAKE_CXX_STANDARD 17)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a CMake cache variable to manage the log level
set(LOG_LEVEL "INFO" CACHE STRING "Set the spdlog level (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF)")

# Map the string to the spdlog numeric macros
if(LOG_LEVEL STREQUAL "TRACE")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
elseif(LOG_LEVEL STREQUAL "DEBUG")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()

# PahoMqttCpp is built and installed locally. find_package should be able to find it.
find_package(PahoMqttCpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(SQLite3 REQUIRED)

set(SRC
    src/Sensor.cpp
    src/Mqtt_publish.cpp
    src/DatabaseManager.cpp
    src/SmartEdgeGateway.cpp
    src/main.cpp)

# Use APPLE variable to detect macOS, else the idea is to cross-compile for Yocto.
if(APPLE)
    message(STATUS "Configuring for macOS (Apple) - Setting RPATH")
    # This ensures the executable finds the dylibs in /usr/local/lib on your Mac
    set(CMAKE_INSTALL_RPATH "/usr/local/lib")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
else()
    message(STATUS "Configuring for Linux/Embedded - Standard RPATH handling")
    # Yocto/Bitbake will handle RPATH and library linking via the toolchain file
endif()

add_executable(smartedge_gateway ${SRC})

target_include_directories(smartedge_gateway PRIVATE ${CMAKE_SOURCE_DIR}/inc)

target_link_libraries(smartedge_gateway 
    PRIVATE 
    PahoMqttCpp::paho-mqttpp3
    SQLite::SQLite3
    spdlog::spdlog
)
