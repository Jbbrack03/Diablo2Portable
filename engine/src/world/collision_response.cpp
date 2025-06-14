#include "world/collision_response.h"
#include "world/collision_system.h"
#include "world/collision_entity.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

void CollisionResponse::update(float deltaTime) {
    // Clear previous frame's trigger collisions
    triggerCollisions.clear();
    
    // Update entity positions based on velocity
    auto entities = collisionSystem->getEntities();
    for (auto& [id, entity] : entities) {
        if (!entity->isStatic() && !entity->isDestroyed()) {
            glm::vec2 oldPos = entity->getPosition();
            glm::vec2 velocity = entity->getVelocity();
            
            // Continuous collision detection for fast entities
            if (entity->isContinuous() && glm::length(velocity) > 0.0f) {
                if (!performCCD(entity.get(), deltaTime)) {
                    // CCD prevented movement, entity position already updated
                    continue;
                }
            }
            
            // Normal movement
            entity->setPosition(oldPos + velocity * deltaTime);
        }
    }
    
    // Resolve collisions
    resolveCollisions();
}

void CollisionResponse::resolveCollisions() {
    auto collisions = collisionSystem->checkCollisions();
    
    for (const auto& [id1, id2] : collisions) {
        auto entity1 = collisionSystem->getEntity(id1);
        auto entity2 = collisionSystem->getEntity(id2);
        
        if (!entity1 || !entity2) continue;
        
        
        // Check group collision rules
        if (!canGroupsCollide(entity1->getCollisionGroup(), entity2->getCollisionGroup())) {
            continue;
        }
        
        // Handle trigger collisions
        if (entity1->isTrigger() || entity2->isTrigger()) {
            triggerCollisions.push_back({id1, id2});
            
            // Call collision callback for triggers
            if (collisionCallback) {
                CollisionInfo info;
                info.isTrigger = true;
                collisionCallback(id1, id2, info);
            }
            continue;  // No physical response for triggers
        }
        
        // Apply damage if applicable
        applyDamage(entity1.get(), entity2.get());
        
        // Resolve physical collision
        if (entity1->getShape() == CollisionShape::AABB && entity2->getShape() == CollisionShape::AABB) {
            resolveAABBCollision(entity1.get(), entity2.get());
        } else if (entity1->getShape() == CollisionShape::CIRCLE && entity2->getShape() == CollisionShape::CIRCLE) {
            resolveCircleCollision(entity1.get(), entity2.get());
        } else {
            resolveMixedCollision(entity1.get(), entity2.get());
        }
        
        // Apply push forces
        if (entity1->isPushable() || entity2->isPushable()) {
            if (!entity1->isStatic() && entity2->isPushable()) {
                applyPushForce(entity1.get(), entity2.get());
            }
            if (!entity2->isStatic() && entity1->isPushable()) {
                applyPushForce(entity2.get(), entity1.get());
            }
        }
        
        // Call collision callback
        if (collisionCallback) {
            CollisionInfo info;
            info.isTrigger = false;
            collisionCallback(id1, id2, info);
        }
    }
}

std::vector<std::pair<int, int>> CollisionResponse::detectAndResolveCollisions() {
    resolveCollisions();
    
    // Return all collisions (physical + triggers)
    auto physicalCollisions = collisionSystem->checkCollisions();
    physicalCollisions.insert(physicalCollisions.end(), triggerCollisions.begin(), triggerCollisions.end());
    
    // Filter by group collision rules
    physicalCollisions.erase(
        std::remove_if(physicalCollisions.begin(), physicalCollisions.end(),
            [this](const std::pair<int, int>& collision) {
                auto entity1 = collisionSystem->getEntity(collision.first);
                auto entity2 = collisionSystem->getEntity(collision.second);
                if (!entity1 || !entity2) return true;
                return !canGroupsCollide(entity1->getCollisionGroup(), entity2->getCollisionGroup());
            }),
        physicalCollisions.end()
    );
    
    return physicalCollisions;
}

void CollisionResponse::setGroupCollision(int group1, int group2, bool canCollide) {
    groupCollisionMatrix[group1][group2] = canCollide;
    groupCollisionMatrix[group2][group1] = canCollide;  // Symmetric
}

void CollisionResponse::resolveAABBCollision(CollisionEntity* entity1, CollisionEntity* entity2) {
    AABB box1(entity1->getPosition(), entity1->getSize());
    AABB box2(entity2->getPosition(), entity2->getSize());
    
    
    glm::vec2 penetration = collisionSystem->getPenetrationVector(box1, box2);
    
    // For static entities, only move the non-static one
    if (entity2->isStatic() && !entity1->isStatic()) {
        // Add small epsilon to prevent floating point issues
        const float epsilon = 0.01f;
        glm::vec2 newPos = entity1->getPosition() + penetration * (1.0f + epsilon);
        entity1->setPosition(newPos);
        
        // Wall sliding - if moving against a static wall
        if (glm::length(entity1->getVelocity()) > 0.0f && glm::length(penetration) > 0.0f) {
            glm::vec2 normal = glm::normalize(penetration);
            applyWallSliding(entity1, normal);
        }
        
        // Velocity reflection for bouncy entities
        if (entity1->getBounciness() > 0.0f && glm::length(penetration) > 0.0f) {
            glm::vec2 normal = glm::normalize(penetration);
            glm::vec2 velocity = entity1->getVelocity();
            glm::vec2 reflected = velocity - 2.0f * glm::dot(velocity, normal) * normal;
            entity1->setVelocity(reflected * entity1->getBounciness());
        }
        return;
    }
    
    if (entity1->isStatic() && !entity2->isStatic()) {
        // Add small epsilon to prevent floating point issues
        const float epsilon = 0.01f;
        glm::vec2 newPos = entity2->getPosition() - penetration * (1.0f + epsilon);
        entity2->setPosition(newPos);
        
        // Wall sliding - if moving against a static wall
        if (glm::length(entity2->getVelocity()) > 0.0f && glm::length(penetration) > 0.0f) {
            glm::vec2 normal = -glm::normalize(penetration);  // Normal points from entity1 to entity2
            applyWallSliding(entity2, normal);
        }
        
        // Velocity reflection for bouncy entities
        if (entity2->getBounciness() > 0.0f && glm::length(penetration) > 0.0f) {
            glm::vec2 normal = -glm::normalize(penetration);
            glm::vec2 velocity = entity2->getVelocity();
            glm::vec2 reflected = velocity - 2.0f * glm::dot(velocity, normal) * normal;
            entity2->setVelocity(reflected * entity2->getBounciness());
        }
        return;
    }
    
    // Both non-static: Mass-based resolution
    float totalMass = entity1->getMass() + entity2->getMass();
    float ratio1 = entity2->getMass() / totalMass;
    float ratio2 = entity1->getMass() / totalMass;
    
    entity1->setPosition(entity1->getPosition() + penetration * ratio1);
    entity2->setPosition(entity2->getPosition() - penetration * ratio2);
}

void CollisionResponse::resolveCircleCollision(CollisionEntity* entity1, CollisionEntity* entity2) {
    glm::vec2 diff = entity2->getPosition() - entity1->getPosition();
    float distance = glm::length(diff);
    float radiusSum = entity1->getRadius() + entity2->getRadius();
    
    if (distance < 0.001f) {
        // Entities are at same position, push apart arbitrarily
        diff = glm::vec2(1.0f, 0.0f);
        distance = 1.0f;
    }
    
    glm::vec2 normal = diff / distance;
    float overlap = radiusSum - distance;
    
    // Mass-based resolution
    float totalMass = entity1->getMass() + entity2->getMass();
    float ratio1 = entity2->isStatic() ? 1.0f : entity2->getMass() / totalMass;
    float ratio2 = entity1->isStatic() ? 1.0f : entity1->getMass() / totalMass;
    
    if (!entity1->isStatic()) {
        entity1->setPosition(entity1->getPosition() - normal * overlap * ratio1);
        
        // Velocity reflection
        if (entity1->getBounciness() > 0.0f) {
            glm::vec2 velocity = entity1->getVelocity();
            glm::vec2 reflected = velocity - 2.0f * glm::dot(velocity, normal) * normal;
            entity1->setVelocity(reflected * entity1->getBounciness());
        }
    }
    
    if (!entity2->isStatic()) {
        entity2->setPosition(entity2->getPosition() + normal * overlap * ratio2);
        
        // Velocity reflection
        if (entity2->getBounciness() > 0.0f) {
            glm::vec2 velocity = entity2->getVelocity();
            glm::vec2 reflected = velocity - 2.0f * glm::dot(velocity, -normal) * (-normal);
            entity2->setVelocity(reflected * entity2->getBounciness());
        }
    }
}

void CollisionResponse::resolveMixedCollision(CollisionEntity* entity1, CollisionEntity* entity2) {
    // Simple implementation - treat circles as AABBs for now
    if (entity1->getShape() == CollisionShape::CIRCLE) {
        std::swap(entity1, entity2);
    }
    
    
    // entity1 is AABB, entity2 is Circle
    AABB box(entity1->getPosition(), entity1->getSize());
    Circle circle(entity2->getPosition(), entity2->getRadius());
    
    // Find closest point on box to circle
    float closestX = std::max(box.position.x, std::min(circle.center.x, box.position.x + box.size.x));
    float closestY = std::max(box.position.y, std::min(circle.center.y, box.position.y + box.size.y));
    glm::vec2 closestPoint(closestX, closestY);
    
    glm::vec2 diff = circle.center - closestPoint;
    float distance = glm::length(diff);
    
    
    if (distance < circle.radius && distance > 0.001f) {
        glm::vec2 normal = diff / distance;
        float overlap = circle.radius - distance;
        
        // Mass-based resolution
        float totalMass = entity1->getMass() + entity2->getMass();
        float ratio1 = entity2->isStatic() ? 1.0f : entity2->getMass() / totalMass;
        float ratio2 = entity1->isStatic() ? 1.0f : entity1->getMass() / totalMass;
        
        if (!entity1->isStatic()) {
            entity1->setPosition(entity1->getPosition() - normal * overlap * ratio1);
        }
        
        if (!entity2->isStatic()) {
            entity2->setPosition(entity2->getPosition() + normal * overlap * ratio2);
            
            // Velocity reflection for bouncy circle
            if (entity2->getBounciness() > 0.0f) {
                glm::vec2 velocity = entity2->getVelocity();
                glm::vec2 reflected = velocity - 2.0f * glm::dot(velocity, normal) * normal;
                entity2->setVelocity(reflected * entity2->getBounciness());
            }
        }
    }
}

void CollisionResponse::applyDamage(CollisionEntity* entity1, CollisionEntity* entity2) {
    if (entity1->getDamageOnCollision() > 0.0f) {
        entity2->setHealth(entity2->getHealth() - entity1->getDamageOnCollision());
        if (entity2->getHealth() <= 0.0f) {
            entity2->destroy();
        }
        // Projectiles destroy themselves on impact
        if (entity1->getLayer() == static_cast<int>(CollisionLayer::PROJECTILE)) {
            entity1->destroy();
        }
    }
    
    if (entity2->getDamageOnCollision() > 0.0f) {
        entity1->setHealth(entity1->getHealth() - entity2->getDamageOnCollision());
        if (entity1->getHealth() <= 0.0f) {
            entity1->destroy();
        }
        // Projectiles destroy themselves on impact
        if (entity2->getLayer() == static_cast<int>(CollisionLayer::PROJECTILE)) {
            entity2->destroy();
        }
    }
}

void CollisionResponse::applyPushForce(CollisionEntity* pusher, CollisionEntity* pushed) {
    if (pushed->isPushable() && !pushed->isStatic()) {
        // Transfer some velocity from pusher to pushed
        glm::vec2 pushVelocity = pusher->getVelocity() * 0.5f;  // Transfer 50% of velocity
        pushed->setVelocity(pushed->getVelocity() + pushVelocity);
    }
}

bool CollisionResponse::canGroupsCollide(int group1, int group2) const {
    // Group 0 means no group - always collides
    if (group1 == 0 || group2 == 0) {
        return true;
    }
    
    auto it1 = groupCollisionMatrix.find(group1);
    if (it1 != groupCollisionMatrix.end()) {
        auto it2 = it1->second.find(group2);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    
    // Default: groups can collide unless specified otherwise
    return true;
}

bool CollisionResponse::performCCD(CollisionEntity* entity, float deltaTime) {
    glm::vec2 startPos = entity->getPosition();
    glm::vec2 velocity = entity->getVelocity();
    glm::vec2 endPos = startPos + velocity * deltaTime;
    
    // Raycast from start to end position
    float stepSize = entity->getShape() == CollisionShape::CIRCLE ? entity->getRadius() : 
                    std::min(entity->getSize().x, entity->getSize().y) * 0.5f;
    
    glm::vec2 direction = endPos - startPos;
    float distance = glm::length(direction);
    
    if (distance < 0.001f) {
        return true;  // No movement
    }
    
    direction = glm::normalize(direction);
    int steps = static_cast<int>(distance / stepSize) + 1;
    
    // Check collision at each step
    for (int i = 1; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        glm::vec2 testPos = startPos + direction * distance * t;
        
        // Temporarily move entity to test position
        glm::vec2 oldPos = entity->getPosition();
        entity->setPosition(testPos);
        
        // Check for collisions
        auto collisions = collisionSystem->getCollidingEntities(entity->getId());
        
        if (!collisions.empty()) {
            // Collision detected, move to just before collision
            if (i > 1) {
                float safeT = static_cast<float>(i - 1) / steps;
                entity->setPosition(startPos + direction * distance * safeT);
            } else {
                entity->setPosition(oldPos);
            }
            
            // Stop velocity in collision direction
            entity->setVelocity(glm::vec2(0.0f, 0.0f));
            return false;  // Movement prevented
        }
        
        entity->setPosition(oldPos);  // Restore position
    }
    
    return true;  // No collision, movement allowed
}

void CollisionResponse::applyWallSliding(CollisionEntity* entity, const glm::vec2& collisionNormal) {
    glm::vec2 velocity = entity->getVelocity();
    
    // Project velocity onto the wall (perpendicular to normal)
    glm::vec2 wallDirection(-collisionNormal.y, collisionNormal.x);  // 90 degree rotation
    float slideSpeed = glm::dot(velocity, wallDirection);
    
    // Set velocity to slide along wall
    entity->setVelocity(wallDirection * slideSpeed);
}