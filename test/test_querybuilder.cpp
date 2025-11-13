#include <gtest/gtest.h>
#include "querybuilder/query_builder.h"

// Basic test: typical complex SELECT
TEST(QueryBuilderTest, BasicSelectQuery) {
    QueryBuilder qb;
    std::string sql = qb.table("users u")
                       .select("u.id")
                       .select("u.name")
                       .select("o.total")
                       .join("orders o", "u.id", "o.user_id")
                       .where("u.active = true")
                       .orderBy("o.total DESC")
                       .limit(10)
                       .offset(20)
                       .str();

    EXPECT_TRUE(sql.find("FROM users u") != std::string::npos);
    EXPECT_TRUE(sql.find("JOIN orders o") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE u.active = true") != std::string::npos);
    EXPECT_TRUE(sql.find("ORDER BY o.total DESC") != std::string::npos);
    EXPECT_TRUE(sql.find("LIMIT 10") != std::string::npos);
    EXPECT_TRUE(sql.find("OFFSET 20") != std::string::npos);
}

// Empty builder
TEST(QueryBuilderTest, EmptyQueryBuilderShouldReturnEmptyString) {
    QueryBuilder qb;
    std::string sql = qb.str();
    EXPECT_TRUE(sql.empty() || sql == "");
}

// Only table
TEST(QueryBuilderTest, TableOnlyQuery) {
    QueryBuilder qb;
    std::string sql = qb.table("posts").str();
    EXPECT_TRUE(sql.find("posts") != std::string::npos);
}

// Multiple WHERE clauses
TEST(QueryBuilderTest, MultipleWhereClauses) {
    QueryBuilder qb;
    std::string sql = qb.table("items")
                       .where("price > 100")
                       .where("stock > 0")
                       .str();
    EXPECT_TRUE(sql.find("price > 100") != std::string::npos);
    EXPECT_TRUE(sql.find("AND stock > 0") != std::string::npos);
}

// Complex join chain
TEST(QueryBuilderTest, MultiJoinQuery) {
    QueryBuilder qb;
    std::string sql = qb.table("employees e")
                       .join("departments d", "e.dept_id", "d.id")
                       .join("roles r", "e.role_id", "r.id")
                       .select("e.name")
                       .select("d.title")
                       .select("r.level")
                       .where("r.active = 1")
                       .str();

    EXPECT_TRUE(sql.find("JOIN departments d") != std::string::npos);
    EXPECT_TRUE(sql.find("JOIN roles r") != std::string::npos);
}
