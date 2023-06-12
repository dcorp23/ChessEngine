#include <iostream>
#include <chrono>
#include <vector>


typedef unsigned long long map;

using std::string;

//starting FEN string
const std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//board by chess notation
//use this by: 1ULL << f1 which gives a bitboard with just f1
#ifndef BOARDNOT
#define BOARDNOT
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
#endif //BOARDNOT

enum {
    WPawn, WBishop, WKnight, WRook, WQueen, WKing, 
    BPawn, BBishop, BKnight, BRook, BQueen, BKing
};

//print a single bit map in an 8x8 grid
void printMap(map printingMap) {
    std::cout << '\n';
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            std::cout << ((printingMap & (1ULL << ((rank * 8) + file))) ? " 1" : " 0");
        }
        std::cout << '\n';
    }
};

//starting is 0x01;
struct BoardState {
    unsigned short whiteToMove : 1;
    unsigned short whiteShortCastle : 1;
    unsigned short whiteLongCastle : 1;
    unsigned short blackShortCastle : 1; 
    unsigned short blackLongCastle : 1;
    unsigned short enPassant : 6; //square that is enPassant able
                                //0 is no enpassant because you can't enpassant on a8 anyway
    unsigned short check : 1;
    unsigned short checkMate: 1;

    BoardState() {
        whiteToMove = 1;
        whiteShortCastle = 1;
        whiteLongCastle = 1;
        blackShortCastle = 1; 
        blackLongCastle = 1;
        enPassant = 0;
        check = 0;
        checkMate = 0;
    }
};

//piece 0-5 pawn bishop knight rook queen king
struct MoveCode {
    unsigned int startSquare : 6;
    unsigned int endSquare : 6;
    unsigned int piece: 3;
    unsigned int capture: 1;
    unsigned int promotion: 3; //piece promoting to same numbers as piece
    unsigned int whiteLong: 1;
    unsigned int whiteShort: 1;
    unsigned int blackLong: 1;
    unsigned int blackShort: 1;
    unsigned int enPassantSquare: 6; //square that is enPassant able when moving double
                            //0 is no enpassant because you can't enpassant on a8 anyway
    unsigned int enPassantFlag: 1; //if the move was enPassant
};

struct Board {
    map White;
    map Black;
    map All;

    map bitMaps[12];

    BoardState state;

    //default constructor for blank board
    Board() {
        for (int i = WPawn; i <= BKing; i++) {
            bitMaps[i] = 0ULL;
        }
        White = 0ULL;
        Black = 0ULL;
        for (int i = WPawn; i <= WKing; i++) {
            White |= bitMaps[i];
        }
        for (int i = BPawn; i <= BKing; i++) {
            Black |= bitMaps[i];
        }
        All = 0ULL;
        BoardState newState = BoardState();
        state = newState;
    }

    //FEN Board to board
    Board(std::string fenString) {
        //initialize blank boards
        for (int i = WPawn; i <= BKing; i++) {
            bitMaps[i] = 0ULL;
        }

        std::vector<std::string> fenParts;
        int stringLength = fenString.length();
        std::string fenPart = "";
        int stringIndex = 0;
        for (stringIndex = 0; stringIndex < stringLength; stringIndex++) {
            if (fenString.at(stringIndex) == ' ') {
                fenParts.push_back(fenPart);
                fenPart = "";
                continue;
            }
            fenPart = fenPart + fenString.at(stringIndex);
        }

        int square = 0;
        std::string currentString = fenParts[0];
        stringLength = currentString.length();
        for (stringIndex = 0; stringIndex < stringLength; stringIndex++) {
            switch (currentString[stringIndex])
            {
            case 'P':
                bitMaps[WPawn] |= (1ULL << square);
                square++;
                break;
            case 'B':
                bitMaps[WBishop] |= (1ULL << square);
                square++;
                break;
            case 'N':
                bitMaps[WKnight] |= (1ULL << square);
                square++;
                break;
            case 'R':
                bitMaps[WRook] |= (1ULL << square);
                square++;
                break;
            case 'Q':
                bitMaps[WQueen] |= (1ULL << square);
                square++;
                break;
            case 'K':
                bitMaps[WKing] |= (1ULL << square);
                square++;
                break;
            case 'p':
                bitMaps[BPawn] |= (1ULL << square);
                square++;
                break;
            case 'b':
                bitMaps[BBishop] |= (1ULL << square);
                square++;
                break;
            case 'n':
                bitMaps[BKnight] |= (1ULL << square);
                square++;
                break;
            case 'r':
                bitMaps[BRook] |= (1ULL << square);
                square++;
                break;
            case 'q':
                bitMaps[BQueen] |= (1ULL << square);
                square++;
                break;
            case 'k':
                bitMaps[BKing] |= (1ULL << square);
                square++;
                break;
            case '/':
                break;
            default:
                square = square + (int)(currentString[stringIndex] - 48);
                break;
            }
        }

        BoardState newState = BoardState();
        newState.whiteToMove = (fenParts[1] == "w") ? 1 : 0;

        currentString = fenParts[2];
        stringLength = currentString.length();
        for (stringIndex = 0; stringIndex < stringLength; stringIndex++) {
            if (currentString[stringIndex] == '-') {
                newState.whiteLongCastle = 0;
                newState.whiteShortCastle = 0;
                newState.blackLongCastle = 0;
                newState.blackShortCastle = 0;
            } else {
                if (currentString[stringIndex] == 'K') newState.whiteShortCastle = 1;
                if (currentString[stringIndex] == 'Q') newState.whiteLongCastle = 1;
                if (currentString[stringIndex] == 'k') newState.blackShortCastle = 1;
                if (currentString[stringIndex] == 'q') newState.blackLongCastle = 1;
            }
        }

        currentString = fenParts[2];
        stringLength = currentString.length();
        if (currentString[0] == '-') {
            newState.enPassant = 0;
        } else {
            int column = (int)(currentString[0] - 97);
            int row = (int)(currentString[1] - 48);

            newState.enPassant = (row * 8) + column;
        }

        White = 0ULL;
        Black = 0ULL;
        for (int i = WPawn; i <= WKing; i++) {
            White |= bitMaps[i];
        }
        for (int i = BPawn; i <= BKing; i++) {
            Black |= bitMaps[i];
        }
        All = White | Black;

        state = newState;
    }

    //constructor for making a move
    Board(int piece, int side, map newPieceMap, int secondPiece, map secondPieceMap, Board* board, BoardState newState) {
        for (int i = WPawn; i <= BKing; i++) {
            bitMaps[i] = board->bitMaps[i];
        }

        bitMaps[piece + (side ? 0 : 6)] = newPieceMap;
        if (secondPieceMap != 0ULL) bitMaps[secondPiece] = secondPieceMap;

        White = 0ULL;
        Black = 0ULL;
        for (int i = WPawn; i <= WKing; i++) {
            White |= bitMaps[i];
        }
        for (int i = BPawn; i <= BKing; i++) {
            Black |= bitMaps[i];
        }
        All = White | Black;
        
        state = newState;
    }

    Board move(MoveCode code) {
        map moveMask = 0ULL;
        moveMask |= (1ULL << code.startSquare);
        moveMask |= (1ULL << code.endSquare);
        //map with piece in updated square
        map pieceMap;
        
        if (state.whiteToMove) {
            pieceMap = bitMaps[code.piece];
            pieceMap ^= moveMask;
        }
        else {
            pieceMap = bitMaps[code.piece + 6];
            pieceMap ^= moveMask;
        }

        BoardState newState = state;
        newState.enPassant = code.enPassantSquare;
        newState.whiteToMove = !newState.whiteToMove;
        if (state.whiteToMove) {
            if (code.piece == 5) {
                newState.whiteLongCastle = 0;
                newState.whiteShortCastle = 0;
            } 
            if (state.whiteLongCastle) {
                if (code.piece == 3 && code.startSquare == a1) newState.whiteLongCastle = 0;
            }
            if (state.whiteShortCastle) {
                if (code.piece == 3 && code.startSquare == h1) newState.whiteShortCastle = 0;
            }
        } else {
            if (code.piece == 5) {
                newState.blackLongCastle = 0;
                newState.blackShortCastle = 0;
            }
            if (state.blackLongCastle) {
                if (code.piece == 3 && code.startSquare == a8) newState.blackLongCastle = 0;
            }
            if (state.blackShortCastle) {
                if (code.piece == 3 && code.startSquare == h8) newState.blackShortCastle = 0;
            }
        }

        if (code.promotion) {
            pieceMap ^= (1ULL << code.endSquare);
            map newPieceMap = bitMaps[code.promotion + (state.whiteToMove ? 0 : 6)];
            newPieceMap ^= (1ULL << code.endSquare);

            return Board(code.piece, state.whiteToMove, pieceMap, code.promotion + (state.whiteToMove ? 0 : 6), newPieceMap, this, newState);
        }

        //enPassant
        if(code.enPassantFlag) {
            map captureMap = (1ULL << (state.enPassant + (state.whiteToMove ? 8 : -8)));
            captureMap ^= bitMaps[0 + (state.whiteToMove ? 6 : 0)];

            return Board(code.piece, state.whiteToMove, pieceMap, 0 + (state.whiteToMove ? 6 : 0), captureMap, this, newState);
        }

        //check for captures
        if (code.capture) {
            map captureMap = 1ULL << code.endSquare;

            int startPiece = state.whiteToMove ? 6 : 0;
            int endPiece = state.whiteToMove ? 12 : 6;
            int secondPiece = 0;
            for (int i = startPiece; i < endPiece; i++) {
                if (captureMap & bitMaps[i]) {
                    captureMap ^= bitMaps[i];
                    secondPiece = i;
                }
            }

            return Board(code.piece, state.whiteToMove, pieceMap, secondPiece,captureMap, this, newState);
        }

        //long castle
        if (code.blackLong || code.whiteLong) {
            map rookMap = 0b1001ULL;
            rookMap <<= code.whiteLong ? a1 : a8;
            rookMap ^= bitMaps[3 + (code.whiteLong ? 0 : 6)];

            return Board(code.piece, state.whiteToMove, pieceMap, 3 + (state.whiteToMove ? 0 : 6), rookMap, this, newState);
        }

        //short castle
        if (code.blackShort || code.whiteShort) {
            map rookMap = 0b101ULL;
            rookMap <<= code.whiteShort ? f1 : f8;
            rookMap ^= bitMaps[3 + (code.whiteShort ? 0 : 6)];

            return Board(code.piece, state.whiteToMove, pieceMap, 3 + (state.whiteToMove ? 0 : 6), rookMap, this, newState);
        }
        
        //quiet moves
        return Board(code.piece, state.whiteToMove, pieceMap, 0, 0ULL, this, newState);
    };

    void printBoard() {
        map currentBit;

        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                currentBit = (1ULL << ((rank * 8) + file));

                if (currentBit & All) {
                    if (currentBit & White) {
                        if (currentBit & bitMaps[WPawn]) std::cout << " P";
                        if (currentBit & bitMaps[WKnight]) std::cout << " N";
                        if (currentBit & bitMaps[WBishop]) std::cout << " B";
                        if (currentBit & bitMaps[WRook]) std::cout << " R";
                        if (currentBit & bitMaps[WQueen]) std::cout << " Q";
                        if (currentBit & bitMaps[WKing]) std::cout << " K";
                    } else {
                        if (currentBit & bitMaps[BPawn]) std::cout << " p";
                        if (currentBit & bitMaps[BKnight]) std::cout << " n";
                        if (currentBit & bitMaps[BBishop]) std::cout << " b";
                        if (currentBit & bitMaps[BRook]) std::cout << " r";
                        if (currentBit & bitMaps[BQueen]) std::cout << " q";
                        if (currentBit & bitMaps[BKing]) std::cout << " k";
                    }
                } else {
                    std::cout << " 0";
                }
            }
            std::cout << '\n';
        }
    };

    bool isEqual(Board board) {
        return this->All == board.All;
    }
};