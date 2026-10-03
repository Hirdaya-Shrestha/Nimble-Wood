// Nimble Wood - SDL3 sample made using SDL's main callbacks, which is the one
// entry-point style that works on desktop, Android, iOS and the web (Emscripten).
// Window, fixed-timestep loop, one square moved with A/D, arrow keys, or by touching/clicking
// the left or right half of the screen. Esc quits (desktop).

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>  // must be included in the file that defines the entry points

namespace {

constexpr int    kLogicalW = 480;  // virtual resolution, scaled to fit the window
constexpr int    kLogicalH = 270;
constexpr float  kGroundY  = 220.0f;
constexpr float  kPlayerSz = 16.0f;
constexpr double kStep     = 1.0 / 60.0;  // fixed update rate (seconds)

struct Player {
    float x = 40.0f;
    float y = kGroundY - kPlayerSz;
};

struct Game {
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    Player        player;
    double        acc  = 0.0;
    Uint64        prev = 0;
};

float inputDirection(SDL_Renderer* r) {
    float dir = 0.0f;

    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])  dir -= 1.0f;
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) dir += 1.0f;

    // Touch is delivered as a left mouse button by default.
    float wx = 0.0f, wy = 0.0f;
    if (SDL_GetMouseState(&wx, &wy) & SDL_BUTTON_LMASK) {
        float lx = 0.0f, ly = 0.0f;
        SDL_RenderCoordinatesFromWindow(r, wx, wy, &lx, &ly);
        dir += (lx < kLogicalW / 2.0f) ? -1.0f : 1.0f;
    }

    if (dir < -1.0f) dir = -1.0f;
    if (dir > 1.0f) dir = 1.0f;
    return dir;
}

void update(Player& p, SDL_Renderer* r, float dt) {
    p.x += inputDirection(r) * 120.0f * dt;
    if (p.x < 0.0f) p.x = 0.0f;
    if (p.x > kLogicalW - kPlayerSz) p.x = kLogicalW - kPlayerSz;
}

void draw(SDL_Renderer* r, const Player& p) {
    SDL_SetRenderDrawColor(r, 28, 44, 36, 255);  // background
    SDL_RenderClear(r);

    const SDL_FRect ground = {0.0f, kGroundY, static_cast<float>(kLogicalW), kLogicalH - kGroundY};
    SDL_SetRenderDrawColor(r, 92, 62, 40, 255);
    SDL_RenderFillRect(r, &ground);

    const SDL_FRect player = {p.x, p.y, kPlayerSz, kPlayerSz};
    SDL_SetRenderDrawColor(r, 240, 200, 80, 255);
    SDL_RenderFillRect(r, &player);

    SDL_RenderPresent(r);
}

}  // namespace

SDL_AppResult SDL_AppInit(void** appstate, int, char**) {
    SDL_SetAppMetadata("Nimble Wood", NIMBLE_VERSION, "dev.nimblewood.game");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto* g   = new Game();
    *appstate = g;

    if (!SDL_CreateWindowAndRenderer("Nimble Wood", kLogicalW * 2, kLogicalH * 2,
                                     SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY,
                                     &g->window, &g->renderer)) {
        SDL_Log("Window/renderer creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderVSync(g->renderer, 1);
    SDL_SetRenderLogicalPresentation(g->renderer, kLogicalW, kLogicalH,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    g->prev = SDL_GetTicksNS();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* e) {
    if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
#if !defined(SDL_PLATFORM_EMSCRIPTEN)
    if (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_ESCAPE) return SDL_APP_SUCCESS;
#endif
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* g = static_cast<Game*>(appstate);

    const Uint64 now = SDL_GetTicksNS();
    g->acc += static_cast<double>(now - g->prev) / 1e9;
    g->prev = now;
    if (g->acc > 0.25) g->acc = 0.25;  // avoid a spiral of death after a stall

    while (g->acc >= kStep) {
        update(g->player, g->renderer, static_cast<float>(kStep));
        g->acc -= kStep;
    }

    draw(g->renderer, g->player);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult) {
    auto* g = static_cast<Game*>(appstate);
    if (g) {
        SDL_DestroyRenderer(g->renderer);
        SDL_DestroyWindow(g->window);
        delete g;
    }
    // SDL calls SDL_Quit() itself after this returns.
}
