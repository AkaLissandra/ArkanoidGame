#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
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

enum class PowerUpType {
    NONE,
    EXTRA_LIFE,
    BONUS_POINTS
    // Sau này có thể thêm: PADDLE_WIDE, BALL_SPEED_UP, MULTI_BALL v.v...
};

struct PowerUpItem {
    SDL_Rect rect;        // Vị trí và kích thước
    PowerUpType type;
    bool isActive;        // Còn hoạt động (đang rơi / có thể nhặt) không?
    std::string textureID;
    float y_floatPos;     // Vị trí Y dạng float để rơi mượt hơn
    // float fallSpeed; // Đã có trong Constants.h, không cần lưu ở đây nữa
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

    Mix_Music* m_musicPlay;         // Cho nhạc nền (MP3)
    Mix_Chunk* m_sfxBallHit;        // Cho tiếng bóng chạm (WAV)
    Mix_Chunk* m_sfxLoseLife;       // Cho tiếng mất mạng (WAV)
    Mix_Chunk* m_sfxButtonClick;    // Cho tiếng click nút (WAV)
    Mix_Chunk* m_sfxReward;

    bool m_isSoundMuted;
    SDL_Rect m_soundIconRect;
};

#endif