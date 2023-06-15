#include <iostream>
#include "bitFunctions.hpp"

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

//counts number of bits in a bit map
int getBitCount(map bitBoard) {
    int bitCount = 0;
    
    //subtracting 1 flips trailing 0s and the original 1 is fliped
    //ex 100100 -> 100011 because you borrow from the next binary place
    //so &ing them together deletes the LSB and you just do it till 0 and count
    while (bitBoard) {
        bitBoard &= bitBoard - 1;
        bitCount++;
    }

    return bitCount;   
};

//bit index of least significant bit
int getLSBIndex(map bitBoard) {
    if (bitBoard) {
        return getBitCount((bitBoard & -bitBoard) - 1);
    } else {
        return -1;
    }
};

map popLSB(map bitBoard) {
    return (bitBoard ^ (1ULL << getLSBIndex(bitBoard)));
};