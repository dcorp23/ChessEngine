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

enum {
    P, B, N, R, K, Q,
    p, b, n, r, k, q
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
    unsigned char whiteToMove : 1;
    unsigned char whiteShortCastle : 1;
    unsigned char whiteLongCastle : 1;
    unsigned char blackShortCastle : 1; 
    unsigned char blackLongCastle : 1;
    unsigned char enPassant : 1;
    unsigned char check : 1;
    unsigned char checkMate: 1;
};

//piece 0-5 pawn bishop knight rook queen king
struct moveCode {
    unsigned int startSquare : 6;
    unsigned int endSquare : 6;
    unsigned int piece: 3;
    unsigned int capture: 1;
    unsigned int check: 1;
    unsigned int promotion: 3;
    unsigned int whiteLong: 1;
    unsigned int whiteShort: 1;
    unsigned int blackLong: 1;
    unsigned int blackShort: 1;
    unsigned int enPassant: 1;
};

struct Board {
    map White;
    map Black;
    map All;

    map WPawn; map WKnight; map WBishop; map WRook; 
    map WQueen; map WKing;
    map BPawn; map BKnight; map BBishop; map BRook; 
    map BQueen; map BKing;

    map bitMaps[12];

    boardState state;

    Board(
        map wp, map wn, map wb, map wr, map wq, map wk,
        map bp, map bn, map bb, map br, map bq, map bk, boardState state) :
        WPawn(wp), WKnight(wn), WBishop(wb), WRook(wr), WQueen(wq), WKing(wk),
        BPawn(bp), BKnight(bn), BBishop(bb), BRook(br), BQueen(bq), BKing(bk), 
        White(wp | wn | wb | wr | wq | wk), 
        Black(bp | bn | bb | br | bq | bk), 
        All(White | Black), 
        state(state), 
        bitMaps({wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk})  
    {};

    Board(int piece, int side, map newPieceMap, Board* board, boardState newState) {
        WPawn = board->WPawn;
        WBishop = board->WBishop;
        WKnight = board->WKnight;
        WRook = board->WRook;
        WQueen = board->WQueen;
        WKing = board->WKing;
        BPawn = board->BPawn;
        BBishop = board->BBishop;
        BKnight = board->BKnight;
        BRook = board->BRook;
        BQueen = board->BQueen;
        BKing = board->BKing;

        switch (piece + (!side ? 6 : 0))
        {
        case 0:
            WPawn = newPieceMap;
            break;
        case 1:
            WBishop = newPieceMap;
            break;
        case 2:
            WKnight = newPieceMap;
            break;
        case 3:
            WRook = newPieceMap;
            break;
        case 4:
            WQueen = newPieceMap;
            break;
        case 5:
            WKing = newPieceMap;
            break;
        case 6:
            BPawn = newPieceMap;
            break;
        case 7:
            BBishop = newPieceMap;
            break;
        case 8:
            BKnight = newPieceMap;
            break;
        case 9:
            BRook = newPieceMap;
            break;
        case 10:
            BQueen = newPieceMap;
            break;
        case 11:
            BKing = newPieceMap;
            break;
        default:
            break;
        }

        White = WPawn | WBishop | WKnight | WRook | WQueen | WKing;
        Black = BPawn | BBishop | BKnight | BRook | BQueen | BKing;
        All = White | Black;
        bitMaps[0] = WPawn;
        bitMaps[1] = WBishop;
        bitMaps[2] = WKnight;
        bitMaps[3] = WRook;
        bitMaps[4] = WQueen;
        bitMaps[5] = WKing;
        bitMaps[6] = BPawn;
        bitMaps[7] = BBishop;
        bitMaps[8] = BKnight;
        bitMaps[9] = BRook;
        bitMaps[10] = BQueen;
        bitMaps[11] = BKing;

        state = newState;
    }

    Board move(moveCode code) {
        map moveMask = 0ULL;
        moveMask |= (1ULL << code.startSquare);
        moveMask |= (1ULL << code.endSquare);
        map pieceMap;
        //check for captures
        if (state.whiteToMove) {
            pieceMap = bitMaps[code.piece];
            pieceMap ^= moveMask;
        }
        else {
            pieceMap = bitMaps[code.piece + 6];
            pieceMap ^= moveMask;
        }
        boardState newState = state;
        newState.whiteToMove = !newState.whiteToMove;
        return Board(code.piece, state.whiteToMove, pieceMap, this, newState);
    };

    void printBoard() {
        map currentBit;
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                currentBit = (1ULL << ((rank * 8) + file));

                if (currentBit & All) {
                    if (currentBit & White) {
                        if (currentBit & WPawn) std::cout << " 1";
                        if (currentBit & WKnight) std::cout << " 2";
                        if (currentBit & WBishop) std::cout << " 3";
                        if (currentBit & WRook) std::cout << " 4";
                        if (currentBit & WQueen) std::cout << " 5";
                        if (currentBit & WKing) std::cout << " 9";
                    } else {
                        if (currentBit & BPawn) std::cout << "-1";
                        if (currentBit & BKnight) std::cout << "-2";
                        if (currentBit & BBishop) std::cout << "-3";
                        if (currentBit & BRook) std::cout << "-4";
                        if (currentBit & BQueen) std::cout << "-5";
                        if (currentBit & BKing) std::cout << "-9";
                    }
                } else {
                    std::cout << " 0";
                }
            }
            std::cout << '\n';
        }
    };
    
};

int main(void) {
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
}