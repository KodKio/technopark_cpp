cmake_minimum_required(VERSION 3.0.0)
project(exchange_rates)

add_subdirectory(project)

add_executable(${PROJECT_NAME} main.c)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -coverage -lgcov" )

target_include_directories(${PROJECT_NAME} PUBLIC ${EXCHANGE_RATES_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXCHANGE_RATES_LIBRARIES})

add_subdirectory(tests)