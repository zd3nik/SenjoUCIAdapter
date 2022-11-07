cmake_minimum_required(VERSION 3.1)

file(GLOB SENJO_HDR *.h)
file(GLOB SENJO_SRC *.cpp)

project(senjo CXX)
set(CMAKE_CXX_STANDARD 11)
add_library(${PROJECT_NAME} STATIC ${SENJO_HDR} ${SENJO_SRC})

