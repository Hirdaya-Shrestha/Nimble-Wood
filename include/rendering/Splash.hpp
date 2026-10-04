#pragma once

struct SDL_Renderer;
struct SDL_Texture;

namespace NimbleWood {

class Splash {
public:
    explicit Splash(SDL_Renderer* renderer);
    ~Splash();

    bool load(const char* path);
    void update(double dt);
    void draw();
    bool finished() const;

private:
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_texture = nullptr;
    double m_elapsed = 0.0;
};

} // namespace NimbleWood
