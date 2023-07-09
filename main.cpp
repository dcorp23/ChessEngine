#include "game.hpp"
#include <mutex>
#include <future>
#include <queue>

int main( int argc, char *argv[] ) {
    Game* game = new Game();

    game->init();

    while(game->running) {
        game->render();
        if (game->engineTurn) game->engineMove();
        else {
            game->handleEvents();
            game->update();
        }
    }

    game->clean();
    delete game;
    return 0;
}