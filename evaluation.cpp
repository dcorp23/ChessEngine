#include "evaluation.hpp"
#include "chessBoard.hpp"
#include "bitFunctions.hpp"
#include "moveGen.hpp"
#include "attackTables.hpp"

static MoveGenerator moveGen = MoveGenerator();

//evaluates the material of the position returning a negative score for black
//and returns a 0 for the position being equal in material
int material(Board board) {
    int score = 0;

    int pawns = getBitCount(board.bitMaps[WPawn]) - getBitCount(board.bitMaps[BPawn]);
    int bishops = getBitCount(board.bitMaps[WPawn]) - getBitCount(board.bitMaps[BBishop]);
    int knights = getBitCount(board.bitMaps[WKnight]) - getBitCount(board.bitMaps[BKnight]);
    int rooks = getBitCount(board.bitMaps[WRook]) - getBitCount(board.bitMaps[BRook]);
    int queens = getBitCount(board.bitMaps[WQueen]) - getBitCount(board.bitMaps[BQueen]);

    score = (pawns * 10) + (bishops * 30) + (knights * 30) + (rooks * 50) + (queens * 90);
    score *= (board.state.whiteToMove ? 1 : -1);
    return score;
}

//evaluates the safety of the king
int kingSafety(Board board) {
    const int squareEval[64] = {
        10, 10, 5, 4, 4, 5, 10, 10, 
        6, 4, 4, -1, -1, 4, 4, 6, 
        2, -1, -1, -4, -4, -1, -1, 2,
        -1, -3, -10, -10, -10, -10, -3, 2,
        -1, -3, -10, -10, -10, -10, -3, 2,
        2, -1, -3, -4, -4, -3, -1, 2,
        6, 4, 4, -1, -1, 4, 4, 6,
        10, 10, 5, 4, 4, 5, 10, 10,
    };

    int kingSquare = getLSBIndex(board.bitMaps[board.state.whiteToMove ? 5 : 11]);
    return squareEval[kingSquare];
}

//gets the pseudo legal moves for bishops knights
//queens and rooks and will count how many moves they have
int pieceActivity(Board board) {
    int startPiece = board.state.whiteToMove ? WBishop : BBishop;
    int endPiece = board.state.whiteToMove ? WQueen : BQueen;
    int squares = 0;

    for (int i = startPiece; i <= endPiece; i++) {
        if ((i % 6) == 1) squares += moveGen.getBishopMoves(board).size();
        if ((i % 6) == 2) squares += moveGen.getKnightMoves(board).size();
        if ((i % 6) == 3) squares += moveGen.getRookMoves(board).size();
        if ((i % 6) == 4) squares += moveGen.getQueenMoves(board).size();
    }

    return squares;
}

//counts then number of pawns in the center for that side
int centerControl(Board board) {
    const map centerMask = 0x0000001818000000;
    map pawns = board.bitMaps[board.state.whiteToMove ? 0 : 6];
    int centerPawns = getBitCount(centerMask & pawns);

    return centerPawns * 10;
}

float evaluate(Board board) {
    int center = centerControl(board);
    int king = kingSafety(board);
    int activity = pieceActivity(board);
    int mat = material(board);
    return (2 * mat) + (.7 * king) + (1.2 * activity) + (.7 * center);
}
