cmake_minimum_required(VERSION 3.12)
project(socketDemo C)

set(CMAKE_C_STANDARD 11)

add_executable(socketDemo main.c socketConnect.c socketConnect.h)