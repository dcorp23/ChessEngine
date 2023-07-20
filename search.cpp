#include <iostream>
#include "search.hpp"
#include "chessBoard.hpp"
#include "evaluation.hpp"
#include "moveGen.hpp"

static std::mutex evalMutex;

float Search::quiesce(Board* board, float alpha, float beta, EvaluationWeights weights) {
    int standPat = Evaluation::evaluate(board, weights);
    if (standPat >= beta) return beta;
    if (alpha < standPat) alpha = standPat;

    std::vector<Board> possibleBoards = MoveGenerator::getAllLegalBoards(*board);
    int numBoards = possibleBoards.size();
    
    int score;
    for (int i = 0; i < numBoards; i++) {
        Board currentBoard = possibleBoards.at(i);
        //(currentBoard.state.check || MoveGenerator::checkForCapture(currentBoard, *board) || currentBoard.state.checkMate)
        if (!(MoveGenerator::checkForCapture(currentBoard, *board))) continue;
        score = -quiesce(&currentBoard, -beta, -alpha, weights);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    
    return alpha;
}

//wait until there are openThreads available in a vector of active threads
void Search::waitForThreads(int openThreads, std::vector<std::future<void>>* threads) {
    int j = 0;
    if (threads->empty()) return;
    while ((NUM_THREADS - threads->size()) < openThreads ) {
        //std::cout << (NUM_THREADS - threads->size()) << " waiting for " << openThreads << " " << j << '\n';
        if (threads->at(j).wait_for(std::chrono::milliseconds(500)) == std::future_status::ready) {
            threads->erase(threads->begin() + j);
        }
        j++;
        if (j >= threads->size()) j = 0;
    }
};

float Search::minimax(Board* board, int depth, bool isMaximizingPlayer, float alpha, float beta, EvaluationWeights weights, bool nullMoveSide) {
    if (depth <= 0 || board->state.checkMate == 1) return quiesce(board, nullMoveSide ? -beta : alpha, nullMoveSide ? -alpha : beta, weights);
    
    std::vector<Board> boards = MoveGenerator::getAllLegalBoards(*board);
    int boardsSize = boards.size();

    if (isMaximizingPlayer)  {
        //null move pruning
        if (nullMoveSide && depth >= NULL_MOVE_THRESHOLD) {
            board->state.whiteToMove = !board->state.whiteToMove;
            alpha = std::max(alpha, minimax(board, depth - 2, true, alpha, beta, weights, nullMoveSide));
            board->state.whiteToMove = !board->state.whiteToMove;
        }

        float bestVal = -INFINITY;
        for (int i = 0; i < boardsSize; i++)  {
            float value = minimax(&boards.at(i), depth - 1, false, alpha, beta, weights, nullMoveSide);
            bestVal = std::max( bestVal, value);
            alpha = std::max( alpha, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
    else {
        //null move pruning
        if (!nullMoveSide && depth >=  NULL_MOVE_THRESHOLD) {
            board->state.whiteToMove = !board->state.whiteToMove;
            beta = std::min(beta, minimax(board, depth - 2, false, alpha, beta, weights, nullMoveSide));
            board->state.whiteToMove = !board->state.whiteToMove;
        }

        float bestVal = INFINITY;
        for (int i = 0; i < boardsSize; i++) {
            float value = minimax(&boards.at(i), depth - 1, true, alpha, beta, weights, nullMoveSide);
            bestVal = std::min( bestVal, value);
            beta = std::min( beta, value);
            if (beta <= alpha) break;
        }
        return bestVal;
    }
}

void Search::threadMinMax(Board* board, int currentIndex, int* bestIndex, float* bestEval, int depth, float alpha, float beta, EvaluationWeights weights, bool nullMoveSide) {
    float eval = minimax(board, depth, board->state.whiteToMove ? true : false, alpha, beta, weights, nullMoveSide);
    std::lock_guard<std::mutex> lock(evalMutex);
    if (board->state.whiteToMove ? eval < *bestEval : eval > *bestEval) {
        *bestIndex = currentIndex;
        *bestEval = eval;
    }
}