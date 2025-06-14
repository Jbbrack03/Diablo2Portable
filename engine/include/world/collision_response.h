#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <unordered_map>

class CollisionSystem;

struct CollisionInfo {
    glm::vec2 contactPoint;
    glm::vec2 contactNormal;
    float penetrationDepth;
    bool isTrigger;
};

class CollisionResponse {
public:
    CollisionResponse() = default;
    ~CollisionResponse() = default;
    
    // Set the collision system to work with
    void setCollisionSystem(CollisionSystem* system) { collisionSystem = system; }
    
    // Main update function
    void update(float deltaTime);
    
    // Collision resolution
    void resolveCollisions();
    std::vector<std::pair<int, int>> detectAndResolveCollisions();
    
    // Get trigger collisions (no physical response)
    std::vector<std::pair<int, int>> getTriggerCollisions() const { return triggerCollisions; }
    
    // Collision callbacks
    using CollisionCallback = std::function<void(int, int, const CollisionInfo&)>;
    void setCollisionCallback(CollisionCallback callback) { collisionCallback = callback; }
    
    // Group collision settings
    void setGroupCollision(int group1, int group2, bool canCollide);
    
private:
    CollisionSystem* collisionSystem = nullptr;
    std::vector<std::pair<int, int>> triggerCollisions;
    CollisionCallback collisionCallback;
    
    // Group collision matrix
    std::unordered_map<int, std::unordered_map<int, bool>> groupCollisionMatrix;
    
    // Helper functions
    void resolveAABBCollision(class CollisionEntity* entity1, class CollisionEntity* entity2);
    void resolveCircleCollision(class CollisionEntity* entity1, class CollisionEntity* entity2);
    void resolveMixedCollision(class CollisionEntity* entity1, class CollisionEntity* entity2);
    void applyDamage(class CollisionEntity* entity1, class CollisionEntity* entity2);
    void applyPushForce(class CollisionEntity* pusher, class CollisionEntity* pushed);
    bool canGroupsCollide(int group1, int group2) const;
    
    // Continuous collision detection
    bool performCCD(class CollisionEntity* entity, float deltaTime);
    
    // Wall sliding
    void applyWallSliding(class CollisionEntity* entity, const glm::vec2& collisionNormal);
};