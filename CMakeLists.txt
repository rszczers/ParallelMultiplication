cmake_minimum_required(VERSION 3.2)
project(projekt)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES
    loader.c
    loader.h
    main.c)

add_executable(projekt ${SOURCE_FILES} deadlock.h deadlock.c cannon.h cannon.c)

find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(projekt ${MPI_LIBRARIES})