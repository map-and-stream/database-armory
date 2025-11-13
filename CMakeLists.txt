cmake_minimum_required(VERSION 3.10)
project(database_armory)

set(CMAKE_CXX_STANDARD 20)
set(LIB_ALIAS isiran::database_armory)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LOG_BUILD_TESTS "Build log library tests" OFF)
option(LOG_BUILD_EXAMPLE "Build log example tests" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
if(DATABASE_ARMORY_BUILD_TESTS)
    add_subdirectory(test)
else()
    message("-- DATABASE_ARMORY_BUILD_TESTS is not set")
endif()
if(DATABASE_ARMORY_BUILD_EXAMPLE)
    add_subdirectory(example)
else()
    message("-- DATABASE_ARMORY_BUILD_EXAMPLE is not set")
endif()