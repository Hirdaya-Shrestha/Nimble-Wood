#pragma once

#include <string>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace NimbleWood {

inline std::string assetPath(const std::string& path) {
#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_EMSCRIPTEN)
    return path;
#else
    const char* base = SDL_GetBasePath();
    if (base) {
        const std::string full = std::string(base) + path;
        if (SDL_GetPathInfo(full.c_str(), nullptr)) {
            return full;
        }
    }
    return path;
#endif
}

inline SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
    const std::string full = assetPath(path);
    SDL_Texture* texture = IMG_LoadTexture(renderer, full.c_str());
    if (!texture) {
        SDL_Log("Failed to load '%s': %s", full.c_str(), SDL_GetError());
    }
    return texture;
}

}  // namespace NimbleWood
