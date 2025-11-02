#pragma once

#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "database.h"
#include "connection/connection_pool.h"

class PostgreSQL : public IDatabase, public std::enable_shared_from_this<PostgreSQL> {
public:
    PostgreSQL(ConnectionConfig cfg, ILogger* logger);

    bool open() override;
    void close() override;
    bool is_open() const override;

    bool insert(const QueryBuilder& qb) override;
    bool update(const QueryBuilder& qb) override;
    bool remove(const QueryBuilder& qb) override;
    QueryResult select(const QueryBuilder& qb) override;

    PostgreSQL(const PostgreSQL&) = delete;
    PostgreSQL& operator=(const PostgreSQL&) = delete;
    PostgreSQL(PostgreSQL&&) = default;
    PostgreSQL& operator=(PostgreSQL&&) = default;

    ~PostgreSQL();

private:
    std::unique_ptr<pqxx::connection> connection_;
};
