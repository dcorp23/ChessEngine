#include "moveGen.hpp"
#include "evaluation.hpp"
#include "chessBoard.hpp"
#include "attackTables.hpp"
#include "moveGen.hpp"
#include <chrono>
#define INFINITY 99999999

float minimax(Board board, int depth, bool isMaximizingPlayer, float alpha, float beta) {
    if (depth == 0) return Evaluation::evaluate(board);
    
    std::vector<Board> boards = MoveGenerator::getAllLegalBoards(board);
    int boardsSize = boards.size();

    if (isMaximizingPlayer)  {
        float bestVal = -INFINITY;
        for (int i = 0; i < boardsSize; i++)  {
            float value = minimax(boards.at(i), depth - 1, false, alpha, beta);
            bestVal = std::max( bestVal, value);
            alpha = std::max( alpha, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
    else {
        float bestVal = INFINITY;
        for (int i = 0; i < boardsSize; i++) {
            float value = minimax(boards.at(i), depth - 1, true, alpha, beta);
            bestVal = std::min( bestVal, value);
            beta = std::min( beta, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
}

int main(void) {
    AttackTables::initAttackTables();
    int playerSide;
    std::cout << "Pick a side: white = 1, black = 0\n";
    std::cin >> playerSide;
    Board board = Board(startingFEN);
    std::cout << "checkmate = " << board.state.checkMate << '\n';

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
            if (piece == 0) moveList = MoveGenerator::getPawnMoves(board);
            if (piece == 1) moveList = MoveGenerator::getBishopMoves(board);
            if (piece == 2) moveList = MoveGenerator::getKnightMoves(board);
            if (piece == 3) moveList = MoveGenerator::getRookMoves(board);
            if (piece == 4) moveList = MoveGenerator::getQueenMoves(board);
            if (piece == 5) moveList = MoveGenerator::getKingMoves(board);

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
            float eval;
            float extreme = board.state.whiteToMove ? -INFINITY : INFINITY;
            Board nextBoard;

            int boardsSize = boards.size();
            for (int i = 0; i < boardsSize; i++) {
                eval = minimax(boards.at(i), 3, board.state.whiteToMove ? false : true, -INFINITY, INFINITY);
                if (board.state.whiteToMove ? eval > extreme : eval < extreme) {
                    nextBoard = boards.at(i);
                    extreme = eval;
                }
            }

            std::cout << "\nEvaluation: " << extreme << '\n';
            board = nextBoard;
        }
    }


    return 0;
}