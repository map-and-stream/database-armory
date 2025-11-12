set(LIBRARY_NAME database)

set(DATABASE_SOURCES
postgres/postgresql.cpp
sqlite/sqlite.cpp
)

set(DATABASE_HEADERS
postgres/postgresql.h
factory.h
config.h
database.h
sqlite/sqlite.h
query_result.h
querybuilder/query_builder.h
)

add_subdirectory(postgres/libpqxx)
add_subdirectory(sqlite/driver)
add_subdirectory(log)

# Ensure pthread is linked
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(${LIBRARY_NAME} STATIC ${DATABASE_SOURCES})

#define public in order we cane use library in test folder
target_link_libraries(${LIBRARY_NAME} PUBLIC pqxx sqlite3 Threads::Threads isiran::log)

# Keep headers associated with this target for IDEs, but do not propagate
target_sources(${LIBRARY_NAME} PRIVATE ${DATABASE_HEADERS})

target_include_directories(${LIBRARY_NAME} 
PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/log
)

# Convenient namespaced alias for downstream targets
add_library(${LIB_ALIAS} ALIAS ${LIBRARY_NAME})

if(DATBASE_BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(test_querybuilder test/test_querybuilder.cpp)
    target_link_libraries(test_querybuilder PRIVATE GTest::gtest_main isiran::database)
    include(GoogleTest)
    gtest_discover_tests(test_querybuilder)
endif()