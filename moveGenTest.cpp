#include <iostream>
#include <vector>
#include <chrono>
#include "moveGen.cpp"

//each depth is 1 move for 1 side so depth 6 would total 3 moves white 3 moves black
//boards is a pointer to a vector with 1 starting baord
std::vector<Board>* getBoardsAtDepth(std::vector<Board>* boards, int depth) {
    static MoveGenerator moveGen = MoveGenerator();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    if (depth == 0) {
        return boards;
    } else {
        Board currentBoard = Board();
        std::vector<Board>* newBoards = new std::vector<Board>; //new boards generated from current board
        std::vector<Board>* allNewBoards = new std::vector<Board>; //all boards generated
        int numberOfBoards = boards->size();
        for (int boardIndex = 0; boardIndex < numberOfBoards; boardIndex++) {
            currentBoard = boards->at(boardIndex);
            newBoards = moveGen.getAllLegalBoards(currentBoard);
            allNewBoards->insert(allNewBoards->end(), newBoards->begin(), newBoards->end());
            delete newBoards;
        }

        delete boards;

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Layer " << depth << " Completed" << '\n';
        std::cout << allNewBoards->size() << " Boards Found\n";
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
        return getBoardsAtDepth(allNewBoards, depth - 1);
    }
}

int main(void) {
    std::cout << "Starting\n";
    Board board = Board(startingFEN);

    board.printBoard();

    std::vector<Board>* boards = new std::vector<Board>;
    boards->push_back(board);
    std::cout << boards->size() << '\n';

    boards = getBoardsAtDepth(boards, 3);

    delete boards;
    return 0;
}