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
        spdlog::error("DatabaseManager: Failed to prepare insert statement: {}", sqlite3_errmsg(db));
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
        spdlog::error("DatabaseManager: Insert to database failed: {}", sqlite3_errmsg(db));
        
    }

}

std::vector<OfflineMessage> DataBaseManager::fetchAllFromDb() {
    std::vector<OfflineMessage> messages;
    const char* sql = "SELECT id, payload FROM smartedge_data_buffer ORDER BY id ASC;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* jsonText = sqlite3_column_text(stmt, 1);
            
            // Convert const unsigned char* to const char* using reinterpret_cast. Then wrap into std::string object.
            messages.push_back({id, std::string(reinterpret_cast<const char*>(jsonText))});
        }
    }
    sqlite3_finalize(stmt);
    return messages;
}

void DataBaseManager::deleteById(int id) {
    const char* sql = "DELETE FROM smartedge_data_buffer WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}