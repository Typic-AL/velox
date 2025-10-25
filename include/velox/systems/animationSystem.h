#include "../components/animation.h"
#include "velox/assetManager.h"
#include "velox/components/core.h"
#include "velox/registry.h"
#include "velox/time.h"

namespace vl {

inline void animateSprites(Registry &reg) {
    for (auto &entity : reg.view<SpriteRenderer, SpriteAnimator>()) {
        auto &sprite = reg.get<SpriteRenderer>(entity);
        auto &animator = reg.get<SpriteAnimator>(entity);

        if (!animator.isPlaying) continue;

        const SpriteAnimation &anim = reg.getResource<AssetManager>().idToAnim(animator.currentAnim);

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
