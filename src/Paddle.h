#ifndef PADDLE_H
#define PADDLE_H

#include <SDL2/SDL.h>
#include <string>
#include "TextureManager.h"
#include "Constants.h"  

class Paddle {
public:
    Paddle();

    void init(const std::string& textureID, int x, int y, int width, int height);

    void render(SDL_Renderer* renderer, TextureManager& texManager);

    void update();

    void handleEvents(SDL_Event& event);

    SDL_Rect getRect() const;

    void setX(int x);
    void setY(int y);


private:
    SDL_Rect m_destRect;
    std::string m_textureID;
    float m_x;
    bool m_movingLeft;
    bool m_movingRight;
};

#endif