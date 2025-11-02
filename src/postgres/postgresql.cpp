#include "postgresql.h"
#include "connection/connection_pool.h"
#include <pqxx/pqxx>
#include <stdexcept>
#include "spdlog/fmt/bundled/format.h"

PostgreSQL::PostgreSQL(ConnectionConfig cfg, ILogger* logger)
    : IDatabase(std::move(cfg), std::move(logger)) {}

bool PostgreSQL::open() {
    logger_->info("PostgreSQL::open() called");
    if (connection_) {
        logger_->info("DB already open");
        return true;
    }
    try {
        connection_ = std::make_unique<pqxx::connection>(config_.toPostgresConnection());
        logger_->info("PostgreSQL connection opened successfully");
        return connection_->is_open();
    } catch (const std::exception& e) {
        logger_->error(fmt::format("Open connection failed: {}", e.what()));
        connection_.reset();
        return false;
    }
}

void PostgreSQL::close() {
    logger_->info("PostgreSQL::close() called");

    if (connection_) {
        connection_.reset();
        logger_->info("Connection reset");
    }

    if (pool_) {
        try {
            auto self = shared_from_this();
            if (self.use_count() > 1) {
                pool_->release(self);
                logger_->info("Connection released to pool");
            } else {
                logger_->warn(fmt::format("Skip release: shared_from_this() not safe (use_count = {})", self.use_count()));
            }
        } catch (const std::bad_weak_ptr& e) {
            logger_->error(fmt::format("Release failed: {}", e.what()));
        }
    }
}

bool PostgreSQL::is_open() const {
    return connection_ && connection_->is_open();
}

PostgreSQL::~PostgreSQL() {
    if (connection_) {
        connection_.reset();
    }
}

bool PostgreSQL::insert(const QueryBuilder& qb) {
    if (!is_open()) {
        logger_->error("Cannot insert: database not open.");
        return false;
    }
    try {
        pqxx::work txn(*connection_);
        txn.exec(qb.str());  // remove warning exec_params
        txn.commit();
        logger_->info("Insert successful");
        return true;
    } catch (const std::exception& e) {
        logger_->error(fmt::format("Insert failed: {}", e.what()));
        return false;
    }
}

QueryResult convert_result(const pqxx::result& res) {
    std::vector<std::string> columns;
    if (res.columns() > 0) {
        columns.reserve(res.columns());
        for (pqxx::row_size_type i = 0; i < res.columns(); ++i) {
            columns.push_back(std::string(res.column_name(i)));
        }
    }

    QueryResult::Table table;
    table.reserve(res.size());

    for (const auto& row : res) {
        QueryResult::Row r;
        r.reserve(row.size());
        for (const auto& field : row) {
            r.emplace_back(field.is_null() ? "NULL" : std::string(field.c_str()));
        }
        table.push_back(std::move(r));
    }

    return QueryResult(std::move(table), std::move(columns));
}

QueryResult PostgreSQL::select(const QueryBuilder& qb) {
    try {
        pqxx::work txn(*connection_);
        pqxx::result res = txn.exec(qb.str());
        txn.commit();
        logger_->info("Select executed successfully");
        return convert_result(res);
    } catch (const std::exception& e) {
        logger_->error(fmt::format("SELECT failed: {}", e.what()));
        return convert_result(pqxx::result{});
    }
}

bool PostgreSQL::update(const QueryBuilder& qb) {
    if (!is_open()) {
        logger_->error("Cannot update: database not open.");
        return false;
    }
    try {
        pqxx::work txn(*connection_);
        txn.exec(qb.str());
        txn.commit();
        logger_->info("Update successful");
        return true;
    } catch (const std::exception& e) {
        logger_->error(fmt::format("Update failed: {}", e.what()));
        return false;
    }
}

bool PostgreSQL::remove(const QueryBuilder& qb) {
    if (!is_open()) {
        logger_->error("Cannot delete: database not open.");
        return false;
    }
    try {
        pqxx::work txn(*connection_);
        txn.exec(qb.str());
        txn.commit();
        logger_->info("Delete successful");
        return true;
    } catch (const std::exception& e) {
        logger_->error(fmt::format("Delete failed: {}", e.what()));
        return false;
    }
}
