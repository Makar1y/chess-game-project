CXX = g++
TARGET = chess

SRC = src/main.cpp src/Game.cpp src/Draw.cpp src/Board.cpp src/Piece.cpp src/Player.cpp src/Move.cpp

CXXFLAGS = $(shell pkg-config --cflags raylib)
LDFLAGS = $(shell pkg-config --libs raylib)

ifeq ($(OS),Windows_NT)
	TARGET = chess.exe
	LDFLAGS = -lraylib -lgdi32 -lwinmm -lopengl32
endif

run:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)
	./$(TARGET)
