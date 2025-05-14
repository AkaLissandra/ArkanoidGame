#include "Game.h"
#include "Constants.h"
#include <string>

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

void Game::updateGameOverState() {
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