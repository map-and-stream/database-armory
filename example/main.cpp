#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <chrono>
#include "config.h"
#include "database.h"
#include "factory.h"
#include "log/src/factory.h"
#include "log/src/logger.h"
#include "querybuilder/query_builder.h"
#include "connection/connection_pool.h"
#include "spdlog/fmt/bundled/format.h"

void run_concurrent_insert(std::shared_ptr<db::ConnectionPool> pool, std::shared_ptr<ILogger> logger, int id) {
    auto db = pool->acquire();
    int retries = 5;
    while ((!db || !db->is_open()) && retries--) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        db = pool->acquire();
    }

    if (!db || !db->is_open()) {
        logger->error(fmt::format("Thread {} failed to acquire connection", id));
        return;
    }

    QueryBuilder qb;
    qb.table("users").insert(
        {"username", "password", "city", "email"},
        {fmt::format("user{}", id), "pass", "city", fmt::format("user{}@example.com", id)}
    );

    bool ok = db->insert(qb);
    logger->info(fmt::format("Thread {} insert result: {}", id, ok));

    pool->release(db);
}

int main() {
    LogConfig lcfg;
    lcfg.filePath = ".";
    lcfg.maxLogRotate = 100;
    lcfg.logLevel = LogLevel::info;

    ILogger* rawLogger = LoggerFactory::createLogger(LoggerType::Spdlog, lcfg);
    std::shared_ptr<ILogger> logger(rawLogger);

    logger->info("=== Starting database example ===");

    //Direct PostgreSQL connection
    ConnectionConfig cfg;
    cfg.host = "127.0.0.1";
    cfg.port = 5432;
    cfg.dbname = "mydb";
    cfg.user = "postgres";
    cfg.password = "qazwsx";
    cfg.connect_timeout = 5;
    cfg.mode = ConnectionMode::Direct;

    logger->info("Opening PostgreSQL connection (direct)...");
    std::shared_ptr<IDatabase> pg = DatabaseFactory::createDatabase(DatabaseType::PostgreSQL, cfg, logger);
    bool connected = pg->open();
    std::cout << "result of open postgres (direct): " << connected << std::endl;

    if (connected) {
        logger->info("Connected to PostgreSQL (direct)");

        QueryBuilder createTable;
        createTable.raw("CREATE TABLE IF NOT EXISTS users (id SERIAL PRIMARY KEY, username TEXT, password TEXT, city TEXT, email TEXT)");
        pg->insert(createTable);

        QueryBuilder insertUser;
        insertUser.table("users").insert({"username", "password", "city", "email"}, {"ali", "123", "shiraz", "ali@example.com"});
        pg->insert(insertUser);

        QueryBuilder updateUser;
        updateUser.table("users").update({{"city", "mashhad"}}).where("username = 'ali'");
        pg->update(updateUser);

        QueryBuilder selectUser;
        selectUser.table("users").select("id").select("username").select("city").select("email").where("username = 'ali'");
        logger->info("Selecting user 'ali'...");
        pg->select(selectUser).print();

        QueryBuilder deleteUser;
        deleteUser.table("users").remove().where("username = 'ali'");
        pg->remove(deleteUser);
    } else {
        logger->error("Failed to connect to PostgreSQL (direct)");
    }

    //Pooled PostgreSQL connection
    logger->info("Opening PostgreSQL connection (pooled)...");
    ConnectionConfig pooledCfg = cfg;
    pooledCfg.mode = ConnectionMode::Pooled;
    pooledCfg.poolSize = 10;

    std::shared_ptr<db::ConnectionPool> pool = std::make_shared<db::ConnectionPool>(
        DatabaseType::PostgreSQL, pooledCfg, logger, pooledCfg.poolSize
    );

    std::shared_ptr<IDatabase> pgPooled = pool->acquire();
    bool pooledConnected = pgPooled && pgPooled->open();
    std::cout << "result of open postgres (pooled): " << pooledConnected << std::endl;

    if (pooledConnected) {
        logger->info("Connected to PostgreSQL (pooled)");

        QueryBuilder insertUser;
        insertUser.table("users").insert({"username", "password", "city", "email"}, {"reza", "456", "tehran", "reza@example.com"});
        pgPooled->insert(insertUser);

        QueryBuilder selectUser;
        selectUser.table("users").select("id").select("username").select("city").select("email").where("username = 'reza'");
        logger->info("Selecting user 'reza'...");
        pgPooled->select(selectUser).print();

        QueryBuilder deleteUser;
        deleteUser.table("users").remove().where("username = 'reza'");
        pgPooled->remove(deleteUser);

        pool->release(pgPooled);
        logger->info("PostgreSQL pooled connection released");

        // test concurrent inserts with pooled connection
        logger->info("Starting concurrent inserts with pooled connection...");
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back(run_concurrent_insert, pool, logger, i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //latency between threads
        }
        for (auto& t : threads) {
            t.join();
        }
        logger->info("Concurrent inserts finished");
    } else {
        logger->error("Failed to connect to PostgreSQL (pooled)");
    }

   // SQLite connection
    logger->info("Opening SQLite connection...");
    cfg.path = "mydb.db";
    cfg.mode = ConnectionMode::Direct;

    std::shared_ptr<IDatabase> sq = DatabaseFactory::createDatabase(DatabaseType::sqlite, cfg, logger);
    bool sqliteConnected = sq->open();
    std::cout << "result of open sqlite: " << sqliteConnected << std::endl;

    if (sqliteConnected) {
        logger->info("Connected to SQLite");

        //CREATE TABLE
        logger->info("Creating 'users' table if not exists...");
        QueryBuilder createTable;
        createTable.raw("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT, city TEXT, email TEXT)");
        bool createOk = sq->insert(createTable);
        logger->info(fmt::format("Table creation result: {}", createOk ? "success" : "failed"));

        //INSERT
        logger->info("Inserting user 'sara'...");
        QueryBuilder insertUser;
        insertUser.table("users").insert({"username", "password", "city", "email"}, {"sara", "abc", "tabriz", "sara@example.com"});
        bool insertOk = sq->insert(insertUser);
        logger->info(fmt::format("Insert result: {}", insertOk ? "success" : "failed"));

        //UPDATE
        logger->info("Updating city for user 'sara'...");
        QueryBuilder updateUser;
        updateUser.table("users").update({{"city", "rasht"}}).where("username = 'sara'");
        bool updateOk = sq->update(updateUser);
        logger->info(fmt::format("Update result: {}", updateOk ? "success" : "failed"));

        //SELECT
        logger->info("Selecting user 'sara'...");
        QueryBuilder selectUser;
        selectUser.table("users").select("id").select("username").select("city").select("email").where("username = 'sara'");
        auto result = sq->select(selectUser);
        logger->info("Select executed successfully");
        result.print();

        //DELETE
        logger->info("Deleting user 'sara'...");
        QueryBuilder deleteUser;
        deleteUser.table("users").remove().where("username = 'sara'");
        bool deleteOk = sq->remove(deleteUser);
        logger->info(fmt::format("Delete result: {}", deleteOk ? "success" : "failed"));
    } else {
        logger->error("Failed to connect to SQLite");
    }


    logger->info("=== Database example finished ===");
    return 0;
}
