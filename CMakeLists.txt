cmake_minimum_required(VERSION 3.15)
project(web_service)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party/crow)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} Crow::Crow)