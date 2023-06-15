typedef unsigned long long map;
#include <algorithm>
#include <iostream>
#include "attackTables.hpp"

//board by chess notation
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

//helper functions

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

PseudoRNG::PseudoRNG() {};

//psuedo RNG class
PseudoRNG::PseudoRNG(map seed) {
    this->seed = seed;
}

map PseudoRNG::nextRN() {
    seed += 0x9e3779b97f4a7c15;                // increment the seed variable 
    map z = seed;                              // copy the seed to a working variable 
    z = (z ^ (z >> 15)) * 0xbf58476d1ce4e5b9;  // xor the variable with the variable right bit shifted 30 then multiply by a constant 
    z = (z ^ (z << 12)) * 0x94d049bb133111eb;  // xor the variable with the variable right bit shifted 27 then multiply by a constant 
    return z ^ (z >> 9);                       // return the variable xored with itself right bit shifted 31 
}

map PseudoRNG::magicNumberCandidate() {
    return (nextRN() & nextRN() & nextRN());
}

/*
Takes the attack mask and returns a version of it where bits are occupied
instead of being attacked by the piece;
int index - which version of the occupancy board 0 is all blocked (2^bitcount - 1) is none blocked
*/
map AttackTables::setOccupancy(int index, int bitCount, map attackMask) {
    map occupancyBoard = 0ULL;

    for (int i = 0; i < bitCount; i++) {
        int square = getLSBIndex(attackMask);
        attackMask ^= (1ULL << square);
        if (index & (1ULL << i)) {
            occupancyBoard |= (1ULL << square);
        }
    }

    return occupancyBoard;
}


//each attack mask takes the starting square
//and returns a bitboard of the squares attacked from 
//that square sliding pieces don't cover the edges

//side either white = 1, black = 0;
map AttackTables::getPawnAttackMask(int square, int side) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;

    if (side) {
        attacks |= ((NOT_H_FILE & startBoard) >> 7);
        attacks |= ((NOT_A_FILE & startBoard) >> 9);
    }
    else {
        attacks |= ((NOT_A_FILE & startBoard) << 7);
        attacks |= ((NOT_H_FILE & startBoard) << 9);
    }
    return attacks;
}

map AttackTables::getKnightAttackMask(int square) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;
    
    attacks |= ((NOT_A_FILE & startBoard) << 15);
    attacks |= ((NOT_H_FILE & startBoard) << 17);
    attacks |= ((NOT_AB_FILE & startBoard) << 6);
    attacks |= ((NOT_HG_FILE & startBoard) << 10);

    attacks |= ((NOT_H_FILE & startBoard) >> 15);
    attacks |= ((NOT_A_FILE & startBoard) >> 17);
    attacks |= ((NOT_HG_FILE & startBoard) >> 6);
    attacks |= ((NOT_AB_FILE & startBoard) >> 10);

    return attacks;
};

map AttackTables::getKingAttackMask(int square) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;
    
    attacks |= (startBoard >> 8);
    attacks |= ((NOT_H_FILE & startBoard) >> 7);
    attacks |= ((NOT_A_FILE & startBoard) >> 9);

    attacks |= ((NOT_H_FILE & startBoard) << 1);
    attacks |= ((NOT_A_FILE & startBoard) >> 1);

    attacks |= (startBoard << 8);
    attacks |= ((NOT_A_FILE & startBoard) << 7);
    attacks |= ((NOT_H_FILE & startBoard) << 9);

    return attacks;
}

//attack mask to get the relevant bits for a rook and bishop attack
//edges aren't relevant because if there is a piece on the
//edge it doesn't block other squares so we can exclude it
//to save space on the amount of occupancy variations
map AttackTables::getRookAttackMask(int square) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;
    
    //rank 1 is 8 on a chess board
    int rank = (square / 8) + 1;
    //file 1 is a on a chess board
    int file = (square % 8) + 1;

    for (int i = 1; i <= (8 - rank) - 1; i++) {
        attacks |= (startBoard << (i * 8));
    }
    for (int i = 1; i < rank - 1; i++) {
        attacks |= (startBoard >> (i * 8));
    }

    for (int i = 1; i <= (8 - file) - 1; i++) {
        attacks |= (startBoard << i);
    }
    for (int i = 1; i < file - 1; i++) {
        attacks |= (startBoard >> i);
    }

    return attacks;
}

map AttackTables::getBishopAttackMask(int square) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;
    
    //rank 1 is 8 on a chess board
    int rank = (square / 8) + 1;
    //file 1 is a on a chess board
    int file = (square % 8) + 1;

    //up right ray
    for (int i = 1; i <= std::min(rank - 1, 8 - file) - 1; i++) {
        attacks |= ((startBoard >> (i * 8)) << i);
    }

    //down right ray
    for (int i = 1; i <= std::min(8 - rank, 8 - file) - 1; i++) {
        attacks |= ((startBoard << (i * 8)) << i);
    }

    //up left ray
    for (int i = 1; i <= (std::min(rank - 1, file - 1)) - 1; i++) {
        attacks |= ((startBoard >> (i * 8)) >> i);
    }

    //down left ray
    for (int i = 1; i <= (std::min(8 - rank, file - 1)) - 1; i++) {
        attacks |= ((startBoard << (i * 8)) >> i);
    }

    return attacks;
}

map AttackTables::getQueenAttackMask(int square) {
    map attacks = 0ULL;
    
    attacks |= getBishopAttackMask(square);
    attacks |= getRookAttackMask(square);

    return attacks;
}

//takes an occupancy mask and will stop attack rays
//when hitting a blocker, the rays will go to the edge of the board
map AttackTables::getBishopAttackMaskWithBlockers(int square, map blockers) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;
    
    //rank 1 is 8 on a chess board
    int rank = (square / 8) + 1;
    //file 1 is a on a chess board
    int file = (square % 8) + 1;

    //up right ray
    for (int i = 1; i <= std::min(rank - 1, 8 - file); i++) {
        attacks |= ((startBoard >> (i * 8)) << i);
        if (((startBoard >> (i * 8)) << i) & blockers) break;
    }

    //down right ray
    for (int i = 1; i <= std::min(8 - rank, 8 - file); i++) {
        attacks |= ((startBoard << (i * 8)) << i);
        if (((startBoard << (i * 8)) << i) & blockers) break;
    }

    //up left ray
    for (int i = 1; i <= (std::min(rank - 1, file - 1)); i++) {
        attacks |= ((startBoard >> (i * 8)) >> i);
        if (((startBoard >> (i * 8)) >> i) & blockers) break;
    }

    //down left ray
    for (int i = 1; i <= (std::min(8 - rank, file - 1)); i++) {
        attacks |= ((startBoard << (i * 8)) >> i);
        if (((startBoard << (i * 8)) >> i) & blockers) break;
    }

    return attacks;
}

//takes an occupancy mask and will stop attack rays
//when hitting a blocker, the rays will go to the edge of the board
map AttackTables::getRookAttackMaskWithBlockers(int square, map blockers) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;
    
    //rank 1 is 8 on a chess board
    int rank = (square / 8) + 1;
    //file 1 is a on a chess board
    int file = (square % 8) + 1;

    //Down ray
    for (int i = 1; i <= (8 - rank); i++) {
        attacks |= (startBoard << (i * 8));
        if ((startBoard << (i * 8)) & blockers) break;
    }

    //Up ray
    for (int i = 1; i < rank; i++) {
        attacks |= (startBoard >> (i * 8));
        if ((startBoard >> (i * 8)) & blockers) break;
    }

    //Right ray
    for (int i = 1; i <= (8 - file); i++) {
        attacks |= (startBoard << i);
        if ((startBoard << i) & blockers) break;
    }

    //Left ray
    for (int i = 1; i < file; i++) {
        attacks |= (startBoard >> i);
        if ((startBoard >> i) & blockers) break;
    }

    return attacks;
}

/* 
finds magic number for square through brute force trial and error
bishop = 1, rook = 0; 
*/
map AttackTables::findMagicNumber(int square, int relevantBits, int bishop) {
    map occupancies[4096];
    map attacks[4096];
    map usedAttacks[4096];
    
    //get attack mask from square
    map attackMask = bishop ? getBishopAttackMask(square) : getRookAttackMask(square);

    //2^number of bits in an attack mask at that square
    //because there are that many occupancy variations for that attack mask
    int occupancyIndicies = 1ULL << relevantBits;

    //loop through those occupancy variations
    for (int i = 0; i < occupancyIndicies; i++) {
        //create the occupancy array
        occupancies[i] = setOccupancy(i, relevantBits, attackMask);

        //set the attacks for that occupancy where the whole ray is blocked
        attacks[i] = bishop ? getBishopAttackMaskWithBlockers(square, occupancies[i]) : 
                                getRookAttackMaskWithBlockers(square, occupancies[i]);
    }

    //generating a random number
    for (int i = 0; i < 10000000; i++) {
        map magicNumber = rng.magicNumberCandidate(); //actual number

        //filter bad magic numbers
        if (getBitCount((attackMask * magicNumber) & 0xFF00000000000000) < 6) continue;

        //initialized usedAttacks at 0
        std::fill(usedAttacks, usedAttacks + 4096, 0ULL);

        int index, fail;
        //loop through attacks of all variations of the occupancy attack maps or till failure
        for (index = 0, fail = 0; fail != 1 && index < occupancyIndicies; index++) {
            int magicIndex = (int)((occupancies[index] * magicNumber) >> (64 - relevantBits));

            //sets the used attack if its 0
            if (usedAttacks[magicIndex] == 0ULL) {
                usedAttacks[magicIndex] = attacks[index];
            } 
            //checks if the usedAttack matches the correct attack for all indicies
            else if (usedAttacks[magicIndex] != attacks[index]) {
                fail = 1;
            }
        }
        if (!fail) {
            return magicNumber;
        }
    }

    std::cout << "Magic Number fails\n";
    return 0ULL;
}

/* max occupancies based on the highest relevant squares attacked 
from any square which is 9 for bishops and 12 for rooks
so the max occupancy would be 2^9 and 2^12 */

//pawns, knights, kings
//can't be blocked by a piece from attacking a square
void AttackTables::initJumpingPieceAttacks() {
    //loop over squares on the board
    for (int i = 0; i < 64; i++) {
        pawnAttacks[0][i] = getPawnAttackMask(i, 0); //black pawns
        pawnAttacks[1][i] = getPawnAttackMask(i, 1); //white pawns

        knightAttacks[i] = getKnightAttackMask(i); //knights

        kingAttacks[i] = getKingAttackMask(i); //king
    }
}

//bishops, rooks, queens
//can be blocked by a piece from attacking a square
void AttackTables::initSlidingPieceAttack(int bishop) {
    //loop over all board squares
    for (int square = 0; square < 64; square++) {
        bishopMasks[square] = getBishopAttackMask(square);
        rookMasks[square] = getRookAttackMask(square);

        map attackMask = bishop ? bishopMasks[square] : rookMasks[square];

        int relevantBitCount = getBitCount(attackMask);

        int occupancyIndicies = (1 << relevantBitCount);

        for (int index = 0; index < occupancyIndicies; index++) {
            if (bishop) {
                map occupancy = setOccupancy(index, relevantBitCount, attackMask);
                int magicIndex = (occupancy * bishopMagicNumbers[square]) >> (64 - relevantBitCount);
                bishopAttacks[square][magicIndex] = getBishopAttackMaskWithBlockers(square, occupancy);
            } else {
                map occupancy = setOccupancy(index, relevantBitCount, attackMask);
                int magicIndex = (occupancy * rookMagicNumbers[square]) >> (64 - relevantBitCount);
                rookAttacks[square][magicIndex] = getRookAttackMaskWithBlockers(square, occupancy);
            }
        }
    }
}

void AttackTables::initMagicNumbers() {
    std::cout << "ROOKS\n\n\n";
    for (int i = 0; i < 64; i++) {
        std::cout << "0x" << std::hex << findMagicNumber(i, relevantRookBits[i], 0) << "ULL,\n";
    }
    std::cout << "\n\n BISHOPS \n\n\n";
    for (int i = 0; i < 64; i++) {
        std::cout << "0x" << std::hex << findMagicNumber(i, relevantBishopBits[i], 1) << "ULL,\n";
    }
};

AttackTables::AttackTables() {
    rng = PseudoRNG(0x66ffcd0e);
    initJumpingPieceAttacks();
    initSlidingPieceAttack(0); //rooks
    initSlidingPieceAttack(1); //bishops
};

//side white = 1; black = 0;
map AttackTables::getPawnAttacks(int square, int side) {
    return pawnAttacks[side][square];
};

map AttackTables::getKnightAttacks(int square) {
    return knightAttacks[square];
}

map AttackTables::getKingAttacks(int square) {
    return kingAttacks[square];
};

map AttackTables::getBishopAttacks(int square, map occupancy) {
    occupancy &= bishopMasks[square];
    occupancy *= bishopMagicNumbers[square];
    occupancy >>= (64 - relevantBishopBits[square]);

    return bishopAttacks[square][occupancy];
};

map AttackTables::getRookAttacks(int square, map occupancy) {
    occupancy &= rookMasks[square];
    occupancy *= rookMagicNumbers[square];
    occupancy >>= (64 - relevantRookBits[square]);

    return rookAttacks[square][occupancy];
};

map AttackTables::getQueenAttacks(int square, map occupancy) {
    return getBishopAttacks(square, occupancy) ^ getRookAttacks(square, occupancy);
};