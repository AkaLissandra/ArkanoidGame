#include "Game.h"
#include "Constants.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), m_currentGameState(GameStateEnum::NONE) {
    m_isMouseOverStart = false;
    m_isMouseOverQuit = false;
    m_isMouseOverReplay_GO = false;
    m_isMouseOverMenu_GO = false;

    m_isMouseOverResume_PAUSE = false;
    m_isMouseOverMenu_PAUSE = false;
    m_isMouseOverReplay_PAUSE = false;

    m_musicPlay = nullptr;
    m_sfxBallHit = nullptr;
    m_sfxLoseLife = nullptr;
    m_sfxButtonClick = nullptr;
    m_sfxReward = nullptr;

    m_isSoundMuted = false;

    m_isMagicFlashActive = false;
    m_magicFlashTimer = 0.0f;

    m_menuBgScrollY = 0.0f;
}

Game::~Game() {
}

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


bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    srand(static_cast<unsigned int>(time(0)));

    if (!initializeSubsystems()) {
        clean();
        return false;
    }

    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        clean();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        clean();
        return false;
    }

    if (!loadFonts()) {
        clean();
        return false;
    }

    if (!loadMedia()) {
        clean();
        return false;
    }

    initGameObjects();

    setupInitialUI();

    m_score = 0;

    loadHighScore();

    m_lives = 3;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    m_currentGameState = GameStateEnum::MENU;

    if (m_musicPlay != nullptr) {
        if (Mix_PlayingMusic() == 0) {
            if (Mix_PlayMusic(m_musicPlay, -1) == -1) {
                std::cerr << "Mix_PlayMusic failed to play music from init: " << Mix_GetError() << std::endl;
            }
        }
    }

    isRunning = true; 
    std::cout << "Game initialized successfully!" << std::endl;
    return true;
    
}

void Game::renderText(TTF_Font* fontToUse, const std::string& message, int x, int y, SDL_Color color) {
    if (fontToUse == nullptr || renderer == nullptr) {
        return;
    }

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(fontToUse, message.c_str(), color);
    if (surfaceMessage == nullptr) {
        std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    if (messageTexture == nullptr) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surfaceMessage);
        return;
    }

    SDL_Rect destRect = { x, y, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, messageTexture, nullptr, &destRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(messageTexture);
}

void Game::createBrickLayout() {
    m_bricks.clear(); 

    const int rowScores[] = {60, 50, 40, 30, 20, 10};
    std::string brickTextureIDs[NUM_BRICK_ROWS] = { 
        "brick_red",
        "brick_pink",
        "brick_yellow",
        "brick_green", 
        "brick_blue",
        "brick_purple"
    };

    for (int row = 0; row < NUM_BRICK_ROWS; ++row) {
        if (row >= (sizeof(rowScores)/sizeof(rowScores[0])) ) continue; 

        std::string currentBrickTextureID = brickTextureIDs[row];
        int currentScoreValue = rowScores[row];

        for (int col = 0; col < NUM_BRICK_COLUMNS; ++col) {
            int brickX = BRICK_AREA_START_X + col * (BRICK_WIDTH + BRICK_SPACING);
            int brickY = BRICK_AREA_TOP_OFFSET + row * (BRICK_HEIGHT + BRICK_SPACING);

            m_bricks.emplace_back(brickX, brickY,
                                  BRICK_WIDTH, BRICK_HEIGHT,
                                  currentBrickTextureID, currentScoreValue);
        }
    }
}

void Game::handleMenuEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;
        SDL_Point mousePoint_motion = {mouseX, mouseY};

        m_isMouseOverStart = SDL_PointInRect(&mousePoint_motion, &m_startButtonRect);
        m_isMouseOverQuit = SDL_PointInRect(&mousePoint_motion, &m_quitButtonRect);

    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            int mouseX_click = event.button.x;
            int mouseY_click = event.button.y;
            SDL_Point mousePoint_click = {mouseX_click, mouseY_click};

            if (m_isMouseOverStart) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                resetGameForPlay();
            } else if (m_isMouseOverQuit) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                isRunning = false;
            }
            else if (SDL_PointInRect(&mousePoint_click, &m_soundIconRect)) { 
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                m_isSoundMuted = !m_isSoundMuted;
                if (m_isSoundMuted) {
                    Mix_VolumeMusic(0); Mix_Volume(-1, 0);
                } else {
                    Mix_VolumeMusic(MIX_MAX_VOLUME / 2); Mix_Volume(-1, MIX_MAX_VOLUME);
                }
            }
        }
    } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
            if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
            resetGameForPlay();
        } else if (event.key.keysym.sym == SDLK_ESCAPE) {
            if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
            isRunning = false;
        }
    }
}

void Game::handlePlayEvents(SDL_Event& event) {
    m_paddle.handleEvents(event);

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
            m_currentGameState = GameStateEnum::PAUSE;
            m_isMouseOverResume_PAUSE = false;
            m_isMouseOverMenu_PAUSE = false;
            m_isMouseOverReplay_PAUSE = false;
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            int mouseX = event.button.x;
            int mouseY = event.button.y;
            SDL_Point mousePoint_click = {mouseX, mouseY};

            if (SDL_PointInRect(&mousePoint_click, &m_pauseIconDisplayRect)) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                m_currentGameState = GameStateEnum::PAUSE;
                m_isMouseOverResume_PAUSE = false;
                m_isMouseOverMenu_PAUSE = false;
                m_isMouseOverReplay_PAUSE = false;
            } 
            else if (SDL_PointInRect(&mousePoint_click, &m_soundIconRect)) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                m_isSoundMuted = !m_isSoundMuted;
                if (m_isSoundMuted) {
                    Mix_VolumeMusic(0);
                    Mix_Volume(-1, 0);
                } else {
                    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
                    Mix_Volume(-1, MIX_MAX_VOLUME);
                }
            }
        }
    }
}

void Game::handlePauseEvents(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
            m_currentGameState = GameStateEnum::PLAY;
        }
    } else if (event.type == SDL_MOUSEMOTION) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;
        SDL_Point mousePoint = {mouseX, mouseY};
        m_isMouseOverResume_PAUSE = SDL_PointInRect(&mousePoint, &m_resumeButtonRect_PAUSE);
        m_isMouseOverMenu_PAUSE = SDL_PointInRect(&mousePoint, &m_homeButtonRect_PAUSE);
        m_isMouseOverReplay_PAUSE = SDL_PointInRect(&mousePoint, &m_replayButtonRect_PAUSE);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            if (m_isMouseOverResume_PAUSE) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                m_currentGameState = GameStateEnum::PLAY;
            } else if (m_isMouseOverMenu_PAUSE) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                m_currentGameState = GameStateEnum::MENU;
                m_isMouseOverStart = false; 
                m_isMouseOverQuit = false;
            } else if (m_isMouseOverReplay_PAUSE) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                resetGameForPlay();
            }
        }
    }
}

void Game::handleGameOverEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;
        SDL_Point mousePoint = {mouseX, mouseY};

        m_isMouseOverReplay_GO = SDL_PointInRect(&mousePoint, &m_replayButtonRect_GO);
        m_isMouseOverMenu_GO = SDL_PointInRect(&mousePoint, &m_menuButtonRect_GO);

    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            if (m_isMouseOverReplay_GO) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                resetGameForPlay();
            } else if (m_isMouseOverMenu_GO) {
                if (m_sfxButtonClick != nullptr) Mix_PlayChannel(-1, m_sfxButtonClick, 0);
                m_currentGameState = GameStateEnum::MENU;
                m_isMouseOverStart = false; 
                m_isMouseOverQuit = false;
            }
        }
    } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            isRunning = false;
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            return;
        }

        switch (m_currentGameState) {
            case GameStateEnum::MENU:
                handleMenuEvents(event);
                break;
            case GameStateEnum::PLAY:
                handlePlayEvents(event);
                break;
            case GameStateEnum::PAUSE:
                handlePauseEvents(event);
                break;
            case GameStateEnum::GAMEOVER:
                handleGameOverEvents(event);
                break;
            default:
                break;
        }
    }
}

void Game::updateMenuState() {
    m_menuBgScrollY += MENU_BG_SCROLL_SPEED;
    // Reset vị trí
    if (m_menuBgScrollY >= SCREEN_HEIGHT) {
        m_menuBgScrollY -= SCREEN_HEIGHT;
    }
}

void Game::updatePlayState() {
    m_paddle.update();
    m_ball.update();

    SDL_Rect ballRect = m_ball.getRect();
    SDL_Rect paddleRect = m_paddle.getRect();

    if (SDL_HasIntersection(&ballRect, &paddleRect)) {
        if (m_ball.getVelY() > 0 && ballRect.y + ballRect.h >= paddleRect.y && ballRect.y < paddleRect.y + paddleRect.h / 2) {
            
            m_ball.setVelY(-std::abs(m_ball.getVelY()));

            m_ball.setY(static_cast<float>(paddleRect.y - ballRect.h)); // Đặt lại vị trí Y
            
            float minAbsSpeedX = 4.0f;
            float maxAbsSpeedX = 6.5f;
            
            float randomMagnitude = minAbsSpeedX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxAbsSpeedX - minAbsSpeedX)));
            if (rand() % 2 == 0) {
                m_ball.setVelX(randomMagnitude);
            } else {
                m_ball.setVelX(-randomMagnitude);
            }
            if (m_sfxBallHit != nullptr) Mix_PlayChannel(-1, m_sfxBallHit, 0);
        }
    }

    for (Brick& brick : m_bricks) {
        if (brick.isAlive()) {
            SDL_Rect brickRect = brick.getRect();
            if (SDL_HasIntersection(&ballRect, &brickRect)) {
                m_score += brick.getScoreValue();
                brick.hit();

                m_ball.setVelY(-m_ball.getVelY());

                if (m_sfxBallHit != nullptr) Mix_PlayChannel(-1, m_sfxBallHit, 0);

                if (rand() % POWERUP_SPAWN_CHANCE_NORMAL == 0) {
                    PowerUpItem newItem;
                    newItem.rect.w = POWERUP_SIZE;
                    newItem.rect.h = POWERUP_SIZE;
                    newItem.rect.x = brickRect.x + (brickRect.w / 2) - (POWERUP_SIZE / 2);
                    newItem.rect.y = brickRect.y;
                    newItem.y_floatPos = static_cast<float>(newItem.rect.y);
                    newItem.isActive = true;

                    int powerUpChoice = rand() % 3;
                    if (powerUpChoice == 0) { 
                        newItem.type = PowerUpType::EXTRA_LIFE;
                        newItem.textureID = "powerup_life";
                    } else if (powerUpChoice == 1) {
                        newItem.type = PowerUpType::BONUS_POINTS;
                        newItem.textureID = "powerup_bonus";
                    } else {
                        newItem.type = PowerUpType::MAGIC_HAT_DESTROY_ROW;
                        newItem.textureID = "powerup_magichat";
                    }
                    m_powerUps.push_back(newItem);
                }   
            }
        }
    }

    for (size_t i = 0; i < m_powerUps.size(); ++i) {
        if (m_powerUps[i].isActive) {
            m_powerUps[i].y_floatPos += POWERUP_FALL_SPEED;
            m_powerUps[i].rect.y = static_cast<int>(m_powerUps[i].y_floatPos);

            if (m_powerUps[i].rect.y > SCREEN_HEIGHT) {
                m_powerUps[i].isActive = false;
            } 
            else if (SDL_HasIntersection(&paddleRect, &m_powerUps[i].rect)) {
                bool collected = false;
                switch (m_powerUps[i].type) {
                    case PowerUpType::EXTRA_LIFE:
                        if (m_lives < MAX_LIVES) { 
                            m_lives++;
                            collected = true;
                        } else {
                            m_score += 25;
                            collected = true;
                        }
                        break;
                    case PowerUpType::BONUS_POINTS:
                        m_score += BONUS_POINTS_VALUE;
                        collected = true;
                        break;
                    case PowerUpType::MAGIC_HAT_DESTROY_ROW:
                        if (!m_isMagicFlashActive) {
                            m_isMagicFlashActive = true;
                            m_magicFlashTimer = MAGIC_FLASH_DURATION;
                            
                            std::vector<int> activeRowsWithBricks;

                            for (int rowIndex = 0; rowIndex < NUM_BRICK_ROWS; ++rowIndex) {
                                float currentRowTargetY = static_cast<float>(BRICK_AREA_TOP_OFFSET + rowIndex * (BRICK_HEIGHT + BRICK_SPACING));
                                bool rowHasLiveBricks = false;
                                for (const Brick& brick : m_bricks) {
                                    if (brick.isAlive() && std::abs(static_cast<float>(brick.getRect().y) - currentRowTargetY) < 1.0f) {
                                        rowHasLiveBricks = true;
                                        break;
                                    }
                                }
                                if (rowHasLiveBricks) {
                                    activeRowsWithBricks.push_back(rowIndex);
                                }   
                            }

                            if (!activeRowsWithBricks.empty()) {
                                int randomListIndex = rand() % activeRowsWithBricks.size();
                                int randomRowToDestroy = activeRowsWithBricks[randomListIndex];

                                float targetY = static_cast<float>(BRICK_AREA_TOP_OFFSET + randomRowToDestroy * (BRICK_HEIGHT + BRICK_SPACING)); // Tính Y của hàng được chọn

                                int scoreFromDestroyedRow = 0;
                                int bricksInRowDestroyed = 0;

                                for (Brick& brick : m_bricks) {
                                    if (brick.isAlive() && std::abs(static_cast<float>(brick.getRect().y) - targetY) < 1.0f) {
                                        scoreFromDestroyedRow += brick.getScoreValue();
                                        brick.hit();
                                        bricksInRowDestroyed++;
                                    }
                                }
                            if (bricksInRowDestroyed > 0) m_score += scoreFromDestroyedRow;
                            else m_score += 20;
                            }
                        }
                        collected = true;
                        break;
                    default:
                        break;
                }
                if (collected) {
                   if (m_sfxReward != nullptr) Mix_PlayChannel(-1, m_sfxReward, 0);
                }
                m_powerUps[i].isActive = false;
            }
        }
    }

    if (areAllBricksCleared()) {
        std::cout << "LEVEL CLEARED! Resetting bricks." << std::endl;
        m_score += 100;
        if (m_sfxReward != nullptr) Mix_PlayChannel(-1, m_sfxReward, 0);

        int ballInitialX = m_paddle.getRect().x + (m_paddle.getRect().w / 2) - (BALL_WIDTH / 2);
        int ballInitialY = m_paddle.getRect().y - BALL_HEIGHT - 10;
        float currentBallVelX = m_ball.getVelX(); 
        if (std::abs(currentBallVelX) < 1.0f) currentBallVelX = (rand() % 2 == 0) ? 3.0f : -3.0f;
        float newBallVelY = -std::abs(m_ball.getVelY()); 
        if (newBallVelY == 0) newBallVelY = -3.0f;
        m_ball.reset(ballInitialX, ballInitialY, currentBallVelX, newBallVelY);

        createBrickLayout();
       
        if (!m_isMagicFlashActive) {
            m_isMagicFlashActive = true;
            m_magicFlashTimer = MAGIC_FLASH_DURATION;
        }
    }
    if (m_ball.isOutOfBounds()) {
        if (m_sfxLoseLife != nullptr) Mix_PlayChannel(-1, m_sfxLoseLife, 0);
        m_lives--;
        std::cout << "==> Ball is Out! Lives NOW: " << m_lives << std::endl;

        if (m_lives > 0) {
            
            int ballInitialX = SCREEN_WIDTH / 2 - BALL_WIDTH / 2; 
            int ballInitialY = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;  
            
            float ballVelX = (rand() % 2 == 0) ? 3.0f : -3.0f;
            float ballVelY = -3.0f;
            m_ball.reset(ballInitialX, ballInitialY, ballVelX, ballVelY);
        } else {

            if (m_score > m_highScore) {
                m_highScore = m_score;
                saveHighScore();
            } 
            m_currentGameState = GameStateEnum::GAMEOVER;
            Mix_HaltMusic();
        }  
    }
    // Cập nhật timer cho hiệu ứng lóe màn hình
    if (m_isMagicFlashActive) {
        m_magicFlashTimer -= 0.0166f;

        if (m_magicFlashTimer <= 0.0f) {
            m_isMagicFlashActive = false;
            m_magicFlashTimer = 0.0f;
        }
    }
}

void Game::updatePauseState() {
}

void Game::updateGameOverState() {
}

void Game::update() {
    switch (m_currentGameState) {
        case GameStateEnum::MENU:
            updateMenuState();
            break;

        case GameStateEnum::PLAY:
            updatePlayState();
            break;

        case GameStateEnum::PAUSE:
            updatePauseState();
            break;

        case GameStateEnum::GAMEOVER:
            updateGameOverState();
            break;

        default:
            break;
        
    }
}

void Game::renderMenuScreen() {
    // Vẽ background 2 lần để tạo hiệu ứng cuộn vô tận theo chiều dọc
    SDL_Rect bgRect1 = {0, static_cast<int>(m_menuBgScrollY), SCREEN_WIDTH, SCREEN_HEIGHT};
    textureManager.draw(renderer, "background_shared", bgRect1.x, bgRect1.y, bgRect1.w, bgRect1.h);
    SDL_Rect bgRect2 = {0, static_cast<int>(m_menuBgScrollY) - SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT};
    textureManager.draw(renderer, "background_shared", bgRect2.x, bgRect2.y, bgRect2.w, bgRect2.h);

    textureManager.draw(renderer, "logo_game", m_logoDisplayRect.x, m_logoDisplayRect.y, m_logoDisplayRect.w, m_logoDisplayRect.h);

    SDL_Color startColor = m_isMouseOverStart ? m_buttonHoverColor : m_buttonNormalColor;
    renderText(m_fontMenuItem, "START GAME", m_startButtonRect.x, m_startButtonRect.y, startColor);

    SDL_Color quitColor = m_isMouseOverQuit ? m_buttonHoverColor : m_buttonNormalColor;
    renderText(m_fontMenuItem, "QUIT", m_quitButtonRect.x, m_quitButtonRect.y, quitColor);

    SDL_Color goldColor = {255, 215, 0, 255};
    std::string highScoreText_Menu = "Best: " + std::to_string(m_highScore);
    int textW_hs, textH_hs;
    if (m_fontUIText != nullptr && TTF_SizeText(m_fontUIText, highScoreText_Menu.c_str(), &textW_hs, &textH_hs) == 0) {
        renderText(m_fontUIText, highScoreText_Menu, UI_SIDE_MARGIN, SCREEN_HEIGHT - textH_hs - UI_BOTTOM_MARGIN, goldColor);
    }
    m_soundIconRect.x = UI_SIDE_MARGIN; 
    std::string soundIconToDraw = m_isSoundMuted ? "icon_sound_off" : "icon_sound_on";
    textureManager.draw(renderer, soundIconToDraw, 
        m_soundIconRect.x, m_soundIconRect.y, 
        m_soundIconRect.w, m_soundIconRect.h);
}

void Game::renderPlayScreen() {
    textureManager.draw(renderer, "background_shared", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    m_paddle.render(renderer, textureManager);
    m_ball.render(renderer, textureManager);
    for (Brick& brick : m_bricks) {
        brick.render(renderer, textureManager);
    }
    SDL_Color whiteColor = {255, 255, 255, 255};

    int textW, textH;

    std::string scoreDisplayString = std::to_string(m_score);
    if (m_fontScore != nullptr && TTF_SizeText(m_fontScore, scoreDisplayString.c_str(), &textW, &textH) == 0) {
        int scoreX = (SCREEN_WIDTH - textW) / 2;
        int scoreY = UI_TOP_MARGIN;              
        renderText(m_fontScore, scoreDisplayString, scoreX, scoreY, whiteColor);
    }

    for (int i = 0; i < m_lives; ++i) {
        int heartX = SCREEN_WIDTH - UI_SIDE_MARGIN - (i + 1) * HEART_ICON_WIDTH - i * HEART_SPACING;
        int heartY = UI_TOP_MARGIN;
        textureManager.draw(renderer, "heart_icon", heartX, heartY, HEART_ICON_WIDTH, HEART_ICON_HEIGHT);
    }
    textureManager.draw(renderer, "icon_pause", 
        m_pauseIconDisplayRect.x, m_pauseIconDisplayRect.y, 
        m_pauseIconDisplayRect.w, m_pauseIconDisplayRect.h);

    m_soundIconRect.x = m_pauseIconDisplayRect.x + m_pauseIconDisplayRect.w + ICON_SPACING;
    std::string soundIconToDraw_Play = m_isSoundMuted ? "icon_sound_off" : "icon_sound_on";
    textureManager.draw(renderer, soundIconToDraw_Play, 
        m_soundIconRect.x, m_soundIconRect.y, 
        m_soundIconRect.w, m_soundIconRect.h);
    
    for (const PowerUpItem& pu : m_powerUps) {
        if (pu.isActive) {
            textureManager.draw(renderer, pu.textureID, pu.rect.x, pu.rect.y, pu.rect.w, pu.rect.h);
        }
    }

    if (m_isMagicFlashActive) {
        // Tính toán độ trong suốt
        float flashAlphaPercentage = 0.0f;
        if (MAGIC_FLASH_DURATION > 0.001f) {
            flashAlphaPercentage = m_magicFlashTimer / MAGIC_FLASH_DURATION;
        }

        flashAlphaPercentage = std::max(0.0f, std::min(1.0f, flashAlphaPercentage));
        Uint8 alpha = static_cast<Uint8>(255 * flashAlphaPercentage * 0.65f);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);

        SDL_Rect flashRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &flashRect);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

void Game::renderPauseScreen() {
    m_paddle.render(renderer, textureManager);
    m_ball.render(renderer, textureManager);
    for (Brick& brick : m_bricks) {
        brick.render(renderer, textureManager);
    }
    SDL_Color whiteColor = {255, 255, 255, 255};
    SDL_Color goldColor = {255, 215, 0, 255};
    int textW, textH;
    std::string scoreDisplayString = std::to_string(m_score);
    if (m_fontScore != nullptr && TTF_SizeText(m_fontScore, scoreDisplayString.c_str(), &textW, &textH) == 0) {
        renderText(m_fontScore, scoreDisplayString, (SCREEN_WIDTH - textW) / 2, UI_TOP_MARGIN, whiteColor);
    }
    for (int i = 0; i < m_lives; ++i) {
        textureManager.draw(renderer, "heart_icon", SCREEN_WIDTH - UI_SIDE_MARGIN - (i + 1) * HEART_ICON_WIDTH - i * HEART_SPACING, UI_TOP_MARGIN, HEART_ICON_WIDTH, HEART_ICON_HEIGHT);
    }
    textureManager.draw(renderer, "icon_pause", m_pauseIconDisplayRect.x, m_pauseIconDisplayRect.y, m_pauseIconDisplayRect.w, m_pauseIconDisplayRect.h);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect screenOverlayRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screenOverlayRect);

    SDL_Color titlePausedColor = {255, 255, 0, 255};
    SDL_Color optionColorNormal = m_buttonNormalColor;
    SDL_Color optionColorHover = m_buttonHoverColor;

    int currentTextY = SCREEN_HEIGHT / 3;

    std::string pausedMsg = "PAUSED";
    if (m_fontScore != nullptr && TTF_SizeText(m_fontScore, pausedMsg.c_str(), &textW, &textH) == 0) {
        int msgX = (SCREEN_WIDTH - textW) / 2;
        renderText(m_fontScore, pausedMsg, msgX, currentTextY, titlePausedColor);
        currentTextY += textH + 40; 
    }

    std::string resumeText = "RESUME";
    if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, resumeText.c_str(), &textW, &textH) == 0) {
        m_resumeButtonRect_PAUSE = {(SCREEN_WIDTH - textW) / 2, currentTextY, textW, textH};
        SDL_Color resumeColor = m_isMouseOverResume_PAUSE ? optionColorHover : optionColorNormal;
        renderText(m_fontMenuItem, resumeText, m_resumeButtonRect_PAUSE.x, m_resumeButtonRect_PAUSE.y, resumeColor);
        currentTextY += textH + 15;
    }

    std::string replayText_Pause = "REPLAY";
    if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, replayText_Pause.c_str(), &textW, &textH) == 0) {
        m_replayButtonRect_PAUSE = {(SCREEN_WIDTH - textW) / 2, currentTextY, textW, textH};
        SDL_Color replayColor = m_isMouseOverReplay_PAUSE ? optionColorHover : optionColorNormal;
        renderText(m_fontMenuItem, replayText_Pause, m_replayButtonRect_PAUSE.x, m_replayButtonRect_PAUSE.y, replayColor);
        currentTextY += textH + 15;
    }
    
    std::string menuText_Pause = "MENU";
     if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, menuText_Pause.c_str(), &textW, &textH) == 0) {
        m_homeButtonRect_PAUSE = {(SCREEN_WIDTH - textW) / 2, currentTextY, textW, textH};
        SDL_Color menuBtnColor = m_isMouseOverMenu_PAUSE ? optionColorHover : optionColorNormal;
        renderText(m_fontMenuItem, menuText_Pause, m_homeButtonRect_PAUSE.x, m_homeButtonRect_PAUSE.y, menuBtnColor);
    }
}

void Game::renderGameOverScreen() {
    SDL_Color titleColor = {255, 69, 0, 255};    
    SDL_Color textColor = m_buttonNormalColor;
    SDL_Color hoverColor = m_buttonHoverColor;
    SDL_Color highScoreColor = {255, 215, 0, 255};

    int textW, textH;
    int panelX = SCREEN_WIDTH / 8;
    int panelW = SCREEN_WIDTH * 3 / 4;
    int currentTextY = SCREEN_HEIGHT / 4;

    std::string gameOverMsg = "GAME OVER";
    if (m_fontScore != nullptr && TTF_SizeText(m_fontScore, gameOverMsg.c_str(), &textW, &textH) == 0) {
        int msgX = (SCREEN_WIDTH - textW) / 2;
        renderText(m_fontScore, gameOverMsg, msgX, currentTextY, titleColor);
        currentTextY += textH + 30; 
    }

    std::string finalScoreMsg = "Final Score: " + std::to_string(m_score);
    if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, finalScoreMsg.c_str(), &textW, &textH) == 0) {
        int msgX = (SCREEN_WIDTH - textW) / 2;
        renderText(m_fontMenuItem, finalScoreMsg, msgX, currentTextY, textColor);
        currentTextY += textH + 15; 
    }

    std::string highScoreStr = "Best Score: " + std::to_string(m_highScore);
    if (m_fontUIText != nullptr && TTF_SizeText(m_fontUIText, highScoreStr.c_str(), &textW, &textH) == 0) {
        int msgX = (SCREEN_WIDTH - textW) / 2;
        renderText(m_fontUIText, highScoreStr, msgX, currentTextY, highScoreColor);
        currentTextY += textH + 50; 
    }

    std::string replayText = "REPLAY";
    if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, replayText.c_str(), &textW, &textH) == 0) {
        m_replayButtonRect_GO.x = (SCREEN_WIDTH - textW) / 2;
        m_replayButtonRect_GO.y = currentTextY;
        m_replayButtonRect_GO.w = textW;
        m_replayButtonRect_GO.h = textH;
        SDL_Color replayBtnColor = m_isMouseOverReplay_GO ? hoverColor : textColor;
        renderText(m_fontMenuItem, replayText, m_replayButtonRect_GO.x, m_replayButtonRect_GO.y, replayBtnColor);
        currentTextY += textH + 15;
    }

    std::string menuText = "MENU";
     if (m_fontMenuItem != nullptr && TTF_SizeText(m_fontMenuItem, menuText.c_str(), &textW, &textH) == 0) {
        m_menuButtonRect_GO.x = (SCREEN_WIDTH - textW) / 2;
        m_menuButtonRect_GO.y = currentTextY;
        m_menuButtonRect_GO.w = textW;
        m_menuButtonRect_GO.h = textH;
        SDL_Color menuBtnColor = m_isMouseOverMenu_GO ? hoverColor : textColor;
        renderText(m_fontMenuItem, menuText, m_menuButtonRect_GO.x, m_menuButtonRect_GO.y, menuBtnColor);
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    switch (m_currentGameState) {
        case GameStateEnum::MENU:
            renderMenuScreen();
            break;
        case GameStateEnum::PLAY:
            renderPlayScreen();
            break;
        case GameStateEnum::PAUSE:
            renderPauseScreen();
            break;
        case GameStateEnum::GAMEOVER:
        renderGameOverScreen();
            break;            
        default:
            break;
    }
   SDL_RenderPresent(renderer);
}


void Game::resetGameForPlay() {
    std::cout << "Resetting game for PLAY state..." << std::endl;

    m_powerUps.clear();
    
    m_lives = 3;
    
    m_score = 0;

    m_isMagicFlashActive = false;
    m_magicFlashTimer = 0.0f;

    int ballInitialX = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
    int ballInitialY = SCREEN_HEIGHT / 2;
    
    float ballInitialVelX = (rand() % 2 == 0) ? 3.0f : -3.0f;
    float ballInitialVelY = -3.0f;
    m_ball.init("ball", ballInitialX, ballInitialY, BALL_WIDTH, BALL_HEIGHT, ballInitialVelX, ballInitialVelY, m_sfxBallHit);

    createBrickLayout(); 

    m_currentGameState = GameStateEnum::PLAY;

    if (m_musicPlay != nullptr) {
        if (Mix_PlayingMusic() == 0) {
            Mix_PlayMusic(m_musicPlay, -1);
            std::cout << "Background music (re)started for PLAY state." << std::endl;
        } else if (Mix_PausedMusic()) {
            Mix_ResumeMusic();
            std::cout << "Background music resumed for PLAY state." << std::endl;
        }
    }
}

bool Game::areAllBricksCleared() const {
    for (const Brick& brick : m_bricks) {
        if (brick.isAlive()) {
            return false;
        }
    }
    return true;
}

void Game::loadHighScore() {
    std::ifstream highScoreFile("highscore.txt");
    if (highScoreFile.is_open()) {
        if (!(highScoreFile >> m_highScore)) {
            std::cerr << "Warning: Could not read high score from highscore.txt or file is empty/corrupt. Defaulting to 0." << std::endl;
            m_highScore = 0;
        }
        highScoreFile.close();
        std::cout << "Loaded high score: " << m_highScore << std::endl;
    } else {
        std::cout << "highscore.txt not found. Starting with high score 0." << std::endl;
        m_highScore = 0;
    }
}

void Game::saveHighScore() {
    std::ofstream highScoreFile("highscore.txt");
    if (highScoreFile.is_open()) {
        highScoreFile << m_highScore;
        highScoreFile.close();
        std::cout << "High score " << m_highScore << " saved to highscore.txt" << std::endl;
    } else {
        std::cerr << "Error: Unable to open highscore.txt for writing." << std::endl;
    }
}


void Game::clean() {
    std::cout << "Cleaning up game..." << std::endl;

    textureManager.clearAllTextures();

    m_bricks.clear();
    m_powerUps.clear();

    if (Mix_PlayingMusic()) {
       Mix_HaltMusic();
    }
    if(m_musicPlay != nullptr) {
        Mix_FreeMusic(m_musicPlay);
        m_musicPlay = nullptr;
    }
    if(m_sfxBallHit != nullptr) {
        Mix_FreeChunk(m_sfxBallHit);
        m_sfxBallHit = nullptr;
    }
    if(m_sfxLoseLife != nullptr) {
        Mix_FreeChunk(m_sfxLoseLife);
        m_sfxLoseLife = nullptr;
    }
    if(m_sfxButtonClick != nullptr) {
        Mix_FreeChunk(m_sfxButtonClick);
        m_sfxButtonClick = nullptr;
    }
     if(m_sfxReward != nullptr) {
        Mix_FreeChunk(m_sfxReward);
        m_sfxReward = nullptr;
    }

    if (m_fontScore != nullptr) {
        TTF_CloseFont(m_fontScore);
        m_fontScore = nullptr;
    }
    if (m_fontMenuItem != nullptr) {
        TTF_CloseFont(m_fontMenuItem);
        m_fontMenuItem = nullptr;
    }
    if (m_fontUIText != nullptr) {
        TTF_CloseFont(m_fontUIText);
        m_fontUIText = nullptr;
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();

    IMG_Quit();

    TTF_Quit();

    SDL_Quit();
    std::cout << "Cleanup finished." << std::endl;
}