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

//each moves takes the starting square
//and returns a bitboard of the moves from that square

//pawn needs side either white = 1, black = 0;
map getPawnAttackMask(int square, int side) {
    map startBoard = 1ULL << square;
    map attacks = 0ULL;

    if (side) {
        attacks |= (startBoard >> 8);
        attacks |= ((NOT_H_FILE & startBoard) >> 7);
        attacks |= ((NOT_A_FILE & startBoard) >> 9);

        if (square >= a2) {
            attacks |= (startBoard >> 16);
        }
    }
    else {
        attacks |= (startBoard << 8);
        attacks |= ((NOT_A_FILE & startBoard) << 7);
        attacks |= ((NOT_H_FILE & startBoard) << 9);

        if (square <= h7) {
            attacks |= (startBoard << 16);
        }
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

    for (int i = 1; i <= (8 - rank); i++) {
        attacks |= (startBoard << (i * 8));
    }
    for (int i = 1; i < rank; i++) {
        attacks |= (startBoard >> (i * 8));
    }

    for (int i = 1; i <= (8 - file); i++) {
        attacks |= (startBoard << i);
    }
    for (int i = 1; i < file; i++) {
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
    for (int i = 1; i <= std::min(rank - 1, 8 - file); i++) {
        attacks |= ((startBoard >> (i * 8)) << i);
    }

    //down right ray
    for (int i = 1; i <= std::min(8 - rank, 8 - file); i++) {
        attacks |= ((startBoard << (i * 8)) << i);
    }

    //up left ray
    for (int i = 1; i <= (std::min(rank - 1, file - 1)); i++) {
        attacks |= ((startBoard >> (i * 8)) >> i);
    }

    //down left ray
    for (int i = 1; i <= (std::min(8 - rank, file - 1)); i++) {
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

