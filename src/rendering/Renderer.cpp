#include "rendering/Renderer.hpp"

#include "core/Config.hpp"
#include "entities/Player.hpp"

namespace NimbleWood {

Renderer::Renderer(SDL_Renderer* renderer)
    : m_renderer(renderer) {}

void Renderer::drawGame(const Player& player) {
  SDL_SetRenderDrawColor(m_renderer, 135, 206, 235, 255);
  SDL_RenderClear(m_renderer);

  const SDL_FRect ground = {0.0f, kGroundY, static_cast<float>(kLogicalWidth),
                            static_cast<float>(kLogicalHeight) - kGroundY};
  SDL_SetRenderDrawColor(m_renderer, 92, 62, 40, 255);
  SDL_RenderFillRect(m_renderer, &ground);

  const_cast<Player&>(player).render(m_renderer);

  SDL_RenderPresent(m_renderer);
}

} // namespace NimbleWood
