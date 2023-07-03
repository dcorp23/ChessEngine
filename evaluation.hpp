#pragma once
#include "chessBoard.hpp"

struct EvaluationWeights {
    float material;
    float kingSafety;
    float activity;
    float pawn;
    float bishop;
    float knight;
    float rook;
    float queen;

    EvaluationWeights();

    printWeights();
};

namespace Evaluation {
    //returns full evaluation of a board for the person moving
    float evaluate(Board* board, EvaluationWeights weights);

    void initEvaluation();

    //returns a vector of integers showing how many times a square
    //is attacked if they are attacked by both sides they cancel out to 0
    //so + is white and - is black
    std::vector<int> getVectorOfAttackers(Board* board);
}