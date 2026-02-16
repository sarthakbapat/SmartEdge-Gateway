#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <iostream>
#include <sqlite3.h>
#include <string>

class DataBaseManager {
    private:
        sqlite3* db;
        sqlite3_stmt* insertStmt;
    public:
        DataBaseManager(const std::string& dbPath);

        ~DataBaseManager();

        void storeToDb(const std::string& jsonPayload);

    //    void fetchFromDb();
};

#endif
