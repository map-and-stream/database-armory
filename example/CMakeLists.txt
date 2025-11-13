cmake_minimum_required(VERSION 3.10)
project(database_armory_example)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_ALIAS})
