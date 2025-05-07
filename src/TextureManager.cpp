#include "TextureManager.h"
#include <iostream>

bool TextureManager::load(SDL_Renderer* renderer, const std::string& fileName, const std::string& id) {
    SDL_Texture* pTexture = IMG_LoadTexture(renderer, fileName.c_str());

    if (pTexture == nullptr) {
        std::cerr << "Failed to load texture file '" << fileName << "'! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    textureMap[id] = pTexture;
    std::cout << "Loaded texture '" << fileName << "' with ID: " << id << std::endl;
    return true;
}

void TextureManager::draw(SDL_Renderer* renderer, const std::string& id, int x, int y, int width, int height, SDL_RendererFlip flip) {
    if (textureMap.count(id)) {
        SDL_Rect destRect;
        destRect.x = x;
        destRect.y = y;
        destRect.w = width;
        destRect.h = height;

        SDL_RenderCopyEx(renderer, textureMap[id], nullptr, &destRect, 0.0, nullptr, flip);
    } else {
        std::cerr << "Could not draw texture with ID '" << id << "'. Texture not loaded." << std::endl;
    }
}

void TextureManager::clearFromTextureMap(const std::string& id) {
    if (textureMap.count(id)) {
        SDL_DestroyTexture(textureMap[id]);
        textureMap.erase(id);
        std::cout << "Cleared texture with ID: " << id << std::endl;
    } else {
         std::cerr << "Could not clear texture with ID '" << id << "'. ID not found." << std::endl;
    }
}

void TextureManager::clearAllTextures() {
    std::cout << "Clearing all textures..." << std::endl;
    for (auto const& [id, texture] : textureMap) {
        SDL_DestroyTexture(texture);
    }
    textureMap.clear();
    std::cout << "All textures cleared." << std::endl;
}