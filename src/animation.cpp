#include "velox/components/animation.h"
#include "velox/assetManager.h"
#include "velox/components/core.h"

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
}
