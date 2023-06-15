#pragma once
#include <iostream>

typedef unsigned long long map;

//shift 1ULL left by this enumeration to get the square
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

//Index a boards bitmaps by this enumeration
enum {
    WPawn, WBishop, WKnight, WRook, WQueen, WKing, 
    BPawn, BBishop, BKnight, BRook, BQueen, BKing
};

//Fen string that creates the starting position
const std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//Unsigned short holding information on the current board
struct BoardState {
    unsigned short whiteToMove : 1;
    unsigned short whiteShortCastle : 1;
    unsigned short whiteLongCastle : 1;
    unsigned short blackShortCastle : 1; 
    unsigned short blackLongCastle : 1;
    unsigned short enPassant : 6; //square that is enPassant able
                                //0 is no enpassant because you can't enpassant on a8 anyway
    unsigned short check : 1;
    unsigned short checkMate: 1;

    BoardState();
};

//piece 0-5 pawn bishop knight rook queen king
//Unsigned int that is used to make a move on a board
struct MoveCode {
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

    //prints the piece what squares, capture and promotion
    void printCode();
};

struct Board {
    map White;
    map Black;
    map All;

    map bitMaps[12];

    BoardState state;

    //default constructor for blank board
    Board();

    //FEN string to board
    Board(std::string fenString);

    //constructor for making a move thats not a promotion
    Board(int piece, int side, map newPieceMap, int secondPiece, map secondPieceMap, Board* board, BoardState newState, int capture);

    //Constructor for promotions only
    Board(int side, map newPawnMap, int promotionPiece, map promotionPieceMap, int capturedPiece, map captureMap, Board* board, BoardState newState, int capture);

    //returns a new board based on the move that is given
    Board move(MoveCode code);

    //prints the all bitmaps together as one board
    void printBoard();

    //tests if all the bitmaps are the same doesn't check the state
    bool isEqual(Board board);
};
