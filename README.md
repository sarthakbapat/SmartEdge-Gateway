![Static Analysis](https://github.com/sarthakbapat/SmartEdge-Gateway/actions/workflows/Static_analysis.yml/badge.svg)

# Smart Edge Gateway (C++17, MQTT, SQLite3)

A resilient, multi-threaded IoT Edge Gateway designed for industrial data acquisition. This project bridges the gap between local sensors and the cloud, ensuring high-availability and zero data loss through local persistence and automated synchronization.

## 🏗 System Architecture

The gateway implements a **Producer-Consumer architecture** to ensure that sensor data ingestion is never affected by network latency or cloud downtime.



### Core Components:
* **Sensor Thread:** High-frequency ingestion from local sensors (simulated or BLE) into a thread-safe **Ring Buffer**.
* **Main Gateway Thread:** Consumes telemetry from the buffer and manages the primary **MQTT** upload.
* **Persistence Layer:** A **SQLite3** "Store-and-Forward" mechanism that activates automatically during network outages.
* **Background Sync Task:** An asynchronous thread triggered by MQTT callbacks to restore data integrity upon reconnection by triggering cloud upload of stored data.

---

## 🛠 Design Decisions

### 1. Multi-threaded Synchronization
**Decision:** Used a fixed-size thread-safe Ring Buffer with `std::mutex`.
**Reasoning:** Unlike a standard queue, a Ring Buffer provides a predictable memory footprint. This is essential for embedded devices to prevent memory fragmentation and "Out of Memory" (OOM) crashes in long-running applications.

### 2. Fault Tolerance & Persistence
**Decision:** Implemented SQLite3 for offline buffering instead of CSV files.
**Reasoning:** SQLite provides **ACID compliance** (Atomicity, Consistency, Isolation, Durability). This ensures that if the gateway loses power mid-write, the database remains uncorrupted. It also allows for structured "First-In-First-Out" (FIFO) recovery.

### 3. Asynchronous Communication
**Decision:** Inherited from `mqtt::callback` to handle connection events.
**Reasoning:** By using non-blocking callbacks, the gateway can detect connection loss and restoration instantly without polling the network, saving CPU cycles and improving response time for data recovery.

---

## 🚀 Key Features
* **Fault Tolerance:** Automatic diversion of JSON payloads to database during MQTT broker disconnects.
* **Zero Data Loss:** Background synchronization flushes buffered data once the uplink is restored.
* **Concurrency:** Efficiently manages shared resources using Modern C++ synchronization primitives.
* **Scalable Serialization:** Uses `nlohmann/json` for flexible and human-readable data packets.

---

## 🛠 Tech Stack
* **Language:** Modern C++ (C++17)
* **Protocols:** MQTT (Paho C++), BLE (Planned/Future)
* **Database:** SQLite3
* **Build System:** CMake
* **Dependencies:** `paho.mqtt.cpp`, `nlohmann_json`, `sqlite3`, `spdlog`

---

## 💻 Installation & Usage

### Prerequisites
Ensure you have the following installed:
* CMake (3.10+)
* Paho MQTT C & C++ Libraries
* SQLite3 Development Headers

### Build Instructions
```bash
mkdir build && cd build
cmake ..
make
./SmartEdgeGateway
