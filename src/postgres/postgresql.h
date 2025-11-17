#pragma once

#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <utility>
#include <vector>

#include "database.h"
#include "querybuilder/query_builder.h"

class PostgreSQL : public IDatabase {
  public:
    PostgreSQL(ConnectionConfig cfg, ILogger *logger) : IDatabase(std::move(cfg), std::move(logger))  {}

    bool open() override;
    void close() override;
    bool is_open() const override;

    bool insert(const QueryBuilder& qb) override;
    bool update(const QueryBuilder& qb) override;
    bool remove(const QueryBuilder& qb) override;

    QueryResult select(const QueryBuilder& qb) override;

    // Non-copyable
    PostgreSQL(const PostgreSQL&) = delete;
    PostgreSQL& operator=(const PostgreSQL&) = delete;

    // Movable
    PostgreSQL(PostgreSQL&&) = default;
    PostgreSQL& operator=(PostgreSQL&&) = default;

    ~PostgreSQL();

  private:
    std::unique_ptr<pqxx::connection> connection_;
};
