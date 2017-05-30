cmake_minimum_required(VERSION 3.5)
project(lab8)

# adds ./include to compiler include directories
include_directories(include)

# uncomment to use libxml2 XML headers
# include_directories(/usr/include/libxml2)

# compiles code files from ./src folder
file(GLOB SOURCES "src/film.cpp" "src/requestHandler.cpp")

add_executable(lab8 "main.cpp" ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} progbase-cpp progbase ssl jansson)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Werror -Wall")