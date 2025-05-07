#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
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
    void createBrickLayout();
    bool areAllBricksCleared() const;
    
    int m_lives;
    
    int m_score;
    TTF_Font* m_fontScore;
    TTF_Font* m_fontMenuItem;
    TTF_Font* m_fontUIText;
    void renderText(TTF_Font* fontToUse, const std::string& message, int x, int y, SDL_Color color);

    int m_highScore;

    void loadHighScore();
    void saveHighScore();

    SDL_Rect m_logoDisplayRect;
    SDL_Rect m_startButtonRect;
    SDL_Rect m_quitButtonRect;

    bool m_isMouseOverStart;
    bool m_isMouseOverQuit;

    SDL_Color m_buttonNormalColor;
    SDL_Color m_buttonHoverColor;

    SDL_Rect m_replayButtonRect_GO;
    SDL_Rect m_menuButtonRect_GO;     // Vùng chữ "Menu" trên màn Game Over
    bool m_isMouseOverReplay_GO;    // Cờ hover cho nút Replay  
    bool m_isMouseOverMenu_GO;      // Cờ hover cho nút Menu (để về Menu chí

    SDL_Rect m_pauseIconDisplayRect;
    SDL_Rect m_resumeButtonRect_PAUSE;
    SDL_Rect m_menuButtonRect_PAUSE;  // Để về Menu chính từ Pause
    SDL_Rect m_replayButtonRect_PAUSE; // Để chơi lại từ Pause
    SDL_Rect m_homeButtonRect_PAUSE;
    
    bool m_isMouseOverResume_PAUSE;
    bool m_isMouseOverMenu_PAUSE;
    bool m_isMouseOverReplay_PAUSE;


    void resetGameForPlay();
};

#endif