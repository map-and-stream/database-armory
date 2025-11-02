# Set library name
set(LIBRARY_NAME database)

# Source files
set(DATABASE_SOURCES
    postgres/postgresql.cpp
    sqlite/sqlite.cpp
    connection/connection_pool.cpp         
)

# Header files
set(DATABASE_HEADERS
    postgres/postgresql.h
    factory.h
    config.h
    database.h
    sqlite/sqlite.h
    query_result.h
    querybuilder/query_builder.h
    connection/connection_pool.h         
)

# Submodules and dependencies
add_subdirectory(postgres/libpqxx)
add_subdirectory(sqlite/driver)
add_subdirectory(log)

# Ensure pthread is linked
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Create static library
add_library(${LIBRARY_NAME} STATIC ${DATABASE_SOURCES})

# Link dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC
    pqxx
    sqlite3
    Threads::Threads
    isiran::log
)

# Associate headers with target (for IDEs)
target_sources(${LIBRARY_NAME} PRIVATE ${DATABASE_HEADERS})

# Include directories
target_include_directories(${LIBRARY_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/connection    
)

# Create namespaced alias
add_library(${LIB_ALIAS} ALIAS ${LIBRARY_NAME})
