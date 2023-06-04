#include <iostream>
#include "attackTables.cpp"

typedef unsigned long long map;

using std::string;

//starting positions from the white side
const map Pawns = 0x00FF000000000000;
const map King = 0x0800000000000000;
const map Queen = 0x1000000000000000;
const map Bishops = 0x2400000000000000;
const map Knights = 0x4200000000000000;
const map Rooks = 0x8100000000000000;

//board by chess notation
//use this by: 1ULL << f1 which gives a bitboard with just f1
/*
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
*/

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

struct Board {
    const map White;
    const map Black;
    const map All;

    const map WPawn; const map WKnight; map WBishop; const map WRook; 
    const map WQueen; const map WKing;
    const map BPawn; const map BKnight; const map BBishop; const map BRook; 
    const map BQueen; const map BKing;

    Board(
        map wp, map wn, map wb, map wr, map wq, map wk,
        map bp, map bn, map bb, map br, map bq, map bk) :
        WPawn(wp), WKnight(wn), WBishop(wb), WRook(wr), WQueen(wq), WKing(wk),
        BPawn(bp), BKnight(bn), BBishop(bb), BRook(br), BQueen(bq), BKing(bk),
        White(wp | wn | wb | wr | wq | wk),
        Black(bp | bn | bb | br | bq | bk),
        All(Black | White)
    {};

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
    Board board = Board(Pawns, Knights, Bishops, Rooks, Queen, King, 
                        Pawns>>40, Knights>>56, Bishops>>56, Rooks>>56, Queen>>56, King>>56);

    initMagicNumbers();

    return 0;
}
