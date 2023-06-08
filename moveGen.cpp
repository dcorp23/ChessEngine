#include "attackTables.cpp"
#include "chessBoard.cpp"


class MoveGenerator {
    private: 
        AttackTables attackTable = AttackTables();
        moveCode moveList[256];
        int moveCount;

        void addMove(moveCode move) {
            moveList[moveCount] = move;
            moveCount++;
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
        }

        //piece 1-6: pawn, bishops, knights, rooks, queens, kings
        bool isCheck(int square, int piece, Board board) {
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

            if (attack & (king)) return true;
            return false;
        }

    public:
        void getPawnMoves(Board board) {
            map pawns;
            map moves = 0ULL;
            map rank2 = 0x000000000000FF00;
            map rank7 = 0x00FF000000000000;

            if (board.state.whiteToMove) {
                for (int i = 0; i < getBitCount(board.WPawn); i++) {

                }
            }
            else {
                pawns = board.BPawn;
                moves |= pawns << 8;
                moves |= (pawns & rank7) << 16;
            }

            int moveCount = getBitCount(moves);
            for (int i = 0; i < moveCount; i++) {
                
            }

            return;
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
};


