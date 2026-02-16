#include "DatabaseManager.hpp"

DataBaseManager::DataBaseManager(const std::string& dbPath) {
    sqlite3_open(dbPath.c_str(), &db);
    // Ensuring Table exists at the startup
    std::string sql = "CREATE TABLE IF NOT EXISTS smartedge_data_buffer (id INTEGER PRIMARY KEY AUTOINCREMENT, payload TEXT);";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

    // Prepare the Insert Template (The '?' is a placeholder)
    const char* insertSql = "INSERT INTO smartedge_data_buffer (payload) VALUES (?);";
    
    int rc = sqlite3_prepare_v2(db, insertSql, -1, &insertStmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << std::endl;
        insertStmt = nullptr;
    }
}

DataBaseManager::~DataBaseManager() {
    // Cleanup the bytecode memory
    if (insertStmt) {
        sqlite3_finalize(insertStmt);
    }
    if (db) {
        sqlite3_close(db);
    }
}

void DataBaseManager::storeToDb(const std::string& jsonPayload) {

    sqlite3_reset(insertStmt);
    sqlite3_clear_bindings(insertStmt);

    sqlite3_bind_text(insertStmt, 1, jsonPayload.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(insertStmt) != SQLITE_DONE) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(db) << std::endl;
        std::cout << "Insert to database failed." << std::endl;
    }

}