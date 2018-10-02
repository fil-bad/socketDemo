cmake_minimum_required(VERSION 3.9.6)
project(socketDemo C)

set(CMAKE_C_STANDARD 11)

find_package(Threads)

add_executable(socketDemo main.c socketConnect.c socketConnect.h)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
