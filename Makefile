CXX = g++
TARGET = chess

SRC = src/main.cpp src/Game.cpp src/Draw.cpp src/UserInput.cpp src/Board.cpp src/Piece.cpp src/Player.cpp src/Move.cpp src/Stockfish.cpp src/Audio.cpp

ifeq ($(OS),Windows_NT)
	SRC += src/StockfishInput_win.cpp
	TARGET = chess.exe
	CXXFLAGS = -Isrc
	LDFLAGS = -lraylib -lgdi32 -lwinmm -lopengl32
else
	SRC += src/StockfishInput_unix.cpp
	CXXFLAGS = -Isrc $(shell pkg-config --cflags raylib)
	LDFLAGS = $(shell pkg-config --libs raylib)
endif

run:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)
	./$(TARGET)
