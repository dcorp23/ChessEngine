#include <iostream>
#include <chrono>
#include <vector>
#include "chessBoard.hpp"
#include "bitFunctions.hpp"

BoardState::BoardState() {
    whiteToMove = 1;
    whiteShortCastle = 1;
    whiteLongCastle = 1;
    blackShortCastle = 1; 
    blackLongCastle = 1;
    enPassant = 0;
    check = 0;
    checkMate = 0;
    halfMove = 0;
    fullMove = 0;
};

//piece 0-5 pawn bishop knight rook queen king
void MoveCode::printCode() {
    std::cout << " Piece: " << this->piece;
    std::cout << " Start: " << intToChessNotation(this->startSquare);
    std::cout << " End: " << intToChessNotation(this->endSquare);
    std::cout << " Capture: " << this->capture;
    std::cout << " Promotion: " << this->promotion;
    std::cout << '\n';
};

int chessNotationToInt(std::string chessNotation) {
    int col = chessNotation[0] - 97;
    int row = chessNotation[1] - 48;
    return ((8 - row) * 8) + col;
}

std::string intToChessNotation(int squareNumber) {
    int col = squareNumber % 8;
    int row = (8 - (squareNumber / 8));
    std::string chessNotation;
    chessNotation.push_back((char)(col + 97));
    chessNotation.push_back((char)(row + 48));
    return chessNotation;
}

//default constructor for blank board
Board::Board() {
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
};

//FEN Board to board
Board::Board(std::string fenString) {
    //initialize blank boards
    for (int i = WPawn; i <= BKing; i++) {
        bitMaps[i] = 0ULL;
    }

    std::vector<std::string> fenParts;
    std::string currentString;
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
    fenParts.push_back(fenPart);

    int square = 0;
    currentString = fenParts[0];
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
    newState.whiteLongCastle = 0;
    newState.whiteShortCastle = 0;
    newState.blackLongCastle = 0;
    newState.blackShortCastle = 0;
    for (stringIndex = 0; stringIndex < stringLength; stringIndex++) {
        if (currentString[stringIndex] == 'K') newState.whiteShortCastle = 1;
        if (currentString[stringIndex] == 'Q') newState.whiteLongCastle = 1;
        if (currentString[stringIndex] == 'k') newState.blackShortCastle = 1;
        if (currentString[stringIndex] == 'q') newState.blackLongCastle = 1;
    }

    currentString = fenParts[3];
    stringLength = currentString.length();
    if (currentString[0] == '-') {
        newState.enPassant = 0;
    } else {
        int column = (int)(currentString[0] - 97);
        int row = (int)(currentString[1] - 48);

        newState.enPassant = ((8 - row) * 8) + column;
    }

    
    currentString = fenParts[4];
    newState.halfMove = std::stoi(currentString);

    currentString = fenParts[5];
    newState.fullMove = std::stoi(currentString);

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
};

//constructor for move that only moves one piece
Board::Board(int piece, int side, map newPieceMap, Board* board, BoardState newState) {
    for (int i = WPawn; i <= BKing; i++) {
        bitMaps[i] = board->bitMaps[i];
    }

    bitMaps[piece + (side ? 0 : 6)] = newPieceMap;
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

//constructor for making a move that moves more than one piece not a promotion
Board::Board(int piece, int side, map newPieceMap, int secondPiece, map secondPieceMap, Board* board, BoardState newState, int capture) {
    for (int i = WPawn; i <= BKing; i++) {
        bitMaps[i] = board->bitMaps[i];
    }

    bitMaps[piece + (side ? 0 : 6)] = newPieceMap;
    if (secondPieceMap != 0ULL) bitMaps[secondPiece] = secondPieceMap;
    else if (capture) bitMaps[secondPiece] = secondPieceMap;

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
};

//Constructor for promotions only
Board::Board(int side, map newPawnMap, int promotionPiece, map promotionPieceMap, int capturedPiece, map captureMap, Board* board, BoardState newState, int capture) {
    for (int i = WPawn; i <= BKing; i++) {
        bitMaps[i] = board->bitMaps[i];
    }

    //set the pawn map
    bitMaps[side ? 0 : 6] = newPawnMap;
    //set the promoted piece
    bitMaps[promotionPiece + (side ? 0 : 6)] = promotionPieceMap;
    //remove the captured piece
    if (capture == 1) {
        bitMaps[capturedPiece] = captureMap;
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
};

Board Board::move(MoveCode code) {
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
    newState.halfMove++;
    if (!state.whiteToMove) newState.fullMove++;

    if (code.piece == 0 || code.capture) {
        newState.halfMove = 0;
    }

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
        if (code.capture == 0) return Board(state.whiteToMove, pieceMap, code.promotion, newPieceMap, 0, 0ULL, this, newState, 0);
        
        map captureMap = 0ULL;
        captureMap |= (1ULL << code.endSquare);

        int startPiece = state.whiteToMove ? 6 : 0;
        int endPiece = state.whiteToMove ? 12 : 6;
        int secondPiece = 0;
        for (int i = startPiece; i < endPiece; i++) {
            if (captureMap & bitMaps[i]) {
                captureMap ^= bitMaps[i];
                secondPiece = i;
            }
        }

        return Board(state.whiteToMove, pieceMap, code.promotion, newPieceMap, secondPiece, captureMap, this, newState, 1);
    }

    //enPassant
    if(code.enPassantFlag) {
        map captureMap = (1ULL << (state.enPassant + (state.whiteToMove ? 8 : -8)));
        captureMap ^= bitMaps[state.whiteToMove ? 6 : 0];

        return Board(code.piece, state.whiteToMove, pieceMap, state.whiteToMove ? 6 : 0, captureMap, this, newState, 1);
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

        return Board(code.piece, state.whiteToMove, pieceMap, secondPiece,captureMap, this, newState, 1);
    }

    //long castle
    if (code.blackLong || code.whiteLong) {
        map rookMap = 0b1001ULL;
        rookMap <<= code.whiteLong ? a1 : a8;
        rookMap ^= bitMaps[3 + (code.whiteLong ? 0 : 6)];

        return Board(code.piece, state.whiteToMove, pieceMap, 3 + (state.whiteToMove ? 0 : 6), rookMap, this, newState, 0);
    }

    //short castle
    if (code.blackShort || code.whiteShort) {
        map rookMap = 0b101ULL;
        rookMap <<= code.whiteShort ? f1 : f8;
        rookMap ^= bitMaps[3 + (code.whiteShort ? 0 : 6)];

        return Board(code.piece, state.whiteToMove, pieceMap, 3 + (state.whiteToMove ? 0 : 6), rookMap, this, newState, 0);
    }
    
    //quiet moves
    return Board(code.piece, state.whiteToMove, pieceMap, this, newState);
};

void Board::printBoard() {
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

//tests if all the bitmaps are the same doesn't check the state
bool Board::isEqual(Board board) {
    if (this->White != board.White) return false;
    if (this->Black != board.Black) return false;
    if (this->All != board.All) return false;
    for (int i = WPawn; i <= BKing; i++) {
        if (this->bitMaps[i] != board.bitMaps[i]) return false;
    }
    if (this->state.whiteToMove != board.state.whiteToMove) return false;
    return true;
};
