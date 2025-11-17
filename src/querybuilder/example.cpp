#include <iostream>

#include "query_builder.h"

int main() {
    Query q;
    std::string sql = q.table("users u")
                          .select("u.id")
                          .select("u.name")
                          .select("o.total")
                          .join("orders o", "u.id", "o.user_id")
                          .where("u.active = true")
                          .orderBy("o.total DESC")
                          
                          .limit(10)
                          .offset(20)
                          .str();

    std::cout << sql << "\n";
}
