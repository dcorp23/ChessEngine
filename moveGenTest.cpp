#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include "bitFunctions.hpp"
#include "attackTables.hpp"
#include "chessBoard.hpp"
#include "moveGen.hpp"

//each depth is 1 move for 1 side so depth 6 would total 3 moves white 3 moves black
//boards is a pointer to a vector with 1 starting baord
std::vector<Board> timeBoardsAtDepth(std::vector<Board> boards, int depth) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    if (depth == 0) {
        return boards;
    } else {
        Board currentBoard = Board();
        std::vector<Board> newBoards; //new boards generated from current board
        std::vector<Board> allNewBoards; //all boards generated
        int numberOfBoards = boards.size();
        for (int boardIndex = 0; boardIndex < numberOfBoards; boardIndex++) {
            currentBoard = boards.at(boardIndex);
            if (currentBoard.state.checkMate != 1) {
                newBoards = MoveGenerator::getAllLegalBoards(currentBoard);
                allNewBoards.insert(allNewBoards.end(), newBoards.begin(), newBoards.end());
            }
        }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << allNewBoards.size() << " Boards Found\n";
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
        return timeBoardsAtDepth(allNewBoards, depth - 1);
    }
}

bool testForCorrectBoards(std::vector<Board> actual, std::vector<Board> expected, int expectedSize) {
    if (actual.size() != expectedSize) {
        std::cout << "Acutal Size: " << actual.size() << "Expected Size: " << expectedSize << '\n';
        return false;
    };
    int sampleSize = expected.size();
    int actualSize = actual.size();
    for (int i = 0; i < sampleSize; i++) {
        bool equal = false;
        for (int j = 0; j < actualSize; j++) {
            if (expected.at(i) == actual.at(j)) {
                equal = true;
                break;
            }
        }
        if (equal) continue;
        std::cout << "Board not found:\n";
        expected.at(i).printBoard();
        return false;
    }
    return true;
}

void testPawnMoves() {
    std::cout << "Test Pawn Moves: 16 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board> newBoards;
    std::vector<Board> testBoards;

    std::cout << "White Pawns: \n";

    //pawns on starting square
    board = Board("8/8/8/8/8/8/PPPPPPPP/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/8/8/P7/1PPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/P7/8/1PPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/1P6/P1PPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/1P6/8/P1PPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/2P5/PP1PPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/2P5/8/PP1PPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/3P4/PPP1PPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/3P4/8/PPP1PPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/4P3/PPPP1PPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/4P3/8/PPPP1PPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/5P2/PPPPP1PP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/5P2/8/PPPPP1PP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/6P1/PPPPPP1P/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/6P1/8/PPPPPP1P/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/7P/PPPPPPP1/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/7P/8/PPPPPPP1/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 16));
    testBoards.clear();
    std::cout << "Checkpoint #1\n";

    //pawns on starting square knights blocking pushing 2 squares
    board = Board("8/8/8/8/nnnnnnnn/8/PPPPPPPP/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/P7/1PPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/1P6/P1PPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/2P5/PP1PPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/3P4/PPP1PPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/4P3/PPPP1PPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/5P2/PPPPP1PP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/6P1/PPPPPP1P/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/nnnnnnnn/7P/PPPPPPP1/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 8));
    testBoards.clear();
    std::cout << "Checkpoint #2\n";

    //2 rows of pawns
    board = Board("8/8/8/8/8/PPPPPPPP/PPPPPPPP/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/8/P7/1PPPPPPP/PPPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/1P6/P1PPPPPP/PPPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/2P5/PP1PPPPP/PPPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/3P4/PPP1PPPP/PPPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/4P3/PPPP1PPP/PPPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/5P2/PPPPP1PP/PPPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/6P1/PPPPPP1P/PPPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/7P/PPPPPPP1/PPPPPPPP/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 8));
    testBoards.clear();
    std::cout << "Checkpoint #3\n";

    //pawns on starting squares with knights in front of them
    board = Board("8/8/8/8/8/nnnnnnnn/PPPPPPPP/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/8/8/nPnnnnnn/1PPPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/Pnnnnnnn/P1PPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnPnnnnn/P1PPPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nPnnnnnn/PP1PPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnPnnnn/PP1PPPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnPnnnnn/PPP1PPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnnPnnn/PPP1PPPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnPnnnn/PPPP1PPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnnnPnn/PPPP1PPP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnnPnnn/PPPPP1PP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnnnnPn/PPPPP1PP/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnnnPnn/PPPPPP1P/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnnnnnP/PPPPPP1P/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/nnnnnnPn/PPPPPPP1/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 14));
    testBoards.clear();
    std::cout << "Checkpoint #4\n";

    //8 pawns 1 square from promotion
    board = Board("8/PPPPPPPP/8/8/8/8/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("Q7/1PPPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("B7/1PPPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("R7/1PPPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("1Q6/P1PPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("1B6/P1PPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("1R6/P1PPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("3R4/PPP1PPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("5N2/PPPPP1PP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("7Q/PPPPPPP1/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("7R/PPPPPPP1/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("7N/PPPPPPP1/8/8/8/8/8/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 32));
    testBoards.clear();
    std::cout << "Checkpoint #5\n";

    //8 pawns 1 square from promotion with a knights in front of them
    board = Board("nnnnnnnn/PPPPPPPP/8/8/8/8/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("nQnnnnnn/1PPPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nBnnnnnn/1PPPPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nQnnnnnn/PP1PPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nBnnnnnn/PP1PPPPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nnnRnnnn/PPPP1PPP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nnnnnnBn/PPPPPPP1/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nnnnQnnn/PPPPP1PP/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nnnnnnnN/PPPPPP1P/8/8/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("nnnnnQnn/PPPPPP1P/8/8/8/8/8/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 56));
    testBoards.clear();
    std::cout << "Checkpoint #6\n";

    //random captures
    board = Board("5nn1/5P2/2nn4/1n1Pn1n1/1P3P2/8/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("5nn1/5P2/2nn4/1n1PnPn1/1P6/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("5nB1/8/2nn4/1n1Pn1n1/1P3P2/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("5nn1/5P2/2Pn4/1n2n1n1/1P3P2/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("5nn1/5P2/2nn4/1n1Pn1P1/1P6/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("5nn1/5P2/2nn4/1n1PP1n1/1P6/8/8/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 8));
    testBoards.clear();
    std::cout << "Checkpoint #7\n";

    //en passant
    board = Board("8/8/5P2/3PpP2/8/8/8/8 w - e6 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/4PP2/5P2/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/3P1P2/4pP2/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("8/5P2/8/3PpP2/8/8/8/8 b - - 0 1"));
        testBoards.push_back(Board("8/8/4PP2/3P4/8/8/8/8 b - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 4));
    testBoards.clear();
    std::cout << "Checkpoint #8\n";

    std::cout << "\nBlack Pawns: \n";

    //pawns on starting square
    board = Board("8/pppppppp/8/8/8/8/8/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/1ppppppp/8/p7/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/ppppppp1/8/7p/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/ppppppp1/7p/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/1ppppppp/p7/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/ppp1pppp/3p4/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/ppppp1pp/8/5p2/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pp1ppppp/8/2p5/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppp1p/6p1/8/8/8/8/8 w - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 16));
    testBoards.clear();
    std::cout << "Checkpoint #9\n";

    //pawns on starting square knights blocking pushing 2 squares
    board = Board("8/pppppppp/8/NNNNNNNN/8/8/8/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/ppppppp1/7p/NNNNNNNN/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/1ppppppp/p7/NNNNNNNN/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/ppp1pppp/3p4/NNNNNNNN/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppp1p/6p1/NNNNNNNN/8/8/8/8 w - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 8));
    testBoards.clear();
    std::cout << "Checkpoint #10\n";

    //2 rows of pawns
    board = Board("8/pppppppp/pppppppp/8/8/8/8/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/pppppppp/1ppppppp/p7/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppppp/ppppppp1/7p/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppppp/ppp1pppp/3p4/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppppp/p1pppppp/1p6/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppppp/ppp1pppp/3p4/8/8/8/8 w - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 8));
    testBoards.clear();
    std::cout << "Checkpoint #11\n";

    //pawns on starting squares with knights in front of them
    board = Board("8/pppppppp/NNNNNNNN/8/8/8/8/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/pp1ppppp/NNNpNNNN/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/p1pppppp/pNNNNNNN/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/1ppppppp/NpNNNNNN/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/ppppppp1/NNNNNNpN/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppp1p/NNNNNNNp/8/8/8/8/8 w - - 0 1"));
        testBoards.push_back(Board("8/pppppp1p/NNNNNpNN/8/8/8/8/8 w - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 14));
    testBoards.clear();
    std::cout << "Checkpoint #12\n";

    //8 pawns 1 square from promotion
    board = Board("8/8/8/8/8/8/pppppppp/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/8/8/8/ppp1pppp/3b4 w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/1ppppppp/q7 w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/ppppppp1/7q w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/ppppppp1/7n w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/ppp1pppp/3b4 w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/pp1ppppp/2r5 w - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 32));
    testBoards.clear();
    std::cout << "Checkpoint #13\n";

    //8 pawns 1 square from promotion with a knights in front of them
    board = Board("8/8/8/8/8/8/pppppppp/NNNNNNNN b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/8/8/8/1ppppppp/NqNNNNNN w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/p1pppppp/bNNNNNNN w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/pppppp1p/NNNNNNNn w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/pppppp1p/NNNNNNNr w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/pppp1ppp/NNNNNrNN w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/pppppp1p/NNNNNnNN w - - 0 1"));
        testBoards.push_back(Board("8/8/8/8/8/8/ppp1pppp/NNbNNNNN w - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 14 * 4));
    testBoards.clear();
    std::cout << "Checkpoint #14\n";

    //random captures
    board = Board("8/8/8/2p3p1/1N1Np1N1/4NN2/2p5/1NN5 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/6p1/1NpNp1N1/4NN2/2p5/1NN5 w - - 0 1"));
        testBoards.push_back(Board("8/8/8/6p1/1p1Np1N1/4NN2/2p5/1NN5 w - - 0 1"));
        testBoards.push_back(Board("8/8/8/6p1/1N1pp1N1/4NN2/2p5/1NN5 w - - 0 1"));
        testBoards.push_back(Board("8/8/8/2p3p1/1N1Np1N1/4NN2/8/1rN5 w - - 0 1"));
        testBoards.push_back(Board("8/8/8/2p3p1/1N1N2N1/4Np2/2p5/1NN5 w - - 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 8));
    testBoards.clear();
    std::cout << "Checkpoint #15\n";

    //en passant
    board = Board("8/8/8/8/2pPp3/2p5/8/8 b - d3 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    {//push the expected boards to the test boards
        testBoards.push_back(Board("8/8/8/8/2pP4/2p1p3/8/8 w - d3 0 1"));
        testBoards.push_back(Board("8/8/8/8/2pPp3/8/2p5/8 w - d3 0 1"));
        testBoards.push_back(Board("8/8/8/8/4p3/2pp4/8/8 w - d3 0 1"));
        testBoards.push_back(Board("8/8/8/8/2p5/2pp4/8/8 w - d3 0 1"));
    }
    assert(true == testForCorrectBoards(newBoards, testBoards, 4));
    testBoards.clear();
    std::cout << "Checkpoint #16\n";

    std::cout << "Pawn Tests Passed\n\n";
}

void testBishopMoves() {
    std::cout << "Test Bishop Moves: 5 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board> newBoards;

    //bishops on starting square
    board = Board("8/8/8/8/8/8/8/2B2B2 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 14);
    std::cout << "Checkpoint #1\n";

    //bishops blocked in center
    board = Board("8/8/2PPPP2/2PBBP2/2PBBP2/2PPPP2/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #2\n";

    //bishops randomly blocked
    board = Board("6B1/6P1/3P4/1P6/BBBB4/1P2P3/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 27);
    std::cout << "Checkpoint #3\n";

    //bishops captures
    board = Board("8/8/2pppp2/2pBBp2/2pBBp2/2pppp2/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 12);
    std::cout << "Checkpoint #4\n";

    //random bishops captures
    board = Board("8/3p3p/1p6/2B2B2/1n4n1/3p4/5n2/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 15);
    std::cout << "Checkpoint #5\n";
    
    std::cout << "Bishop Tests Passed\n\n";
}

void testKnightMoves() {
    std::cout << "Test Knight Moves: 7 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board> newBoards;

    //knights on starting squares
    board = Board("8/8/8/8/8/8/8/1N4N1 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 6);
    std::cout << "Checkpoint #1\n";

    //knight with all squares blocked by team pawns and they can't move
    board = Board("2n1n3/1nP1Pn2/1P3P2/1n1N1n2/1Pn1nP2/2P1P3/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 0);
    std::cout << "Checkpoint #2\n";

    //knight with all squares blocked by enemy pawns
    board = Board("8/2p1p3/1p3p2/3N4/1p3p2/2p1p3/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #3\n";

    //knight with both squares taken by team pawns and enemy pawns
    board = Board("4n1n1/4P1Pn/3p3P/5N1n/3p3P/4p1p1/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #4\n";

    //black knight blocked by all team pawns
    board = Board("8/2p1p3/1pN1Np2/1N1n1N2/1p3p2/1Np1pN2/2N1N3/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 0);
    std::cout << "Checkpoint #5\n";

    //8 white knights in a row
    board = Board("8/8/8/8/NNNNNNNN/8/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 52);
    std::cout << "Checkpoint #6\n";

    //8 black knights in a row
    board = Board("8/8/8/nnnnnnnn/8/8/8/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 52);
    std::cout << "Checkpoint #7\n";
    
    std::cout << "Knight Tests Passed\n\n";
}

void testRookMoves() {
    std::cout << "Test Rook Moves: 5 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board> newBoards;

    //Rooks on their starting squares
    board = Board("8/8/8/8/8/8/8/R6R w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 26);
    std::cout << "Checkpoint #1\n";

    //Rooks blocked in the center by pawns
    board = Board("8/8/2PPPP2/2PRRP2/2PRRP2/2PPPP2/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #2\n";

    //Rooks blocked by enemy knights to test captures
    board = Board("8/8/4n3/8/2n1R2n/4n3/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #3\n";

    //Random blocks and captures for white rooks
    board = Board("7n/2n2R1P/2P5/8/2R2q2/2R5/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 23);
    std::cout << "Checkpoint #4\n";

    //Random blocks and captures for black rooks
    board = Board("6N1/3Q2r1/8/6N1/1N1r4/3p4/3N2r1/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 24);
    std::cout << "Checkpoint #5\n";
    
    std::cout << "Rook Tests Passed\n\n";
}

void testQueenMoves() {
    std::cout << "Test Queen Moves: 6 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board> newBoards;

    //Queen on starting square
    board = Board("8/8/8/8/8/8/8/3Q4 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 21);
    std::cout << "Checkpoint #1\n";

    //Queens in the center blocked by team pawns
    board = Board("8/8/2PPPP2/2PQQP2/2PQQP2/2PPPP2/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 4);
    std::cout << "Checkpoint #2\n";

    //Queens in the center blocked by enemy knights
    board = Board("8/8/2nnnn2/2nQQn2/2nQQn2/2nnnn2/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 20);
    std::cout << "Checkpoint #3\n";

    //White queens randomly blocked by team and enemy pieces
    board = Board("5n2/2n1nPn1/1n4P1/1P2QQ2/4PP2/1n6/1P6/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 18);
    std::cout << "Checkpoint #4\n";

    //Black queens randomly blocked by team and enemy pieces
    board = Board("8/8/N2q2p1/6P1/1N1p4/3N2q1/8/q5N1 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 43);
    std::cout << "Checkpoint #5\n";

    //Queens on the long dark square diagonal
    board = Board("7Q/6Q1/5Q2/4Q3/3Q4/2Q5/1Q6/Q7 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 136);
    std::cout << "Checkpoint #6\n";
    
    std::cout << "Queen Tests Passed\n\n";
}

void testKingMoves() {
    std::cout << "Test King Moves: 10 Checkpoints\n";
    Board board;
    int numberOfBoards;
    std::vector<Board> newBoards;

    //King on the starting square
    board = Board("8/8/8/8/8/8/8/4K3 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 5);
    std::cout << "Checkpoint #1\n";

    //King in the center
    board = Board("8/8/8/4K3/8/8/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #2\n";

    //King in the center blocked by team pawns
    board = Board("8/8/3PPP2/3PKP2/3PPP2/8/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 3);
    std::cout << "Checkpoint #3\n";

    //King in the center blocked by 4 enemy kings
    board = Board("8/8/4k3/8/2k1K1k1/8/4k3/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 0);
    std::cout << "Checkpoint #4\n";

    //King with three enemy pawns behind him
    board = Board("8/8/8/8/2K5/1ppp4/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 8);
    std::cout << "Checkpoint #5\n";

    //Black King trapped by rooks in stalemate
    board = Board("3R1R2/8/8/7R/4k3/7R/8/8 b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 0);
    std::cout << "Checkpoint #6\n";

    //White king castle long and short
    board = Board("8/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 26);
    std::cout << "Checkpoint #7\n";

    //Black king castle long and short
    board = Board("r3k2r/8/8/8/8/8/8/8 b kq - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 26);
    std::cout << "Checkpoint #8\n";

    //White king castle blocked by rooks attacking squares next to him
    board = Board("3r1r2/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 20);
    std::cout << "Checkpoint #9\n";

    //Black king castle blocked by rooks attacking squares next to him
    board = Board("r3k2r/8/8/8/8/8/8/3R1R2 b kq - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfBoards = newBoards.size();
    assert(numberOfBoards == 20);
    std::cout << "Checkpoint #10\n";
    
    std::cout << "King Tests Passed\n\n";
}

void testChecks() {
    std::cout << "Testing Checks: 6 Checkpoints\n";
    Board board;
    int numberOfChecks;
    std::vector<Board> newBoards;

    //3 rooks with 2 checks each
    board = Board("7k/8/8/8/8/R7/1R6/2R5 w - d3 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfChecks = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.check == 1) numberOfChecks++;
    }
    assert(numberOfChecks == 6);
    std::cout << "Checkpoint #1\n";

    //all the pieces have a few checks each
    board = Board("6N1/8/8/3k4/8/4P3/2P4R/1Q3B2 w - d3 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfChecks = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.check == 1) numberOfChecks++;
    }
    assert(numberOfChecks == 13);
    std::cout << "Checkpoint #2\n";

    //discovered checks
    board = Board("8/8/3k1P1R/8/5N2/3N4/7B/3R4 w - d3 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfChecks = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.check == 1) numberOfChecks++;
    }
    assert(numberOfChecks == 15);
    std::cout << "Checkpoint #3\n";

    //knight promotions
    board = Board("8/1P1k1P2/8/8/8/8/8/8 w - d3 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfChecks = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.check == 1) numberOfChecks++;
    }
    assert(numberOfChecks == 2);
    std::cout << "Checkpoint #4\n";

    //rook and queen promotions
    board = Board("3k4/1P3P2/8/8/8/8/8/8 w - d3 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfChecks = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.check == 1) numberOfChecks++;
    }
    assert(numberOfChecks == 4);
    std::cout << "Checkpoint #5\n";

    //long castle check
    board = Board("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfChecks = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.check == 1) numberOfChecks++;
    }
    assert(numberOfChecks == 3);
    std::cout << "Checkpoint #6\n";

    std::cout << "Check Tests Passed\n\n";
}

void testCheckmates() {
    std::cout << "Testing Checkmate: 5 Checkpoints\n";
    Board board;
    int numberOfCheckmates;
    std::vector<Board> newBoards;

    //5 rooks each with mate in 1
    board = Board("8/5K1k/8/R7/R7/R7/R7/R7 w - d3 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfCheckmates = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.checkMate == 1) numberOfCheckmates++;
    }
    assert(numberOfCheckmates == 5);
    std::cout << "Checkpoint #1\n";

    //queen with 3 checks that are not checkmate
    board = Board("8/8/8/2ppp2Q/2pk4/2ppp3/8/8 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfCheckmates = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.checkMate == 1) numberOfCheckmates++;
    }
    assert(numberOfCheckmates == 0);
    std::cout << "Checkpoint #2\n";

    //3 smothered mates in the corner
    board = Board("8/8/8/8/6n1/3n4/6PP/3n2RK b - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfCheckmates = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.checkMate == 1) numberOfCheckmates++;
    }
    assert(numberOfCheckmates == 3);
    std::cout << "Checkpoint #3\n";

    //that one opening with a smothered mate in the middle of the board
    board = Board("r1b1kbnr/pppp1Npp/8/8/3nq3/8/PPPPBP1P/RNBQKR2 b Qkq - 1 7");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfCheckmates = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.checkMate == 1) numberOfCheckmates++;
    }
    assert(numberOfCheckmates == 1);
    std::cout << "Checkpoint #4\n";

    //1B1Q1Q2/2R5/pQ4QN/RB2k3/1Q5Q/N4Q2/K2Q4/6Q1
    //puzzle with a bunch of mate in 1s
    board = Board("1B1Q1Q2/2R5/pQ4QN/RB2k3/1Q5Q/N4Q2/K2Q4/6Q1 w - - 0 1");
    newBoards = MoveGenerator::getAllLegalBoards(board);
    numberOfCheckmates = 0;
    for (int i = 0; i < newBoards.size(); i++) {
        if (newBoards.at(i).state.checkMate == 1) numberOfCheckmates++;
    }
    assert(numberOfCheckmates == 105);
    std::cout << "Checkpoint #5\n";

    std::cout << "Checkmate Tests Passed\n\n";
}

void testNumberOfBoardsAtDepth5() {
    Board board = Board(startingFEN);
    int numberOfBoards;
    int numberOfChecks;
    int numberOfCheckmates;

    const int depth1Boards = 20;
    const int depth1Checks = 0;
    const int depth1Checkmates = 0;
    std::vector<Board> boards;
    boards.push_back(board);

    boards = timeBoardsAtDepth(boards, 1);
    numberOfBoards = boards.size();
    numberOfChecks = 0;
    numberOfCheckmates = 0;
    for (int i = 0; i < numberOfBoards; i++) {
        if (boards.at(i).state.check) numberOfChecks++;
        if (boards.at(i).state.checkMate) numberOfCheckmates++;
    }

    assert(depth1Boards == numberOfBoards);
    std::cout << "Depth 1 number of boards correct\n";
    assert(depth1Checks == numberOfChecks);
    std::cout << "Depth 1 number of checks correct\n";
    assert(depth1Checkmates == numberOfCheckmates);
    std::cout << "Depth 1 number of checkmates correct\n";
    std::cout << '\n';

    const int depth2Boards = 400;
    const int depth2Checks = 0;
    const int depth2Checkmates = 0;

    boards = timeBoardsAtDepth(boards, 1);
    numberOfBoards = boards.size();
    numberOfChecks = 0;
    numberOfCheckmates = 0;
    for (int i = 0; i < numberOfBoards; i++) {
        if (boards.at(i).state.check) numberOfChecks++;
        if (boards.at(i).state.checkMate) numberOfCheckmates++;
    }

    assert(depth2Boards == numberOfBoards);
    std::cout << "Depth 2 number of boards correct\n";
    assert(depth2Checks == numberOfChecks);
    std::cout << "Depth 2 number of checks correct\n";
    assert(depth2Checkmates == numberOfCheckmates);
    std::cout << "Depth 2 number of checkmates correct\n";
    std::cout << '\n';

    const int depth3Boards = 8902;
    const int depth3Checks = 12;
    const int depth3Checkmates = 0;

    boards = timeBoardsAtDepth(boards, 1);
    numberOfBoards = boards.size();
    numberOfChecks = 0;
    numberOfCheckmates = 0;
    for (int i = 0; i < numberOfBoards; i++) {
        if (boards.at(i).state.check) numberOfChecks++;
        if (boards.at(i).state.checkMate) numberOfCheckmates++;
    }
    
    assert(depth3Boards == numberOfBoards);
    std::cout << "Depth 3 number of boards correct\n";
    assert(depth3Checks == numberOfChecks);
    std::cout << "Depth 3 number of checks correct\n";
    assert(depth3Checkmates == numberOfCheckmates);
    std::cout << "Depth 3 number of checkmates correct\n";
    std::cout << '\n';

    const int depth4Boards = 197281;
    const int depth4Checks = 469;
    const int depth4Checkmates = 8;

    boards = timeBoardsAtDepth(boards, 1);
    numberOfBoards = boards.size();
    numberOfChecks = 0;
    numberOfCheckmates = 0;
    for (int i = 0; i < numberOfBoards; i++) {
        if (boards.at(i).state.check) numberOfChecks++;
        if (boards.at(i).state.checkMate) numberOfCheckmates++;
    }

    assert(depth4Boards == numberOfBoards);
    std::cout << "Depth 4 number of boards correct\n";
    assert(depth4Checks == numberOfChecks);
    std::cout << "Depth 4 number of checks correct\n";
    assert(depth4Checkmates == numberOfCheckmates);
    std::cout << "Depth 4 number of checkmates correct\n";
    std::cout << '\n';

    const int depth5Boards = 4865609;
    const int depth5Checks = 27351;
    const int depth5Checkmates = 347;

    boards = timeBoardsAtDepth(boards, 1);
    numberOfBoards = boards.size();
    numberOfChecks = 0;
    numberOfCheckmates = 0;
    for (int i = 0; i < numberOfBoards; i++) {
        if (boards.at(i).state.check) numberOfChecks++;
        if (boards.at(i).state.checkMate) {
            if (numberOfCheckmates > 500 && numberOfCheckmates < 600) {
                boards.at(i).printBoard();
                std::cout << '\n';
            }
            numberOfCheckmates++;
        }
    }
    
    assert(depth5Boards == numberOfBoards);
    std::cout << "Depth 5 number of boards correct\n";
    assert(depth5Checks == numberOfChecks);
    std::cout << "Depth 5 number of checks correct\n";
    assert(depth5Checkmates == numberOfCheckmates);
    std::cout << "Depth 5 number of checkmates correct\n";
    std::cout << '\n';
}

int main(void) {
    AttackTables::initAttackTables();
    std::cout << "Move Generation Tests: \n";
    testPawnMoves();
    testBishopMoves();
    testKnightMoves();
    testRookMoves();
    testQueenMoves();
    testKingMoves();
    testChecks();
    testCheckmates();
    testNumberOfBoardsAtDepth5();
    
    return 0;
}