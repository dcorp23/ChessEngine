#include <mutex>
#include <future>
#include <vector>
#include "chessBoard.hpp"

#define INFINITY 99999999
#define NUM_THREADS 4
#define MAX_DEPTH 3

namespace Search {
    float minimax(Board* board, int depth, bool isMaximizingPlayer, float alpha, float beta);

    void threadMinMax(Board* board, int currentIndex, int* bestIndex, float* bestEval, int depth, float* alpha, float* beta);
}