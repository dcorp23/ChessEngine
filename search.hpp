#include <mutex>
#include <future>
#include <vector>
#include "chessBoard.hpp"
#include "evaluation.hpp"

#define INFINITY 99999999
#define NUM_THREADS 4
#define MAX_DEPTH 3
#define NULL_MOVE_THRESHOLD MAX_DEPTH - 1

namespace Search {
    float quiesce(Board* board, float alpha, float beta, EvaluationWeights weights);

    void waitForThreads(int openThreads, std::vector<std::future<void>>* threads);

    float minimax(Board* board, int depth, bool isMaximizingPlayer, float alpha, float beta, EvaluationWeights weights, bool nullMoveSide);

    void threadMinMax(Board* board, int currentIndex, int* bestIndex, float* bestEval, int depth, float alpha, float beta, EvaluationWeights weights, bool nullMoveSide);
}