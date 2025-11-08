#pragma once

#include <memory>
#include <string>
#include <vector>

#include "database.h"
#include "driver/sqlite3.h"
#include "spdlog/fmt/bundled/format.h"


class SQLite : public IDatabase {
  public:
    explicit SQLite(ConnectionConfig cfg, ILogger* logger);
    ~SQLite() override;
  
    bool open() override;
    void close() override;
    bool is_open() const override;

    bool insert(const QueryBuilder& qb) override;
    bool update(const QueryBuilder& qb) override;
    bool remove(const QueryBuilder& qb) override;
    QueryResult select(const QueryBuilder& qb) override;

    SQLite(const SQLite&) = delete;
    SQLite& operator=(const SQLite&) = delete;

    // Movable
    SQLite(SQLite&&) = default;
    SQLite& operator=(SQLite&&) = default;

  private:
    sqlite3* db_ = nullptr;
    bool executeQuery(const QueryBuilder& qb, bool returnsData, QueryResult* result = nullptr);
};
