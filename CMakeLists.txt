cmake_minimum_required(VERSION 3.21)
project(Exchange_rates)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++0x -Wall -ggdb3")

enable_testing()
find_package(GTest REQUIRED)

add_executable(exchange_rates main.c array_of_rates.c exchange_rate.c)

include_directories(${GTEST_INCLUDE_DIRS})
add_executable(tests tests.cpp array_of_rates.c exchange_rate.c)
target_link_libraries(tests ${GTEST_LIBRARIES} m pthread)
