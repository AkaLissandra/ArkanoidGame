#include "Game.h"
#include "Constants.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

bool Game::initializeSubsystems() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize (Mix_OpenAudio)! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }

    int mixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
    if ((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
        std::cerr << "SDL_mixer could not initialize for MP3/OGG support! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

bool Game::loadMedia() {
    m_musicPlay = Mix_LoadMUS("assets/sounds/music_play.mp3");
    if (m_musicPlay == nullptr) {
        std::cerr << "Failed to load music_play.mp3! Mix_Error: " << Mix_GetError() << std::endl;
    }

    m_sfxBallHit = Mix_LoadWAV("assets/sounds/sfx_ball_hit.wav");
    if (m_sfxBallHit == nullptr) {
        std::cerr << "Failed to load sfx_ball_hit.wav! Mix_Error: " << Mix_GetError() << std::endl;
    }

    m_sfxLoseLife = Mix_LoadWAV("assets/sounds/sfx_lose_life.wav");
    if (m_sfxLoseLife == nullptr) {
        std::cerr << "Failed to load sfx_lose_life.wav! Mix_Error: " << Mix_GetError() << std::endl;
    }

    m_sfxButtonClick = Mix_LoadWAV("assets/sounds/sfx_button_click.wav");
    if (m_sfxButtonClick == nullptr) {
        std::cerr << "Failed to load sfx_button_click.wav! Mix_Error: " << Mix_GetError() << std::endl;
    }

    m_sfxReward = Mix_LoadWAV("assets/sounds/sfx_reward.wav");
    if (m_sfxReward == nullptr) {
        std::cerr << "Failed to load sfx_reward.wav! Mix_Error: " << Mix_GetError() << std::endl;
    }

    if (!textureManager.load(renderer, "assets/images/logo.png", "logo_game")) {
        std::cerr << "Failed to load logo_game.png" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/paddle.png", "paddle")) {
        std::cerr << "Failed to load paddle texture!" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/ball.png", "ball")) {
        std::cerr << "Failed to load ball texture!" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/brick_blue.png", "brick_blue")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_red.png", "brick_red")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_yellow.png", "brick_yellow")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_green.png", "brick_green")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_pink.png", "brick_pink")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_purple.png", "brick_purple")) return false;

    if (!textureManager.load(renderer, "assets/images/heart.png", "heart_icon")) {
        std::cerr << "Failed to load heart_icon.png!" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/icon_pause.png", "icon_pause")) {
        std::cerr << "Failed to load icon_pause.png!" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/icon_sound_on.png", "icon_sound_on")) {
        std::cerr << "Failed to load icon_sound_on.png!" << std::endl; return false;
    }
    if (!textureManager.load(renderer, "assets/images/icon_sound_off.png", "icon_sound_off")) {
        std::cerr << "Failed to load icon_sound_off.png!" << std::endl; return false;
    }

    if (!textureManager.load(renderer, "assets/images/powerup_extralife.png", "powerup_life")) {
        std::cerr << "Failed to load powerup_extralife.png!" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/powerup_bonus.png", "powerup_bonus")) {
        std::cerr << "Failed to load powerup_bonus.png!" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/powerup_magichat.png", "powerup_magichat")) {
        std::cerr << "Failed to load powerup_magichat_icon.png!" << std::endl;
        return false;
    }

    if (!textureManager.load(renderer, "assets/images/bg.png", "background_shared")) {
        std::cerr << "Failed to load bg.png as shared background!" << std::endl;
        return false;
    }

    std::cout << "All media loaded successfully." << std::endl;
    return true;
}

bool Game::loadFonts() {
    m_fontScore = TTF_OpenFont("assets/fonts/jersey.ttf", FONT_SIZE_SCORE);
    if (m_fontScore == nullptr) {
        std::cerr << "Failed to load font for Score! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    m_fontMenuItem = TTF_OpenFont("assets/fonts/jersey.ttf", FONT_SIZE_MENU_ITEM);
    if (m_fontMenuItem == nullptr) {
        std::cerr << "Failed to load font for Menu Items! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    m_fontUIText = TTF_OpenFont("assets/fonts/jersey.ttf", FONT_SIZE_UI_TEXT);
    if (m_fontUIText == nullptr) {
        std::cerr << "Failed to load font for UI Text! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    std::cout << "All fonts loaded successfully." << std::endl;
    return true;
}

void Game::initGameObjects() {
    int initialPaddleX = (SCREEN_WIDTH / 2) - (PADDLE_WIDTH / 2);
    int initialPaddleY = SCREEN_HEIGHT - PADDLE_HEIGHT - PADDLE_MARGIN_BOTTOM;
    m_paddle.init("paddle", initialPaddleX, initialPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT);


    int ballInitialX = m_paddle.getRect().x + (m_paddle.getRect().w / 2) - (BALL_WIDTH / 2);
    int ballInitialY = m_paddle.getRect().y - BALL_HEIGHT - 5;

    float ballInitialVelX = 5.0f;
    float ballInitialVelY = -5.0f;

    m_ball.init("ball", ballInitialX, ballInitialY, BALL_WIDTH, BALL_HEIGHT, ballInitialVelX, ballInitialVelY, m_sfxBallHit);

    createBrickLayout();
}

void Game::setupInitialUI() {
    m_buttonNormalColor = {255, 255, 255, 255};
    m_buttonHoverColor = {255, 255, 0, 255};
    m_isMouseOverStart = false;
    m_isMouseOverQuit = false;
    m_isMouseOverReplay_GO = false;
    m_isMouseOverMenu_GO = false;
    m_isMouseOverResume_PAUSE = false;
    m_isMouseOverMenu_PAUSE = false;
    m_isMouseOverReplay_PAUSE = false;


    const int ORIGINAL_LOGO_W = 896;
    const int ORIGINAL_LOGO_H = 772;
    const int LOGO_DISPLAY_WIDTH = 300;
    m_logoDisplayRect.w = LOGO_DISPLAY_WIDTH;
    m_logoDisplayRect.h = static_cast<int>(static_cast<float>(ORIGINAL_LOGO_H) * (static_cast<float>(LOGO_DISPLAY_WIDTH) / static_cast<float>(ORIGINAL_LOGO_W)));

    int textW_Start, textH_Start;
    int textW_Quit, textH_Quit;

    std::string startText = "START GAME";
    if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, startText.c_str(), &textW_Start, &textH_Start) != 0) {
        std::cerr << "TTF_SizeText error for START GAME: " << TTF_GetError() << std::endl;
        textW_Start = 150; textH_Start = 30;
    }
    m_startButtonRect.w = textW_Start;
    m_startButtonRect.h = textH_Start;

    std::string quitText = "QUIT";
    if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, quitText.c_str(), &textW_Quit, &textH_Quit) != 0) {
        std::cerr << "TTF_SizeText error for QUIT: " << TTF_GetError() << std::endl;
        textW_Quit = 80; textH_Quit = 30;
    }
    m_quitButtonRect.w = textW_Quit;
    m_quitButtonRect.h = textH_Quit;

    const int LOGO_TO_START_SPACING = 40;
    const int START_TO_QUIT_SPACING = 20;

    int totalBlockHeight = m_logoDisplayRect.h + LOGO_TO_START_SPACING +
                           m_startButtonRect.h + START_TO_QUIT_SPACING +
                           m_quitButtonRect.h;

    int blockStartY = (SCREEN_HEIGHT - totalBlockHeight) / 2;
    if (blockStartY < UI_TOP_MARGIN) blockStartY = UI_TOP_MARGIN;

    m_logoDisplayRect.x = (SCREEN_WIDTH - m_logoDisplayRect.w) / 2;
    m_logoDisplayRect.y = blockStartY;

    m_startButtonRect.x = (SCREEN_WIDTH - m_startButtonRect.w) / 2;
    m_startButtonRect.y = m_logoDisplayRect.y + m_logoDisplayRect.h + LOGO_TO_START_SPACING;

    m_quitButtonRect.x = (SCREEN_WIDTH - m_quitButtonRect.w) / 2;
    m_quitButtonRect.y = m_startButtonRect.y + m_startButtonRect.h + START_TO_QUIT_SPACING;

    m_pauseIconDisplayRect.x = UI_SIDE_MARGIN;
    m_pauseIconDisplayRect.y = UI_TOP_MARGIN;
    m_pauseIconDisplayRect.w = PAUSE_ICON_WIDTH;
    m_pauseIconDisplayRect.h = PAUSE_ICON_HEIGHT;

    m_soundIconRect.y = UI_TOP_MARGIN;
    m_soundIconRect.w = SOUND_ICON_WIDTH;
    m_soundIconRect.h = SOUND_ICON_HEIGHT;
}