#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>

#include "core/App.hpp"

#ifndef NIMBLE_VERSION
#define NIMBLE_VERSION "0.0.1"
#endif

SDL_AppResult SDL_AppInit(
    void** appstate,
    int,
    char**
) {
    SDL_SetAppMetadata(
        "Nimble Wood",
        NIMBLE_VERSION,
        "dev.nimblewood.game"
    );

    auto* app = new NimbleWood::App();

    if (!app->initialize()) {
        delete app;
        return SDL_APP_FAILURE;
    }

    *appstate = app;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(
    void* appstate,
    SDL_Event* event
) {
    auto* app =
        static_cast<NimbleWood::App*>(appstate);

    app->handleEvent(*event);

    return app->shouldQuit()
        ? SDL_APP_SUCCESS
        : SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app =
        static_cast<NimbleWood::App*>(appstate);

    static Uint64 previousTime = SDL_GetTicksNS();

    const Uint64 currentTime = SDL_GetTicksNS();

    const double deltaTime =
        static_cast<double>(
            currentTime - previousTime
        ) / 1e9;

    previousTime = currentTime;

    if (!app->update(deltaTime)) {
        return SDL_APP_SUCCESS;
    }

    app->render();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(
    void* appstate,
    SDL_AppResult
) {
    auto* app =
        static_cast<NimbleWood::App*>(appstate);

    delete app;

    // SDL_Quit() is called automatically by SDL.
}
