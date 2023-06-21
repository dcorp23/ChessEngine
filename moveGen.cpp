#include <iostream>
#include <vector>
#include <chrono>

#include "attackTables.hpp"
#include "bitFunctions.hpp"
#include "chessBoard.hpp"
#include "moveGen.hpp"

//everything outside this is set seperately for special moves
MoveCode MoveGenerator::createMove(int startSquare, int endSquare, int piece, int capture) {
    MoveCode move;
    move.startSquare = startSquare;
    move.endSquare = endSquare;
    move.piece = piece;
    move.capture = capture;
    move.promotion = 0;
    move.whiteLong = 0;
    move.whiteShort = 0;
    move.blackLong = 0;
    move.blackShort = 0;
    move.enPassantSquare = 0;
    move.enPassantFlag = 0;
    return move;
};

//check if a square is attacked by the side given
bool MoveGenerator::isSquareAttacked(int square, int side, Board board, map occupancy) {
    //set queen attack from the square and have blockers be everything but enemy bishops rooks and queens
    //and the attack mask with a map of bishop rooks and queens and if it intersects then it is attacked
    //then set a knight attack from the square and if it intersects with enemy knights then its attacked
    //then use a pawn attack mask backwards to the enemy and if there is a pawn there then it is attacked
    //then use a king attack mask and check if there is a king in the square
    //this way we don't have to use a loop and all and it might be faster when checking if a king is in check or not

    //use rays from a bishop attack origin of the square in question
    map attackingBishopsQueens;

    map bishopAttackMask = AttackTables::getBishopAttacks(square, occupancy);
    if (side) attackingBishopsQueens = board.bitMaps[WBishop] | board.bitMaps[WQueen];
    else attackingBishopsQueens = board.bitMaps[BBishop] | board.bitMaps[BQueen];
    
    if (bishopAttackMask & attackingBishopsQueens) return true;

    //use rays from a rook origin of the square in question
    map attackingRooksQueens;

    map rookAttackMask = AttackTables::getRookAttacks(square, occupancy);
    if (side) attackingRooksQueens = board.bitMaps[WRook] | board.bitMaps[WQueen];
    else attackingRooksQueens = board.bitMaps[BRook] | board.bitMaps[BQueen];

    if (rookAttackMask & attackingRooksQueens) return true;

    //use knight attack from the square in question
    map attackingKnights;

    map knightAttackMask = AttackTables::getKnightAttacks(square);
    if (side) attackingKnights = board.bitMaps[WKnight];
    else attackingKnights = board.bitMaps[BKnight];

    if (attackingKnights & knightAttackMask) return true;

    //use pawn attack mask from the opposite side
    map attackingPawns;

    map pawnAttackMask = AttackTables::getPawnAttacks(square, !side);
    if (side) attackingPawns = board.bitMaps[WPawn];
    else attackingPawns = board.bitMaps[BPawn];

    if (attackingPawns & pawnAttackMask) return true;

    //use a king attack mask to see if the king is near by
    map attackingKing;

    map kingAttackMask = AttackTables::getKingAttacks(square);
    if (side) attackingKing = board.bitMaps[WKing];
    else attackingKing = board.bitMaps[BKing];

    if (attackingKing & kingAttackMask) return true;

    return false;
};

bool isBoardValid(Board board) {
    map king = board.bitMaps[board.state.whiteToMove ? 11 : 5];
    if (king == 0ULL) return true; //if there is no king it is a valid board for tests
    int kingSquare = getLSBIndex(king);
    return !(MoveGenerator::isSquareAttacked(kingSquare, board.state.whiteToMove, board, board.All));
};

bool isBoardCheck(Board board) {
        map king = board.bitMaps[board.state.whiteToMove ? 5 : 11];
        if (king == 0ULL) return false; //if there is no king it is not check for tests
        int kingSquare = getLSBIndex(king);
        return MoveGenerator::isSquareAttacked(kingSquare, !board.state.whiteToMove, board, board.All);
    };

bool isBoardCheckMate(Board board) {
    map kingMap;
    map attackMask;
    map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
    MoveCode move;
    std::vector<MoveCode> moveList;

    kingMap = board.bitMaps[board.state.whiteToMove ? 5 : 11]; //get the kings starting square
    map occupancy = board.All ^ kingMap; //remove the king from the occupancy
    int kingSquare = getLSBIndex(kingMap);

    //get all moves for the King
    attackMask = AttackTables::getKingAttacks(kingSquare);
    int attackMaskBitCount = getBitCount(attackMask);

    //loop through each square and check if there are any open escape squares
    for (int j = 0; j < attackMaskBitCount; j++) {
        int endSquare = getLSBIndex(attackMask);
        attackMask = popLSB(attackMask);

        if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
        //if the square isn't attacked then the king can escape there doesn't matter if there is an enemy there or not
        if (!MoveGenerator::isSquareAttacked(endSquare, !board.state.whiteToMove, board, occupancy)) return false;
    }

    //find the checker and see if we can block them if we find 2 checkers then we know its checkmate 
    //because we have to move the king in double check and we know that there are no escape squares

    //loop through all other enemy pieces other than the king
    int startPiece = board.state.whiteToMove ? BPawn : WPawn;
    int endPiece = board.state.whiteToMove ? BQueen: WQueen;

    int checkingPieceSquare = 0;
    int checkingPiece = 0;

    for (int i = startPiece; i <= endPiece; i++) {
        map currentMap = board.bitMaps[i];
        int numberOfAttackers = getBitCount(currentMap);

        for (int j = 0; j < numberOfAttackers; j++){
            int enemyStartSquare = getLSBIndex(currentMap);
            currentMap = popLSB(currentMap);
            attackMask = 0ULL;
            if ((i % 6) == 0) attackMask = AttackTables::getPawnAttacks(enemyStartSquare, !board.state.whiteToMove);
            if ((i % 6) == 1) attackMask = AttackTables::getBishopAttacks(enemyStartSquare, board.All);
            if ((i % 6) == 2) attackMask = AttackTables::getKnightAttacks(enemyStartSquare);
            if ((i % 6) == 3) attackMask = AttackTables::getRookAttacks(enemyStartSquare, board.All);
            if ((i % 6) == 4) attackMask = AttackTables::getQueenAttacks(enemyStartSquare, board.All);

            attackMask |= (1ULL << enemyStartSquare);

            if (attackMask & kingMap) {
                if (checkingPiece || checkingPieceSquare) return true;
                checkingPiece = i;
                checkingPieceSquare = enemyStartSquare;
            }
        }
    }

    //Now we see if there are any moves to block this attack
    attackMask = 0ULL;
    //pawn can't be blocked so the square its on is added and we will check if we can capture that pawn
    if ((checkingPiece % 6) == 1) attackMask = AttackTables::getBishopAttacks(checkingPieceSquare, board.All);
    //knight can't be blocked so the square its on is added and we will check if we can capture that pawn
    if ((checkingPiece % 6) == 3) attackMask = AttackTables::getRookAttacks(checkingPieceSquare, board.All);
    if ((checkingPiece % 6) == 4) attackMask = AttackTables::getQueenAttacks(checkingPieceSquare, board.All);



    attackMask |= ((1ULL << checkingPieceSquare) | (1ULL << kingSquare)); //add the square the attacking piece is on so we can check if we can capture it
    attackMask ^= (1ULL << kingSquare); //remove the king from the mask


    startPiece = board.state.whiteToMove ? WPawn : BPawn;
    endPiece = board.state.whiteToMove ? WQueen: BQueen;

    for (int i = startPiece; i <= endPiece; i++) {
        if ((i % 6) == 0) moveList = MoveGenerator::getPawnMoves(board);
        if ((i % 6) == 1) moveList = MoveGenerator::getBishopMoves(board);
        if ((i % 6) == 2) moveList = MoveGenerator::getKnightMoves(board);
        if ((i % 6) == 3) moveList = MoveGenerator::getRookMoves(board);
        if ((i % 6) == 4) moveList = MoveGenerator::getQueenMoves(board);
        
        int moveCount = moveList.size();
        for (int i = 0; i < moveCount; i++) {
            if (attackMask & (1ULL << moveList[i].endSquare)) {
                Board newBoard = board.move(moveList[i]);
                if (isBoardValid(newBoard)) return false;
            }
        }
    }
    return true;
};

std::vector<MoveCode> MoveGenerator::getPawnMoves(Board board) {
    map pawns;
    map moves = 0ULL;

    map beforeMove;
    map afterMove;
    map attackMask;

    int attackMaskBitCount;
    MoveCode move;
    std::vector<MoveCode> moveList;
    
    if (board.state.whiteToMove) {
        beforeMove = board.bitMaps[WPawn];
        if (beforeMove == 0ULL) return moveList;
        int numberOfPawns = getBitCount(board.bitMaps[WPawn]);
        for (int pawns = 0; pawns < numberOfPawns; pawns++) {
            //get the pawns location and remove from pawns
            int startSquare = getLSBIndex(beforeMove);
            beforeMove = popLSB(beforeMove);
            
            //check for captures from current square
            attackMask = AttackTables::getPawnAttacks(startSquare, 1);
            map capturables = board.Black; //map of black pieces and enpassant square if there is one
            if (board.state.enPassant) capturables |= (1ULL << board.state.enPassant);
            map captures = attackMask & capturables;
            int numberOfCaptures = getBitCount(captures);
            for (int i = 0; i < numberOfCaptures; i++) {
                int attackSquare = getLSBIndex(captures);
                captures = popLSB(captures);

                move = createMove(startSquare, attackSquare, 0, 1);
                if(attackSquare <= h8) { //promotions
                    for (int i = 1; i < 5; i++) {
                        move.promotion = i;
                        moveList.push_back(move);
                    }
                } else { //not a promotion
                    if (attackSquare == board.state.enPassant && board.state.enPassant) move.enPassantFlag = 1;
                    moveList.push_back(move);
                }
            }

            //check if it can move forward 1 square
            afterMove = (1ULL << (startSquare - 8));
            if ((afterMove & board.All) != 0ULL) continue;
            int endSquare = getLSBIndex(afterMove);
            move = createMove(startSquare, endSquare, 0, 0);
            if(endSquare <= h8) { //promotions
                for (int i = 1; i < 5; i++) {
                    move.promotion = i;
                    moveList.push_back(move);
                }
            } else { //not a promotion
                moveList.push_back(move);
            }

            //check if it can move forware 2 squares
            if (startSquare <= h3) continue;
            afterMove = (1ULL << (startSquare - 16));
            if ((afterMove & board.All) != 0ULL) continue;
            endSquare = getLSBIndex(afterMove);
            move = createMove(startSquare, endSquare, 0, 0);
            move.enPassantSquare = endSquare + 8;
            moveList.push_back(move);
        }
    }
    else {
        beforeMove = board.bitMaps[BPawn];
        if (beforeMove == 0ULL) return moveList;
        int numberOfPawns = getBitCount(board.bitMaps[BPawn]);
        for (int i = 0; i < numberOfPawns; i++) {
            int startSquare = getLSBIndex(beforeMove);
            beforeMove = popLSB(beforeMove);

            //check for captures
            attackMask = AttackTables::getPawnAttacks(startSquare, 0);
            map capturables = board.White; //map of white pieces and enpassant square if there is one
            if (board.state.enPassant) capturables |= (1ULL << board.state.enPassant); //set enpassant
            map captures = attackMask & capturables; //get all captures

            int numberOfCaptures = getBitCount(captures);
            for (int i = 0; i < numberOfCaptures; i++) {
                int attackSquare = getLSBIndex(captures);
                captures = popLSB(captures);

                move = createMove(startSquare, attackSquare, 0, 1);
                if(attackSquare >= a1) { //promotions
                    for (int i = 1; i < 5; i++) {
                        move.promotion = i;
                        moveList.push_back(move);
                    }
                } else { //not a promotion
                    if (attackSquare == board.state.enPassant && board.state.enPassant) move.enPassantFlag = 1;
                    moveList.push_back(move);
                }
            }

            //check if it can move forward 1 square
            afterMove = (1ULL << (startSquare + 8));
            if ((afterMove & board.All) != 0ULL) continue;
            int endSquare = getLSBIndex(afterMove);
            move = createMove(startSquare, endSquare, 0, 0);
            if(endSquare >= a1) { //promotions
                for (int i = 1; i < 5; i++) {
                    move.promotion = i;
                    moveList.push_back(move);
                }
            } else { //not a promotion
                moveList.push_back(move);
            }

            //check if it can move forware 2 squares
            if (startSquare >= a6) continue;
            afterMove = (1ULL << (startSquare + 16));
            if ((afterMove & board.All) != 0ULL) continue;
            endSquare = getLSBIndex(afterMove);
            move = createMove(startSquare, endSquare, 0, 0);
            move.enPassantSquare = endSquare - 8;
            moveList.push_back(move);
        }
    }
    return moveList;
};

std::vector<MoveCode> MoveGenerator::getBishopMoves(Board board) {
    map beforeMove;
    map attackMask;
    map afterMove;
    map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
    map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
    std::vector<MoveCode> moveList;
    MoveCode move;

    beforeMove = board.bitMaps[board.state.whiteToMove ? WBishop: BBishop];
    if (beforeMove == 0ULL) return moveList;
    int numberOfBishops = getBitCount(beforeMove);
    //loop through each Bishop
    for (int i = 0; i < numberOfBishops; i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the Bishop
        attackMask = AttackTables::getBishopAttacks(startSquare, board.All);
        map removeMask = attackMask & teamPieces;
        //remove the squares where team pieces are
        attackMask ^= removeMask;

        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each square
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            if (enemyPieces & (1ULL << endSquare)) { //check for captures
                move = createMove(startSquare, endSquare, 1, 1);
                moveList.push_back(move);
                continue;
            }
            move = createMove(startSquare, endSquare, 1, 0); //quite moves
            moveList.push_back(move);
        }
    }
    return moveList;
};

std::vector<MoveCode> MoveGenerator::getKnightMoves(Board board) {
    map beforeMove;
    map attackMask;
    map afterMove;
    map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
    map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
    MoveCode move;
    std::vector<MoveCode> moveList;

    beforeMove = board.bitMaps[board.state.whiteToMove ? WKnight : BKnight];
    if (beforeMove == 0ULL) return moveList;
    int numberOfKnights = getBitCount(beforeMove);
    //loop through each knight
    for (int i = 0; i < numberOfKnights; i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the knight 
        attackMask = AttackTables::getKnightAttacks(startSquare);
        map removeMask = attackMask & teamPieces;
        //remove the squares where team pieces are
        attackMask ^= removeMask;

        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each square
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            //if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
            if (enemyPieces & (1ULL << endSquare)) { //check for captures
                move = createMove(startSquare, endSquare, 2, 1);
                moveList.push_back(move);
                continue;
            }
            move = createMove(startSquare, endSquare, 2, 0); //quite moves
            moveList.push_back(move);
        }
    }
    return moveList;
};

std::vector<MoveCode> MoveGenerator::getRookMoves(Board board) {
    map beforeMove;
    map attackMask;
    map afterMove;
    map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
    map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
    MoveCode move;
    std::vector<MoveCode> moveList;

    beforeMove = board.bitMaps[board.state.whiteToMove ? WRook: BRook];
    if (beforeMove == 0ULL) return moveList;
    int numberOfRooks = getBitCount(beforeMove);
    //loop through each Rook
    for (int i = 0; i < numberOfRooks; i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the Rook
        attackMask = AttackTables::getRookAttacks(startSquare, board.All);
        map removeMask = attackMask & teamPieces;
        //remove the squares where team pieces are
        attackMask ^= removeMask;

        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each square
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            //if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
            if (enemyPieces & (1ULL << endSquare)) { //check for captures
                move = createMove(startSquare, endSquare, 3, 1);
                moveList.push_back(move);
                continue;
            }
            move = createMove(startSquare, endSquare, 3, 0); //quite moves
            moveList.push_back(move);
        }
    }
    return moveList;
};

std::vector<MoveCode> MoveGenerator::getQueenMoves(Board board) {
    map beforeMove;
    map attackMask;
    map afterMove;
    map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
    map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
    MoveCode move;
    std::vector<MoveCode> moveList;

    beforeMove = board.bitMaps[board.state.whiteToMove ? WQueen: BQueen];
    if (beforeMove == 0ULL) return moveList;
    int numberOfQueens = getBitCount(beforeMove);
    //loop through each Queen
    for (int i = 0; i < numberOfQueens; i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the Queen
        attackMask = AttackTables::getQueenAttacks(startSquare, board.All);
        map removeMask = attackMask & teamPieces;
        //remove the squares where team pieces are
        attackMask ^= removeMask;

        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each move
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            //if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
            if (enemyPieces & (1ULL << endSquare)) { //check for captures
                move = createMove(startSquare, endSquare, 4, 1);
                moveList.push_back(move);
                continue;
            }
            move = createMove(startSquare, endSquare, 4, 0); //quite moves
            moveList.push_back(move);
        }
    }
    return moveList;
};

std::vector<MoveCode> MoveGenerator::getKingMoves(Board board) {
    map beforeMove;
    map attackMask;
    map afterMove;
    map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
    map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
    MoveCode move;
    std::vector<MoveCode> moveList;

    beforeMove = board.bitMaps[board.state.whiteToMove ? WKing: BKing];
    if (beforeMove == 0ULL) return moveList;
    int startSquare = getLSBIndex(beforeMove);
    beforeMove = popLSB(beforeMove);

    //get all moves for the King
    attackMask = AttackTables::getKingAttacks(startSquare);
    map removeMask = attackMask & teamPieces;
    attackMask ^= removeMask;

    int attackMaskBitCount = getBitCount(attackMask);

    //loop through each square
    for (int j = 0; j < attackMaskBitCount; j++) {
        int endSquare = getLSBIndex(attackMask);
        attackMask = popLSB(attackMask);

        //if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
        if (enemyPieces & (1ULL << endSquare)) { //check for captures
            move = createMove(startSquare, endSquare, 5, 1);
            moveList.push_back(move);
            continue;
        }
        move = createMove(startSquare, endSquare, 5, 0); //quite moves
        moveList.push_back(move);
    }

    //castling
    bool castlingRights = (board.state.whiteToMove ? (board.state.whiteLongCastle || board.state.whiteShortCastle) :
                                                    (board.state.blackLongCastle || board.state.blackShortCastle)) &&
                                                    !board.state.check;

    if (castlingRights) {
        bool square1, square2;
        if (board.state.whiteShortCastle || board.state.blackShortCastle) {
            map piecesInBetweenMask = 0b11ULL;
            piecesInBetweenMask <<= board.state.whiteToMove ? f1 : f8;
            if (!(piecesInBetweenMask & board.All)) {
                int kingSquare = getLSBIndex(board.bitMaps[board.state.whiteToMove ? 5 : 11]);
                square1 = isSquareAttacked(kingSquare + 1, !board.state.whiteToMove, board, board.All);
                square2 = isSquareAttacked(kingSquare + 2, !board.state.whiteToMove, board, board.All);
                if (!square1 && !square2) {
                    move = createMove(kingSquare, kingSquare + 2, 5, 0);
                    if (board.state.whiteToMove) {
                        move.whiteShort = 1;
                    }
                    else {
                        move.blackShort = 1;
                    }
                    moveList.push_back(move);
                }
            }
        }
        if (board.state.whiteLongCastle || board.state.blackLongCastle) {
            map piecesInBetweenMask = 0b111ULL;
            piecesInBetweenMask <<= board.state.whiteToMove ? b1 : b8;
            if (!(piecesInBetweenMask & board.All)) {
                int kingSquare = getLSBIndex(board.bitMaps[board.state.whiteToMove ? 5 : 11]);
                square1 = isSquareAttacked(kingSquare - 1, !board.state.whiteToMove, board, board.All);
                square2 = isSquareAttacked(kingSquare - 2, !board.state.whiteToMove, board, board.All);
                if (!square1 && !square2) {
                    move = createMove(kingSquare, kingSquare - 2, 5, 0);
                    if (board.state.whiteToMove) {
                        move.whiteLong = 1;
                    }
                    else {
                        move.blackLong = 1;
                    }
                    moveList.push_back(move);
                }
            }
        }
    }
    return moveList;
};

std::vector<MoveCode> MoveGenerator::calculateAllMoves(Board board) {
    std::vector<MoveCode> moveList;
    std::vector<MoveCode> newMoves;
    newMoves = getPawnMoves(board);
    moveList.insert(moveList.end(), newMoves.begin(), newMoves.end());
    newMoves = getKnightMoves(board);
    moveList.insert(moveList.end(), newMoves.begin(), newMoves.end());
    newMoves = getBishopMoves(board);
    moveList.insert(moveList.end(), newMoves.begin(), newMoves.end());
    newMoves = getRookMoves(board);
    moveList.insert(moveList.end(), newMoves.begin(), newMoves.end());
    newMoves = getQueenMoves(board);
    moveList.insert(moveList.end(), newMoves.begin(), newMoves.end());
    newMoves = getKingMoves(board);
    moveList.insert(moveList.end(), newMoves.begin(), newMoves.end());
    return moveList;
};

std::vector<Board> MoveGenerator::validateAllMoves(Board board, std::vector<MoveCode> moveList) {
    std::vector<Board> validBoards;
    for (int i = 0; i < moveList.size(); i++) {
        Board newBoard = board.move(moveList.at(i));
        if (isBoardValid(newBoard)) {
            validBoards.push_back(newBoard);
        }
    }
    return validBoards;
}

std::vector<Board> MoveGenerator::getAllLegalBoards(Board board) {
    std::vector<MoveCode> moveList = calculateAllMoves(board);
    std::vector<Board> validBoards = validateAllMoves(board, moveList);
    int boardIndex = 0;
    int vectorSize = validBoards.size();
    for (int i = 0; i < vectorSize; i++) {
        Board currentBoard = validBoards.at(i);
        if (isBoardCheck(currentBoard)) {
            validBoards.at(i).state.check = 1;
            if (isBoardCheckMate(currentBoard)) {
                validBoards.at(i).state.checkMate = 1;
            }
        } else {
            validBoards.at(i).state.check = 0;
        }
        boardIndex++;
    }
    return validBoards;
};
