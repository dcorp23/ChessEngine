typedef unsigned long long map;

//board by chess notation
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

//constants to remove bad moves
const map NOT_A_FILE = 0xFEFEFEFEFEFEFEFE;
const map NOT_AB_FILE = 0xFCFCFCFCFCFCFCFC;
const map NOT_H_FILE = 0x7F7F7F7F7F7F7F7F;
const map NOT_HG_FILE = 0x3F3F3F3F3F3F3F3F;

//psuedo RNG class
class PseudoRNG {
    public: 
        map seed;

        PseudoRNG(map seed) {
            this->seed = seed;
        }

        map nextRN() {
            seed += 0x9e3779b97f4a7c15;                /* increment the seed variable */
            map z = seed;                              /* copy the seed to a working variable */
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;  /* xor the variable with the variable right bit shifted 30 then multiply by a constant */
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;  /* xor the variable with the variable right bit shifted 27 then multiply by a constant */
            return z ^ (z >> 31);                      /* return the variable xored with itself right bit shifted 31 */
        }
};

//global rng
PseudoRNG rng = PseudoRNG(1804289383);


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
}

//bit index of least significant bit
int getLSBIndex(map bitBoard) {
    if (bitBoard) {
        return getBitCount((bitBoard & -bitBoard) - 1);
    } else {
        return -1;
    }
}

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
    0xe4d15a015a60f865ULL,
    0x6c1d7b088e612181ULL,
    0x1b0a98aa766833e3ULL,
    0xf786d8b2a49b1610ULL,
    0x21ac4c4f0f6a6676ULL,
    0xbeb01ec8ed3115e8ULL,
    0x7982c2a25bf347caULL,
    0x9d06649dfccf4385ULL,
    0xb42e25705b2d8978ULL,
    0x2058f416ea8d6381ULL,
    0x5086f0f4080c4d79ULL,
    0x4a68c30a44d63e48ULL,
    0x653c35279464f972ULL,
    0x874bd3b652683e9cULL,
    0x10129adfe483798dULL,
    0xfa9c9aa3bcdd3db7ULL,
    0xb016ef4f4434c3adULL,
    0x835ba85de7d8ba77ULL,
    0xda68243cc18009d0ULL,
    0xe98fffe3f916cb2cULL,
    0x918cdcb542e2ad4eULL,
    0x1a19e008971efe33ULL,
    0xce279926bb12f8fdULL,
    0xf21f5a06122ceff7ULL,
    0xa5dbdefc633ea09cULL,
    0xe338185cafaec99ULL,
    0xdc9d05946c35e2b1ULL,
    0x8778bf63484e6b66ULL,
    0x7d45780810e757c9ULL,
    0xc23007a29fe33df4ULL,
    0xde61da510ff002f3ULL,
    0xb010b48ce12fb816ULL,
    0xa02ec03b601539ebULL,
    0x2450d1a2ddfdba4aULL,
    0x638c781816c08590ULL,
    0x974336b796aa1fcaULL,
    0x1252f9d04b4fc29ULL,
    0xc4679a2f3d69a7f0ULL,
    0x589bec9634d04ULL,
    0xc3fe7720904c5f96ULL,
    0x5325faa282b997d2ULL,
    0xc7ac712581c48809ULL,
    0x2019b05319781a7bULL,
    0xd997e6bcf93f2e1bULL,
    0x9cbfa92c0173a413ULL,
    0x7b480012431ed872ULL,
    0x157e1caf3ee25679ULL,
    0x74ddb69569e1f5e7ULL,
    0x77bc179cfff99c28ULL,
    0xcb0417a516207a8dULL,
    0xf3f2585771a1c5dbULL,
    0x9938686127be4cffULL,
    0x62ca09a074b1818ULL,
    0x786e3807fb236f7bULL,
    0xdc0b91643f7c0173ULL,
    0xcad6e9c5a045c292ULL,
    0x2cdab4bebc27e627ULL,
    0xea9026afd6ab5fc3ULL,
    0xcd555a499880ff56ULL,
    0x933e37b233d79e20ULL,
    0x574ac91c5caa7317ULL,
    0x4cd98c81bbcc9a00ULL,
    0x74a5a11aa162f88fULL,
    0x9e70025298550564ULL
};

const map rookMagicNumbers[64] = {
    0xe62cda4fc5d0bedeULL,
    0x71db0f33edbf0dacULL,
    0x15a4e2c272a6e021ULL,
    0xdd054353dde9bf34ULL,
    0xa8d8887b552e86eeULL,
    0x8539ff6d3c740979ULL,
    0xc17daff803baeb9bULL,
    0xdba87d70dc9b9e71ULL,
    0x34d9320d8378ca8fULL,
    0x101fd1629e43ad32ULL,
    0xd138e0f8988363a5ULL,
    0x78e15b4c8a66a298ULL,
    0x4d192406b7efec42ULL,
    0x3b23cc7e5fd08b6aULL,
    0x4ea6175171454b9cULL,
    0xef4c6112593fb469ULL,
    0x1dd4f175bc7d9e4aULL,
    0x8f771cdef34274e6ULL,
    0xdfd88a052f531d2dULL,
    0x9827377940a1eaafULL,
    0x7b02573dc3341acbULL,
    0x8f7165e9882bd590ULL,
    0x1b8ca000a707651ULL,
    0x641be0cab8ab7fe7ULL,
    0xb847a7f8a5862301ULL,
    0x185afb8f2366b4aaULL,
    0x7cce587f2c77ac17ULL,
    0x725bcbec398617cULL,
    0x44c065c38e0e3ad7ULL,
    0x19991c44d6964affULL,
    0x881163237fa72deeULL,
    0x852c391fd5e173faULL,
    0x7e967e9eeeac6687ULL,
    0x7e46d73dd9886389ULL,
    0x4d1a26162ec4824fULL,
    0xfb033ce26c08e8f0ULL,
    0x263e940bb6ee31cdULL,
    0xd35e36fe45bc7ae4ULL,
    0xfabc819e06604fd9ULL,
    0x830b151d5ded7b3dULL,
    0xf4cb48d4c8ff316bULL,
    0x48cf7b0bc821426aULL,
    0xcdd45a7d6ea01231ULL,
    0x61b1dfd4cf13d5c7ULL,
    0xe0b00f9db4f28800ULL,
    0xd571cf0f179cbb03ULL,
    0x34124d253411a525ULL,
    0x2e13bf87f6f7ced2ULL,
    0x710ee20b677fdb93ULL,
    0xe6a3973a15b5a0f6ULL,
    0x4d47375a8aa0d382ULL,
    0xe46a12d5b2c47777ULL,
    0xe3fd82fc656c21aeULL,
    0x647076f86ab92fbaULL,
    0x7299fc40dac1d6e0ULL,
    0x85723189a48adc88ULL,
    0xb7086ef4fe20a561ULL,
    0xc42c9224501e4816ULL,
    0xcec2ac22a663bc7bULL,
    0x19ece37845444285ULL,
    0x6004a6c872fa4571ULL,
    0xa00cbd7d629d97b1ULL,
    0xe97bd18a9965858eULL,
    0xa2ebb92e21416d54ULL
};

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
        //attacks |= (startBoard >> 8);
        attacks |= ((NOT_H_FILE & startBoard) >> 7);
        attacks |= ((NOT_A_FILE & startBoard) >> 9);

        /*
        if (square >= a2) {
            attacks |= (startBoard >> 16);
        }
        */
    }
    else {
        //attacks |= (startBoard << 8);
        attacks |= ((NOT_A_FILE & startBoard) << 7);
        attacks |= ((NOT_H_FILE & startBoard) << 9);

        /*
        if (square <= h7) {
            attacks |= (startBoard << 16);
        }
        */
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

map getRookAttackMaskWithBlockers(int square, map blockers) {
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
        if (((startBoard << (i * 8)) << i) & blockers) break;
    }

    //down left ray
    for (int i = 1; i <= (std::min(8 - rank, file - 1)); i++) {
        attacks |= ((startBoard << (i * 8)) >> i);
        if (((startBoard << (i * 8)) >> i) & blockers) break;
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
    for (int i = 0; i < 100000; i++) {
        map magicNumber = rng.nextRN(); //actual number

        //filter bad magic numbers
        if (getBitCount((attackMask * magicNumber) & 0xFF00000000000000) < 6) continue;

        //initialized usedAttacks at 0
        std::fill(usedAttacks, usedAttacks + 4095, 0ULL);

        int index, fail;
        //loop through attacks of all variations of the occupancy attack maps or till failure
        for (index = 0, fail = 0; fail != 1 && index << occupancyIndicies; index++) {
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
        if (!fail) return magicNumber;
    }

    std::cout << "Magic Number fails\n";
    return 0ULL;
}

void initMagicNumbers() {
    for (int i = 0; i < 64; i++) {
        std::cout << "0x" << std::hex << findMagicNumber(i, relevantBishopBits[i], 1) << "ULL,\n";
    }
    std::cout << "\n\n\n";
    for (int i = 0; i < 64; i++) {
        std::cout << "0x" << std::hex << findMagicNumber(i, relevantRookBits[i], 0) << "ULL,\n";
    }
}
