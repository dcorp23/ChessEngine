#include "chessBoard.hpp"
#include "bitFunctions.hpp"
#include "moveGen.hpp"
#include "attackTables.hpp"
#include "evaluation.hpp"

//evaluates the material of the position returning a negative score for black
//and returns a 0 for the position being equal in material
float Evaluation::material(Board board) {
    int score = 0;

    int pawns = getBitCount(board.bitMaps[WPawn]) - getBitCount(board.bitMaps[BPawn]);
    int bishops = getBitCount(board.bitMaps[WBishop]) - getBitCount(board.bitMaps[BBishop]);
    int knights = getBitCount(board.bitMaps[WKnight]) - getBitCount(board.bitMaps[BKnight]);
    int rooks = getBitCount(board.bitMaps[WRook]) - getBitCount(board.bitMaps[BRook]);
    int queens = getBitCount(board.bitMaps[WQueen]) - getBitCount(board.bitMaps[BQueen]);

    score = (pawns * 100) + (bishops * 300) + (knights * 300) + (rooks * 500) + (queens * 900);
    return score;
}

//evaluates the safety of the king
float Evaluation::kingSafety(Board board) {
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

    int whiteKingSquare = getLSBIndex(board.bitMaps[WKing]);
    int blackKingSquare = getLSBIndex(board.bitMaps[BKing]);
    float whiteKingEval = (squareEval[whiteKingSquare] * 12) * (board.state.whiteToMove ? 1 : .9);
    float blackKingEval = (squareEval[blackKingSquare] * 12) * (board.state.whiteToMove ? .9 : 1);

    return whiteKingEval - blackKingEval;
}

//gets the pseudo legal moves for bishops knights
//queens and rooks and will count how many moves they have
float Evaluation::pieceActivity(Board board) {
    float whiteSquares = 0;
    float blackSquares = 0;

    Board tempBoard = board;

    tempBoard.state.whiteToMove = 1;
    for (int i = WBishop; i <= WQueen; i++) {
        if (i == WBishop) whiteSquares += MoveGenerator::getBishopMoves(tempBoard).size();
        if (i == WKnight) whiteSquares += MoveGenerator::getKnightMoves(tempBoard).size();
        if (i == WRook) whiteSquares += MoveGenerator::getRookMoves(tempBoard).size();
        if (i == WQueen) whiteSquares += MoveGenerator::getQueenMoves(tempBoard).size();
    }

    tempBoard.state.whiteToMove = 0;
    for (int i = BBishop; i <= BQueen; i++) {
        if (i == BBishop) blackSquares += MoveGenerator::getBishopMoves(tempBoard).size();
        if (i == BKnight) blackSquares += MoveGenerator::getKnightMoves(tempBoard).size();
        if (i == BRook) blackSquares += MoveGenerator::getRookMoves(tempBoard).size();
        if (i == BQueen) blackSquares += MoveGenerator::getQueenMoves(tempBoard).size();
    }

    whiteSquares = (whiteSquares * 10) * (board.state.whiteToMove ? 1.2 : .8); 
    blackSquares = (blackSquares * 10) * (board.state.whiteToMove ? .8 : 1.2); 

    return whiteSquares - blackSquares;
}

//counts then number of pawns in the center for that side
float Evaluation::centerControl(Board board) {
    const map centerMask = 0x0000001818000000;
    const map whiteSupportingCenter = 0x00000000263C0000;
    const map blackSupportingCenter = 0x00003C2600000000;
    int whiteCenterPawns = getBitCount(centerMask & board.bitMaps[WPawn]);
    int whiteSupportingPawns = getBitCount(whiteSupportingCenter & board.bitMaps[WPawn]);
    int blackCenterPawns = getBitCount(centerMask & board.bitMaps[BPawn]);
    int blackSupportingPawns = getBitCount(blackSupportingCenter & board.bitMaps[BPawn]);

    float white = ((whiteCenterPawns * 20) + (whiteSupportingPawns * 10)) * (board.state.whiteToMove ? 1.1 : 1);
    float black = ((blackCenterPawns * 20) + (blackSupportingPawns * 10)) * (board.state.whiteToMove ? 1 : 1.1);
    
    return white - black;
}



float Evaluation::evaluate(Board board) {
    int centerValue = centerControl(board);
    int kingSafetyValue = kingSafety(board);
    int activityValue = pieceActivity(board);
    int materialValue = material(board);

    const float materialConst = 1;
    const float kingSafetyConst = .4;
    const float activityConst = .7;
    const float centerConst = .7;
    

    return (materialConst * materialValue) + (kingSafetyConst * kingSafetyValue) + (activityConst * activityValue) + (centerConst * centerValue);
}
