#include "SmartEdgeGateway.hpp"


int main () {

    SmartEdgeGateway gateway("tcp://localhost:1883", "iot/tempData");

    std::thread gateway_thread(&SmartEdgeGateway::run, &gateway);

    std::this_thread::sleep_for(std::chrono::seconds(45));

    gateway.stop(); // Signals all internal loops to stop

    if (gateway_thread.joinable()) {
        gateway_thread.join(); // Wait for the whole system to clean up
    }

    return 0;
}