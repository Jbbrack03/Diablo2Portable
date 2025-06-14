#pragma once

#include <glm/glm.hpp>

enum class CollisionShape {
    AABB,
    CIRCLE
};

enum class CollisionLayer {
    DEFAULT = 0,
    PLAYER = 1,
    MONSTER = 2,
    WALL = 3,
    PROJECTILE = 4,
    ITEM = 5,
    TRIGGER = 6
};

class CollisionEntity {
public:
    CollisionEntity(int id, CollisionShape shape)
        : entityId(id), shape(shape) {}
    
    // Getters
    int getId() const { return entityId; }
    CollisionShape getShape() const { return shape; }
    glm::vec2 getPosition() const { return position; }
    glm::vec2 getSize() const { return size; }
    float getRadius() const { return radius; }
    glm::vec2 getVelocity() const { return velocity; }
    float getMass() const { return mass; }
    bool isStatic() const { return isStaticEntity; }
    bool isTrigger() const { return isTriggerEntity; }
    bool isPushable() const { return isPushableEntity; }
    int getLayer() const { return static_cast<int>(layer); }
    int getCollisionGroup() const { return collisionGroup; }
    float getBounciness() const { return bounciness; }
    bool isContinuous() const { return continuousCollision; }
    float getHealth() const { return health; }
    bool isDestroyed() const { return destroyed; }
    float getDamageOnCollision() const { return damageOnCollision; }
    
    // Setters
    void setPosition(const glm::vec2& pos) { position = pos; }
    void setSize(const glm::vec2& sz) { size = sz; }
    void setRadius(float r) { radius = r; }
    void setVelocity(const glm::vec2& vel) { velocity = vel; }
    void setMass(float m) { mass = m; }
    void setStatic(bool s) { isStaticEntity = s; }
    void setTrigger(bool t) { isTriggerEntity = t; }
    void setPushable(bool p) { isPushableEntity = p; }
    void setLayer(CollisionLayer l) { layer = l; }
    void setCollisionGroup(int group) { collisionGroup = group; }
    void setBounciness(float b) { bounciness = b; }
    void setContinuous(bool c) { continuousCollision = c; }
    void setHealth(float h) { health = h; }
    void setDamageOnCollision(float d) { damageOnCollision = d; }
    void destroy() { destroyed = true; }
    
    // Get AABB bounds for any shape
    void getAABB(glm::vec2& min, glm::vec2& max) const {
        if (shape == CollisionShape::AABB) {
            min = position;
            max = position + size;
        } else { // Circle
            min = position - glm::vec2(radius);
            max = position + glm::vec2(radius);
        }
    }
    
private:
    int entityId;
    CollisionShape shape;
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 size{1.0f, 1.0f};  // For AABB
    float radius = 1.0f;          // For Circle
    glm::vec2 velocity{0.0f, 0.0f};
    float mass = 1.0f;
    bool isStaticEntity = false;
    bool isTriggerEntity = false;
    bool isPushableEntity = false;
    CollisionLayer layer = CollisionLayer::DEFAULT;
    int collisionGroup = 0;  // 0 = no group
    float bounciness = 0.0f;  // 0 = no bounce, 1 = perfect bounce
    bool continuousCollision = false;
    float health = 100.0f;
    bool destroyed = false;
    float damageOnCollision = 0.0f;
};