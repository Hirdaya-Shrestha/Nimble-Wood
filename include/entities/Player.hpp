#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <array>
#include <string>
#include <vector>

namespace NimbleWood {

enum class PlayerState {
  Idle,
  Run,
  Jump,
  Crouch,
  CrouchWalk
};

struct Animation {
  std::vector<SDL_Texture*> frames;
  float frameTime = 0.1f;
  bool loop = true;
};

class Player {
public:
  Player();
  ~Player();

  bool load(SDL_Renderer* renderer);
  void update(float direction, bool jumping, bool crouching, float dt);
  void render(SDL_Renderer* renderer);

  float x() const;
  float y() const;
  bool facingRight() const;
  PlayerState state() const;

private:
  void setState(PlayerState newState);
  SDL_Texture* getCurrentFrame() const;

  float m_x;
  float m_y;
  float m_velX = 0.0f;
  float m_velY = 0.0f;
  bool m_facingRight = true;
  bool m_onGround = true;
  bool m_wasCrouching = false;
  PlayerState m_state = PlayerState::Idle;

  float m_animTimer = 0.0f;
  size_t m_currentFrame = 0;

  Animation m_idleAnim;
  Animation m_runAnim;
  Animation m_jumpAnim;
  Animation m_crouchAnim;
  Animation m_crouchWalkAnim;
};

} // namespace NimbleWood
