#include <iostream>
#include "attackTables.cpp"
#include "chessBoard.cpp"

class MoveGenerator {
    private: 
        AttackTables attackTable = AttackTables();
        moveCode moveList[256] = {};
        int moveCount = 0;

        void addMove(moveCode move) {
            moveList[moveCount] = move;
            moveCount++;
        }

        //everything outside this is set seperately for special moves
        moveCode createMove(int startSquare, int endSquare, int piece, int capture) {
            moveCode move;
            move.startSquare = startSquare;
            move.endSquare = endSquare;
            move.piece = piece;
            move.capture = capture;
            return move;
        }

        bool isSquareAttacked(int square, int side, Board board) {
            map attackedMask;
            map current;
            map occupancy = board.All;
            map isAttacked = 1ULL << square;
            if (side) {
                for (int i = WPawn; i <= WKing; i++) {
                    current = board.bitMaps[i];
                    for (int i = 0; i < getBitCount(board.bitMaps[i]); i++) {
                        attackedMask |= attackTable.getPawnAttacks(getLSBIndex(current), 0);
                        popLSB(current);
                    }
                }
            } else {
                for (int i = BPawn; i <= BKing; i++) {
                    current = board.bitMaps[i];
                    for (int i = 0; i < getBitCount(board.bitMaps[i]); i++) {
                        attackedMask |= attackTable.getPawnAttacks(getLSBIndex(current), 0);
                        popLSB(current);
                    }
                }
            }

            if (attackedMask & isAttacked) return true;
            return false;
        }

        //piece 1-6: pawn, bishops, knights, rooks, queens, kings
        int isCheck(int square, int piece, Board board) {
            map attack;
            map king = board.bitMaps[5 + (board.state.whiteToMove ? 0 : 6)];
            switch (piece)
            {
            case 1:
                attack = attackTable.getPawnAttacks(square, board.state.whiteToMove);
                break;
            case 2:
                attack = attackTable.getBishopAttacks(square, board.All);
                break;
            case 3: 
                attack = attackTable.getKnightAttacks(square);
                break;
            case 4:
                attack = attackTable.getRookAttacks(square, board.All);
                break;
            case 5:
                attack = attackTable.getQueenAttacks(square, board.All);
                break;
            case 6: 
                attack = attackTable.getKingAttacks(square);
            default:
                std::cout << "Piece is invalid";
                break;
            }

            if (attack & king) return 1;
            return 0;
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
            moveCode move;
            
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
                            if(attackSquare < 8) { //promotions
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
                    afterMove = (1ULL << startSquare) >> 8;
                    if ((afterMove & board.All) != 0ULL) continue;
                    int endSquare = getLSBIndex(afterMove);
                    move = createMove(startSquare, endSquare, 0, 0);
                    if(endSquare < a7) { //promotions
                        for (int i = 1; i < 5; i++) {
                            move.promotion = i;
                            addMove(move);
                        }
                    } else { //not a promotion
                        addMove(move);
                    }

                    //check if it can move forware 2 squares
                    if (beforeMove & rank2) continue;
                    afterMove = (1ULL << startSquare) >> 16;
                    if ((afterMove & board.All) != 0ULL) continue;
                    endSquare = getLSBIndex(afterMove);
                    move = createMove(startSquare, endSquare, 0, 0);
                    move.enPassantSquare = endSquare - 8;
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
                            if(attackSquare > h2) { //promotions
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
                    afterMove = (1ULL << startSquare) << 8;
                    if ((afterMove & board.All) != 0ULL) continue;
                    int endSquare = getLSBIndex(afterMove);
                    move = createMove(startSquare, endSquare, 0, 0);
                    if(endSquare > h2) { //promotions
                        for (int i = 1; i < 5; i++) {
                            move.promotion = i;
                            addMove(move);
                        }
                    } else { //not a promotion
                        addMove(move);
                    }

                    //check if it can move forware 2 squares
                    if (beforeMove & rank7) continue;
                    afterMove = (1ULL << startSquare) << 16;
                    if ((afterMove & board.All) != 0ULL) continue;
                    endSquare = getLSBIndex(afterMove);
                    move = createMove(startSquare, endSquare, 0, 0);
                    move.enPassantSquare = endSquare + 8;
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
            moveCode move;

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
            moveCode move;

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
            moveCode move;

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
                        move = createMove(startSquare, endSquare, 2, 1);
                        addMove(move);
                        continue;
                    }
                    move = createMove(startSquare, endSquare, 2, 0); //quite moves
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
            moveCode move;

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
            moveCode move;

            beforeMove = board.bitMaps[board.state.whiteToMove ? WKing: BKing];
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
        }

        MoveGenerator() {};

        void calculateAllMoves(Board board) {
            getPawnMoves(board);
            getKnightMoves(board);
            getBishopMoves(board);
            getRookMoves(board);
            getQueenMoves(board);
            getKingMoves(board);
        }

        moveCode* getMoveList() {
            return this->moveList;
        }

        int getMoveCount() {
            return this->moveCount;
        }
};

int main(void) {
    std::cout << "hello" << '\n';
    boardState startingState;
    startingState.whiteToMove = 1;

    Board board = Board(startingPawns, startingKnights, startingBishops, startingRooks, startingQueen, startingKing, 
            startingPawns >> 40, startingKnights >> 56, startingBishops >> 56, startingRooks >> 56, startingQueen >> 56, startingKing >> 56,
            startingState);

    board.printBoard();

    std::cout << '\n';

    moveCode move;
    move.startSquare = c2;
    move.endSquare = c4;
    move.piece = 0;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';

    move.startSquare = e7;
    move.endSquare = e5;
    move.piece = 0;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';

    move.startSquare = c4;
    move.endSquare = c5;
    move.piece = 0;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';

    move.startSquare = e5;
    move.endSquare = e4;
    move.piece = 0;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';

    move.startSquare = c5;
    move.endSquare = c6;
    move.piece = 0;

    board = board.move(move);
    board.printBoard();
    std::cout << '\n';

    std::cout << "hello" << '\n';
    
    static MoveGenerator moveGen = MoveGenerator();

    moveGen.calculateAllMoves(board);

    moveCode* moveList = moveGen.getMoveList();
    int moveCount = moveGen.getMoveCount();

    std::cout << moveCount << '\n';

    for (int i = 0; i < moveCount; i++) {
        Board newBoard = board.move(moveList[i]);
        newBoard.printBoard();
        std::cout << '\n';
    }

    return 0;
};


