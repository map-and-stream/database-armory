#pragma once

#include <string>
#include <vector>
#include "config.h"
#include "query_result.h"
#include "querybuilder/query_builder.h"
#include "log/src/logger.h"
#include "connection/connection_pool.h" 

class IDatabase {
public:
    virtual ~IDatabase() = default;

    IDatabase(ConnectionConfig cfg, ILogger* logger)
        : config_(cfg), logger_(logger), pool_(nullptr) {}

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool is_open() const = 0;

    virtual bool insert(const QueryBuilder& qb) = 0;
    virtual bool update(const QueryBuilder& qb) = 0;
    virtual bool remove(const QueryBuilder& qb) = 0;
    virtual QueryResult select(const QueryBuilder& qb) = 0;

    // fix pool type to prevent incomplete type error
    void setPool(db::ConnectionPool* pool) { pool_ = pool; }
    db::ConnectionPool* getPool() const { return pool_; }

protected:
    ConnectionConfig config_;
    ILogger* logger_;
    db::ConnectionPool* pool_;  // fix type to prevent incomplete type error
};
