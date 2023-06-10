#include <iostream>
#include <chrono>


typedef unsigned long long map;

using std::string;

//starting positions from the white side
map startingPawns = 0x00FF000000000000;
map startingKing = 0x0800000000000000;
map startingQueen = 0x1000000000000000;
map startingBishops = 0x2400000000000000;
map startingKnights = 0x4200000000000000;
map startingRooks = 0x8100000000000000;

//board by chess notation
//use this by: 1ULL << f1 which gives a bitboard with just f1
#ifndef BOARDNOT
#define BOARDNOT
enum {
    a8, b8, c8, d8, e8, f8, g8, h8, 
    a7, b7, c7, d7, e7, f7, g7, h7, 
    a6, b6, c6, d6, e6, f6, g6, h6, 
    a5, b5, c5, d5, e5, f5, g5, h5, 
    a4, b4, c4, d4, e4, f4, g4, h4, 
    a3, b3, c3, d3, e3, f3, g3, h3, 
    a2, b2, c2, d2, e2, f2, g2, h2, 
    a1, b1, c1, d1, e1, f1, g1, h1
};
#endif //BOARDNOT

enum {
    WPawn, WBishop, WKnight, WRook, WQueen, WKing, 
    BPawn, BBishop, BKnight, BRook, BQueen, BKing
};

//print a single bit map in an 8x8 grid
void printMap(map printingMap) {
    std::cout << '\n';
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            std::cout << ((printingMap & (1ULL << ((rank * 8) + file))) ? " 1" : " 0");
        }
        std::cout << '\n';
    }
};

//starting is 0x01;
struct boardState {
    unsigned short whiteToMove : 1;
    unsigned short whiteShortCastle : 1;
    unsigned short whiteLongCastle : 1;
    unsigned short blackShortCastle : 1; 
    unsigned short blackLongCastle : 1;
    unsigned short enPassant : 6; //square that is enPassant able
                                //0 is no enpassant because you can't enpassant on a8 anyway
    unsigned short check : 1;
    unsigned short checkMate: 1;
};

//piece 0-5 pawn bishop knight rook queen king
struct moveCode {
    unsigned int startSquare : 6;
    unsigned int endSquare : 6;
    unsigned int piece: 3;
    unsigned int capture: 1;
    unsigned int promotion: 3; //piece promoting to same numbers as piece
    unsigned int whiteLong: 1;
    unsigned int whiteShort: 1;
    unsigned int blackLong: 1;
    unsigned int blackShort: 1;
    unsigned int enPassantSquare: 6; //square that is enPassant able when moving double
                            //0 is no enpassant because you can't enpassant on a8 anyway
    unsigned int enPassantFlag: 1; //if the move was enPassant
};

struct Board {
    map White;
    map Black;
    map All;

    map bitMaps[12];

    boardState state;

    Board(
        map wp, map wn, map wb, map wr, map wq, map wk,
        map bp, map bn, map bb, map br, map bq, map bk, boardState state) :
        White(wp | wn | wb | wr | wq | wk), 
        Black(bp | bn | bb | br | bq | bk), 
        All(White | Black), 
        state(state), 
        bitMaps{wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk}
    {};

    Board(int piece, int side, map newPieceMap, int secondPiece, map secondPieceMap, Board* board, boardState newState) {
        for (int i = WPawn; i <= BKing; i++) {
            bitMaps[i] = board->bitMaps[i];
        }

        bitMaps[piece + (side ? 0 : 6)] = newPieceMap;
        if (secondPieceMap != 0ULL) bitMaps[secondPiece] = secondPieceMap;

        White = 0ULL;
        Black = 0ULL;
        for (int i = WPawn; i <= WKing; i++) {
            White |= bitMaps[i];
        }
        for (int i = BPawn; i <= BKing; i++) {
            Black |= bitMaps[i];
        }
        All = White | Black;
        
        state = newState;
    }

    Board move(moveCode code) {
        map moveMask = 0ULL;
        moveMask |= (1ULL << code.startSquare);
        moveMask |= (1ULL << code.endSquare);
        //map with piece in updated square
        map pieceMap;
        
        if (state.whiteToMove) {
            pieceMap = bitMaps[code.piece];
            pieceMap ^= moveMask;
        }
        else {
            pieceMap = bitMaps[code.piece + 6];
            pieceMap ^= moveMask;
        }

        boardState newState = state;
        newState.enPassant = code.enPassantSquare;
        newState.whiteToMove = !newState.whiteToMove;

        if(code.enPassantFlag) {
            map captureMap = (1ULL << (state.enPassant + (state.whiteToMove ? 8 : -8)));
            captureMap ^= bitMaps[0 + (state.whiteToMove ? 6 : 0)];

            return Board(code.piece, state.whiteToMove, pieceMap, 0 + (state.whiteToMove ? 6 : 0), captureMap, this, newState);
        }

        //check for captures
        if (code.capture) {
            map captureMap = 1ULL << code.endSquare;

            int startPiece = state.whiteToMove ? 6 : 0;
            int endPiece = state.whiteToMove ? 12 : 6;
            int secondPiece = 0;
            for (int i = startPiece; i < endPiece; i++) {
                if (captureMap & bitMaps[i]) {
                    captureMap ^= bitMaps[i];
                    secondPiece = i;
                }
            }

            return Board(code.piece, state.whiteToMove, pieceMap, secondPiece,captureMap, this, newState);
        }

        //long castle
        if (code.blackLong || code.whiteLong) {
            map rookMap = 1ULL << (code.whiteLong ? a1 : a8);
            rookMap |= 1ULL << (code.whiteLong ? d1 : d8);

            rookMap ^= bitMaps[3 + (code.whiteLong ? 0 : 6)];

            return Board(code.piece, state.whiteToMove, pieceMap, 3 + (code.whiteLong ? 0 : 6), rookMap, this, newState);
        }

        //short castle
        if (code.blackShort || code.whiteShort) {
            map rookMap = (1ULL << code.whiteLong ? h1 : h8);
            rookMap |= (1ULL << code.whiteLong ? f1 : f8);

            rookMap ^= bitMaps[3 + (code.whiteLong ? 0 : 6)];

            return Board(code.piece, state.whiteToMove, pieceMap, 3 + (code.whiteLong ? 0 : 6), rookMap, this, newState);
        }
        
        //quiet moves
        return Board(code.piece, state.whiteToMove, pieceMap, 0, 0ULL, this, newState);
    };

    void printBoard() {
        map currentBit;

        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                currentBit = (1ULL << ((rank * 8) + file));

                if (currentBit & All) {
                    if (currentBit & White) {
                        if (currentBit & bitMaps[WPawn]) std::cout << " 1";
                        if (currentBit & bitMaps[WKnight]) std::cout << " 2";
                        if (currentBit & bitMaps[WBishop]) std::cout << " 3";
                        if (currentBit & bitMaps[WRook]) std::cout << " 4";
                        if (currentBit & bitMaps[WQueen]) std::cout << " 5";
                        if (currentBit & bitMaps[WKing]) std::cout << " 9";
                    } else {
                        if (currentBit & bitMaps[BPawn]) std::cout << "-1";
                        if (currentBit & bitMaps[BKnight]) std::cout << "-2";
                        if (currentBit & bitMaps[BBishop]) std::cout << "-3";
                        if (currentBit & bitMaps[BRook]) std::cout << "-4";
                        if (currentBit & bitMaps[BQueen]) std::cout << "-5";
                        if (currentBit & bitMaps[BKing]) std::cout << "-9";
                    }
                } else {
                    std::cout << " 0";
                }
            }
            std::cout << '\n';
        }
    };
    
};

/* int main(void) {
    boardState startingState;
    startingState.whiteToMove = 1;

    Board board = Board(startingPawns, startingKnights, startingBishops, startingRooks, startingQueen, startingKing, 
            startingPawns >> 40, startingKnights >> 56, startingBishops >> 56, startingRooks >> 56, startingQueen >> 56, startingKing >> 56,
            startingState);

    board.printBoard();

    std::cout << '\n';

    moveCode move;
    move.startSquare = e2;
    move.endSquare = e4;
    move.piece = 0;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';

    move.startSquare = e7;
    move.endSquare = e5;
    move.piece = 0;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';

    move.startSquare = f1;
    move.endSquare = c4;
    move.piece = 1;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';
} */