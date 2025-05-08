#include "Ball.h"
#include <iostream>
#include <cmath>

Ball::Ball() {
    m_x = 0.0f;
    m_y = 0.0f;
    m_destRect = {0, 0, 0, 0};
    m_textureID = "";
    m_velX = 0.0f;
    m_velY = 0.0f;

    m_isOutOfBounds = false;
    m_sfxHitSound = nullptr;
}

void Ball::init(const std::string& textureID, int x, int y, int width, int height, float velX, float velY, Mix_Chunk* hitSoundEffect) {
    m_textureID = textureID;
    m_x = static_cast<float>(x);
    m_y = static_cast<float>(y);
    m_destRect.w = width;
    m_destRect.h = height;
    m_velX = velX;
    m_velY = velY;
    m_sfxHitSound = hitSoundEffect;

    m_destRect.x = static_cast<int>(m_x);
    m_destRect.y = static_cast<int>(m_y);

    m_isOutOfBounds = false;
}

void Ball::update() {
    if (m_isOutOfBounds) {
        return;
    }

    m_x += m_velX;
    m_y += m_velY;

    m_destRect.x = static_cast<int>(m_x);
    m_destRect.y = static_cast<int>(m_y);

    if (m_destRect.y < 0) {
        m_y = 0;
        m_velY = -m_velY;
        m_destRect.y = static_cast<int>(m_y);
        if (m_sfxHitSound != nullptr) Mix_PlayChannel(-1, m_sfxHitSound, 0);
    }

    if (m_destRect.x < 0) {
        m_x = 0;
        m_velX = -m_velX;
        m_destRect.x = static_cast<int>(m_x);
        if (m_sfxHitSound != nullptr) Mix_PlayChannel(-1, m_sfxHitSound, 0);
    }
    if (m_destRect.x + m_destRect.w > SCREEN_WIDTH) {
        m_x = static_cast<float>(SCREEN_WIDTH - m_destRect.w);
        m_velX = -m_velX;
        m_destRect.x = static_cast<int>(m_x);
        if (m_sfxHitSound != nullptr) Mix_PlayChannel(-1, m_sfxHitSound, 0);
    }

    if (m_destRect.y > SCREEN_HEIGHT) {
        m_isOutOfBounds = true;
        std::cout << "Ball is out of bounds!" << std::endl;
    }
}

void Ball::render(SDL_Renderer* renderer, TextureManager& texManager) {
    texManager.draw(renderer, m_textureID, m_destRect.x, m_destRect.y, m_destRect.w, m_destRect.h);
}

SDL_Rect Ball::getRect() const {
    return m_destRect;
}

bool Ball::isOutOfBounds() const {
    return m_isOutOfBounds;
}

void Ball::reset(int x, int y, float velX, float velY) {
    m_x = static_cast<float>(x);
    m_y = static_cast<float>(y);
    m_velX = velX;
    m_velY = velY;
    m_destRect.x = static_cast<int>(m_x);
    m_destRect.y = static_cast<int>(m_y);
    m_isOutOfBounds = false;
}


float Ball::getVelY() const {
    return m_velY;
}

void Ball::setVelY(float vy) {
    m_velY = vy;
}

float Ball::getVelX() const {
    return m_velX;
}

void Ball::setVelX(float vx) {
    m_velX = vx;
}
 void Ball::setY(float y) {
    m_y = y;
    m_destRect.y = static_cast<int>(m_y);
}