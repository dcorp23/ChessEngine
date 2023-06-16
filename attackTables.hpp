#pragma once
#include <algorithm>
#include <iostream>
#include "bitFunctions.hpp"

namespace AttackTables {
    //initializes the attack arrays for indexing
    void initAttackTables();

    //side white = 1; black = 0;
    map getPawnAttacks(int square, int side);

    //get a bitmap of a knights attack from a square
    map getKnightAttacks(int square);

    //get a bitmap of a kings attack from a square
    map getKingAttacks(int square);

    //get a bitmap of a bishops attack from a square given a map of blockers
    map getBishopAttacks(int square, map occupancy);

    //get a bitmap of a rooks attacks from a square given a map of blockers
    map getRookAttacks(int square, map occupancy);

    //get a bitmap of queen attacks from a square given a map of blockers
    map getQueenAttacks(int square, map occupancy);
}