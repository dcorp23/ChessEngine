#include "moveGen.hpp"
#include "evaluation.hpp"
#include "chessBoard.hpp"
#include "attackTables.hpp"
#include "moveGen.hpp"
#include <chrono>
#include <mutex>
#include <future>
#include <queue>
#include <thread>
#define INFINITY 99999999
#define NUM_THREADS 4
#define MAX_DEPTH 5

static std::mutex evalMutex;
std::vector<std::future<void>> futures;

float minimax(Board* board, int depth, bool isMaximizingPlayer, float alpha, float beta) {
    if (depth == 0 || board->state.checkMate == 1) return Evaluation::evaluate(board);
    
    std::vector<Board> boards = MoveGenerator::getAllLegalBoards(*board);
    int boardsSize = boards.size();

    if (isMaximizingPlayer)  {
        float bestVal = -INFINITY;
        for (int i = 0; i < boardsSize; i++)  {
            float value = minimax(&boards.at(i), depth - 1, false, alpha, beta);
            bestVal = std::max( bestVal, value);
            alpha = std::max( alpha, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
    else {
        float bestVal = INFINITY;
        for (int i = 0; i < boardsSize; i++) {
            float value = minimax(&boards.at(i), depth - 1, true, alpha, beta);
            bestVal = std::min( bestVal, value);
            beta = std::min( beta, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
}

void threadMinMax(Board* board, int currentIndex, int* bestIndex, float* bestEval, int depth, float* alpha, float* beta) {
    float eval = minimax(board, depth, board->state.whiteToMove ? true : false, *alpha, *beta);
    std::lock_guard<std::mutex> lock(evalMutex);
    if (board->state.whiteToMove ? eval < *bestEval : eval > *bestEval) {
        *bestIndex = currentIndex;
        *bestEval = eval;
    }
}

int main(void) {
    AttackTables::initAttackTables();
    int playerSide;
    std::cout << "Pick a side: white = 1, black = 0\n";
    std::cin >> playerSide;
    Board board = Board(startingFEN);

    while (!board.state.checkMate) {
        std::cout << (board.state.whiteToMove ? "\nWhite to move\n" : "\nBlack to move\n");
        board.printBoard();
        std::vector<Board> boards = MoveGenerator::getAllLegalBoards(board);
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
                    futures.push_back(std::async(std::launch::async, threadMinMax, &boards.at(i), i, &bestIndex, &bestEval, MAX_DEPTH, &alpha, &beta));
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

    board.printBoard();
    return 0;
}