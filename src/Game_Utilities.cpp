#include "Game.h"
#include "Constants.h"
#include <iostream>
#include <fstream>
#include <string>

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