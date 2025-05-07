#include "Paddle.h"

Paddle::Paddle() {
    m_destRect.x = 0;
    m_destRect.y = 0;
    m_destRect.w = 0;
    m_destRect.h = 0;
    m_textureID = "";
    m_x = 0.0f;
    m_movingLeft = false;
    m_movingRight = false;
}

void Paddle::init(const std::string& textureID, int x, int y, int width, int height) {
    m_textureID = textureID;
    m_destRect.x = x;
    m_x = static_cast<float>(x);
    m_movingLeft = false;
    m_movingRight = false;
    m_destRect.y = y;
    m_destRect.w = width;
    m_destRect.h = height;
}

void Paddle::render(SDL_Renderer* renderer, TextureManager& texManager) {
    texManager.draw(renderer, m_textureID, m_destRect.x, m_destRect.y, m_destRect.w, m_destRect.h);
}



void Paddle::update() {
    float requestedMovement = 0.0f;

    if (m_movingLeft) {
        requestedMovement -= PADDLE_SPEED;
    }
    if (m_movingRight) {
        requestedMovement += PADDLE_SPEED;
    }

    m_x += requestedMovement;


    if (m_x < 0) {
        m_x = 0;
    }
    if (m_x + m_destRect.w > SCREEN_WIDTH) {
        m_x = static_cast<float>(SCREEN_WIDTH - m_destRect.w);
    }

    m_destRect.x = static_cast<int>(m_x);
}

void Paddle::handleEvents(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                m_movingLeft = true;
                break;
            case SDLK_RIGHT:
                m_movingRight = true;
                break;
            default:
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                m_movingLeft = false;
                break;
            case SDLK_RIGHT:
                m_movingRight = false;
                break;
            default:
                break;
        }
    }
}

SDL_Rect Paddle::getRect() const {
    return m_destRect;
}

void Paddle::setX(int x) {
    m_destRect.x = x;
}

void Paddle::setY(int y) {
    m_destRect.y = y;
}