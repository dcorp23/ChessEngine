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
    std::cout << "Test Bishop Moves: 5 Checkpoints\n";
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

void testKnightMoves() {
    std::cout << "Test Knight Moves: 7 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board>* newBoards;

    //knights on starting squares
    board = Board("8/8/8/8/8/8/8/1N4N1 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 6);
    std::cout << "Checkpoint #1\n";

    //knight with all squares blocked by team pawns and they can't move
    board = Board("2n1n3/1nP1Pn2/1P3P2/1n1N1n2/1Pn1nP2/2P1P3/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 0);
    std::cout << "Checkpoint #2\n";

    //knight with all squares blocked by enemy pawns
    board = Board("8/2p1p3/1p3p2/3N4/1p3p2/2p1p3/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #3\n";

    //knight with both squares taken by team pawns and enemy pawns
    board = Board("4n1n1/4P1Pn/3p3P/5N1n/3p3P/4p1p1/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #4\n";

    //black knight blocked by all team pawns
    board = Board("8/2p1p3/1pN1Np2/1N1n1N2/1p3p2/1Np1pN2/2N1N3/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 0);
    std::cout << "Checkpoint #5\n";

    //8 white knights in a row
    board = Board("8/8/8/8/NNNNNNNN/8/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 52);
    std::cout << "Checkpoint #6\n";

    //8 black knights in a row
    board = Board("8/8/8/nnnnnnnn/8/8/8/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 52);
    std::cout << "Checkpoint #7\n";
    
    std::cout << "Knight Tests Passed\n\n";
    delete newBoards;
}

void testRookMoves() {
    std::cout << "Test Rook Moves: 5 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board>* newBoards;

    //Rooks on their starting squares
    board = Board("8/8/8/8/8/8/8/R6R w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 26);
    std::cout << "Checkpoint #1\n";

    //Rooks blocked in the center by pawns
    board = Board("8/8/2PPPP2/2PRRP2/2PRRP2/2PPPP2/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #2\n";

    //Rooks blocked by enemy knights to test captures
    board = Board("8/8/4n3/8/2n1R2n/4n3/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #3\n";

    //Random blocks and captures for white rooks
    board = Board("7n/2n2R1P/2P5/8/2R2q2/2R5/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 23);
    std::cout << "Checkpoint #4\n";

    //Random blocks and captures for black rooks
    board = Board("6N1/3Q2r1/8/6N1/1N1r4/3p4/3N2r1/8 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 24);
    std::cout << "Checkpoint #5\n";
    
    std::cout << "Rook Tests Passed\n\n";
    delete newBoards;
}

void testQueenMoves() {
    std::cout << "Test Queen Moves: 6 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board>* newBoards;

    //Queen on starting square
    board = Board("8/8/8/8/8/8/8/3Q4 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 21);
    std::cout << "Checkpoint #1\n";

    //Queens in the center blocked by team pawns
    board = Board("8/8/2PPPP2/2PQQP2/2PQQP2/2PPPP2/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #2\n";

    //Queens in the center blocked by enemy knights
    board = Board("8/8/2nnnn2/2nQQn2/2nQQn2/2nnnn2/8/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 20);
    std::cout << "Checkpoint #3\n";

    //White queens randomly blocked by team and enemy pieces
    board = Board("5n2/2n1nPn1/1n4P1/1P2QQ2/4PP2/1n6/1P6/8 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 18);
    std::cout << "Checkpoint #4\n";

    //Black queens randomly blocked by team and enemy pieces
    board = Board("8/8/N2q2p1/6P1/1N1p4/3N2q1/8/q5N1 b - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 43);
    std::cout << "Checkpoint #5\n";

    //Queens on the long dark square diagonal
    board = Board("7Q/6Q1/5Q2/4Q3/3Q4/2Q5/1Q6/Q7 w - - 0 1");
    newBoards = moveGen.getAllLegalBoards(board);
    numberOfBoards = newBoards->size();
    assert(numberOfBoards == 136);
    std::cout << "Checkpoint #6\n";
    
    std::cout << "Queen Tests Passed\n\n";
    delete newBoards;
}

int main(void) {
    std::cout << "Move Generation Tests: \n";
    testBishopMoves();
    testKnightMoves();
    testRookMoves();
    testQueenMoves();
    return 0;
}