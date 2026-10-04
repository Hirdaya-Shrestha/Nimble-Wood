#pragma once

struct SDL_Renderer;

namespace NimbleWood {

class Player;

class Renderer {
public:
    explicit Renderer(SDL_Renderer* renderer);

    void drawGame(const Player& player);

private:
    SDL_Renderer* m_renderer;
};

} // namespace NimbleWood
