cmake_minimum_required(VERSION 3.20)
project(ChessGame)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

file(GLOB SOURCES
        src/*.cpp
)

add_executable(ChessGame ${SOURCES})