#include "Game.h"
#include "Constants.h"
#include <string>

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

void Game::updateMenuState() {
    m_menuBgScrollY += MENU_BG_SCROLL_SPEED;
    // Reset vị trí
    if (m_menuBgScrollY >= SCREEN_HEIGHT) {
        m_menuBgScrollY -= SCREEN_HEIGHT;
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