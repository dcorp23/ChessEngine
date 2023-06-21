#include "chessBoard.hpp"
#include "bitFunctions.hpp"
#include "moveGen.hpp"
#include "attackTables.hpp"
#include "evaluation.hpp"

const map AFILE = 0x8080808080808080;
const map BFILE = 0x4040404040404040;
const map CFILE = 0x2020202020202020;
const map DFILE = 0x1010101010101010;
const map EFILE = 0x0808080808080808;
const map FFILE = 0x0404040404040404;
const map GFILE = 0x0202020202020202;
const map HFILE = 0x0101010101010101;

const map FILES[8] = {
    AFILE, BFILE, CFILE, DFILE, 
    EFILE, FFILE, GFILE, HFILE
};


//evaluates the material of the position returning a negative score for black
//and returns a 0 for the position being equal in material
float Evaluation::material(Board board) {
    int score = 0;

    int pawns = getBitCount(board.bitMaps[WPawn]) - getBitCount(board.bitMaps[BPawn]);
    int bishops = getBitCount(board.bitMaps[WBishop]) - getBitCount(board.bitMaps[BBishop]);
    int knights = getBitCount(board.bitMaps[WKnight]) - getBitCount(board.bitMaps[BKnight]);
    int rooks = getBitCount(board.bitMaps[WRook]) - getBitCount(board.bitMaps[BRook]);
    int queens = getBitCount(board.bitMaps[WQueen]) - getBitCount(board.bitMaps[BQueen]);

    score = (pawns * 100) + (bishops * 320) + (knights * 320) + (rooks * 500) + (queens * 900);
    return score;
}

//evaluates the safety of the king
float Evaluation::kingSafety(Board board) {
    const int squareEval[64] = {
        300, 300, 50, 50, 50, 100, 300, 250, 
        0, 0, -50, -100, -100, -50, 0, 0, 
        -50, -50, -150, -150, -150, -150, -50, -50,
        -50, -100, -200, -300, -300, -200, -100, -50,
        -50, -100, -200, -300, -300, -200, -100, -50,
        -50, -50, -150, -150, -150, -150, -50, -50, 
        0, 0, -50, -100, -100, -50, 0, 0,
        300, 300, 50, 50, 50, 100, 300, 250
    };

    int whiteKingSquare = getLSBIndex(board.bitMaps[WKing]);
    int blackKingSquare = getLSBIndex(board.bitMaps[BKing]);
    int whiteScore = squareEval[whiteKingSquare];
    int blackScore = squareEval[blackKingSquare];

    whiteScore += (board.state.whiteShortCastle ? 0 : -50);
    whiteScore += (board.state.whiteLongCastle ? 0 : -30);
    blackScore += (board.state.blackShortCastle ? 0 : -50);
    blackScore += (board.state.blackLongCastle ? 0 : -30);

    return whiteScore - blackScore;
}

float bishopEval(Board board) {
    //check if the sides have the bishop pair doesn't
    //account for bishop promotions so if you promote on
    //the same square color it will count as bishop pair
    //doing this to increase speed by discarding edge case
    float whiteScore = 0;
    float blackScore = 0;

    bool whitePair = getBitCount(board.bitMaps[WBishop]) >= 2;
    bool blackPair = getBitCount(board.bitMaps[BBishop]) >= 2;

    //give points for bishop pair
    //else check if our bishop is bad or not
    //check if the majority of our pawns are on light 
    //or dark squares and compare that to the remaining bishop
    if (whitePair) whiteScore += 150;
    else {
        //get which square the bishop is on
        bool whiteLightSquare = (getLSBIndex(board.bitMaps[WBishop]) % 2 == 0 ? true : false);
        map whitePawns = board.bitMaps[WPawn]; //get a copy of the whit pawns bitmap
        int numPawns = getBitCount(whitePawns); //number of pawns
        int lightPawns = 0; //pawns on light squares
        int darkPawns = 0; //pawns on dark squares
        //count the number of light and dark pawns
        for (int i = 0; i < numPawns; i++) {
            getLSBIndex(whitePawns) % 2 == 0 ? lightPawns++ : darkPawns++;
            whitePawns = popLSB(whitePawns);
        }
        if ((lightPawns - darkPawns) >= 1) { //majority light pawns
            if (whiteLightSquare) whiteScore -= 75;
            else whiteScore += 100;
        } 
        if ((darkPawns - lightPawns) >= 1) { //majority dark pawns
            if (whiteLightSquare) whiteScore += 100;
            else whiteScore -= 75;
        }
        if (darkPawns == lightPawns) {
            whiteScore += 50;
        }
    }
    if (blackPair) blackScore += 150;
    else {
        //get which square the bishop is on
        bool blackLightSquare = (getLSBIndex(board.bitMaps[BBishop]) % 2 == 0 ? true : false);
        map blackPawns = board.bitMaps[BPawn]; //get a copy of the whit pawns bitmap
        int numPawns = getBitCount(blackPawns); //number of pawns
        int lightPawns = 0; //pawns on light squares
        int darkPawns = 0; //pawns on dark squares
        //count the number of light and dark pawns
        for (int i = 0; i < numPawns; i++) {
            getLSBIndex(blackPawns) % 2 == 0 ? lightPawns++ : darkPawns++;
            blackPawns = popLSB(blackPawns);
        }
        if ((lightPawns - darkPawns) >= 1) { //majority light pawns
            if (blackLightSquare) blackScore -= 75;
            else blackScore += 100;
        } 
        if ((darkPawns - lightPawns) >= 1) { //majority dark pawns
            if (blackLightSquare) blackScore += 100;
            else blackScore -= 75;
        }
        if (darkPawns == lightPawns) {
            blackScore += 50;
        }
    }
    return whiteScore - blackScore;
}

float knightEval(Board board) {
    //find the half open files and see if there are potential knight outposts
    //see if there is a knight there or see if we can move a knight towards that square
    //lose points for knights on the edge of the board
    const int knightSquareEval[64] = {
        -75, -50, -40, -35, -35, -40, -50, -75, 
        -50, -50, -35, -10, -10, -35, -50, -50, 
        -40, -20, 60, 20, 20, 60, -20, -40, 
        -20, -5, 25, 35, 35, 25, -5, -20, 
        -20, -5, 25, 35, 35, 25, -5, -20, 
        -40, -20, 60, 20, 20, 60, -20, -40, 
        -50, -50, -35, -10, -10, -35, -50, -50, 
        -75, -30, -40, -35, -35, -40, -30, -75
    };

    float whiteScore = 0;
    float blackScore = 0;

    map whiteKnights = board.bitMaps[WKnight];
    int numberOfWhiteKnights = getBitCount(whiteKnights);
    for (int i = 0; i < numberOfWhiteKnights; i++) {
        int square = getLSBIndex(whiteKnights);
        whiteKnights = popLSB(whiteKnights);

        whiteScore += knightSquareEval[square];
    }

    map blackKnights = board.bitMaps[BKnight];
    int numberOfBlackKnights = getBitCount(blackKnights);
    for (int i = 0; i < numberOfBlackKnights; i++) {
        int square = getLSBIndex(blackKnights);
        blackKnights = popLSB(blackKnights);

        whiteScore += knightSquareEval[square];
    }

    return whiteScore - blackScore;
}

float rookEval(Board board) {
    //look for open files and see if we can put rooks there
    //see if the rook is on the 2nd or 7th rank
    float whiteScore = 0;
    float blackScore = 0;

    map allPawns = board.bitMaps[WPawn] | board.bitMaps[BPawn];

    map whiteRooks = board.bitMaps[WRook];
    int numberOfWhiteRooks = getBitCount(whiteRooks);
    for (int i = 0; i < numberOfWhiteRooks; i++) {
        int square = getLSBIndex(whiteRooks);
        whiteRooks = popLSB(whiteRooks);

        int col = square % 8;
        int row = square / 8;
        if (!(FILES[col] & allPawns)) whiteScore += 75; //open file
        if (row == 7) whiteScore += 75; //7th rank
        if (col == 4 || col == 5) whiteScore += 25; //center the rooks
        if (col == 3 || col == 6) whiteScore += 25; //center the rooks
    }

    map blackRooks = board.bitMaps[BRook];
    int numberOfBlackRooks = getBitCount(blackRooks);
    for (int i = 0; i < numberOfBlackRooks; i++) {
        int square = getLSBIndex(blackRooks);
        blackRooks = popLSB(blackRooks);

        int col = square % 8;
        int row = square / 8;
        if (!(FILES[col] & allPawns)) blackScore += 75; //open file
        if (row == 2) blackScore += 75; //2nd rank
        if (col == 4 || col == 5) blackScore += 35; //center the rooks
        if (col == 3 || col == 6) blackScore += 25; //center the rooks
    }

    return whiteScore - blackScore;
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

    whiteSquares = (whiteSquares * 20); //* (board.state.whiteToMove ? 1.2 : .8); 
    blackSquares = (blackSquares * 20); //* (board.state.whiteToMove ? .8 : 1.2); 

    return whiteSquares - blackSquares;
}

//counts then number of pawns in the center for that side
float Evaluation::pawnEval(Board board) {
    //use constant evaluation of predetermined squares
    //which encourage pushing the center pawns and discourage
    //pushing the wing pawns
    const int whiteSquareEval[64] = {
        0, 0, 0, 0, 0, 0, 0, 0, 
        75, 75, 75, 75, 75, 75, 75, 75,
        20, 20, 20, 50, 50, 20, 20, 20,
        20, 20, 20, 50, 50, 20, 20, 20,
        10, 10, 30, 50, 50, 10, 10, 10,
        20, 20, 30, 30, 30, 10, 20, 20,
        20, 20, 30, -30, -30, 30, 20, 20,
        0, 0, 0, 0, 0, 0, 0, 0,
    };
    const int blackSquareEval[64] = {
        0, 0, 0, 0, 0, 0, 0, 0, 
        20, 20, 30, -30, -30, 30, 20, 20,
        20, 20, 30, 30, 30, 10, 20, 20,
        10, 10, 30, 50, 50, 10, 10, 10,
        20, 20, 20, 50, 50, 20, 20, 20,
        20, 20, 20, 50, 50, 20, 20, 20,
        75, 75, 75, 75, 75, 75, 75, 75,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    float whiteScore = 0;
    float blackScore = 0;

    map whitePawns = board.bitMaps[WPawn];
    int numberOfWhitePawns = getBitCount(whitePawns);
    for (int i = 0; i < numberOfWhitePawns; i++) {
        int square = getLSBIndex(whitePawns);
        whitePawns = popLSB(whitePawns);

        whiteScore += whiteSquareEval[square];
    }

    map blackPawns = board.bitMaps[BPawn];
    int numberOfblackPawns = getBitCount(blackPawns);
    for (int i = 0; i < numberOfblackPawns; i++) {
        int square = getLSBIndex(blackPawns);
        blackPawns = popLSB(blackPawns);

        blackScore += blackSquareEval[square];
    }
    
    return whiteScore - blackScore;
}


float Evaluation::evaluate(Board board) {
    if (board.state.checkMate) return board.state.whiteToMove ? 999999 : -999999;

    int kingSafetyValue = kingSafety(board);
    int activityValue = pieceActivity(board);
    int materialValue = material(board);
    int pawnValue = pawnEval(board);
    int bishopValue = bishopEval(board);
    int knightValue = knightEval(board);
    int rookValue = rookEval(board);

    const float materialConst = 1;
    const float kingSafetyConst = 1;
    const float activityConst = 1;
    const float pawnConst = 1;
    const float bishopConst = 1;
    const float knightConst = 1;
    const float rookConst = 1;

    return ((materialConst * materialValue) + (kingSafetyConst * kingSafetyValue) + (activityConst * activityValue) + (pawnConst * pawnValue) + 
            (bishopConst * bishopValue) + (knightConst * knightValue) + (rookConst * rookValue));
}
