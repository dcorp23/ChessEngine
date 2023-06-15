#pragma once
typedef unsigned long long map;

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

//print a single bit map in an 8x8 grid
void printMap(map printingMap);

//counts number of bits in a bit map
int getBitCount(map bitBoard);

//bit index of least significant bit
int getLSBIndex(map bitBoard);

//takes a bitMap and returns a new bitMap with the LSB removed
map popLSB(map bitBoard);