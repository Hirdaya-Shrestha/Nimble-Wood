#pragma once

#include <SDL3/SDL_render.h>

#include <SDL3/SDL.h>
#include <math.h>

namespace NimbleWood {
    void DrawCircle(SDL_Renderer* renderer, float x, float y, float radius);
    void DrawFilledCircle(SDL_Renderer* renderer, float x, float y, float radius);
}
