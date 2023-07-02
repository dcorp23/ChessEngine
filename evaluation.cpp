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

const int whiteQueenSquareEval[64] = {
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0
};

//dont want to move queen, shouldn't go far
const int blackQueenSquareEval[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -30, -30, -30, -30, -30, -30, -30, -30
};

const int kingSquareEval[64] = {
    350, 400, 50, 0, 0, 100, 400, 350, 
    0, 0, -50, -100, -100, -50, 0, 0, 
    -50, -50, -150, -150, -150, -150, -50, -50,
    -50, -100, -200, -300, -300, -200, -100, -50,
    -50, -100, -200, -300, -300, -200, -100, -50,
    -50, -50, -150, -150, -150, -150, -50, -50, 
    0, 0, -50, -100, -100, -50, 0, 0,
    350, 400, 50, 0, 0, 100, 400, 350
};

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

const int whitePawnSquareEval[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    75, 75, 75, 75, 75, 75, 75, 75,
    20, 20, 20, 50, 50, 20, 20, 20,
    10, 10, 20, 50, 50, 20, 10, 10,
    0, 0, 10, 50, 50, -10, 0, 0,
    0, 0, 0, 30, 30, -20, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

const int blackPawnSquareEval[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, 30, 30, -20, 0, 0,
    0, 0, 10, 50, 50, -10, 0, 0,
    10, 10, 20, 50, 50, 20, 10, 10,
    20, 20, 20, 50, 50, 20, 20, 20,
    75, 75, 75, 75, 75, 75, 75, 75,
    0, 0, 0, 0, 0, 0, 0, 0,
};

//passed pawn masks for a square and side
map passedPawnMasks[64][2];

int determineGamePhase(Board* board);

//evaluates the material of the position returning a negative score for black
//and returns a 0 for the position being equal in material
float material(Board* board) {
    int score = 0;

    int pawns = getBitCount(board->bitMaps[WPawn]) - getBitCount(board->bitMaps[BPawn]);
    int bishops = getBitCount(board->bitMaps[WBishop]) - getBitCount(board->bitMaps[BBishop]);
    int knights = getBitCount(board->bitMaps[WKnight]) - getBitCount(board->bitMaps[BKnight]);
    int rooks = getBitCount(board->bitMaps[WRook]) - getBitCount(board->bitMaps[BRook]);
    int queens = getBitCount(board->bitMaps[WQueen]) - getBitCount(board->bitMaps[BQueen]);

    score = (pawns * 100) + (bishops * 320) + (knights * 320) + (rooks * 500) + (queens * 900);
    return score;
}

//evaluates the safety of the king
float kingSafety(Board* board, int gamePhase) {
    int whiteKingSquare = getLSBIndex(board->bitMaps[WKing]);
    int blackKingSquare = getLSBIndex(board->bitMaps[BKing]);
    int whiteScore = kingSquareEval[whiteKingSquare];
    int blackScore = kingSquareEval[blackKingSquare];

    whiteScore += (board->state.whiteShortCastle ? 0 : -50);
    whiteScore += (board->state.whiteLongCastle ? 0 : -30);
    blackScore += (board->state.blackShortCastle ? 0 : -50);
    blackScore += (board->state.blackLongCastle ? 0 : -30);

    return whiteScore - blackScore;
}

float bishopEval(Board* board, int gamePhase) {
    //check if the sides have the bishop pair doesn't
    //account for bishop promotions so if you promote on
    //the same square color it will count as bishop pair
    //doing this to increase speed by discarding edge case
    float whiteScore = 0;
    float blackScore = 0;

    bool whitePair = getBitCount(board->bitMaps[WBishop]) >= 2;
    bool blackPair = getBitCount(board->bitMaps[BBishop]) >= 2;

    //give points for bishop pair
    //else check if our bishop is bad or not
    //check if the majority of our pawns are on light 
    //or dark squares and compare that to the remaining bishop
    if (whitePair) whiteScore += 150;
    else {
        //get which square the bishop is on
        bool whiteLightSquare = (getLSBIndex(board->bitMaps[WBishop]) % 2 == 0 ? true : false);
        map whitePawns = board->bitMaps[WPawn]; //get a copy of the whit pawns bitmap
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
        bool blackLightSquare = (getLSBIndex(board->bitMaps[BBishop]) % 2 == 0 ? true : false);
        map blackPawns = board->bitMaps[BPawn]; //get a copy of the whit pawns bitmap
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

float knightEval(Board* board, int gamePhase) {
    //find the half open files and see if there are potential knight outposts
    //see if there is a knight there or see if we can move a knight towards that square
    //lose points for knights on the edge of the board
    float whiteScore = 0;
    float blackScore = 0;

    map whiteKnights = board->bitMaps[WKnight];
    int numberOfWhiteKnights = getBitCount(whiteKnights);
    for (int i = 0; i < numberOfWhiteKnights; i++) {
        int square = getLSBIndex(whiteKnights);
        whiteKnights = popLSB(whiteKnights);

        whiteScore += knightSquareEval[square];
    }

    map blackKnights = board->bitMaps[BKnight];
    int numberOfBlackKnights = getBitCount(blackKnights);
    for (int i = 0; i < numberOfBlackKnights; i++) {
        int square = getLSBIndex(blackKnights);
        blackKnights = popLSB(blackKnights);

        whiteScore += knightSquareEval[square];
    }

    return whiteScore - blackScore;
}

float rookEval(Board* board, int gamePhase) {
    //look for open files and see if we can put rooks there
    //see if the rook is on the 2nd or 7th rank
    float whiteScore = 0;
    float blackScore = 0;

    map allPawns = board->bitMaps[WPawn] | board->bitMaps[BPawn];

    map whiteRooks = board->bitMaps[WRook];
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

    map blackRooks = board->bitMaps[BRook];
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

int queenEval(Board* board, int gamePhase) {
    if (gamePhase == 0) { //opening
        //we want the opposite queen so we can evaluate the last move
        map queen = board->bitMaps[board->state.whiteToMove ? BQueen : WQueen];
        int queenSquare = getLSBIndex(queen);
        return board->state.whiteToMove ? blackQueenSquareEval[queenSquare] : whiteQueenSquareEval[queenSquare];
    } 
    return 0;
}

//gets the pseudo legal moves for bishops knights
//queens and rooks and will count how many moves they have
float pieceActivity(Board* board, std::vector<int>* attackVector) {
    float whiteSquares = 0;
    float blackSquares = 0;

    Board tempBoard = *board;

    tempBoard.state.whiteToMove = 1;
    int whiteEndPiece = WQueen;
    if (determineGamePhase(&tempBoard) == 0) whiteEndPiece = WKnight; //opening
    std::vector<MoveCode> whiteMoveList;
    for (int i = WBishop; i <= whiteEndPiece; i++) {
        if (i == WBishop) MoveGenerator::getBishopMoves(tempBoard, &whiteMoveList);
        if (i == WKnight) MoveGenerator::getKnightMoves(tempBoard, &whiteMoveList);
        if (i == WRook) MoveGenerator::getRookMoves(tempBoard, &whiteMoveList);
        if (i == WQueen) MoveGenerator::getQueenMoves(tempBoard, &whiteMoveList);
    }
    int numberOfMoves = whiteMoveList.size();
    for (int i = 0; i < numberOfMoves; i++) {
        MoveCode move = whiteMoveList.at(i);
        if (attackVector->at(move.endSquare) >= 0) {
            Board newBoard = board->move(move);
            if (MoveGenerator::isBoardValid(newBoard)) whiteSquares++;
        }
    }

    tempBoard.state.whiteToMove = 0;
    int blackEndPiece = BQueen;
    if (determineGamePhase(&tempBoard) == 0) blackEndPiece = BKnight; //opening
    std::vector<MoveCode> blackMoveList;
    for (int i = BBishop; i <= blackEndPiece; i++) {
        if (i == BBishop) MoveGenerator::getBishopMoves(tempBoard, &blackMoveList);
        if (i == BKnight) MoveGenerator::getKnightMoves(tempBoard, &blackMoveList);
        if (i == BRook) MoveGenerator::getRookMoves(tempBoard, &blackMoveList);
        if (i == BQueen) MoveGenerator::getQueenMoves(tempBoard, &blackMoveList);
    }
    numberOfMoves = blackMoveList.size();
    for (int i = 0; i < numberOfMoves; i++) {
        MoveCode move = blackMoveList.at(i);
        if (attackVector->at(move.endSquare) <= 0) {
            Board newBoard = board->move(move);
            if (MoveGenerator::isBoardValid(newBoard)) blackSquares++;
        }
    }

    whiteSquares = (whiteSquares * 15) * (board->state.whiteToMove ? 1.1 : 1); 
    blackSquares = (blackSquares * 15) * (board->state.whiteToMove ? 1 : 1.1); 

    return whiteSquares - blackSquares;
}

map getPassedPawnMaks(int square, int side) {
    return passedPawnMasks[square][side];
}

//counts then number of pawns in the center for that side
float pawnEval(Board* board, int gamePhase) {
    //use constant evaluation of predetermined squares
    //which encourage pushing the center pawns and discourage
    //pushing the wing pawns
    float whiteScore = 0;
    float blackScore = 0;

    map allPawns = board->bitMaps[WPawn] | board->bitMaps[BPawn];

    map whitePawns = board->bitMaps[WPawn];
    int numberOfWhitePawns = getBitCount(whitePawns);
    for (int i = 0; i < numberOfWhitePawns; i++) {
        int square = getLSBIndex(whitePawns);
        whitePawns = popLSB(whitePawns);

        map passedPawnMask = getPassedPawnMaks(square, 1);
        if (!(passedPawnMask & allPawns)) whiteScore += 75;

        whiteScore += whitePawnSquareEval[square];
    }

    map blackPawns = board->bitMaps[BPawn];
    int numberOfblackPawns = getBitCount(blackPawns);
    for (int i = 0; i < numberOfblackPawns; i++) {
        int square = getLSBIndex(blackPawns);
        blackPawns = popLSB(blackPawns);

        map passedPawnMask = getPassedPawnMaks(square, 0);
        if (!(passedPawnMask & allPawns)) blackPawns += 75;

        blackScore += blackPawnSquareEval[square];
    }
    
    return whiteScore - blackScore;
}

//returns either 0, 1 for opening or middlegame
//this is side depenent and will determine whether a 
//side needs to focus on development or can attack
int determineGamePhase(Board* board) {
    int openingScore = 0;

    //king out of center
    //we want to check the other team because we are evaluating it from the point of view of 
    //the last move that was doneso we can see if they still need to develop from their or not
    map centerKing = 0x0000000000001C1C << (!board->state.whiteToMove ? 56 : 0);
    if (board->bitMaps[(!board->state.whiteToMove) ? WKing : BKing] & centerKing) return 0;
    return 1;
}

void initPassedPawns() {
    //get an 2d array getting squares infront of a pawn to check if it is a passed pawn or not
    //initialize the white pawns
    //returns a map of all the squares in front of the pawn
    //and in front of the to columns next to it
    
    for (int square = h2; square >= a8; square--) { //counts right to left
        map passedPawnMask = 0ULL;
        int nextSquare = square - 8;
        while (nextSquare >= 0) {
            passedPawnMask |= (1ULL << nextSquare); //add the square in front of the pawn
            if ((1ULL << square) & AFILE) passedPawnMask |= (1ULL << (nextSquare - 1)); //check if you can add a bit to the left
            if ((1ULL << square) & HFILE) passedPawnMask |= (1ULL << (nextSquare + 1)); //check if you can add a bit to the right
            nextSquare -= 8; //go up one rank
        }
        passedPawnMasks[square][1] = passedPawnMask;
    }

    for (int square = a7; square <= h1; square++) { //counts left to right
        map passedPawnMask = 0ULL;
        int nextSquare = square + 8;
        while (nextSquare <= 63) {
            passedPawnMask |= (1ULL << nextSquare); //add the square in front of the pawn
            if ((1ULL << square) & AFILE) passedPawnMask |= (1ULL << (nextSquare - 1)); //check if you can add a bit to the left
            if ((1ULL << square) & HFILE) passedPawnMask |= (1ULL << (nextSquare + 1)); //check if you can add a bit to the right
            nextSquare += 8; //go down one rank
        }
        passedPawnMasks[square][0] = passedPawnMask;
    }
}

//returns a vector of integers showing how many times a square
//is attacked if they are attacked by both sides they cancel out to 0
//so + is white and - is black
std::vector<int> Evaluation::getVectorOfAttackers(Board* board) {
    std::vector<int> attackVector(64, 0);
    
    //loop through all the pieces
    for (int piece = WPawn; piece <= BKing; piece++) {
        map pieceMap = board->bitMaps[piece];
        while (pieceMap) {
            int pieceSquare = getLSBIndex(pieceMap);
            pieceMap = popLSB(pieceMap);
            
            map attackMap;
            if ((piece % 6) == 0) attackMap = AttackTables::getPawnAttacks(pieceSquare, piece ? 0 : 1);
            if ((piece % 6) == 1) attackMap = AttackTables::getBishopAttacks(pieceSquare, board->All);
            if ((piece % 6) == 2) attackMap = AttackTables::getKnightAttacks(pieceSquare);
            if ((piece % 6) == 3) attackMap = AttackTables::getRookAttacks(pieceSquare, board->All);
            if ((piece % 6) == 4) attackMap = AttackTables::getQueenAttacks(pieceSquare, board->All);
            if ((piece % 6) == 5) attackMap = AttackTables::getKingAttacks(pieceSquare);

            while (attackMap) {
                int attackSquare = getLSBIndex(attackMap);
                attackMap = popLSB(attackMap);

                if (piece < 6) attackVector.at(attackSquare)++;
                else attackVector.at(attackSquare)--;
            }
        }
    }
    return attackVector;
}

void Evaluation::initEvaluation() {
    initPassedPawns();
}

EvaluationWeights::EvaluationWeights() {
    material = 1;
    kingSafety = 1;
    activity = 1;
    pawn = 1;
    bishop = 1;
    knight = 1;
    rook = 1;
    queen = 1;
}

float Evaluation::evaluate(Board* board, EvaluationWeights weights) {
    if (board->state.checkMate == 1) return (board->state.whiteToMove ? -99999 : 99999);

    int gamePhase;
    gamePhase = determineGamePhase(board);

    std::vector<int> attackVector = getVectorOfAttackers(board);

    int opening, middleGame;
    if (gamePhase == 0) {
        opening = 1;
        middleGame = 0;
    } else {
        opening = 0;
        middleGame = 1;
    }

    int kingSafetyValue = kingSafety(board, gamePhase);
    float activityValue = pieceActivity(board, &attackVector);
    int materialValue = material(board);
    int pawnValue = pawnEval(board, gamePhase);
    int bishopValue = bishopEval(board, gamePhase);
    int knightValue = knightEval(board, gamePhase);
    int rookValue = rookEval(board, gamePhase);
    int queenValue = queenEval(board, gamePhase);
    
    return ((weights.material * materialValue) + (weights.kingSafety * kingSafetyValue) + (weights.activity * activityValue) + (weights.pawn * pawnValue) + 
            (weights.bishop * bishopValue) + (weights.knight * knightValue) + (weights.rook * rookValue) + (weights.queen * queenValue));
}
