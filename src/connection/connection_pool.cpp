#include "connection_pool.h"
#include "factory.h"
#include "database.h"
#include "postgres/postgresql.h"
#include "sqlite/sqlite.h"
#include "spdlog/fmt/bundled/core.h"  // add for fmt::format

namespace db {

ConnectionPool::ConnectionPool(DatabaseType type,
                               const ConnectionConfig& config,
                               std::shared_ptr<ILogger> logger,
                               size_t maxSize)
    : dbType(type), config(config), logger(logger), maxSize(maxSize) {
    logger->info(fmt::format("ConnectionPool constructed with maxSize = {}", maxSize));
}

std::shared_ptr<IDatabase> ConnectionPool::acquire() {
    std::lock_guard<std::mutex> lock(mtx);
    logger->info("ConnectionPool::acquire() called");

    logger->info(fmt::format("Available connections: {}", availableConnections.size()));
    logger->info(fmt::format("In-use connections: {}", inUseConnections.size()));

    if (!availableConnections.empty()) {
        auto conn = availableConnections.front();
        availableConnections.pop();
        inUseConnections.insert(conn);
        logger->info("Reusing existing connection from pool");
        return conn;
    }

    if (inUseConnections.size() < maxSize) {
        logger->info("Creating new connection...");
        auto conn = createConnection();
        if (conn) {
            inUseConnections.insert(conn);
            logger->info("New connection created and added to in-use");
            return conn;
        } else {
            logger->error("Failed to create new connection");
        }
    } else {
        logger->warn(fmt::format("Max pool size reached: {}", maxSize));
    }

    logger->error("Connection pool exhausted");
    return nullptr;
}

void ConnectionPool::release(std::shared_ptr<IDatabase> db) {
    std::lock_guard<std::mutex> lock(mtx);
    logger->info("ConnectionPool::release() called");

    auto it = inUseConnections.find(db);
    if (it != inUseConnections.end()) {
        inUseConnections.erase(it);
        availableConnections.push(db);
        logger->info("Connection released back to pool");
    } else {
        logger->warn("Attempted to release unknown connection");
    }

    logger->info(fmt::format("Pool status after release - Available: {}, In-use: {}",
                             availableConnections.size(), inUseConnections.size()));
}

std::shared_ptr<IDatabase> ConnectionPool::createConnection() {
    logger->info("ConnectionPool::createConnection() called");

    std::shared_ptr<IDatabase> conn;

    if (dbType == DatabaseType::PostgreSQL) {
        logger->info("Creating PostgreSQL connection directly");
        conn = std::make_shared<PostgreSQL>(config, logger.get());
    } else if (dbType == DatabaseType::sqlite) {
        logger->info("Creating SQLite connection directly");
        conn = std::make_shared<SQLite>(config, logger.get());
    } else {
        logger->error("Unsupported database type");
        return nullptr;
    }

    if (conn) {
        logger->info("Connection created, setting pool reference");
        conn->setPool(this);
    } else {
        logger->error("Connection creation returned null");
    }

    return conn;
}

} // namespace db
