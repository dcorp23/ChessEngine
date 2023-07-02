CPPFLAGS = -Wfatal-errors --std=c++17 -pedantic -Ofast

all: tests main

tests: moveGenTest

main: main.o moveGen.o attackTables.o chessBoard.o bitFunctions.o evaluation.o search.o
	g++ $(CPPFLAGS) main.o bitFunctions.o moveGen.o attackTables.o chessBoard.o evaluation.o search.o -o main

moveGenTest: moveGenTest.o moveGen.o attackTables.o chessBoard.o bitFunctions.o
	g++ $(CPPFLAGS) moveGenTest.o bitFunctions.o moveGen.o attackTables.o chessBoard.o -o moveGenTest

main.o: main.cpp
	g++ $(CPPFLAGS) -c main.cpp -o main.o

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

clean:
	del *.o moveGenTest.exe main.exe