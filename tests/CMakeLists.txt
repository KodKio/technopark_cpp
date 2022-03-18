cmake_minimum_required(VERSION 3.14)
project(test_exchange_rates)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES *.cpp)

enable_testing()
find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${EXCHANGE_RATES_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${EXCHANGE_RATES_LIBRARIES} GTest::gtest_main)