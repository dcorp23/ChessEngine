#include "moveGen.hpp"
#include "evaluation.hpp"
#include "chessBoard.hpp"
#include "attackTables.hpp"
#include "moveGen.hpp"
#include "search.hpp"
#include <mutex>
#include <future>

std::vector<std::future<void>> futures;

int main(void) {
    AttackTables::initAttackTables();
    Evaluation::initEvaluation();
    int playerSide;
    std::cout << "Pick a side: white = 1, black = 0\n";
    std::cin >> playerSide;
    Board board = Board(startingFEN);

    while (!board.state.checkMate) {
        std::cout << (board.state.whiteToMove ? "\nWhite to move\n" : "\nBlack to move\n");
        board.printBoard();
        std::vector<Board> boards = MoveGenerator::getAllLegalBoards(board);
        if (boards.size() == 0) break;
        if (board.state.whiteToMove == playerSide) {
            int piece;
            std::cout << "\nWhat piece do you want to move?\n";
            std::cout << "pawn = 0, bishop = 1, knight = 2, rook = 3, queen = 4, king = 5\n";
            std::cin >> piece;

            
            std::string startingSquareString;
            std::cout << "Starting square? \n";
            std::cin >> startingSquareString;
            int startingSquare = chessNotationToInt(startingSquareString);
            std::cout << "startSquare: " << startingSquare << '\n';

            MoveCode moveCode;
            std::vector<MoveCode> moveList;
            if (piece == 0) MoveGenerator::getPawnMoves(board, &moveList);
            if (piece == 1) MoveGenerator::getBishopMoves(board, &moveList);
            if (piece == 2) MoveGenerator::getKnightMoves(board, &moveList);
            if (piece == 3) MoveGenerator::getRookMoves(board, &moveList);
            if (piece == 4) MoveGenerator::getQueenMoves(board, &moveList);
            if (piece == 5) MoveGenerator::getKingMoves(board, &moveList);

            std::vector<MoveCode> filteredMoveList;
            for (int i = 0; i < moveList.size(); i++) {
                if (moveList.at(i).startSquare == startingSquare) {
                    filteredMoveList.push_back(moveList.at(i));
                }
            }

            if (filteredMoveList.size() == 0) continue;

            for (int i = 0; i < filteredMoveList.size(); i++) {
                std::cout << i << ".";
                filteredMoveList.at(i).printCode();
            }

            int moveIndex;
            std::cout << "give an index to move\n";
            std::cin >> moveIndex;

            Board newBoard = board.move(filteredMoveList.at(moveIndex));
            for (int i = 0; i < boards.size(); i++) {
                if (newBoard.isEqual(boards.at(i))) {
                    board = boards.at(i);
                    break;
                }
            }
        }
        else {
            Board nextBoard;
            int bestIndex = 0;
            float bestEval = board.state.whiteToMove ? -INFINITY : INFINITY;
            float alpha = -INFINITY;
            float beta = INFINITY;
            
            int boardsSize = boards.size();
            for (int i = 0; i < boardsSize; i++) {
                if (futures.size() < NUM_THREADS) {
                    futures.push_back(std::async(std::launch::async, Search::threadMinMax, &boards.at(i), i, &bestIndex, &bestEval, MAX_DEPTH, &alpha, &beta));
                }
                else {
                    //waits while the max number of threads are active
                    int j = 0;
                    while (true) {
                        //checks if a thread has finnished then if it has then break out and start a new thread
                        if (futures.at(j).wait_for(std::chrono::milliseconds(500)) == std::future_status::ready) {
                            futures.erase(futures.begin() + j);
                            break;
                        }
                        j++;
                        if (j == futures.size()) j = 0;
                    }
                    i--;
                }
            }

            //wait for threads to finish
            //while threads are still active
            int size = futures.size();
            while (size != 0) {
                for (int i = 0; i < futures.size(); i++) { //loop through threads
                    //if a thread is done remove it
                    if (futures.at(i).wait_for(std::chrono::milliseconds(500)) == std::future_status::ready) {
                        futures.erase(futures.begin() + i);
                        size--;
                    }
                }
            }
            
            //get the best best board
            nextBoard = boards.at(bestIndex);
            std::cout << "\nEvaluation: " << bestEval << '\n';
            board = nextBoard;
        }
    }

    std::cout << "\nGame Over: \n";
    board.printBoard();
    return 0;
}