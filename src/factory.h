#pragma once

#include "config.h"
#include "log/src/logger.h"
#include "postgres/postgresql.h"
#include "sqlite/sqlite.h"
#include "connection/connection_pool.h" // this should be here to complete the type

#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>

class DatabaseFactory {
public:
    static std::shared_ptr<IDatabase> createDatabase(DatabaseType type,
                                                     const ConnectionConfig& cfg,
                                                     std::shared_ptr<ILogger> logger) {
        if (cfg.mode == ConnectionMode::Pooled) {
            return getPooledConnection(type, cfg, logger);
        }

        return createDirectConnection(type, cfg, logger);
    }

    // fix pool type to prevent incomplete type error
    static void releaseConnection(std::shared_ptr<IDatabase> db) {
        if (!db) return;
        db::ConnectionPool* pool = db->getPool(); // use complete namespace
        if (pool) {
            pool->release(db);
        }
    }

private:
    inline static std::mutex poolMutex;
    inline static std::unordered_map<std::string, std::shared_ptr<db::ConnectionPool>> pools;

    static std::shared_ptr<IDatabase> createDirectConnection(DatabaseType type,
                                                             const ConnectionConfig& cfg,
                                                             std::shared_ptr<ILogger> logger) {
        if (type == DatabaseType::PostgreSQL) {
            return std::make_shared<PostgreSQL>(cfg, logger.get());
        } else if (type == DatabaseType::sqlite) {
            return std::make_shared<SQLite>(cfg, logger.get());
        } else {
            throw std::invalid_argument("Unsupported database type");
        }
    }

    static std::shared_ptr<IDatabase> getPooledConnection(DatabaseType type,
                                                          const ConnectionConfig& cfg,
                                                          std::shared_ptr<ILogger> logger) {
        std::lock_guard<std::mutex> lock(poolMutex);

        std::string poolKey = generatePoolKey(type, cfg);

        if (pools.find(poolKey) == pools.end()) {
            pools[poolKey] = std::make_shared<db::ConnectionPool>(type, cfg, logger, cfg.poolSize);
        }

        return pools[poolKey]->acquire();
    }

    static std::string generatePoolKey(DatabaseType type, const ConnectionConfig& cfg) {
        return std::to_string(static_cast<int>(type)) + "_" + cfg.host + "_" + cfg.dbname;
    }
};
