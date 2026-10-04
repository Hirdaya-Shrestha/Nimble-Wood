#pragma once

#include <SDL3/SDL.h>

namespace NimbleWood {

class Input {
public:
    void handleEvent(const SDL_Event& event);

    float movementDirection(SDL_Renderer* renderer) const;
    bool isJumping() const;
    bool isCrouching() const;

    bool shouldQuit() const;
    bool splashSkipped() const;

    void clearSplashSkip();

private:
    bool m_quit = false;
    bool m_splashSkipped = false;
    bool m_jumping = false;
    bool m_crouching = false;
};

} // namespace NimbleWood
