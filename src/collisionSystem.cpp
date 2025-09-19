#include "velox/systems/collisionSystem.h"
#include "velox/components/collider.h"
#include "velox/components/core.h"
#include "velox/registry.h"
#include <iostream>

namespace vl {

bool checkLayer(Collider &col1, Collider &col2) {
    if (col1.layer >= 32 || col2.layer >= 32) {
        return false;
    }
    return (col1.mask & (1u << col2.layer)) && (col2.mask & (1u << col1.layer));
}

void resolveCollision(Registry &reg, Entity e1, Entity e2) {
    SDL_FRect &rect1 = reg.get<Collider>(e1).rect;
    SDL_FRect &rect2 = reg.get<Collider>(e2).rect;
    glm::vec2 &pos1 = reg.get<Transform>(e1).pos;
    glm::vec2 &pos2 = reg.get<Transform>(e2).pos;
    glm::vec2 &vel1 = reg.get<Rigidbody>(e1).vel;
    glm::vec2 &vel2 = reg.get<Rigidbody>(e2).vel;

    float overlapX = std::min(rect1.x + rect1.w, rect2.x + rect2.w) - std::max(rect1.x, rect2.x);
    float overlapY = std::min(rect1.y + rect1.h, rect2.y + rect2.h) - std::max(rect1.y, rect2.y);

    if (overlapX < overlapY) {
        float separation = overlapX * 0.5f;
        if (rect1.x < rect2.x) {
            pos1.x -= separation;
            pos2.x += separation;
        } else {
            pos1.x += separation;
            pos2.x -= separation;
        }
        vel1.x *= -0.5f;
        vel2.x *= -0.5f;
    } else {
        float separation = overlapY * 0.5f;
        if (rect1.y < rect2.y) {
            pos1.y -= separation;
            pos2.y += separation;
        } else {
            pos1.y += separation;
            pos2.y -= separation;
        }
        vel1.y *= -0.5f;
        vel2.y *= -0.5f;
    }

    //rect1.x = pos1.x;
    //rect1.y = pos1.y;
    //rect2.x = pos2.x;
    //rect2.y = pos2.y;
}

void sweepAndPrune(Registry &reg) {
    std::vector<Entity> entities = std::move(reg.view<Collider, Transform, Rigidbody>());
    if(entities.size() < 2)
        return;

    std::sort(entities.begin(), entities.end(),
                [&](const Entity &a, const Entity &b) {
                    return reg.get<Collider>(a).rect.x < reg.get<Collider>(b).rect.x ;
                });

    for(size_t i = 0; i < entities.size(); ++i) {
        Collider &colA = reg.get<Collider>(entities[i]);
        const SDL_FRect &rectA = colA.rect;

        for(size_t j = i + 1; j < entities.size(); ++j) {
            Collider &colB = reg.get<Collider>(entities[j]);
            const SDL_FRect &rectB = colB.rect;

            if (rectB.x > rectA.x + rectA.w)
                break;

            if (rectA.y + rectA.h < rectB.y || rectA.y > rectB.y + rectB.h)
                continue;

            if(checkLayer(colA, colB)) {
                resolveCollision(reg, entities[i], entities[j]);
            }

        }
    }
}

}
