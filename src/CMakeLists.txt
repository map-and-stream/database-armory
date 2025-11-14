set(LIBRARY_NAME database_armory)

file(GLOB CONFIGURE_DEPENDS QUERYBUILDER_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/querybuilder/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/querybuilde/*.hpp")
file(GLOB CONFIGURE_DEPENDS QUERYBUILDER_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/postgres/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/postgres/*.hpp")
file(GLOB CONFIGURE_DEPENDS QUERYBUILDER_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/sqlite/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/sqlite/*.hpp")

set(DATABASE_SOURCES postgres/postgresql.cpp sqlite/sqlite.cpp)
set(DATABASE_HEADERS
    postgres/postgresql.h
    factory.h
    config.h
    database.h
    sqlite/sqlite.h
    query_result.h
    querybuilder/query_builder.h)

add_subdirectory(postgres/libpqxx)
add_subdirectory(sqlite/driver)
add_subdirectory(log_armory)

# Ensure pthread is linked
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(${LIBRARY_NAME} STATIC ${DATABASE_SOURCES})

# define public in order we cane use library in test folder
target_link_libraries(${LIBRARY_NAME} PUBLIC pqxx sqlite3 Threads::Threads
                                             isiran::log_armory)

# Keep headers associated with this target for IDEs, but do not propagate
target_sources(${LIBRARY_NAME} PRIVATE ${DATABASE_HEADERS})

target_include_directories(
  ${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                         ${CMAKE_CURRENT_SOURCE_DIR}/log_armory)

# Convenient namespaced alias for downstream targets
add_library(${LIB_ALIAS} ALIAS ${LIBRARY_NAME})

