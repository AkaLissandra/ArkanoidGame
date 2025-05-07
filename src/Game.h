#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <vector>
#include "TextureManager.h"
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

enum class GameStateEnum {
    NONE,
    MENU,
    PLAY,
    PAUSE,
    GAMEOVER
};

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

    void handleEvents();

    void update();

    void render();

    void clean();

    bool running() { return isRunning; }

private:
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
    TextureManager textureManager;
    GameStateEnum m_currentGameState;
    Paddle m_paddle;
    Ball m_ball;
    std::vector<Brick> m_bricks;
};

#endif