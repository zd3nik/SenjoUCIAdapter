cmake_minimum_required(VERSION 3.1)
project(senjo CXX)

file(GLOB OBJ_HDR *.h)
file(GLOB OBJ_SRC *.cpp)

include_directories(.)
add_library(${PROJECT_NAME} STATIC ${OBJ_HDR} ${OBJ_SRC})

