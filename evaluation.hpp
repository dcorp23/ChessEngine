#include "chessBoard.hpp"

namespace Evaluation {
    //returns full evaluation of a board for the person moving
    float evaluate(Board* board);

    void initEvaluation();

    //returns a vector of integers showing how many times a square
    //is attacked if they are attacked by both sides they cancel out to 0
    //so + is white and - is black
    std::vector<int> getVectorOfAttackers(Board* board);
}