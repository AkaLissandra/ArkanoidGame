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