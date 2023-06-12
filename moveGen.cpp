#include <iostream>
#include <vector>
#include "attackTables.cpp"
#include "chessBoard.cpp"

class MoveGenerator {
    private: 
        AttackTables attackTable = AttackTables();
        MoveCode moveList[256] = {};
        int moveCount = 0;

        void addMove(MoveCode move) {
            moveList[moveCount] = move;
            moveCount++;
        }

        //everything outside this is set seperately for special moves
        MoveCode createMove(int startSquare, int endSquare, int piece, int capture) {
            MoveCode move;
            move.startSquare = startSquare;
            move.endSquare = endSquare;
            move.piece = piece;
            move.capture = capture;
            return move;
        }

        //check if a square is attacked by the side given
        bool isSquareAttacked(int square, int side, Board board, map occupancy) {
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
                        attackedMask |= attackTable.getPawnAttacks(getLSBIndex(current), side);
                        break;
                    case 1:
                        attackedMask |= attackTable.getBishopAttacks(getLSBIndex(current), occupancy);
                        break;
                    case 2:
                        attackedMask |= attackTable.getKnightAttacks(getLSBIndex(current));
                        break;
                    case 3:
                        attackedMask |= attackTable.getRookAttacks(getLSBIndex(current), occupancy);
                        break;
                    case 4:
                        attackedMask |= attackTable.getQueenAttacks(getLSBIndex(current), occupancy);
                        break;
                    case 5:
                        attackedMask |= attackTable.getKingAttacks(getLSBIndex(current));
                        break;
                    default:
                        break;
                    }
                    current = popLSB(current);
                    if (attackedMask & isAttacked) return true;
                }
            }
            return false;
        }

        bool isBoardValid(Board board) {
            map king = board.bitMaps[board.state.whiteToMove ? 11 : 5];
            if (king == 0ULL) return true; //if there is no king it is a valid board for tests
            int kingSquare = getLSBIndex(king);
            return !(isSquareAttacked(kingSquare, board.state.whiteToMove, board, board.All));
        }

        bool isBoardCheck(Board board) {
            map king = board.bitMaps[board.state.whiteToMove ? 5 : 11];
            int kingSquare = getLSBIndex(king);
            return isSquareAttacked(kingSquare, !board.state.whiteToMove, board, board.All);
        }

    public:
        void getPawnMoves(Board board) {
            map pawns;
            map moves = 0ULL;
            map rank2 = 0x000000000000FF00;
            map rank7 = 0x00FF000000000000;

            map beforeMove;
            map afterMove;
            map attackMask;

            int attackMaskBitCount;
            MoveCode move;
            
            if (board.state.whiteToMove) {
                beforeMove = board.bitMaps[WPawn];
                for (int pawns = 0; pawns < getBitCount(board.bitMaps[WPawn]); pawns++) {
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
                                    addMove(move);
                                }
                            } else { //not a promotion
                                addMove(move);
                            }
                        }
                        //check for enpassant
                        if (attackSquare == board.state.enPassant) {
                            move = createMove(startSquare, attackSquare, 0, 1);
                            move.enPassantFlag = 1;
                            addMove(move);
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
                            addMove(move);
                        }
                    } else { //not a promotion
                        addMove(move);
                    }

                    //check if it can move forware 2 squares
                    if (startSquare <= h3) continue;
                    afterMove = (1ULL << (startSquare - 16));
                    if ((afterMove & board.All) != 0ULL) continue;
                    endSquare = getLSBIndex(afterMove);
                    move = createMove(startSquare, endSquare, 0, 0);
                    move.enPassantSquare = endSquare + 8;
                    addMove(move);
                }
            }
            else {
                beforeMove = board.bitMaps[BPawn];
                for (int i = 0; i < getBitCount(board.bitMaps[BPawn]); i++) {
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
                                    addMove(move);
                                }
                            } else { //not a promotion
                                addMove(move);
                            }
                        }
                        //check for enpassant
                        if (attackSquare == board.state.enPassant) {
                            move = createMove(startSquare, attackSquare, 0, 1);
                            move.enPassantFlag = 1;
                            addMove(move);
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
                            addMove(move);
                        }
                    } else { //not a promotion
                        addMove(move);
                    }

                    //check if it can move forware 2 squares
                    if (startSquare >= a6) continue;
                    afterMove = (1ULL << (startSquare + 16));
                    if ((afterMove & board.All) != 0ULL) continue;
                    endSquare = getLSBIndex(afterMove);
                    move = createMove(startSquare, endSquare, 0, 0);
                    move.enPassantSquare = endSquare - 8;
                    addMove(move);
                }
            }
        }

        void getBishopMoves(Board board) {
            map beforeMove;
            map attackMask;
            map afterMove;
            map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
            map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
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
                        addMove(move);
                        continue;
                    }
                    move = createMove(startSquare, endSquare, 1, 0); //quite moves
                    addMove(move);
                }
            }
        }

        void getKnightMoves(Board board) {
            map beforeMove;
            map attackMask;
            map afterMove;
            map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
            map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
            MoveCode move;

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
                        addMove(move);
                        continue;
                    }
                    move = createMove(startSquare, endSquare, 2, 0); //quite moves
                    addMove(move);
                }
            }
        }

        void getRookMoves(Board board) {
            map beforeMove;
            map attackMask;
            map afterMove;
            map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
            map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
            MoveCode move;

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
                        addMove(move);
                        continue;
                    }
                    move = createMove(startSquare, endSquare, 3, 0); //quite moves
                    addMove(move);
                }
            }
        }

        void getQueenMoves(Board board) {
            map beforeMove;
            map attackMask;
            map afterMove;
            map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
            map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
            MoveCode move;

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
                        addMove(move);
                        continue;
                    }
                    move = createMove(startSquare, endSquare, 4, 0); //quite moves
                    addMove(move);
                }
            }
        }

        void getKingMoves(Board board) {
            map beforeMove;
            map attackMask;
            map afterMove;
            map teamPieces = (board.state.whiteToMove ? board.White : board.Black);
            map enemyPieces = (board.state.whiteToMove ? board.Black : board.White);
            MoveCode move;

            beforeMove = board.bitMaps[board.state.whiteToMove ? WKing: BKing];
            if (beforeMove == 0ULL) return;
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
                    addMove(move);
                    continue;
                }
                move = createMove(startSquare, endSquare, 5, 0); //quite moves
                addMove(move);
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
                            addMove(move);
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
                            addMove(move);
                        }
                    }
                }
            }
        }

        MoveGenerator() {};

        void calculateAllMoves(Board board) {
            getPawnMoves(board);
            getKnightMoves(board);
            getBishopMoves(board);
            getRookMoves(board);
            getQueenMoves(board);
            getKingMoves(board);
            //std::cout << "moves found: " << moveCount << '\n';
        }

        std::vector<Board>* validateAllMoves(Board board) {
            int boardCount = 0;
            std::vector<Board>* validBoards = new std::vector<Board>;
            for (int i = 0; i < moveCount; i++) {
                Board newBoard = board.move(moveList[i]);
                if (isBoardValid(newBoard)) {
                    validBoards->push_back(newBoard);
                    boardCount++;
                }
            }
            return validBoards;
        }

        bool isBoardCheckMate(Board board) {
            calculateAllMoves(board);
            std::vector<Board>* validBoards = validateAllMoves(board);
            int boardIndex = 0;
            int vectorSize = validBoards->size();
            for (int i = 0; i < vectorSize; i++) {
                if (!isBoardCheck((*validBoards)[i])) {
                    delete validBoards;
                    return false;
                }
                boardIndex++;
            }
            delete validBoards;
            return true;
        }

        MoveCode* getMoveList() {
            return this->moveList;
        }

        int getMoveCount() {
            return this->moveCount;
        }

        std::vector<Board>* getAllLegalBoards(Board board) {
            calculateAllMoves(board);
            std::vector<Board>* validBoards = validateAllMoves(board);
            int boardIndex = 0;
            int vectorSize = validBoards->size();
            for (int i = 0; i < vectorSize; i++) {
                Board currentBoard = (*validBoards)[boardIndex];
                if (isBoardCheck(currentBoard)) {
                    currentBoard.state.check = 1;
                    if (isBoardCheckMate(currentBoard)) {
                        currentBoard.state.checkMate = 1;
                    }
                }
                boardIndex++;
            }
            moveCount = 0;
            return validBoards;
        }
};
