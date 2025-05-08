#ifndef BALL_H
#define BALL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include "TextureManager.h"
#include "Constants.h"

class Ball {
public:
    Ball();

    void init(const std::string& textureID, int x, int y, int width, int height, float velX, float velY, Mix_Chunk* hitSoundEffect);

    void update();
    void render(SDL_Renderer* renderer, TextureManager& texManager);

    SDL_Rect getRect() const;

    bool isOutOfBounds() const;

    void reset(int x, int y, float velX, float velY);

    float getVelY() const;
    void setVelY(float vy);

    float getVelX() const;
    void setVelX(float vx);

    void setY(float y);


private:
    SDL_Rect m_destRect;
    std::string m_textureID;
    
    float m_x;
    float m_y;
    float m_velX;
    float m_velY;
    bool m_isOutOfBounds;

    Mix_Chunk* m_sfxHitSound;
};

#endif