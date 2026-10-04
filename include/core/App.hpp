#pragma once

#include <SDL3/SDL.h>

#include "core/GameState.hpp"
#include "entities/Player.hpp"
#include "input/Input.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Splash.hpp"

namespace NimbleWood {

class App {
public:
    App();
    ~App();

    bool initialize();
    void handleEvent(const SDL_Event& event);

    bool update(double dt);
    void render();

    bool shouldQuit() const;

private:
    void transitionToGame();

private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    GameState m_state = GameState::Splash;

    Input m_input;
    Player m_player;

    Renderer* m_gameRenderer = nullptr;
    Splash* m_splash = nullptr;

    double m_accumulator = 0.0;
};

} // namespace NimbleWood
