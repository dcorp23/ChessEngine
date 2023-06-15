#include <iostream>
#include <vector>
#include "attackTables.hpp"
#include "chessBoard.hpp"
#include "moveGen.hpp"

static AttackTables attackTable = AttackTables();

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
    map attackedMask = 0ULL;
    map current;
    map isAttacked = 1ULL << square;

    int startPiece = side ? WPawn : BPawn;
    int endPiece = side ? WKing : BKing;

    for (int i = startPiece; i <= endPiece; i++) {
        current = board.bitMaps[i];
        int bitCount = getBitCount(board.bitMaps[i]);
        for (int j = 0; j < bitCount; j++) {
            switch (i % 6)
            {
            case 0:
                attackedMask = attackTable.getPawnAttacks(getLSBIndex(current), side);
                break;
            case 1:
                attackedMask = attackTable.getBishopAttacks(getLSBIndex(current), occupancy);
                break;
            case 2:
                attackedMask = attackTable.getKnightAttacks(getLSBIndex(current));
                break;
            case 3:
                attackedMask = attackTable.getRookAttacks(getLSBIndex(current), occupancy);
                break;
            case 4:
                attackedMask = attackTable.getQueenAttacks(getLSBIndex(current), occupancy);
                break;
            case 5:
                attackedMask = attackTable.getKingAttacks(getLSBIndex(current));
                break;
            default:
                break;
            }
            current = popLSB(current);
            if (attackedMask & isAttacked) return true;
        }
    }
    return false;
};

bool MoveGenerator::isBoardValid(Board board) {
    map king = board.bitMaps[board.state.whiteToMove ? 11 : 5];
    if (king == 0ULL) return true; //if there is no king it is a valid board for tests
    int kingSquare = getLSBIndex(king);
    return !(isSquareAttacked(kingSquare, board.state.whiteToMove, board, board.All));
};

bool MoveGenerator::isBoardCheck(Board board) {
        map king = board.bitMaps[board.state.whiteToMove ? 5 : 11];
        if (king == 0ULL) return false; //if there is no king it is not check for tests
        int kingSquare = getLSBIndex(king);
        return isSquareAttacked(kingSquare, !board.state.whiteToMove, board, board.All);
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
        int numberOfPawns = getBitCount(board.bitMaps[WPawn]);
        for (int pawns = 0; pawns < numberOfPawns; pawns++) {
            int startSquare = getLSBIndex(beforeMove);
            beforeMove = popLSB(beforeMove);
            
            //check for captures
            attackMask = attackTable.getPawnAttacks(startSquare, 1);
            attackMaskBitCount = getBitCount(attackMask);
            for (int i = 0; i < attackMaskBitCount; i++) {
                int attackSquare = getLSBIndex(attackMask);
                attackMask = popLSB(attackMask);

                //check for normal captures
                if ((1ULL << attackSquare) & board.Black) {
                    move = createMove(startSquare, attackSquare, 0, 1);
                    if(attackSquare <= h8) { //promotions
                        for (int i = 1; i < 5; i++) {
                            move.promotion = i;
                            moveList.push_back(move);
                        }
                    } else { //not a promotion
                        moveList.push_back(move);
                    }
                }
                //check for enpassant
                if (board.state.enPassant != 0) {
                    if (attackSquare == board.state.enPassant) {
                        move = createMove(startSquare, attackSquare, 0, 1);
                        move.enPassantFlag = 1;
                        moveList.push_back(move);
                    }
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
        int numberOfPawns = getBitCount(board.bitMaps[BPawn]);
        for (int i = 0; i < numberOfPawns; i++) {
            int startSquare = getLSBIndex(beforeMove);
            beforeMove = popLSB(beforeMove);

            //check for captures
            attackMask = attackTable.getPawnAttacks(startSquare, 0);
            attackMaskBitCount = getBitCount(attackMask);
            for (int i = 0; i < attackMaskBitCount; i++) {
                int attackSquare = getLSBIndex(attackMask);
                attackMask = popLSB(attackMask);

                //check for normal captures
                if ((1ULL << attackSquare) & board.White) {
                    move = createMove(startSquare, attackSquare, 0, 1);
                    if(attackSquare >= a1) { //promotions
                        for (int i = 1; i < 5; i++) {
                            move.promotion = i;
                            moveList.push_back(move);
                        }
                    } else { //not a promotion
                        moveList.push_back(move);
                    }
                }

                //check for enpassant
                if (board.state.enPassant != 0) {
                    if (attackSquare == board.state.enPassant) {
                        move = createMove(startSquare, attackSquare, 0, 1);
                        move.enPassantFlag = 1;
                        moveList.push_back(move);
                    }
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
    //loop through each Bishop
    for (int i = 0; i < getBitCount(board.bitMaps[board.state.whiteToMove ? WBishop : BBishop]); i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the Bishop
        attackMask = attackTable.getBishopAttacks(startSquare, board.All);
        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each square
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
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
    //loop through each knight
    for (int i = 0; i < getBitCount(board.bitMaps[board.state.whiteToMove ? WKnight : BKnight]); i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the knight 
        attackMask = attackTable.getKnightAttacks(startSquare);
        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each square
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
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
    //loop through each Rook
    for (int i = 0; i < getBitCount(board.bitMaps[board.state.whiteToMove ? WRook : BRook]); i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the Rook
        attackMask = attackTable.getRookAttacks(startSquare, board.All);
        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each square
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
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
    //loop through each Queen
    for (int i = 0; i < getBitCount(board.bitMaps[board.state.whiteToMove ? WQueen : BQueen]); i++) {
        int startSquare = getLSBIndex(beforeMove);
        beforeMove = popLSB(beforeMove);

        //get all moves for the Queen
        attackMask = attackTable.getQueenAttacks(startSquare, board.All);
        int attackMaskBitCount = getBitCount(attackMask);

        //loop through each move
        for (int j = 0; j < attackMaskBitCount; j++) {
            int endSquare = getLSBIndex(attackMask);
            attackMask = popLSB(attackMask);

            if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
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
    attackMask = attackTable.getKingAttacks(startSquare);
    int attackMaskBitCount = getBitCount(attackMask);

    //loop through each square
    for (int j = 0; j < attackMaskBitCount; j++) {
        int endSquare = getLSBIndex(attackMask);
        attackMask = popLSB(attackMask);

        if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
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

MoveGenerator::MoveGenerator() {};

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

std::vector<Board>* MoveGenerator::validateAllMoves(Board board, std::vector<MoveCode> moveList) {
    std::vector<Board>* validBoards = new std::vector<Board>;
    for (int i = 0; i < moveList.size(); i++) {
        Board newBoard = board.move(moveList.at(i));
        if (isBoardValid(newBoard)) {
            validBoards->push_back(newBoard);
        }
    }
    return validBoards;
}

bool MoveGenerator::isBoardCheckMate(Board board) {
    map kingMap;
    map attackMask;
    map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
    MoveCode move;
    std::vector<MoveCode> moveList;

    kingMap = board.bitMaps[board.state.whiteToMove ? 5 : 11]; //get the kings starting square
    map occupancy = board.All ^ kingMap; //remove the king from the occupancy
    int kingSquare = getLSBIndex(kingMap);

    //get all moves for the King
    attackMask = attackTable.getKingAttacks(kingSquare);
    int attackMaskBitCount = getBitCount(attackMask);

    //loop through each square and check if there are any open escape squares
    for (int j = 0; j < attackMaskBitCount; j++) {
        int endSquare = getLSBIndex(attackMask);
        attackMask = popLSB(attackMask);

        if (teamPieces & (1ULL << endSquare)) continue; //skip spaces blocked by team
        //if the square isn't attacked then the king can escape there doesn't matter if there is an enemy there or not
        if (!isSquareAttacked(endSquare, !board.state.whiteToMove, board, occupancy)) return false;
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
            if ((i % 6) == 0) attackMask = attackTable.getPawnAttacks(enemyStartSquare, !board.state.whiteToMove);
            if ((i % 6) == 1) attackMask = attackTable.getBishopAttacks(enemyStartSquare, board.All);
            if ((i % 6) == 2) attackMask = attackTable.getKnightAttacks(enemyStartSquare);
            if ((i % 6) == 3) attackMask = attackTable.getRookAttacks(enemyStartSquare, board.All);
            if ((i % 6) == 4) attackMask = attackTable.getQueenAttacks(enemyStartSquare, board.All);

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
    if ((checkingPiece % 6) == 1) attackMask = attackTable.getBishopAttacks(checkingPieceSquare, board.All);
    //knight can't be blocked so the square its on is added and we will check if we can capture that pawn
    if ((checkingPiece % 6) == 3) attackMask = attackTable.getRookAttacks(checkingPieceSquare, board.All);
    if ((checkingPiece % 6) == 4) attackMask = attackTable.getQueenAttacks(checkingPieceSquare, board.All);



    attackMask |= ((1ULL << checkingPieceSquare) | (1ULL << kingSquare)); //add the square the attacking piece is on so we can check if we can capture it
    attackMask ^= (1ULL << kingSquare); //remove the king from the mask


    startPiece = board.state.whiteToMove ? WPawn : BPawn;
    endPiece = board.state.whiteToMove ? WQueen: BQueen;

    for (int i = startPiece; i <= endPiece; i++) {
        if ((i % 6) == 0) moveList = getPawnMoves(board);
        if ((i % 6) == 1) moveList = getBishopMoves(board);
        if ((i % 6) == 2) moveList = getKnightMoves(board);
        if ((i % 6) == 3) moveList = getRookMoves(board);
        if ((i % 6) == 4) moveList = getQueenMoves(board);
        
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

std::vector<Board>* MoveGenerator::getAllLegalBoards(Board board) {
    std::vector<MoveCode> moveList = calculateAllMoves(board);
    std::vector<Board>* validBoards = validateAllMoves(board, moveList);
    int boardIndex = 0;
    int vectorSize = validBoards->size();
    for (int i = 0; i < vectorSize; i++) {
        Board currentBoard = validBoards->at(i);
        if (isBoardCheck(currentBoard)) {
            validBoards->at(i).state.check = 1;
            if (isBoardCheckMate(currentBoard)) {
                validBoards->at(i).state.checkMate = 1;
            }
        } else {
            validBoards->at(i).state.check = 0;
        }
        boardIndex++;
    }
    return validBoards;
};
