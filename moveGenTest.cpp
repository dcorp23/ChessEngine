#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include "moveGen.cpp"

static MoveGenerator moveGen = MoveGenerator();

//each depth is 1 move for 1 side so depth 6 would total 3 moves white 3 moves black
//boards is a pointer to a vector with 1 starting baord
std::vector<Board>* getBoardsAtDepth(std::vector<Board>* boards, int depth) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    if (depth == 0) {
        return boards;
    } else {
        Board currentBoard = Board();
        std::vector<Board>* newBoards = new std::vector<Board>; //new boards generated from current board
        std::vector<Board>* allNewBoards = new std::vector<Board>; //all boards generated
        int numberOfBoards = boards->size();
        for (int boardIndex = 0; boardIndex < numberOfBoards; boardIndex++) {
            currentBoard = boards->at(boardIndex);
            newBoards = moveGen.getAllLegalBoards(currentBoard);
            allNewBoards->insert(allNewBoards->end(), newBoards->begin(), newBoards->end());
            delete newBoards;
        }

        delete boards;

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Layer " << depth << " Completed" << '\n';
        std::cout << allNewBoards->size() << " Boards Found\n";
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
        return getBoardsAtDepth(allNewBoards, depth - 1);
    }
}

void testPawnMoves() {
    std::cout << "Test Pawn Moves: 16 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board>* newBoards;

    std::cout << "White Pawns: \n";

    //pawns on starting square
    board = Board("8/8/8/8/8/8/PPPPPPPP/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 16);
    std::cout << "Checkpoint #1\n";

    //pawns on starting square knights blocking pushing 2 squares
    board = Board("8/8/8/8/nnnnnnnn/8/PPPPPPPP/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #2\n";

    //2 rows of pawns
    board = Board("8/8/8/8/8/PPPPPPPP/PPPPPPPP/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #3\n";

    //pawns on starting squares with knights in front of them
    board = Board("8/8/8/8/8/nnnnnnnn/PPPPPPPP/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 14);
    std::cout << "Checkpoint #4\n";

    //8 pawns 1 square from promotion
    board = Board("8/PPPPPPPP/8/8/8/8/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 32);
    std::cout << "Checkpoint #5\n";

    //8 pawns 1 square from promotion with a knights in front of them
    board = Board("nnnnnnnn/PPPPPPPP/8/8/8/8/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 14 * 4);
    std::cout << "Checkpoint #6\n";

    //random captures
    board = Board("5nn1/5P2/2nn4/1n1Pn1n1/1P3P2/8/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #7\n";

    //en passant
    board = Board("8/8/8/3Pp3/8/8/8/8 w - e6 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 2);
    std::cout << "Checkpoint #8\n";

    //------------------------------
    //------------------------------
    std::cout << "\nBlack Pawns: \n";

    //pawns on starting square
    board = Board("8/pppppppp/8/8/8/8/8/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 16);
    std::cout << "Checkpoint #9\n";

    //pawns on starting square knights blocking pushing 2 squares
    board = Board("8/pppppppp/8/NNNNNNNN/8/8/8/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #10\n";

    //2 rows of pawns
    board = Board("8/pppppppp/pppppppp/8/8/8/8/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #11\n";

    //pawns on starting squares with knights in front of them
    board = Board("8/pppppppp/NNNNNNNN/8/8/8/8/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 14);
    std::cout << "Checkpoint #12\n";

    //8 pawns 1 square from promotion
    board = Board("8/8/8/8/8/8/pppppppp/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 32);
    std::cout << "Checkpoint #13\n";

    //8 pawns 1 square from promotion with a knights in front of them
    board = Board("8/8/8/8/8/8/pppppppp/NNNNNNNN b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 14 * 4);
    std::cout << "Checkpoint #14\n";

    //random captures
    board = Board("8/8/8/2p3p1/1N1Np1N1/4NN2/2p5/1NN5 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #15\n";

    //en passant
    board = Board("8/8/8/8/3Pp3/8/8/8 b - d3 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 2);
    std::cout << "Checkpoint #16\n";

    std::cout << "Pawn Tests Passed\n\n";
    delete newBoards;
}

void testBishopMoves() {
    std::cout << "Test Pawn Moves: 5 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board>* newBoards;

    //bishops on starting square
    board = Board("8/8/8/8/8/8/8/2B2B2 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 14);
    std::cout << "Checkpoint #1\n";

    //bishops blocked in center
    board = Board("8/8/2PPPP2/2PBBP2/2PBBP2/2PPPP2/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #2\n";

    //bishops randomly blocked
    board = Board("6B1/6P1/3P4/1P6/BBBB4/1P2P3/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 27);
    std::cout << "Checkpoint #3\n";

    //bishops captures
    board = Board("8/8/2pppp2/2pBBp2/2pBBp2/2pppp2/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 12);
    std::cout << "Checkpoint #4\n";

    //random bishops captures
    board = Board("8/3p3p/1p6/2B2B2/1n4n1/3p4/5n2/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 15);
    std::cout << "Checkpoint #5\n";
    
    std::cout << "Bishop Tests Passed\n\n";
    delete newBoards;
}

int main(void) {
    std::cout << "Move Generation Tests: \n";
    testBishopMoves();
    return 0;
}