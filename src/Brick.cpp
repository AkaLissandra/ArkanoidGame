#include "Brick.h"

Brick::Brick(int x, int y, int width, int height, std::string textureID) {
    m_rect.x = x;
    m_rect.y = y;
    m_rect.w = width;
    m_rect.h = height;
    m_textureID = textureID;
    m_isAlive = true;
}

void Brick::render(SDL_Renderer* renderer, TextureManager& texManager) {
    if (m_isAlive) {
        texManager.draw(renderer, m_textureID, m_rect.x, m_rect.y, m_rect.w, m_rect.h);
    }
}

bool Brick::isAlive() const {
    return m_isAlive;
}

void Brick::hit() {
    m_isAlive = false;
}

SDL_Rect Brick::getRect() const {
    return m_rect;
}