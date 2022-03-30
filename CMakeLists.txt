cmake_minimum_required(VERSION 3.0.0)
project(films_recommend)

set(SIMPLE_NAME simple)
set(PARALLEL_NAME parallel)

IF(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
ENDIF()

add_subdirectory(project)

add_executable(${SIMPLE_NAME} main_simple.c)

target_include_directories(${SIMPLE_NAME} PUBLIC ${FILMS_RECOMMEND_SIMPLE_INCLUDE_DIRS})
target_include_directories(${SIMPLE_NAME} PUBLIC ${FILMS_RECOMMEND_GENERAL_INCLUDE_DIRS})
target_link_libraries(${SIMPLE_NAME} PRIVATE ${FILMS_RECOMMEND_SIMPLE_LIBRARIES})

add_executable(${PARALLEL_NAME} main_parallel.c)

target_include_directories(${PARALLEL_NAME} PUBLIC ${FILMS_RECOMMEND_PARALLEL_INCLUDE_DIRS})
target_include_directories(${PARALLEL_NAME} PUBLIC ${FILMS_RECOMMEND_GENERAL_INCLUDE_DIRS})
target_link_libraries(${PARALLEL_NAME} PRIVATE ${FILMS_RECOMMEND_PARALLEL_LIBRARIES})

add_subdirectory(tests)