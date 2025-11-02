#include <iostream>

#include "factory.h"
#include "log/src/factory.h"
#include "log/src/logger.h"
#include "querybuilder/query_builder.h"

int main() {
    // PostgreSQL config
    ConnectionConfig cfg;
    cfg.host = "127.0.0.1";
    cfg.port = 5432;
    cfg.dbname = "mydb";
    cfg.user = "postgres";
    cfg.password = "qazwsx";
    cfg.connect_timeout = 5;

    // Logger config
    LogConfig lcfg;
    lcfg.filePath = ".";
    lcfg.maxLogRotate = 100;
    lcfg.logLevel = LogLevel::info;
    ILogger* logger = LoggerFactory::createLogger(LoggerType::Spdlog, lcfg);

    logger->info("Trying to open PostgreSQL connection...");

    std::unique_ptr<IDatabase> pg =
        DatabaseFactory::createDatabase(DatabaseType::PostgreSQL, cfg, logger);

    bool connected = pg->open();
    std::cout << "result of open postgres: " << connected << std::endl;

    if (connected) {
        logger->info("PostgreSQL connection established.");

        // CREATE TABLE (optional, if not exists)
        QueryBuilder createTable;
        createTable.raw("CREATE TABLE IF NOT EXISTS users ("
                        "id SERIAL PRIMARY KEY, "
                        "username TEXT, "
                        "password TEXT, "
                        "city TEXT, "
                        "email TEXT)");
        pg->insert(createTable);

        // INSERT
        QueryBuilder insertUser;
        insertUser.table("users").insert(
            {"username", "password", "city", "email"},
            {"ali", "123", "shiraz", "ali@example.com"});
        pg->insert(insertUser);

        // UPDATE
        QueryBuilder updateUser;
        updateUser.table("users").update({{"city", "mashhad"}}).where("username = 'ali'");
        pg->update(updateUser);

        // SELECT
        QueryBuilder selectUser;
        selectUser.table("users").select("id").select("username").select("city").select("email")
                  .where("username = 'ali'");
        pg->select(selectUser).print();

        // DELETE
        QueryBuilder deleteUser;
        deleteUser.table("users").remove().where("username = 'ali'");
        pg->remove(deleteUser);
    } else {
        logger->error("Failed to connect to PostgreSQL.");
    }

    // SQLite config
    cfg.path = "mydb.db";
    std::unique_ptr<IDatabase> sq =
        DatabaseFactory::createDatabase(DatabaseType::sqlite, cfg, logger);
    std::cout << "result of open sqlite: " << sq->open() << std::endl;

    if (sq->is_open()) {
        logger->info("SQLite connection established.");

        // CREATE TABLE (required)
        QueryBuilder createTable;
        createTable.raw("CREATE TABLE IF NOT EXISTS users ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "username TEXT, "
                        "password TEXT, "
                        "city TEXT, "
                        "email TEXT)");
        sq->insert(createTable);

        // INSERT
        QueryBuilder insertUser;
        insertUser.table("users").insert(
            {"username", "password", "city", "email"},
            {"sara", "abc", "tabriz", "sara@example.com"});
        sq->insert(insertUser);

        // UPDATE
        QueryBuilder updateUser;
        updateUser.table("users").update({{"city", "rasht"}}).where("username = 'sara'");
        sq->update(updateUser);

        // SELECT
        QueryBuilder selectUser;
        selectUser.table("users").select("id").select("username").select("city").select("email")
                  .where("username = 'sara'");
        sq->select(selectUser).print();

        // DELETE
        QueryBuilder deleteUser;
        deleteUser.table("users").remove().where("username = 'sara'");
        sq->remove(deleteUser);
    } else {
        logger->error("Failed to connect to SQLite.");
    }

    return 0;
}
