#include "moveGen.hpp"
#include "evaluation.hpp"
#include "chessBoard.hpp"
#include "attackTables.hpp"
#include "moveGen.hpp"
#define INFINITY 99999999

float minimax(Board board, int depth, bool isMaximizingPlayer, float alpha, float beta) {
    if (depth == 0) return Evaluation::evaluate(board);
    
    std::vector<Board>* boards = MoveGenerator::getAllLegalBoards(board);
    int boardsSize = boards->size();

    if (isMaximizingPlayer)  {
        float bestVal = -INFINITY;
        for (int i = 0; i < boardsSize; i++)  {
            float value = minimax(boards->at(i), depth - 1, false, alpha, beta);
            bestVal = std::max( bestVal, value);
            alpha = std::max( alpha, bestVal);
            if (beta <= alpha) break;
        }
        delete boards;
        return bestVal;
    }
    else {
        float bestVal = INFINITY;
        for (int i = 0; i < boardsSize; i++) {
            float value = minimax(boards->at(i), depth - 1, true, alpha, beta);
            bestVal = std::min( bestVal, value);
            beta = std::min( beta, bestVal);
            if (beta <= alpha) break;
        }
        delete boards;
        return bestVal;
    }
}


int main(void) {
    AttackTables::initAttackTables();
    Board board = Board("r2qkb1r/pp3ppp/2n1pn2/3p3b/3P1B2/2PB1N1P/PP3PP1/RN1QK2R w KQkq - 0 9");
    std::cout << (board.state.whiteToMove ? "White to move\n" : "Black to move\n");
    board.printBoard();
    
    float eval;
    float maxEval = -INFINITY;
    Board nextBoard;

    std::vector<Board>* boards = MoveGenerator::getAllLegalBoards(board);
    int boardsSize = boards->size();
    for (int i = 0; i < boardsSize; i++) {
        eval = minimax(boards->at(i), 3, false, -INFINITY, INFINITY);
        if (eval > maxEval) {
            nextBoard = boards->at(i);
            maxEval = eval;
        }
    }

    std::cout << '\n';
    nextBoard.printBoard();
    std::cout << "\nEvaluation: " << maxEval << '\n';

    return 0;
}