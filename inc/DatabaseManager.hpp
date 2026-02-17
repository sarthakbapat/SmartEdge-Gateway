#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <iostream>
#include <sqlite3.h>
#include <string>

struct OfflineMessage {
    int id;
    std::string json;
};

class DataBaseManager {
    private:
        sqlite3* db;
        sqlite3_stmt* insertStmt;
    public:
        DataBaseManager(const std::string& dbPath);

        ~DataBaseManager();

        void storeToDb(const std::string& jsonPayload);

        std::vector<OfflineMessage> fetchAllFromDb();

        void deleteById(int id);
};

#endif
