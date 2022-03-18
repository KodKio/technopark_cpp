cmake_minimum_required(VERSION 3.0.0)
project(exchange_rates_lib)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-fprofile-arcs -ftest-coverage -fPIC -O0")

file(GLOB SOURCES ./source/*.c)
file(GLOB INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} ${SOURCES})

set(EXCHANGE_RATES_LIBRARIES ${PROJECT_NAME} PARENT_SCOPE)
set(EXCHANGE_RATES_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)