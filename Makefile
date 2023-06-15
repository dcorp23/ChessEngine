CPPFLAGS = -Wfatal-errors --std=c++17 -pedantic

all: tests

tests: moveGenTest

moveGenTest: moveGenTest.o moveGen.o attackTables.o chessBoard.o 
	g++ $(CPPFLAGS) moveGenTest.o moveGen.o attackTables.o chessBoard.o -o moveGenTest

chessBoard.o: chessBoard.cpp chessBoard.hpp
	g++ $(CPPFLAGS) -c chessBoard.cpp -o chessBoard.o

attackTables.o: attackTables.cpp attackTables.hpp
	g++ $(CPPFLAGS) -c attackTables.cpp -o attackTables.o

moveGenTest.o: moveGenTest.cpp
	g++ $(CPPFLAGS) -c moveGenTest.cpp -o moveGenTest.o

moveGen.o: moveGen.cpp moveGen.hpp
	g++ $(CPPFLAGS) -c moveGen.cpp -o moveGen.o

clean:
	del *.o moveGenTest.exe