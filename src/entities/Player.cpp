#include "entities/Player.hpp"

#include <SDL3/SDL.h>
#include "core/Assets.hpp"
#include <cmath>

#include "core/Config.hpp"

namespace NimbleWood {

namespace {

std::vector<SDL_Texture*> LoadAnimationFrames(
    SDL_Renderer* renderer,
    const std::string& basePath,
    int count) {
  std::vector<SDL_Texture*> frames;
  frames.reserve(count);

  for (int i = 1; i <= count; ++i) {
    std::string filename = basePath;
    if (i < 10) {
      filename += "_0" + std::to_string(i);
    } else {
      filename += "_" + std::to_string(i);
    }
    filename += ".png";
    SDL_Texture* tex = loadTexture(renderer, filename);
    if (tex) {
      frames.push_back(tex);
    } else {
      SDL_Log("Failed to load %s: %s", filename.c_str(), SDL_GetError());
    }
  }
  return frames;
}

} // namespace

Player::Player()
    : m_x(40.0f),
      m_y(kGroundY - kPlayerSize) {
  m_velY = 0.0f;
  m_onGround = true;
  m_wasCrouching = false;
}

Player::~Player() {
  for (auto* t : m_idleAnim.frames) {
    if (t) SDL_DestroyTexture(t);
  }
  for (auto* t : m_runAnim.frames) {
    if (t) SDL_DestroyTexture(t);
  }
  for (auto* t : m_jumpAnim.frames) {
    if (t) SDL_DestroyTexture(t);
  }
  for (auto* t : m_crouchAnim.frames) {
    if (t) SDL_DestroyTexture(t);
  }
  for (auto* t : m_crouchWalkAnim.frames) {
    if (t) SDL_DestroyTexture(t);
  }
}

bool Player::load(SDL_Renderer* renderer) {
  m_idleAnim.frames = LoadAnimationFrames(renderer, "assets/textures/players/nimble/idle/idle", 7);
  m_idleAnim.frameTime = 0.1f;
  m_idleAnim.loop = true;

  m_runAnim.frames = LoadAnimationFrames(renderer, "assets/textures/players/nimble/run/run", 7);
  m_runAnim.frameTime = 0.08f;
  m_runAnim.loop = true;

  m_jumpAnim.frames = LoadAnimationFrames(renderer, "assets/textures/players/nimble/jump/jump", 5);
  m_jumpAnim.frameTime = 0.1f;
  m_jumpAnim.loop = false;

  m_crouchAnim.frames = LoadAnimationFrames(renderer, "assets/textures/players/nimble/crouch/crouch", 5);
  m_crouchAnim.frameTime = 0.1f;
  m_crouchAnim.loop = true;

  m_crouchWalkAnim.frames = LoadAnimationFrames(renderer, "assets/textures/players/nimble/crouch_walk/crouch_walk", 7);
  m_crouchWalkAnim.frameTime = 0.08f;
  m_crouchWalkAnim.loop = true;

  m_state = PlayerState::Idle;
  m_currentFrame = 0;
  m_animTimer = 0.0f;
  m_facingRight = true;
  m_velY = 0.0f;
  m_onGround = true;
  m_wasCrouching = false;
  return true;
}
void Player::setState(PlayerState newState) {
  if (m_state == newState) return;
  m_state = newState;
  m_currentFrame = 0;
  m_animTimer = 0.0f;
}

void Player::update(float direction, bool jumping, bool crouching, float dt) {
  m_velX = direction * kPlayerSpeed;
  m_x += m_velX * dt;

  if (direction > 0.01f) {
    m_facingRight = true;
  } else if (direction < -0.01f) {
    m_facingRight = false;
  }

  const float groundY = kGroundY - kPlayerSize;
  if (m_onGround && jumping) {
    m_velY = -300.0f;
    m_onGround = false;
  }

  if (!m_onGround) {
    m_velY += 900.0f * dt;
  }

  m_y += m_velY * dt;

  if (m_y >= groundY) {
    m_y = groundY;
    m_velY = 0.0f;
    m_onGround = true;
  }

  if (m_x < 0.0f) m_x = 0.0f;
  if (m_x > kLogicalWidth - kPlayerSize) m_x = kLogicalWidth - kPlayerSize;

  if (!m_onGround) {
    setState(PlayerState::Jump);
  } else if (crouching) {
    if (std::fabs(m_velX) > 1.0f) {
      setState(PlayerState::CrouchWalk);
    } else {
      setState(PlayerState::Crouch);
    }
    m_wasCrouching = true;
  } else {
    m_wasCrouching = false;
    if (std::fabs(m_velX) > 1.0f) {
      setState(PlayerState::Run);
    } else {
      setState(PlayerState::Idle);
    }
  }

  Animation* anim = nullptr;
  switch (m_state) {
    case PlayerState::Run: anim = &m_runAnim; break;
    case PlayerState::Jump: anim = &m_jumpAnim; break;
    case PlayerState::Crouch: anim = &m_crouchAnim; break;
    case PlayerState::CrouchWalk: anim = &m_crouchWalkAnim; break;
    default: anim = &m_idleAnim; break;
  }

  if (anim && !anim->frames.empty()) {
    m_animTimer += dt;
    if (m_animTimer >= anim->frameTime) {
      m_animTimer = 0.0f;
      if (m_currentFrame + 1 < anim->frames.size()) {
        ++m_currentFrame;
      } else if (anim->loop) {
        m_currentFrame = 0;
      }
    }
  }
}

SDL_Texture* Player::getCurrentFrame() const {
  const Animation* anim = nullptr;
  switch (m_state) {
    case PlayerState::Run: anim = &m_runAnim; break;
    case PlayerState::Jump: anim = &m_jumpAnim; break;
    case PlayerState::Crouch: anim = &m_crouchAnim; break;
    case PlayerState::CrouchWalk: anim = &m_crouchWalkAnim; break;
    default: anim = &m_idleAnim; break;
  }
  if (!anim || m_currentFrame >= anim->frames.size()) return nullptr;
  return anim->frames[m_currentFrame];
}

void Player::render(SDL_Renderer* renderer) {
  SDL_Texture* tex = getCurrentFrame();
  if (!tex) return;

  float tw = 0.0f, th = 0.0f;
  SDL_GetTextureSize(tex, &tw, &th);

  float scale = (kPlayerSize * 2.0f) / tw;
  float w = tw * scale;
  float h = th * scale;

  SDL_FRect dst = {m_x - (w - kPlayerSize) * 0.5f, m_y - (h - kPlayerSize), w, h};

  SDL_RenderTextureRotated(renderer, tex, nullptr, &dst, 0.0f, nullptr, m_facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
}

float Player::x() const { return m_x; }
float Player::y() const { return m_y; }
bool Player::facingRight() const { return m_facingRight; }
PlayerState Player::state() const { return m_state; }

} // namespace NimbleWood
