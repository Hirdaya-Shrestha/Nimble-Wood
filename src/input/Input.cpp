#include "input/Input.hpp"

#include <SDL3/SDL.h>

#include "core/Config.hpp"

namespace NimbleWood {

void Input::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_QUIT) {
        m_quit = true;
        return;
    }

#if !defined(SDL_PLATFORM_EMSCRIPTEN)
    if (event.type == SDL_EVENT_KEY_DOWN &&
        event.key.key == SDLK_ESCAPE) {
        m_quit = true;
        return;
    }
#endif

    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_SPACE || event.key.key == SDLK_W || event.key.key == SDLK_UP) {
            m_jumping = true;
        }
        if (event.key.key == SDLK_S || event.key.key == SDLK_DOWN || event.key.key == SDLK_LCTRL) {
            m_crouching = true;
        }
        m_splashSkipped = true;
    }

    if (event.type == SDL_EVENT_KEY_UP) {
        if (event.key.key == SDLK_SPACE || event.key.key == SDLK_W || event.key.key == SDLK_UP) {
            m_jumping = false;
        }
        if (event.key.key == SDLK_S || event.key.key == SDLK_DOWN || event.key.key == SDLK_LCTRL) {
            m_crouching = false;
        }
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event.type == SDL_EVENT_FINGER_DOWN) {
        m_splashSkipped = true;
    }
}

float Input::movementDirection(SDL_Renderer* renderer) const {
    float direction = 0.0f;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_LEFT] ||
        keys[SDL_SCANCODE_A]) {
        direction -= 1.0f;
    }

    if (keys[SDL_SCANCODE_RIGHT] ||
        keys[SDL_SCANCODE_D]) {
        direction += 1.0f;
    }

    float windowX = 0.0f;
    float windowY = 0.0f;

    if (SDL_GetMouseState(&windowX, &windowY) &
        SDL_BUTTON_LMASK) {

        float logicalX = 0.0f;
        float logicalY = 0.0f;

        SDL_RenderCoordinatesFromWindow(
            renderer,
            windowX,
            windowY,
            &logicalX,
            &logicalY
        );

        direction +=
            (logicalX < kLogicalWidth / 2.0f)
                ? -1.0f
                : 1.0f;
    }

    if (direction < -1.0f)
        direction = -1.0f;

    if (direction > 1.0f)
        direction = 1.0f;

    return direction;
}

bool Input::isJumping() const {
    return m_jumping;
}

bool Input::isCrouching() const {
    return m_crouching;
}

bool Input::shouldQuit() const {
    return m_quit;
}

bool Input::splashSkipped() const {
    return m_splashSkipped;
}

void Input::clearSplashSkip() {
    m_splashSkipped = false;
}

} // namespace NimbleWood
