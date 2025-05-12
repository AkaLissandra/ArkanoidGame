#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>
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

enum class PowerUpType {
    NONE,
    EXTRA_LIFE,
    BONUS_POINTS,
    MAGIC_HAT_DESTROY_ROW
};

struct PowerUpItem {
    SDL_Rect rect;
    PowerUpType type;
    bool isActive;
    std::string textureID;
    float y_floatPos;
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
    std::vector<PowerUpItem> m_powerUps;

    bool initializeSubsystems();
    bool loadMedia();
    bool loadFonts();
    void initGameObjects();
    void setupInitialUI();

    void handleMenuEvents(SDL_Event& event);
    void handlePlayEvents(SDL_Event& event);
    void handlePauseEvents(SDL_Event& event);
    void handleGameOverEvents(SDL_Event& event);

    void updateMenuState();
    void updatePlayState();
    void updatePauseState();
    void updateGameOverState();
    
    void renderMenuScreen();
    void renderPlayScreen();
    void renderPauseScreen();
    void renderGameOverScreen();

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
    SDL_Rect m_menuButtonRect_GO;
    bool m_isMouseOverReplay_GO; 
    bool m_isMouseOverMenu_GO;

    SDL_Rect m_pauseIconDisplayRect;
    SDL_Rect m_resumeButtonRect_PAUSE;
    SDL_Rect m_menuButtonRect_PAUSE;
    SDL_Rect m_replayButtonRect_PAUSE;
    SDL_Rect m_homeButtonRect_PAUSE;
    
    bool m_isMouseOverResume_PAUSE;
    bool m_isMouseOverMenu_PAUSE;
    bool m_isMouseOverReplay_PAUSE;

    void resetGameForPlay();

    Mix_Music* m_musicPlay;
    Mix_Chunk* m_sfxBallHit;
    Mix_Chunk* m_sfxLoseLife;
    Mix_Chunk* m_sfxButtonClick;
    Mix_Chunk* m_sfxReward;

    bool m_isSoundMuted;
    SDL_Rect m_soundIconRect;

    bool m_isMagicFlashActive;
    float m_magicFlashTimer;
    const float MAGIC_FLASH_DURATION = 0.25f;

    float m_menuBgScrollY;
};

#endif