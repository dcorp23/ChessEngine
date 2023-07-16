#include <SDL2/SDL.h>
#include "chessBoard.hpp"
#include <queue>

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;

public:
    bool running;
    int playerSide;
    bool engineTurn;
    int selectedPieceSquare;
    int clickedSquare = -1;
    bool promotion;
    int promotionSquare;
    Board board;
    std::vector<Board> possibleNextBoards;
    std::vector<MoveCode> filteredMoveList;
    std::queue<Board> boardHistory;

    void init();

    void handleEvents();

    void update();

    void engineMove();

    void setNewBoard(Board nextBoard);
    
    void render();

    void clean();
};
