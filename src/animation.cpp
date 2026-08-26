#include "velox/components/animation.h"
#include "velox/assetManager.h"
#include "velox/components/core.h"
#include "velox/registry.h"
#include "velox/systems/animationSystem.h"
#include "velox/time.h"

namespace vl {
void setAnim(Registry &reg, Entity e, AnimID newAnim) {
    SpriteAnimator &animator = reg.get<SpriteAnimator>(e);
    SpriteRenderer &sprite = reg.get<SpriteRenderer>(e);

    if (animator.currentAnim == newAnim) return;

    animator.currentAnim = newAnim;
    animator.currentFrame = 0;
    animator.frameTimer = 0.0f;
    animator.isPlaying = true;
    animator.complete = false;

    const SpriteAnimation &anim = reg.getResource<AssetManager>().idToAnim(newAnim);
    sprite.src = anim.frames[0];

}

void animateSprites(Registry &reg) {
  for (auto [sprite, animator] : reg.view<SpriteRenderer, SpriteAnimator>()) {

    if (!animator.isPlaying)
      continue;

    const SpriteAnimation &anim =
        reg.getResource<AssetManager>().idToAnim(animator.currentAnim);

    animator.frameTimer += Time::deltaTime;

    if (animator.frameTimer >= 1.0f / anim.fps) {
      animator.frameTimer -= 1.0f / anim.fps;
      ++animator.currentFrame;

      if (animator.currentFrame >= anim.frames.size()) {
        if (anim.loop) {
          animator.currentFrame = 0;
        } else {
          animator.currentFrame = anim.frames.size() - 1;
          animator.complete = true;
        }
      }

      sprite.src = anim.frames[animator.currentFrame];
    }
  }
}
}
