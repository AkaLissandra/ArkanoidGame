#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <map>

class TextureManager {
public:

    bool load(SDL_Renderer* renderer, const std::string& fileName, const std::string& id);

    void draw(SDL_Renderer* renderer, const std::string& id, int x, int y, int width, int height, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void clearFromTextureMap(const std::string& id);

    void clearAllTextures();

private:
    std::map<std::string, SDL_Texture*> textureMap;
};

#endif