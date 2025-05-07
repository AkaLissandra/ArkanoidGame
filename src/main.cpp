#include "Game.h"
#include "Constants.h"
#include <iostream>

int main(int argc, char *argv[]) {

    Game game;

    if (!game.init("Arkanoid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, false)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }

    while (game.running()) {
        Uint64 frameStart = SDL_GetTicks64();

        game.handleEvents();
        game.update();
        game.render();

        Uint64 frameTime = SDL_GetTicks64() - frameStart;
        const int FRAME_DELAY = 1000 / 60;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay((Uint32)(FRAME_DELAY - frameTime));
        }
    }

    game.clean();

    return 0;
}