cmake_minimum_required(VERSION 3.14)
project(test_films_recommend)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES *.cpp)

enable_testing()
find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${FILMS_RECOMMEND_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${FILMS_RECOMMEND_LIBRARIES} GTest::Main)