#include "velox/systems/collisionSystem.h"
#include "velox/components/collider.h"
#include "velox/components/core.h"
#include "velox/registry.h"

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

    Rigidbody &body1 = reg.get<Rigidbody>(e1);
    Rigidbody &body2 = reg.get<Rigidbody>(e2);

    // Kinematic bodies have infinite mass for collision purposes
    float invMass1 = (body1.type == BodyType::DYNAMIC) ? 1.0f / body1.mass : 0.0f;
    float invMass2 = (body2.type == BodyType::DYNAMIC) ? 1.0f / body2.mass : 0.0f;

    // Skip if both are non-dynamic (static or kinematic)
    if (invMass1 == 0.0f && invMass2 == 0.0f) return;

    float overlapX = std::min(rect1.x + rect1.w, rect2.x + rect2.w) - std::max(rect1.x, rect2.x);
    float overlapY = std::min(rect1.y + rect1.h, rect2.y + rect2.h) - std::max(rect1.y, rect2.y);

    glm::vec2 normal;
    float penetration;

    if (overlapX < overlapY) {
        normal = (rect1.x < rect2.x) ? glm::vec2(-1, 0) : glm::vec2(1, 0);
        penetration = overlapX;
    } else {
        normal = (rect1.y < rect2.y) ? glm::vec2(0, -1) : glm::vec2(0, 1);
        penetration = overlapY;
    }

    // Position correction (only dynamic objects move)
    float totalInvMass = invMass1 + invMass2;
    if (totalInvMass > 0) {
        glm::vec2 correction = normal * (penetration / totalInvMass);
        pos1 += correction * invMass1;
        pos2 -= correction * invMass2;
    }

    // Velocity resolution (only dynamic objects get their velocity changed)
    glm::vec2 relativeVelocity = vel2 - vel1;
    float separatingVelocity = glm::dot(relativeVelocity, normal);

    if (separatingVelocity > 0) return;

    float restitution = 0.3f;
    float impulse = -(1 + restitution) * separatingVelocity / totalInvMass;

    glm::vec2 impulseVector = normal * impulse;
    vel1 -= impulseVector * invMass1;  // Only changes if dynamic
    vel2 += impulseVector * invMass2;  // Only changes if dynamic
}

void handleOnEnter(Registry &reg, Entity &entity1, Entity &entity2) {


    Collider &col1 = reg.get<Collider>(entity1);
    Collider &col2 = reg.get<Collider>(entity2);
    if(col1.isTrigger && !prevFrameCols.contains({entity1, entity2}))
        col1.onEnter(entity1, entity2);
    if(col2.isTrigger && !prevFrameCols.contains({entity2, entity1}))
        col2.onEnter(entity2, entity1);

}

void handleOnExit(Registry &reg, Entity &entity1, Entity &entity2) {

    Collider &col1 = reg.get<Collider>(entity1);
    Collider &col2 = reg.get<Collider>(entity2);
    if(col1.isTrigger && !currentFrameCols.contains({entity1, entity2}) && prevFrameCols.contains({entity1, entity2}))
        col1.onExit(entity1, entity2);
    if(col2.isTrigger && !currentFrameCols.contains({entity2, entity1}) && prevFrameCols.contains({entity2, entity1}))
        col2.onExit(entity2, entity1);

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
                currentFrameCols.insert({entities[i], entities[j]});
                currentFrameCols.insert({entities[j], entities[i]});

                if(colA.isTrigger || colB.isTrigger) {
                    handleOnEnter(reg, entities[i], entities[j]);

                    continue;
                }

                resolveCollision(reg, entities[i], entities[j]);

                if (colA.onCollide != nullptr && !colA.isTrigger)
                    colA.onCollide(entities[i], entities[j]);
                if (colB.onCollide != nullptr && !colB.isTrigger)
                    colB.onCollide(entities[i], entities[j]);


            }

        }
    }

    for(auto [e1,e2] : prevFrameCols) {
        handleOnExit(reg, e1, e2);
    }

    prevFrameCols = currentFrameCols;
    currentFrameCols.clear();
}

}
