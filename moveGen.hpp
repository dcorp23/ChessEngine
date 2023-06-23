#pragma once
#include <iostream>
#include <vector>
#include "chessBoard.hpp"
#include "attackTables.hpp"

namespace MoveGenerator {
    //returns a movecode with the given inputs and all special moves are initialized to 0
    MoveCode createMove(int startSquare, int endSquare, int piece, int capture);

    //get a vector of movecodes for the pawns in the position
    void getPawnMoves(Board board, std::vector<MoveCode>* moveList);

    //get a vector of movecodes for the bishops in the position
    void getBishopMoves(Board board, std::vector<MoveCode>* moveList);

    //get a vector of movecodes for the knights in the position
    void getKnightMoves(Board board, std::vector<MoveCode>* moveList);

    //get a vector of movecodes for the rooks in the position
    void getRookMoves(Board board, std::vector<MoveCode>* moveList);

    //get a vector of movecodes for the queens in the position
    void getQueenMoves(Board board, std::vector<MoveCode>* moveList);

    //get a vector of movecodes for the king in the position
    void getKingMoves(Board board, std::vector<MoveCode>* moveList);

    //calculates all pseudo legal moves and returns a vector for them
    void calculateAllMoves(Board board, std::vector<MoveCode>* moveList);

    //goes through list of movecodes and makes moves on the given board
    //and returns a pointer to a vector of boards that have been validated to be legal moves
    void validateAllMoves(Board board, std::vector<MoveCode>* moveList, std::vector<Board>* validBoards);

    //checks if a square is attacked by a certain side given the occupancy map
    bool isSquareAttacked(int square, int side, Board board, map occupancy);

    //returns a pointer to all legal boards and will update the state of the board if
    //it is check or checkmate the returned pointer needs to be freed
    std::vector<Board> getAllLegalBoards(Board board);
};
