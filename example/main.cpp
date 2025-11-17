#include <iostream>

#include "factory.h"
#include "log_armory/src/factory.h"
#include "log_armory/src/logger.h"
#include "querybuilder/query_builder.h"

int main() {
    ConnectionConfig cfg;
    cfg.host = "192.168.10.127";
    cfg.port = 5432;
    cfg.dbname = "mydb";  // users(id, username,password, city, email)
    cfg.user = "postgres";
    cfg.password = "qazwsx";
    cfg.connect_timeout = 5;
    LogConfig lcfg;
    lcfg.filePath = ".";
    lcfg.maxLogRotate = 100;
    lcfg.logLevel = LogLevel::info;
    ILogger* logger = LoggerFactory::createLogger(LoggerType::Spdlog, lcfg);
    // define QueryBuilder Crud Operation
    // qb_insert
    // qb_update
    QueryBuilder qb_insert;
    QueryBuilder qb_update;
    QueryBuilder qb_delete;



    logger->info("try to open connection...");
    // PostgreSQL pg("postgresql://postgres:qazwsx@172.21.144.1:5432/mydb?connect_timeout=2");
    std::unique_ptr<IDatabase> pg =
        DatabaseFactory::createDatabase(DatabaseType::PostgreSQL, cfg, logger);

    //PostgreSQL pg(cfg);  // TODO: use this way for create connection
    std::cout << "result of open postgres :" << pg->open() << std::endl;
    QueryBuilder q;
    std::string sql = q.table("users u")
                          .select("u.id")
                          .select("u.name")
                          .select("u.famile")
                          .select("u.email")
                          .select("u.age")
                          .orderBy("u.id DESC")
                          .limit(10)
                          .offset(0)
                          .str();

    std::cout << "query of querybuilder :" << sql << "\n";
    //pg->select(q).print();
   


    //for handeling insert data to postgres i have wrote these code 

         qb_insert.insert("users")
               .values({{"name", "ebrahim"}, {"email", "keramat@example.com"}, {"famile", "kermat"}, {"age", "45"}});
      std::string insert_sql = qb_insert.str();
      std::cout << "INSERT query: " << insert_sql << "\n";
      bool insert_result = pg->insert(qb_insert);
      std::cout << "Result of insert: " << insert_result << std::endl;


    // for updating row in example and postgre sql i have decided add this code here 

    qb_update.update("users").set("email", "rezaeiOffice23@isIran.ir").where("id = 12");
    bool update_result = pg->update(qb_update);
    std::cout << "Result of update: " << update_result << std::endl;

    //for handling delete row i have called qb-delete method here 

    qb_delete.remove("users").where("id = 18");
    std::string delete_sql = qb_delete.str();
    std::cout << "DELETE query: " << delete_sql << "\n";
    // Execute DELETE
    bool delete_result = pg->remove(qb_delete);
    std::cout << "Result of delete: " << delete_result << std::endl;

    // Verify
    // Show table after update remove insert crud operations
    QueryBuilder show_all;
    show_all.table("users").select("*");
    pg->select(show_all).print();


    // refacot paramter table, parameter query, parameter value
    // create test table
    // pg.insert("insert INTO users(username,password, city, email) values ($1, $2, $3, $4)",
    // std::vector<std::string>{"javad", "javadi","sadsad", "sdfdsfdsf"});
      // using QueryBuilder just for interface compliance
    // In reality, you'd usually pass more detailed information or extend QueryBuilder for update support
    // pg->select(std::string("select * from users;"));
    // bool result = pg->update("UPDATE users SET name = $1 WHERE age = $2", {"Abolfazl Jr.",
    // "19"}); std::cout << "result of update :" << result << std::endl; result = pg->remove("DELETE
    // FROM users WHERE age = $1", {"2"}); std::cout << "result of delete :" << result << std::endl;

    cfg.path = "mydb.db";
    std::unique_ptr<IDatabase> sq =
        DatabaseFactory::createDatabase(DatabaseType::sqlite, cfg, logger);
    std::cout << "result of open sqlite :" << sq->open() << std::endl;
    QueryBuilder q_;
    std::string sql_ = q_.table("users u")
                          .select("u.id")
                          .select("u.name")
                          .select("u.family")
                          .select("u.email")
                          .select("u.age")
                          .orderBy("u.id DESC")
                          .update("users")
                          .limit(10)
                          .offset(0)
                          .str();

    std::cout << "query of querybuilder :" << sql_ << "\n";
   
    sq->select(q_).print();
    
     // for handeling insert to table and calling qb_insert i have wrote these bunch of codes

     qb_insert.insert("users")
              .values({{"name", "reza"}, {"email", "bagheri@example.com"}, {"family", "bagheri"}, {"age", "50"}});
     std::string insert1_sql = qb_insert.str();
     std::cout << "INSERT query: " << insert1_sql << "\n";
     bool insert1_result = sq->insert(qb_insert);
     std::cout << "Result of insert: " << insert1_result << std::endl;



    // for updating row in example i have decided add this code here 

    
    qb_update.update("users").set("email", "navid2@isIran.ir").where("id = 4");
    bool update1_result = sq->update(qb_update);
    std::cout << "Result of update: " << update1_result << std::endl;

    // for handling delete row i have called qb-delete method here 

    
    qb_delete.remove("users").where("id = 3");
    std::string delete1_sql = qb_delete.str();
    std::cout << "DELETE query: " << delete1_sql << "\n";
    // Execute DELETE
    bool delete1_result = sq->remove(qb_delete);
    std::cout << "Result of delete: " << delete1_result << std::endl;

    // Verify
    // Show table after update remove insert crud operations

    //QueryBuilder show_all;
    //show_all.table("users").select("*");
    sq->select(show_all).print();



    return 0;
}
