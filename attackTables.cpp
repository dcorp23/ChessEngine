#include <algorithm>
#include <iostream>
#include "attackTables.hpp"
#include "bitFunctions.hpp"

//constants to remove bad moves
const map NOT_A_FILE = 0xFEFEFEFEFEFEFEFE;
const map NOT_AB_FILE = 0xFCFCFCFCFCFCFCFC;
const map NOT_H_FILE = 0x7F7F7F7F7F7F7F7F;
const map NOT_HG_FILE = 0x3F3F3F3F3F3F3F3F;

//all pawn attacks 2 sides and 64 squares
map pawnAttacks[2][64];

map knightAttacks[64];

map kingAttacks[64];

/* max occupancies based on the highest relevant squares attacked 
from any square which is 9 for bishops and 12 for rooks
so the max occupancy would be 2^9 and 2^12 */

//all bishop attacks 64 squares max 512 variations of occupancies
map bishopAttacks[64][512];

//all bishop attacks 64 squares max 4096 variations of occupancies
map rookAttacks[64][4096];

map bishopMasks[64];
map rookMasks[64];

//for each square shows number of
//relevant squares a bishop can go to
const int relevantBishopBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

//for each square shows number of
//relevant squares a rook can go to
const int relevantRookBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

const map bishopMagicNumbers[64] = {
    0x8004212801130202ULL,
    0x883130008101c020ULL,
    0x141840100410001ULL,
    0x8820a0200900400ULL,
    0x2204042010030000ULL,
    0x101010840800000ULL,
    0x481080104201004ULL,
    0x2228050082084ULL,
    0x40900421140402ULL,
    0x10105002a8110023ULL,
    0x11084224002410ULL,
    0x40080602400012ULL,
    0x24411040040c00ULL,
    0x40108a0802382010ULL,
    0x4204104104001ULL,
    0x8010010880900800ULL,
    0x3842024828010401ULL,
    0x18102012048213ULL,
    0x50083101020414ULL,
    0x400800802004700ULL,
    0xa044102202020205ULL,
    0x483014808821001ULL,
    0x84404401145100ULL,
    0xa6004020840440ULL,
    0x10042008481002ULL,
    0x855c41020280208ULL,
    0x840208050010044ULL,
    0x114080044010411ULL,
    0xe41001009004000ULL,
    0x8092041112004200ULL,
    0x8023100410402ULL,
    0x2002000808800ULL,
    0x1582802202060ULL,
    0x402869088081010ULL,
    0x318203004080288ULL,
    0x12004041c40100ULL,
    0x44004010240100ULL,
    0x888900280110080ULL,
    0x2b288021000420dULL,
    0x11022421a08402ULL,
    0x821110404000ULL,
    0x32206808080a0400ULL,
    0x2010080500aa100ULL,
    0xc020124000202ULL,
    0x6000040104001211ULL,
    0x44881004400408ULL,
    0x4090900200484080ULL,
    0x410020041008051ULL,
    0x4110862108040ULL,
    0x940340104100403ULL,
    0x80c0004218040080ULL,
    0x2a20021020880005ULL,
    0xa920a04424080aULL,
    0x41042021420ULL,
    0x91210604094000ULL,
    0x100102140044ceULL,
    0x880240444c206004ULL,
    0x400004402099030ULL,
    0x8204c022100ULL,
    0x8828180100840400ULL,
    0x70020a102400ULL,
    0x22080404081202ULL,
    0x3080208404080048ULL,
    0x22680808005040ULL
};

const map rookMagicNumbers[64] = {
    0x200102201004080ULL,
    0x8040200040001000ULL,
    0x1080088010002000ULL,
    0x5080100080080004ULL,
    0x8480223800802400ULL,
    0xa900280100024400ULL,
    0x480110000801200ULL,
    0x8100082042088100ULL,
    0x500a002604804100ULL,
    0x4030808040002000ULL,
    0xc0802000100080ULL,
    0x8802001140092200ULL,
    0x40088048009c0080ULL,
    0x800400800200ULL,
    0x9484800100020080ULL,
    0x1001800100004480ULL,
    0x80084000200040ULL,
    0x30004040002004ULL,
    0x4102888020021000ULL,
    0x416808010000800ULL,
    0x408004004004200ULL,
    0xa000080104201040ULL,
    0x2010100040200ULL,
    0x220020009084084ULL,
    0x6480004040002000ULL,
    0x6420022280400280ULL,
    0xc10080020200400ULL,
    0x428100100090420ULL,
    0x810040080080080ULL,
    0x400040080020080ULL,
    0x108120c00902108ULL,
    0x2020801c80044100ULL,
    0x80002000400040ULL,
    0x840002801a01002ULL,
    0x20802012004200ULL,
    0xa000801000800800ULL,
    0x4004004008080080ULL,
    0x260040080800200ULL,
    0x21004000801ULL,
    0x9810142082000841ULL,
    0x28400880218000ULL,
    0x220402010004002ULL,
    0x2200041030012ULL,
    0xa18002010010100ULL,
    0x480080100050010ULL,
    0x8401000400090002ULL,
    0x10080110040002ULL,
    0x4080012040820004ULL,
    0x4030400880082080ULL,
    0x4400400080200180ULL,
    0x1050200010008080ULL,
    0x1800200900100100ULL,
    0x20080004008080ULL,
    0x1701002400088300ULL,
    0x400110248301400ULL,
    0x8008800100004080ULL,
    0x100142040800103ULL,
    0x2048021081042ULL,
    0xa04041000c200011ULL,
    0x26024b0000901ULL,
    0x41030008000224d1ULL,
    0x2001804011026ULL,
    0x21800d00104ULL,
    0x201000032008041ULL
};

//psuedo RNG class
class PseudoRNG {
public:
    map seed;

    PseudoRNG(map seed) {
        this->seed = seed;
    }

    map nextRN() {
        seed += 0x9e3779b97f4a7c15;                // increment the seed variable 
        map z = seed;                              // copy the seed to a working variable 
        z = (z ^ (z >> 15)) * 0xbf58476d1ce4e5b9;  // xor the variable with the variable right bit shifted 30 then multiply by a constant 
        z = (z ^ (z << 12)) * 0x94d049bb133111eb;  // xor the variable with the variable right bit shifted 27 then multiply by a constant 
        return z ^ (z >> 9);                       // return the variable xored with itself right bit shifted 31 
    }

    map magicNumberCandidate() {
        return (nextRN() & nextRN() & nextRN());
    }
};

PseudoRNG rng = PseudoRNG(0x66ffcd0e);

/*
Takes the attack mask and returns a version of it where bits are occupied
instead of being attacked by the piece;
int index - which version of the occupancy board 0 is all blocked (2^bitcount - 1) is none blocked
*/
map setOccupancy(int index, int bitCount, map attackMask) {
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
map getPawnAttackMask(int square, int side) {
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

map getKnightAttackMask(int square) {
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

map getKingAttackMask(int square) {
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
map getRookAttackMask(int square) {
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

map getBishopAttackMask(int square) {
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

map getQueenAttackMask(int square) {
    map attacks = 0ULL;
    
    attacks |= getBishopAttackMask(square);
    attacks |= getRookAttackMask(square);

    return attacks;
}

//takes an occupancy mask and will stop attack rays
//when hitting a blocker, the rays will go to the edge of the board
map getBishopAttackMaskWithBlockers(int square, map blockers) {
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
map getRookAttackMaskWithBlockers(int square, map blockers) {
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
map findMagicNumber(int square, int relevantBits, int bishop) {
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
void initJumpingPieceAttacks() {
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
void initSlidingPieceAttack(int bishop) {
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

void initMagicNumbers() {
    std::cout << "ROOKS\n\n\n";
    for (int i = 0; i < 64; i++) {
        std::cout << "0x" << std::hex << findMagicNumber(i, relevantRookBits[i], 0) << "ULL,\n";
    }
    std::cout << "\n\n BISHOPS \n\n\n";
    for (int i = 0; i < 64; i++) {
        std::cout << "0x" << std::hex << findMagicNumber(i, relevantBishopBits[i], 1) << "ULL,\n";
    }
};

void AttackTables::initAttackTables() {
    initJumpingPieceAttacks();
    initSlidingPieceAttack(0); //rooks
    initSlidingPieceAttack(1); //bishops
}

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