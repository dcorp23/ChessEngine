#include "chessBoard.hpp"

namespace Evaluation {
    //evaluates the material of the position returning a negative score for black
    //and returns a 0 for the position being equal in material
    float material(Board board);

    //evaluates the safety of the king
    float kingSafety(Board board);

    //gets the pseudo legal moves for bishops knights
    //queens and rooks and will count how many moves they have
    float pieceActivity(Board board);

    //counts then number of pawns in the center for that side
    float centerControl(Board board);

    //returns full evaluation of a board for the person moving
    float evaluate(Board board);
}