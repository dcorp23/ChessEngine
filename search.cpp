#include <iostream>
#include "search.hpp"
#include "chessBoard.hpp"
#include "evaluation.hpp"
#include "moveGen.hpp"

static std::mutex evalMutex;

float Search::minimax(Board* board, int depth, bool isMaximizingPlayer, float alpha, float beta, EvaluationWeights weights) {
    if (depth == 0 || board->state.checkMate == 1) return Evaluation::evaluate(board, weights);
    
    std::vector<Board> boards = MoveGenerator::getAllLegalBoards(*board);
    int boardsSize = boards.size();

    if (isMaximizingPlayer)  {
        float bestVal = -INFINITY;
        for (int i = 0; i < boardsSize; i++)  {
            float value = minimax(&boards.at(i), depth - 1, false, alpha, beta, weights);
            bestVal = std::max( bestVal, value);
            alpha = std::max( alpha, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
    else {
        float bestVal = INFINITY;
        for (int i = 0; i < boardsSize; i++) {
            float value = minimax(&boards.at(i), depth - 1, true, alpha, beta, weights);
            bestVal = std::min( bestVal, value);
            beta = std::min( beta, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
}

void Search::threadMinMax(Board* board, int currentIndex, int* bestIndex, float* bestEval, int depth, float alpha, float beta, EvaluationWeights weights) {
    float eval = minimax(board, depth, board->state.whiteToMove ? true : false, alpha, beta, weights);
    std::lock_guard<std::mutex> lock(evalMutex);
    if (board->state.whiteToMove ? eval < *bestEval : eval > *bestEval) {
        *bestIndex = currentIndex;
        *bestEval = eval;
    }
}