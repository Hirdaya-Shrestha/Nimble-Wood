#include "core/App.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>

#include "core/Config.hpp"
#include "icon.h"

#ifndef NIMBLE_VERSION
#define NIMBLE_VERSION "0.0.1"
#endif

namespace NimbleWood {

App::App() = default;

App::~App() {
    delete m_splash;
    delete m_gameRenderer;

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

bool App::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log(
            "SDL_Init failed: %s",
            SDL_GetError()
        );

        return false;
    }

    if (!SDL_CreateWindowAndRenderer(
            "Nimble Wood",
            kLogicalWidth * 2,
            kLogicalHeight * 2,
            SDL_WINDOW_RESIZABLE |
                SDL_WINDOW_HIGH_PIXEL_DENSITY,
            &m_window,
            &m_renderer)) {

        SDL_Log(
            "Window/renderer creation failed: %s",
            SDL_GetError()
        );

        return false;
    }

    SDL_Surface* iconSurface = SDL_CreateSurfaceFrom(
        kIconSize,
        kIconSize,
        SDL_PIXELFORMAT_RGBA32,
        const_cast<unsigned char*>(kIconRGBA),
        kIconSize * 4
    );

    if (iconSurface) {
        SDL_SetWindowIcon(m_window, iconSurface);
        SDL_DestroySurface(iconSurface);
    }

    SDL_SetRenderVSync(m_renderer, 1);

    SDL_SetRenderLogicalPresentation(
        m_renderer,
        kLogicalWidth,
        kLogicalHeight,
        SDL_LOGICAL_PRESENTATION_LETTERBOX
    );

    m_splash = new Splash(m_renderer);

    const bool hasSplash = m_splash->load("assets/logo.png");

    if (!hasSplash) {
        SDL_Log("Splash image not available, skipping the splash screen.");
    }

    m_gameRenderer = new Renderer(m_renderer);

    if (!m_player.load(m_renderer)) {
        SDL_Log("Failed to load player assets");
        return false;
    }

    m_state = hasSplash ? GameState::Splash : GameState::Game;
    m_accumulator = 0.0;

    SDL_Log("Nimble Wood initialized.");

    return true;
}

void App::handleEvent(const SDL_Event& event) {
    m_input.handleEvent(event);

    if (m_state == GameState::Splash &&
        m_input.splashSkipped()) {

        transitionToGame();
        m_input.clearSplashSkip();
    }
}

bool App::update(double dt) {
    if (m_input.shouldQuit()) {
        return false;
    }

    if (m_state == GameState::Splash) {
        m_splash->update(dt);

        if (m_splash->finished()) {
            transitionToGame();
        }

        return true;
    }

    m_accumulator += dt;

    if (m_accumulator > 0.25) {
        m_accumulator = 0.25;
    }

    while (m_accumulator >= kFixedStep) {
        const float direction =
            m_input.movementDirection(m_renderer);

        m_player.update(
            direction,
            m_input.isJumping(),
            m_input.isCrouching(),
            static_cast<float>(kFixedStep)
        );

        m_accumulator -= kFixedStep;
    }

    return true;
}

void App::render() {
    if (m_state == GameState::Splash) {
        m_splash->draw();
        return;
    }

    m_gameRenderer->drawGame(m_player);
}

void App::transitionToGame() {
    if (m_state == GameState::Game) {
        return;
    }

    m_state = GameState::Game;
    m_accumulator = 0.0;

    SDL_Log("Splash finished. Starting Nimble Wood.");
}

bool App::shouldQuit() const {
    return m_input.shouldQuit();
}

} // namespace NimbleWood
