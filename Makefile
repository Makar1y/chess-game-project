run:
	g++ src/main.cpp src/Game.cpp src/Board.cpp src/Piece.cpp src/Player.cpp src/Move.cpp -o chess.exe -lraylib -lgdi32 -lwinmm -lopengl32
	.\chess.exe
