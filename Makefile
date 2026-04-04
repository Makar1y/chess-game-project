run:
	g++ src/main.cpp src/Game.cpp -o chess.exe -lraylib -lgdi32 -lwinmm -lopengl32
	.\chess.exe
