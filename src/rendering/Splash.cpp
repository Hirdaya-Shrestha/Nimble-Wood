#include "rendering/Splash.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "core/Config.hpp"

namespace NimbleWood {

Splash::Splash(SDL_Renderer* renderer)
    : m_renderer(renderer) {
}

Splash::~Splash() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
    }
}

bool Splash::load(const char* path) {
    m_texture = IMG_LoadTexture(
        m_renderer,
        path
    );

    if (!m_texture) {
        SDL_Log(
            "Failed to load splash image: %s",
            SDL_GetError()
        );

        return false;
    }

    return true;
}

void Splash::update(double dt) {
    m_elapsed += dt;
}

void Splash::draw() {
    SDL_SetRenderDrawColor(
        m_renderer,
        18, 28, 23, 255
    );

    SDL_RenderClear(m_renderer);

    if (m_texture) {
        float textureWidth = 0.0f;
        float textureHeight = 0.0f;

        SDL_GetTextureSize(
            m_texture,
            &textureWidth,
            &textureHeight
        );

        constexpr float maxWidth = 260.0f;
        constexpr float maxHeight = 150.0f;

        float scale = 1.0f;

        if (textureWidth > maxWidth) {
            scale = maxWidth / textureWidth;
        }

        if (textureHeight * scale > maxHeight) {
            scale = maxHeight / textureHeight;
        }

        const float width = textureWidth * scale;
        const float height = textureHeight * scale;

        const SDL_FRect destination = {
            (kLogicalWidth - width) * 0.5f,
            (kLogicalHeight - height) * 0.5f,
            width,
            height
        };

        SDL_RenderTexture(
            m_renderer,
            m_texture,
            nullptr,
            &destination
        );
    }

    SDL_RenderPresent(m_renderer);
}

bool Splash::finished() const {
    return m_elapsed >= kSplashDuration;
}

} // namespace NimbleWood
