cmake_minimum_required(VERSION 3.20)
project(ChessGame)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

if(WIN32)
    set(SOURCES
        src/Board.cpp
        src/Draw.cpp
        src/Game.cpp
        src/Move.cpp
        src/Piece.cpp
        src/Player.cpp
        src/Stockfish.cpp
        src/StockfishInput_win.cpp
        src/UserInput.cpp
        src/main.cpp
    )
else()
    set(SOURCES
        src/Board.cpp
        src/Draw.cpp
        src/Game.cpp
        src/Move.cpp
        src/Piece.cpp
        src/Player.cpp
        src/Stockfish.cpp
        src/StockfishInput_unix.cpp
        src/UserInput.cpp
        src/main.cpp
    )
endif()

add_executable(ChessGame ${SOURCES})