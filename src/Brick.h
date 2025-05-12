#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>
#include <string>
#include "TextureManager.h"

class Brick {
public:
    Brick(int x, int y, int width, int height, std::string textureID, int scoreValue);

    void render(SDL_Renderer* renderer, TextureManager& texManager);

    bool isAlive() const;

    void hit();

    SDL_Rect getRect() const;

    int getScoreValue() const;

private:
    SDL_Rect m_rect;
    std::string m_textureID;
    bool m_isAlive;
    int m_scoreValue;
};

#endif