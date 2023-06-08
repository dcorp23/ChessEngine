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
                current = board.BPawn;
                for (int i = 0; i < getBitCount(board.BPawn); i++) {
                    attackedMask |= attackTable.getPawnAttacks(getLSBIndex(current), 0);
                    popLSB(current);
                }

                current = board.BBishop;
                for (int i = 0; i < getBitCount(board.BBishop); i++) {
                    attackedMask |= attackTable.getBishopAttacks(getLSBIndex(current), occupancy);
                    popLSB(current);
                }

                current = board.BKnight;
                for (int i = 0; i < getBitCount(board.BKnight); i++) {
                    attackedMask |= attackTable.getKnightAttacks(getLSBIndex(current));
                    popLSB(current);
                }

                current = board.BRook;
                for (int i = 0; i < getBitCount(board.BRook); i++) {
                    attackedMask |= attackTable.getRookAttacks(getLSBIndex(current), occupancy);
                    popLSB(current);
                }

                current = board.BQueen;
                for (int i = 0; i < getBitCount(board.BQueen); i++) {
                    attackedMask |= attackTable.getQueenAttacks(getLSBIndex(current), occupancy);
                    popLSB(current);
                }

                current = board.BKing;
                for (int i = 0; i < getBitCount(board.BKing); i++) {
                    attackedMask |= attackTable.getKingAttacks(getLSBIndex(current));
                    popLSB(current);
                }
            } else {
                current = board.WPawn;
                for (int i = 0; i < getBitCount(board.WPawn); i++) {
                    attackedMask |= attackTable.getPawnAttacks(getLSBIndex(current), 0);
                    popLSB(current);
                }

                current = board.WBishop;
                for (int i = 0; i < getBitCount(board.WBishop); i++) {
                    attackedMask |= attackTable.getBishopAttacks(getLSBIndex(current), occupancy);
                    popLSB(current);
                }

                current = board.WKnight;
                for (int i = 0; i < getBitCount(board.WKnight); i++) {
                    attackedMask |= attackTable.getKnightAttacks(getLSBIndex(current));
                    popLSB(current);
                }

                current = board.WRook;
                for (int i = 0; i < getBitCount(board.WRook); i++) {
                    attackedMask |= attackTable.getRookAttacks(getLSBIndex(current), occupancy);
                    popLSB(current);
                }

                current = board.WQueen;
                for (int i = 0; i < getBitCount(board.WQueen); i++) {
                    attackedMask |= attackTable.getQueenAttacks(getLSBIndex(current), occupancy);
                    popLSB(current);
                }

                current = board.WKing;
                for (int i = 0; i < getBitCount(board.WKing); i++) {
                    attackedMask |= attackTable.getKingAttacks(getLSBIndex(current));
                    popLSB(current);
                }
            }

            if (attackedMask & isAttacked) return true;
            return false;
        }

        //piece 1-6: pawn, bishops, knights, rooks, queens, kings
        int isCheck(int square, int piece, Board board) {
            map attack;
            map king = board.state.whiteToMove ? board.BKing : board.WKing;
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

            moveCode move;
            
            if (board.state.whiteToMove) {
                beforeMove = board.WPawn;
                afterMove = board.WPawn >> 8;
                for (int pawns = 0; pawns < getBitCount(board.WPawn); pawns++) {
                    int startSquare = getLSBIndex(beforeMove);
                    beforeMove = popLSB(beforeMove);
                    
                    //check for captures
                    attackMask = attackTable.getPawnAttacks(startSquare, 1);
                    for (int i = 0; i < getBitCount(attackMask); i++) {
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
                        if (attackSquare = board.state.enPassant) {
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
                for (int i = 0; i < getBitCount(board.BPawn); i++) {
                    int startSquare = getLSBIndex(beforeMove);
                    beforeMove = popLSB(beforeMove);
                    
                    //check for captures
                    attackMask = attackTable.getPawnAttacks(startSquare, 0);
                    for (int i = 0; i < getBitCount(attackMask); i++) {
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
                        if (attackSquare = board.state.enPassant) {
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
            map bishops;

            if (board.state.whiteToMove) {
                bishops = board.WBishop;
            }
            else {
                bishops = board.BBishop;
            }

            return;
        }

        void getKnightsMoves(Board board) {
            map knights;

            if (board.state.whiteToMove) {
                knights = board.WKnight;
            }
            else {
                knights = board.BKnight;
            }

            return;
        }

        void getRookMoves(Board board) {
            map rooks;

            if (board.state.whiteToMove) {
                rooks = board.WRook;
            }
            else {
                rooks = board.BRook;
            }

            return;
        }

        void getQueenMoves(Board board) {
            map queens;

            if (board.state.whiteToMove) {
                queens = board.WQueen;
            }
            else {
                queens = board.BQueen;
            }

            return;
        }

        void getKingMoves(Board board) {
            map king;

            if (board.state.whiteToMove) {
                king = board.WKing;
            }
            else {
                king = board.BKing;
            }

            return;
        }

        MoveGenerator() {};

        void calculateAllMoves(Board board) {
            getPawnMoves(board);
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
    move.startSquare = e2;
    move.endSquare = e4;
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


