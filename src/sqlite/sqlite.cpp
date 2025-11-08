#include "sqlite.h"

#include <iostream>
#include <stdexcept>
#include <utility>

SQLite::SQLite(ConnectionConfig cfg, ILogger* logger)
    : IDatabase(std::move(cfg), std::move(logger)) {}

SQLite::~SQLite() {
    close();
}

bool SQLite::open() {
    if (is_open()){
        logger_->info("SQLite database already opened.");
        return true;
    }
    logger_->info(fmt::format("Trying to open SQLite database at path: {}", config_.path));
   



    // config.path should contain the SQLite DB file path
    int rc = sqlite3_open(config_.path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        // std::cerr << "Cannot open SQLite database: " << sqlite3_errmsg(db_) << std::endl;
        logger_->error(fmt::format("Cannot open SQLite database: {}", sqlite3_errmsg(db_)));
        close();
        return false;
    }
    logger_->info("SQLite database opened successfully.");
    return true;
}

void SQLite::close() {
    if (db_) {
        logger_->info("Closing SQLite database connection.");
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool SQLite::is_open() const {
    return db_ != nullptr;
}

bool SQLite::insert(const QueryBuilder& qb) {
    logger_->info(fmt::format("Executing INSERT: {}", qb.str()));
    return executeQuery(qb, false);
}

QueryResult SQLite::select(const QueryBuilder& qb) {
    logger_->info(fmt::format("Executing SELECT: {}", qb.str()));
    QueryResult result;
    executeQuery(qb, true, &result);
    return result;
}

bool SQLite::update(const QueryBuilder& qb) {
    logger_->info(fmt::format("Executing UPDATE: {}", qb.str()));
    return executeQuery(qb, false);
}

bool SQLite::remove(const QueryBuilder& qb) {
    logger_->info(fmt::format("Executing DELETE: {}", qb.str()));
    return executeQuery(qb, false);
}

bool SQLite::executeQuery(const QueryBuilder& qb, bool returnsData, QueryResult* result) {
    if (!is_open() && !open()) {
        return false;
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, qb.str().c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // std::cerr << "SQL error (prepare): " << sqlite3_errmsg(db_) << std::endl;
        logger_->error(fmt::format("SQL error (prepare): {}", sqlite3_errmsg(db_)));
        return false;
    }

    if (returnsData) {
        // Fetch column names
        int colCount = sqlite3_column_count(stmt);
        std::vector<std::string> columns;
        for (int i = 0; i < colCount; ++i) {
            const char* name = sqlite3_column_name(stmt, i);
            columns.emplace_back(name ? name : "");
        }

        // Fetch rows
        QueryResult::Table table;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            QueryResult::Row row;
            for (int i = 0; i < colCount; ++i) {
                const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                row.emplace_back(text ? text : "");
            }
            table.push_back(std::move(row));
        }

        if (rc != SQLITE_DONE) {
            // std::cerr << "SQL error (step): " << sqlite3_errmsg(db_) << std::endl;
            logger_->error(fmt::format("SQL error (step): {}", sqlite3_errmsg(db_)));
            sqlite3_finalize(stmt);
            return false;
        }

        *result = QueryResult(std::move(table), std::move(columns));
    } else {
        // Non-SELECT query
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "SQL error (step): " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }

    sqlite3_finalize(stmt);

    logger_->info("SQLite query executed successfully.");

    return true;
}
