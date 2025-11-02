#pragma once

#include <iostream>
#include <string>

enum class DatabaseType { PostgreSQL, sqlite };

enum class ConnectionMode {
    Direct,
    Pooled
};

struct ConnectionConfig {
    std::string host;
    int port = 5432;
    std::string dbname;
    std::string user;
    std::string password;
    int connect_timeout = 10;
    std::string path = "mydb.db";

    ConnectionMode mode = ConnectionMode::Direct;
    size_t poolSize = 5;

    std::string toPostgresConnection() const {
        return "host=" + host + " port=" + std::to_string(port) + " dbname=" + dbname +
               " user=" + user + " password=" + password +
               " connect_timeout=" + std::to_string(connect_timeout);
    }
};
