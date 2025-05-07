#include "Game.h"
#include "Constants.h"
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
}

Game::~Game() {
}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    srand(static_cast<unsigned int>(time(0)));
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "1. SDL core initialized." << std::endl;

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    std::cout << "SDL_ttf initialized." << std::endl;

    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit(); 
        return false;
    }
    std::cout << "2. Window created." << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window); 
        SDL_Quit();
        return false;
    }
    std::cout << "3. Renderer created." << std::endl;

    m_buttonNormalColor = {255, 255, 255, 255};
    m_buttonHoverColor = {255, 255, 0, 255};
    m_isMouseOverStart = false;
    m_isMouseOverQuit = false;

    if (!textureManager.load(renderer, "assets/images/logo.png", "logo_game")) {
        std::cerr << "Failed to load logo" << std::endl;
        return false; 
    }
    std::cout << "Logo texture loaded." << std::endl;

    const int ORIGINAL_LOGO_W = 896;
    const int ORIGINAL_LOGO_H = 772;
    const int LOGO_DISPLAY_WIDTH = 300;
    m_logoDisplayRect.w = LOGO_DISPLAY_WIDTH;
    m_logoDisplayRect.h = static_cast<int>(static_cast<float>(ORIGINAL_LOGO_H) * (static_cast<float>(LOGO_DISPLAY_WIDTH) / static_cast<float>(ORIGINAL_LOGO_W)));
    m_logoDisplayRect.x = (SCREEN_WIDTH - m_logoDisplayRect.w) / 2;
    m_logoDisplayRect.y = SCREEN_HEIGHT / 6 - m_logoDisplayRect.h / 2;
    if (m_logoDisplayRect.y < UI_TOP_MARGIN) m_logoDisplayRect.y = UI_TOP_MARGIN;


    std::cout << "Loading assets..." << std::endl;
    if (!textureManager.load(renderer, "assets/images/paddle.png", "paddle")) { 
        std::cerr << "Failed to load paddle texture!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    std::cout << "Paddle loaded." << std::endl;

    int initialPaddleX = (SCREEN_WIDTH / 2) - (PADDLE_WIDTH / 2);
    int initialPaddleY = SCREEN_HEIGHT - PADDLE_HEIGHT - PADDLE_MARGIN_BOTTOM;
    m_paddle.init("paddle", initialPaddleX, initialPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT); 
    std::cout << "Paddle initialized." << std::endl;

    if (!textureManager.load(renderer, "assets/images/ball.png", "ball")) {
        std::cerr << "Failed to load ball texture!" << std::endl;
        textureManager.clearFromTextureMap("paddle");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    std::cout << "Ball texture loaded." << std::endl;


    int ballInitialX = m_paddle.getRect().x + (m_paddle.getRect().w / 2) - (BALL_WIDTH / 2);
    int ballInitialY = m_paddle.getRect().y - BALL_HEIGHT - 5;

    float ballInitialVelX = 5.0f;
    float ballInitialVelY = -5.0f;

    m_ball.init("ball", ballInitialX, ballInitialY, BALL_WIDTH, BALL_HEIGHT, ballInitialVelX, ballInitialVelY);
    std::cout << "Ball initialized." << std::endl;

    if (!textureManager.load(renderer, "assets/images/brick_blue.png", "brick_blue")) return false; 
    if (!textureManager.load(renderer, "assets/images/brick_red.png", "brick_red")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_yellow.png", "brick_yellow")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_green.png", "brick_green")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_pink.png", "brick_pink")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_purple.png", "brick_purple")) return false;
    std::cout << "Brick textures loaded." << std::endl;

    if (!textureManager.load(renderer, "assets/images/heart.png", "heart_icon")) {
        std::cerr << "Failed to load heart_icon.png!" << std::endl;
        return false;
    }
    std::cout << "Heart icon texture loaded." << std::endl;


    if (!textureManager.load(renderer, "assets/images/icon_pause.png", "icon_pause")) {
        std::cerr << "Failed to load icon_pause.png!" << std::endl;
    return false;
    }
    std::cout << "Pause icon texture loaded." << std::endl;

    m_pauseIconDisplayRect.x = UI_SIDE_MARGIN;
    m_pauseIconDisplayRect.y = UI_TOP_MARGIN;
    m_pauseIconDisplayRect.w = PAUSE_ICON_WIDTH;
    m_pauseIconDisplayRect.h = PAUSE_ICON_HEIGHT;

    
    createBrickLayout();
    

    m_fontScore = TTF_OpenFont("assets/fonts/jersey.ttf", FONT_SIZE_SCORE);
    if (m_fontScore == nullptr) {
        std::cerr << "Failed to load font for Score! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    m_fontMenuItem = TTF_OpenFont("assets/fonts/jersey.ttf", FONT_SIZE_MENU_ITEM);
    if (m_fontMenuItem == nullptr) {
        std::cerr << "Failed to load font for Menu Items! TTF_Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(m_fontScore);
        return false;
    }

    m_fontUIText = TTF_OpenFont("assets/fonts/jersey.ttf", FONT_SIZE_UI_TEXT);
    if (m_fontUIText == nullptr) {
        std::cerr << "Failed to load font for UI Text! TTF_Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(m_fontScore);
        TTF_CloseFont(m_fontMenuItem);
        return false;
    }
    std::cout << "All fonts loaded successfully." << std::endl;


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

    std::cout << "Menu items positions calculated for vertical centering." << std::endl;

    m_score = 0; // Khởi tạo điểm
    std::cout << "Score initialized to: " << m_score << std::endl;

    loadHighScore();

    m_lives = 3; 
    std::cout << "Player lives set to: " << m_lives << std::endl;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    m_currentGameState = GameStateEnum::MENU;
    std::cout << "Current game state set to MENU." << std::endl;

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
    std::cout << "DEBUG_LAYOUT: Creating layout with NUM_BRICK_ROWS = " << NUM_BRICK_ROWS 
              << ", NUM_BRICK_COLUMNS = " << NUM_BRICK_COLUMNS << std::endl;

    std::cout << "Creating brick layout..." << std::endl;
    m_bricks.clear(); 

    std::string brickTextureIDs[NUM_BRICK_ROWS] = { 
        "brick_red",
        "brick_pink",
        "brick_yellow",
        "brick_green", 
        "brick_blue",
        "brick_purple"
    };

    for (int row = 0; row < NUM_BRICK_ROWS; ++row) { 
        for (int col = 0; col < NUM_BRICK_COLUMNS; ++col) {
            int brickX = BRICK_AREA_START_X + col * (BRICK_WIDTH + BRICK_SPACING);
            int brickY = BRICK_AREA_TOP_OFFSET + row * (BRICK_HEIGHT + BRICK_SPACING);

            std::string currentBrickTextureID = brickTextureIDs[row];

            m_bricks.emplace_back(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT, currentBrickTextureID);
        }
    }
    std::cout << "Brick layout created with " << m_bricks.size() << " bricks." << std::endl;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        switch (m_currentGameState) {
            case GameStateEnum::MENU:
            {
                if (event.type == SDL_MOUSEMOTION) {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;

                    SDL_Point mousePoint = {mouseX, mouseY};
                    m_isMouseOverStart = SDL_PointInRect(&mousePoint, &m_startButtonRect);
                    m_isMouseOverQuit = SDL_PointInRect(&mousePoint, &m_quitButtonRect);

                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (m_isMouseOverStart) {
                            std::cout << "Start button clicked!" << std::endl;
                            resetGameForPlay();
                        } else if (m_isMouseOverQuit) {
                            std::cout << "Quit button clicked!" << std::endl;
                            isRunning = false;
                        }
                    }
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                         std::cout << "Enter/Space pressed on Menu - Starting game!" << std::endl;
                         resetGameForPlay();
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        std::cout << "Escape pressed on Menu - Quitting game!" << std::endl;
                        isRunning = false;
                    }
                }
            }
                break;
            case GameStateEnum::PLAY:
                m_paddle.handleEvents(event);
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        m_currentGameState = GameStateEnum::PAUSE;
                        std::cout << "Game Paused (ESC pressed). Current state: PAUSE" << std::endl;
                        m_isMouseOverResume_PAUSE = false;
                        m_isMouseOverMenu_PAUSE = false;
                        m_isMouseOverReplay_PAUSE = false;
                    }
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;
                        SDL_Point mousePoint = {mouseX, mouseY};
                        if (SDL_PointInRect(&mousePoint, &m_pauseIconDisplayRect)) {
                            m_currentGameState = GameStateEnum::PAUSE;
                            std::cout << "Pause icon clicked. Game Paused. Current state: PAUSE" << std::endl;
                            m_isMouseOverResume_PAUSE = false;
                            m_isMouseOverMenu_PAUSE = false;
                            m_isMouseOverReplay_PAUSE = false;
                        }
                    }
                }
                break;
            case GameStateEnum::PAUSE:
            {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        m_currentGameState = GameStateEnum::PLAY;
                        std::cout << "Game Resumed (ESC pressed). Current state: PLAY" << std::endl;
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
                            m_currentGameState = GameStateEnum::PLAY;
                            std::cout << "Resume button clicked. Current state: PLAY" << std::endl;
                        } else if (m_isMouseOverMenu_PAUSE) {
                            m_currentGameState = GameStateEnum::MENU;
                            m_isMouseOverStart = false; 
                            m_isMouseOverQuit = false;
                            std::cout << "Return to Menu button clicked (Pause). Current state: MENU" << std::endl;
                        } else if (m_isMouseOverReplay_PAUSE) {
                            std::cout << "Replay button clicked (Pause)!" << std::endl;
                            resetGameForPlay();
                        }
                    }
                }
            }
                break;
            case GameStateEnum::GAMEOVER:
            {
                if (event.type == SDL_MOUSEMOTION) {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;
                    SDL_Point mousePoint = {mouseX, mouseY};

                    m_isMouseOverReplay_GO = SDL_PointInRect(&mousePoint, &m_replayButtonRect_GO);
                    m_isMouseOverMenu_GO = SDL_PointInRect(&mousePoint, &m_menuButtonRect_GO);

                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (m_isMouseOverReplay_GO) {
                            std::cout << "Replay button clicked (Game Over)!" << std::endl;
                            resetGameForPlay();
                        } else if (m_isMouseOverMenu_GO) {
                            std::cout << "Return to Menu button clicked (Game Over)!" << std::endl;
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
                break;
            default:
                break;
        }
    }
}

void Game::update() {
    switch (m_currentGameState) {
        case GameStateEnum::MENU:
            break;

        case GameStateEnum::PLAY:
        {

            m_paddle.update();
            m_ball.update();

            SDL_Rect ballRect = m_ball.getRect();
            SDL_Rect paddleRect = m_paddle.getRect();

            if (SDL_HasIntersection(&ballRect, &paddleRect)) {
                // Chỉ xử lý va chạm nếu bóng đang đi xuống và ở đúng độ cao của paddle
                if (m_ball.getVelY() > 0 && ballRect.y + ballRect.h >= paddleRect.y && ballRect.y < paddleRect.y + paddleRect.h / 2) { // Thêm điều kiện check y của bóng so với paddle
                    std::cout << "Collision Paddle-Ball Detected! (Random X bounce)" << std::endl;
                    
                    // 1. Đảo chiều vận tốc Y (luôn hướng lên)
                    m_ball.setVelY(-std::abs(m_ball.getVelY())); // Dùng std::abs cho float

                    // 2. Đặt lại vị trí Y của bóng ngay trên paddle để tránh bị dính
                    m_ball.setY(static_cast<float>(paddleRect.y - ballRect.h)); 
                    
                    // 3. TẠO VẬN TỐC X NGẪU NHIÊN MỚI
                    float minAbsSpeedX = 4.0f; // Tốc độ ngang tối thiểu (để không quá thẳng đứng)
                    float maxAbsSpeedX = 6.5f; // Tốc độ ngang tối đa (bạn có thể điều chỉnh các giá trị này)
                    
                    // Tạo giá trị tuyệt đối ngẫu nhiên cho velX trong khoảng [minAbsSpeedX, maxAbsSpeedX]
                    float randomMagnitude = minAbsSpeedX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxAbsSpeedX - minAbsSpeedX)));
                    
                    // Random hướng (50% trái, 50% phải)
                    if (rand() % 2 == 0) {
                        m_ball.setVelX(randomMagnitude);
                    } else {
                        m_ball.setVelX(-randomMagnitude);
                    }
                    std::cout << "Paddle hit - New random velX: " << m_ball.getVelX() << std::endl;
                }
            }

            for (Brick& brick : m_bricks) {
                if (brick.isAlive()) {
                    SDL_Rect brickRect = brick.getRect();
                    if (SDL_HasIntersection(&ballRect, &brickRect)) {
                        std::cout << "Collision Ball-Brick Detected!" << std::endl;
                        
                        brick.hit();

                        m_ball.setVelY(-m_ball.getVelY());

                        m_score += 10;
                        std::cout << "Brick hit! Score: " << m_score << std::endl;

                        break;
                    }
                }
            }

            if (areAllBricksCleared()) {
                std::cout << "LEVEL CLEARED! Resetting bricks." << std::endl;
                
                std::cout << "Score after level bonus: " << m_score << std::endl;

                int ballInitialX = m_paddle.getRect().x + (m_paddle.getRect().w / 2) - (BALL_WIDTH / 2);
                int ballInitialY = m_paddle.getRect().y - BALL_HEIGHT - 10;
                float currentBallVelX = m_ball.getVelX(); 
                if (abs(currentBallVelX) < 1.0f) currentBallVelX = (rand() % 2 == 0) ? 3.0f : -3.0f;
                float newBallVelY = -abs(m_ball.getVelY()); 
                if (newBallVelY == 0) newBallVelY = -3.0f;
                m_ball.reset(ballInitialX, ballInitialY, currentBallVelX, newBallVelY);
                std::cout << "Ball reset for new level." << std::endl;

                createBrickLayout();
            }
            if (m_ball.isOutOfBounds()) {
                m_lives--;
                std::cout << "==> Ball is Out! Lives NOW: " << m_lives << std::endl;

                if (m_lives > 0) {
                    std::cout << "    DEBUG: Lives > 0, resetting ball..." << std::endl;
                    
                    int ballInitialX = SCREEN_WIDTH / 2 - BALL_WIDTH / 2; 
                    int ballInitialY = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;  
                    
                    float ballVelX = (rand() % 2 == 0) ? 3.0f : -3.0f;
                    float ballVelY = -3.0f; // Luôn hướng lên
                    m_ball.reset(ballInitialX, ballInitialY, ballVelX, ballVelY);
                    std::cout << "    DEBUG: Ball reset completed." << std::endl;
                } else { // m_lives <= 0 (Hết mạng)
                    std::cout << "    DEBUG: Lives <= 0. Processing Game Over..." << std::endl;
                    std::cout << "    DEBUG: Current m_score = " << m_score 
                              << ", Current m_highScore = " << m_highScore << std::endl;

                    if (m_score > m_highScore) {
                        m_highScore = m_score;
                        std::cout << "    DEBUG: New High Score ACHIEVED: " << m_highScore << std::endl;
                        saveHighScore();
                    } else {
                        std::cout << "    DEBUG: Score (" << m_score << ") did not beat high score (" << m_highScore << ")." << std::endl;
                    }
                    
                    std::cout << "    DEBUG: Changing current game state to GAMEOVER..." << std::endl;
                    m_currentGameState = GameStateEnum::GAMEOVER;
                    std::cout << "    DEBUG: Current game state is now GAMEOVER." << std::endl;
                }  
            }
        }
        break;

        case GameStateEnum::PAUSE:
            break;

        case GameStateEnum::GAMEOVER:
            break;

        default:
            break;
    }
}


void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    switch (m_currentGameState) {
        case GameStateEnum::MENU:
        {
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
        }
            break;
        case GameStateEnum::PLAY:
        {
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

        }
            break;
        case GameStateEnum::PAUSE:
        {
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
            if (m_fontScore != nullptr && TTF_SizeText(m_fontScore, pausedMsg.c_str(), &textW, &textH) == 0) { // Dùng font to
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
                m_homeButtonRect_PAUSE = {(SCREEN_WIDTH - textW) / 2, currentTextY, textW, textH}; // Sửa tên biến Rect
                SDL_Color menuBtnColor = m_isMouseOverMenu_PAUSE ? optionColorHover : optionColorNormal; // Sửa tên biến cờ hover
                renderText(m_fontMenuItem, menuText_Pause, m_homeButtonRect_PAUSE.x, m_homeButtonRect_PAUSE.y, menuBtnColor);
            }
        }
            break;
        case GameStateEnum::GAMEOVER:
        {
            SDL_Color titleColor = {255, 69, 0, 255};    
            SDL_Color textColor = m_buttonNormalColor;
            SDL_Color hoverColor = m_buttonHoverColor;
            SDL_Color highScoreColor = {255, 215, 0, 255};

            int textW, textH;
            int panelX = SCREEN_WIDTH / 8;
            int panelW = SCREEN_WIDTH * 3 / 4;
            int currentTextY = SCREEN_HEIGHT / 4;

            // Chữ "GAME OVER"
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
            break;
            
        default:
            break;
    }

    SDL_RenderPresent(renderer);
}


void Game::resetGameForPlay() {
    std::cout << "Resetting game for PLAY state..." << std::endl;
    
    m_lives = 3;
    std::cout << "Player lives reset to: " << m_lives << std::endl;
    
    m_score = 0;
    std::cout << "Score reset to: " << m_score << std::endl;

    int ballInitialX = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
    int ballInitialY = SCREEN_HEIGHT / 2;
    
    float ballInitialVelX = (rand() % 2 == 0) ? 3.0f : -3.0f;
    float ballInitialVelY = -3.0f;
    m_ball.init("ball", ballInitialX, ballInitialY, BALL_WIDTH, BALL_HEIGHT, ballInitialVelX, ballInitialVelY);
    std::cout << "Ball reset for new game." << std::endl;

    createBrickLayout(); 

    m_currentGameState = GameStateEnum::PLAY;
    std::cout << "Switched to PLAY state." << std::endl;
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
    std::cout << "Texture Manager cleaned." << std::endl;

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
    std::cout << "All fonts closed." << std::endl;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;

    TTF_Quit();
    std::cout << "SDL_ttf cleaned." << std::endl;

    SDL_Quit();
}