CPPFLAGS = -Wfatal-errors --std=c++17 -pedantic -Ofast
SDL2FLAGS = -I sdl2/include -L sdl2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

all: tests main #evalTuning

tests: moveGenTest

main: main.o game.o moveGen.o attackTables.o chessBoard.o bitFunctions.o evaluation.o search.o
	g++ $(CPPFLAGS) -I sdl2/include -L sdl2/lib main.o game.o bitFunctions.o moveGen.o attackTables.o chessBoard.o evaluation.o search.o -o main -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

#evalTuning: evaluationTuning.o moveGen.o attackTables.o chessBoard.o bitFunctions.o evaluation.o search.o
#	g++ $(CPPFLAGS) evaluationTuning.o bitFunctions.o moveGen.o attackTables.o chessBoard.o evaluation.o search.o -o evalTuning

moveGenTest: moveGenTest.o moveGen.o attackTables.o chessBoard.o bitFunctions.o
	g++ $(CPPFLAGS) moveGenTest.o bitFunctions.o moveGen.o attackTables.o chessBoard.o -o moveGenTest

main.o: main.cpp
	g++ $(CPPFLAGS) $(SDL2FLAGS) -c main.cpp -o main.o

#evaluationTuning.o: evaluationTuning.cpp
#	g++ $(CPPFLAGS) -c evaluationTuning.cpp -o evaluationTuning.o

evaluation.o: evaluation.cpp evaluation.hpp
	g++ $(CPPFLAGS) -c evaluation.cpp -o evaluation.o

chessBoard.o: chessBoard.cpp chessBoard.hpp
	g++ $(CPPFLAGS) -c chessBoard.cpp -o chessBoard.o

search.o: search.cpp search.hpp
	g++ $(CPPFLAGS) -c search.cpp -o search.o

attackTables.o: attackTables.cpp attackTables.hpp
	g++ $(CPPFLAGS) -c attackTables.cpp -o attackTables.o

moveGenTest.o: moveGenTest.cpp
	g++ $(CPPFLAGS) -c moveGenTest.cpp -o moveGenTest.o

moveGen.o: moveGen.cpp moveGen.hpp
	g++ $(CPPFLAGS) -c moveGen.cpp -o moveGen.o

bitFunctions.o: bitFunctions.cpp bitFunctions.hpp
	g++ $(CPPFLAGS) -c bitFunctions.cpp -o bitFunctions.o

game.o: game.cpp game.hpp
	g++ $(CPPFLAGS) $(SDL2FLAGS) -c game.cpp -o game.o

clean:
	del *.o moveGenTest.exe main.exe evalTuning.exe