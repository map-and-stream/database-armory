#pragma once

#include "config.h"
#include "log_armory/src/logger.h"
#include "postgres/postgresql.h"
#include "sqlite/sqlite.h"

class DatabaseFactory {
  public:
    static std::unique_ptr<IDatabase> createDatabase(DatabaseType type, ConnectionConfig cfg,
                                                     ILogger* logger) {
        if (type == DatabaseType::PostgreSQL) {
            return std::make_unique<PostgreSQL>(cfg, logger);
        } else if (type == DatabaseType::sqlite) {
            return std::make_unique<SQLite>(cfg, logger);
        } else {
            throw std::invalid_argument("Invalid logger type");
        }
    }
};
