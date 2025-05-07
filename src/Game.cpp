#include "Game.h"
#include "Constants.h"
#include <iostream>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), m_currentGameState(GameStateEnum::NONE) {
}

Game::~Game() {
}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "1. SDL core initialized." << std::endl;

    // Tạo cửa sổ
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


    std::cout << "Loading assets..." << std::endl;
    if (!textureManager.load(renderer, "assets/images/paddle.png", "paddle")) { 
        std::cerr << "Failed to load paddle texture!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    std::cout << "Assets loaded." << std::endl;

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

    float ballInitialVelX = 3.0f;
    float ballInitialVelY = -3.0f;

    m_ball.init("ball", ballInitialX, ballInitialY, BALL_WIDTH, BALL_HEIGHT, ballInitialVelX, ballInitialVelY);
    std::cout << "Ball initialized." << std::endl;

    if (!textureManager.load(renderer, "assets/images/brick_blue.png", "brick_blue")) return false; 
    if (!textureManager.load(renderer, "assets/images/brick_red.png", "brick_red")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_yellow.png", "brick_yellow")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_green.png", "brick_green")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_pink.png", "brick_pink")) return false;
    if (!textureManager.load(renderer, "assets/images/brick_purple.png", "brick_purple")) return false;
    std::cout << "Brick textures loaded." << std::endl;


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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    m_currentGameState = GameStateEnum::PLAY;
    std::cout << "Current game state set to PLAY." << std::endl;

    isRunning = true; 
    std::cout << "Game initialized successfully!" << std::endl;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        switch (m_currentGameState) {
            case GameStateEnum::MENU:
                break;
            case GameStateEnum::PLAY:
                m_paddle.handleEvents(event);
                break;
            case GameStateEnum::PAUSE:
                break;
            case GameStateEnum::GAMEOVER:
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
                if (m_ball.getVelY() > 0) { 
                    std::cout << "Collision Paddle-Ball Detected!" << std::endl;
                    m_ball.setVelY(-abs(m_ball.getVelY()));
                    m_ball.setY(static_cast<float>(paddleRect.y - ballRect.h)); 
                    
                    float paddleCenter = paddleRect.x + paddleRect.w / 2.0f;
                    float ballCenter = ballRect.x + ballRect.w / 2.0f;
                    float offset = (ballCenter - paddleCenter) / (paddleRect.w / 2.0f); 
                    float maxBounceAngleEffect = 5.0f; 
                    float currentVelX = m_ball.getVelX();
                    float newVelX = currentVelX + offset * maxBounceAngleEffect;
                    newVelX = std::max(-5.0f, std::min(5.0f, newVelX));
                    m_ball.setVelX(newVelX); 
                }
            }

            for (Brick& brick : m_bricks) {
                if (brick.isAlive()) {
                    SDL_Rect brickRect = brick.getRect();
                    if (SDL_HasIntersection(&ballRect, &brickRect)) {
                        std::cout << "Collision Ball-Brick Detected!" << std::endl;
                        
                        brick.hit();

                        m_ball.setVelY(-m_ball.getVelY());

                        break;
                    }
                }
            }

            // KIỂM TRA NẾU BÓNG RA NGOÀI:
            if (m_ball.isOutOfBounds()) {
                std::cout << "Game detected ball out of bounds!" << std::endl;
                // Ví dụ tạm thời: Reset bóng lại vị trí ban đầu
                int ballInitialX = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
                int ballInitialY = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;
                float ballVelX = 3.0f;
                float ballVelY = -3.0f;
                m_ball.reset(ballInitialX, ballInitialY, ballVelX, ballVelY);
                std::cout << "Ball reset." << std::endl;
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

    // Vẽ dựa trên trạng thái hiện tại
    switch (m_currentGameState) {
        case GameStateEnum::MENU:
            break;
        case GameStateEnum::PLAY:
        {
            m_paddle.render(renderer, textureManager);
            m_ball.render(renderer, textureManager);
            for (Brick& brick : m_bricks) {
                brick.render(renderer, textureManager);
            }
            break;
        }
        case GameStateEnum::PAUSE:
            break;
        case GameStateEnum::GAMEOVER:
            break;
        default:
            break;
    }

    SDL_RenderPresent(renderer);
}


void Game::clean() {
    std::cout << "Cleaning up game..." << std::endl;

    textureManager.clearAllTextures();
    std::cout << "Texture Manager cleaned." << std::endl;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    renderer = nullptr;
    window = nullptr;
}