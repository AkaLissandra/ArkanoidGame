#include "Game.h"
#include "Constants.h"
#include <iostream>
#include <string>

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

void Game::updatePauseState() {
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