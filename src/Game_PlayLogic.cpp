#include "Game.h"
#include "Constants.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>

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