#pragma once

#include "logger.h"
#include "config.h"

#include <queue>
#include <unordered_set>
#include <mutex>
#include <memory>

class IDatabase;  // forward declaration is enough here

namespace db {

class DatabaseFactory;

class ConnectionPool {
public:
    ConnectionPool(DatabaseType type,
                   const ConnectionConfig& config,
                   std::shared_ptr<ILogger> logger,
                   size_t maxSize);

    std::shared_ptr<IDatabase> acquire();
    void release(std::shared_ptr<IDatabase> db);

private:
    std::queue<std::shared_ptr<IDatabase>> availableConnections;
    std::unordered_set<std::shared_ptr<IDatabase>> inUseConnections;
    std::mutex mtx;
    size_t maxSize;
    DatabaseType dbType;
    ConnectionConfig config;
    std::shared_ptr<ILogger> logger;

    std::shared_ptr<IDatabase> createConnection();
};

} // namespace db
