#include "utils/DrawCircle.hpp"

namespace NimbleWood {
    // Hollow circle (midpoint algorithm)
    void DrawCircle(SDL_Renderer *renderer, float cx, float cy, float radius)
    {
        float x = radius - 1;
        float y = 0;
        float tx = 1;
        float ty = 1;
        float error = tx - radius * 2;

        while (x >= y)
        {
            SDL_RenderPoint(renderer, cx + x, cy - y);
            SDL_RenderPoint(renderer, cx + x, cy + y);
            SDL_RenderPoint(renderer, cx - x, cy - y);
            SDL_RenderPoint(renderer, cx - x, cy + y);
            SDL_RenderPoint(renderer, cx + y, cy - x);
            SDL_RenderPoint(renderer, cx + y, cy + x);
            SDL_RenderPoint(renderer, cx - y, cy - x);
            SDL_RenderPoint(renderer, cx - y, cy + x);

            if (error <= 0) {
                ++y;
                error += ty;
                ty += 2;
            }
            if (error > 0) {
                --x;
                tx += 2;
                error += tx - radius * 2;
            }
        }
    }

    // Filled circle (row-by-row, Pythagorean)
    void DrawFilledCircle(SDL_Renderer *renderer, float cx, float cy, float radius)
    {
        for (float y = -radius; y <= radius; y++) {
            float halfWidth = sqrtf(radius * radius - y * y);
            SDL_RenderLine(renderer, cx - halfWidth, cy + y, cx + halfWidth, cy + y);
        }
    }

} // namespace NimbleWood
