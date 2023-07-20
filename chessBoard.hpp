#pragma once
#include <iostream>
#include "bitFunctions.hpp"

//Index a boards bitmaps by this enumeration
enum {
    WPawn, WBishop, WKnight, WRook, WQueen, WKing, 
    BPawn, BBishop, BKnight, BRook, BQueen, BKing
};

//check starting rook squares for castling rights
const map whiteShortRookStart = 1ULL << h1;
const map whiteLongRookStart = 1ULL << a1;
const map blackShortRookStart = 1ULL << h8;
const map blackLongRookStart = 1ULL << a8;

const map rank2 = 0x00FF000000000000;
const map rank7 = 0x000000000000FF00;


//maps for starting squares of all pieces
namespace StartingSquares {
const map startingKnights = (1ULL << b8) | (1ULL << g8);
const map startingBishops = (1ULL << c8) | (1ULL << f8);
const map startingRooks = (1ULL << a8) | (1ULL << h8);
const map startingQueen = (1ULL << d8);
const map startingKing = (1ULL << e8);
}

//Fen string that creates the starting position
const std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//given chess notation get integer of square e2->52
int chessNotationToInt(std::string chessNotation);

//given square integer get the chess notation 52->e2
std::string intToChessNotation(int squareNumber);

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
    unsigned char halfMove;
    unsigned char fullMove;

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
    Board(int piece, int side, map newPieceMap, Board* board, BoardState newState);

    //constructor for making a move that moves more than one piece i.e. capture castle
    Board(int piece, int side, map newPieceMap, int secondPiece, map secondPieceMap, Board* board, BoardState newState, int capture);

    //Constructor for promotions only
    Board(int side, map newPawnMap, int promotionPiece, map promotionPieceMap, int capturedPiece, map captureMap, Board* board, BoardState newState, int capture);

    //returns a new board based on the move that is given
    Board move(MoveCode code);

    //prints the all bitmaps together as one board
    void printBoard();

    //tests if all the bitmaps are the same doesn't check the state
    bool operator==(const Board otherBoard) const;

    bool operator!=(const Board otherBoard) const;
};
