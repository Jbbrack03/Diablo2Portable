#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>
#include "world/collision_system.h"
#include "world/collision_entity.h"
#include "world/collision_response.h"

class CollisionResponseTest : public ::testing::Test {
protected:
    CollisionSystem collisionSystem;
    CollisionResponse responseSystem;
    
    void SetUp() override {
        collisionSystem.clear();
        responseSystem.setCollisionSystem(&collisionSystem);
    }
};

// Test 1: Basic position correction after collision
TEST_F(CollisionResponseTest, BasicPositionCorrection) {
    auto entity1 = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    entity1->setPosition(glm::vec2(0.0f, 0.0f));
    entity1->setSize(glm::vec2(40.0f, 40.0f));
    entity1->setVelocity(glm::vec2(10.0f, 0.0f));  // Moving right
    
    auto entity2 = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    entity2->setPosition(glm::vec2(30.0f, 0.0f));
    entity2->setSize(glm::vec2(40.0f, 40.0f));
    entity2->setStatic(true);  // Static wall
    
    collisionSystem.addEntity(entity1);
    collisionSystem.addEntity(entity2);
    
    // Resolve collision
    responseSystem.resolveCollisions();
    
    // Entity1 should be pushed back to not overlap
    auto newPos = entity1->getPosition();
    EXPECT_LT(newPos.x, -10.0f);  // Should be pushed left of its starting position
}

// Test 2: Velocity reflection on collision (bounce)
TEST_F(CollisionResponseTest, VelocityReflection) {
    auto entity = std::make_shared<CollisionEntity>(1, CollisionShape::CIRCLE);
    entity->setPosition(glm::vec2(65.0f, 50.0f));  // Circle touching wall
    entity->setRadius(10.0f);
    entity->setVelocity(glm::vec2(20.0f, 0.0f));  // Moving right
    entity->setBounciness(1.0f);  // Perfect bounce
    
    auto wall = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    wall->setPosition(glm::vec2(70.0f, 0.0f));
    wall->setSize(glm::vec2(20.0f, 100.0f));
    wall->setStatic(true);
    
    collisionSystem.addEntity(entity);
    collisionSystem.addEntity(wall);
    
    responseSystem.resolveCollisions();
    
    // Velocity should be reflected
    auto newVel = entity->getVelocity();
    EXPECT_LT(newVel.x, 0.0f);  // Should now be moving left
    EXPECT_FLOAT_EQ(std::abs(newVel.x), 20.0f);  // Same speed, opposite direction
}

// Test 3: Mass-based collision response
TEST_F(CollisionResponseTest, MassBasedResponse) {
    auto lightEntity = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    lightEntity->setPosition(glm::vec2(0.0f, 0.0f));
    lightEntity->setSize(glm::vec2(30.0f, 30.0f));
    lightEntity->setMass(1.0f);
    lightEntity->setVelocity(glm::vec2(50.0f, 0.0f));
    
    auto heavyEntity = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    heavyEntity->setPosition(glm::vec2(25.0f, 0.0f));  // Overlapping with light entity
    heavyEntity->setSize(glm::vec2(30.0f, 30.0f));
    heavyEntity->setMass(10.0f);
    heavyEntity->setVelocity(glm::vec2(-10.0f, 0.0f));
    
    collisionSystem.addEntity(lightEntity);
    collisionSystem.addEntity(heavyEntity);
    
    auto lightPosBefore = lightEntity->getPosition();
    auto heavyPosBefore = heavyEntity->getPosition();
    
    responseSystem.resolveCollisions();
    
    // Light entity should move more than heavy entity
    auto lightPosAfter = lightEntity->getPosition();
    auto heavyPosAfter = heavyEntity->getPosition();
    
    float lightMovement = std::abs(lightPosAfter.x - lightPosBefore.x);
    float heavyMovement = std::abs(heavyPosAfter.x - heavyPosBefore.x);
    
    EXPECT_GT(lightMovement, heavyMovement * 5);  // Light entity moves much more
}

// Test 4: Sliding along walls
TEST_F(CollisionResponseTest, WallSliding) {
    auto player = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    player->setPosition(glm::vec2(65.0f, 50.0f));  // Touching the wall
    player->setSize(glm::vec2(20.0f, 20.0f));
    player->setVelocity(glm::vec2(10.0f, 10.0f));  // Moving diagonally
    
    auto wall = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    wall->setPosition(glm::vec2(80.0f, 0.0f));
    wall->setSize(glm::vec2(20.0f, 200.0f));  // Vertical wall
    wall->setStatic(true);
    
    collisionSystem.addEntity(player);
    collisionSystem.addEntity(wall);
    
    auto oldY = player->getPosition().y;
    
    // Apply movement and resolve
    responseSystem.update(0.1f);  // 0.1 second timestep
    
    auto newPos = player->getPosition();
    auto newVel = player->getVelocity();
    
    // Player should slide along wall (Y movement continues, X is blocked)
    EXPECT_GT(newPos.y, oldY);  // Y position increased
    EXPECT_LT(newPos.x, 70.0f);  // X position constrained by wall
    EXPECT_NEAR(newVel.x, 0.0f, 0.1f);  // X velocity should be near zero
    EXPECT_GT(newVel.y, 0.0f);  // Y velocity should remain
}

// Test 5: Trigger collision (no physical response)
TEST_F(CollisionResponseTest, TriggerCollision) {
    auto player = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    player->setPosition(glm::vec2(0.0f, 0.0f));
    player->setSize(glm::vec2(30.0f, 30.0f));
    player->setVelocity(glm::vec2(50.0f, 0.0f));
    
    auto trigger = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    trigger->setPosition(glm::vec2(3.0f, 0.0f));  // Will overlap after player moves
    trigger->setSize(glm::vec2(30.0f, 30.0f));
    trigger->setTrigger(true);  // This is a trigger zone
    
    collisionSystem.addEntity(player);
    collisionSystem.addEntity(trigger);
    
    auto oldPos = player->getPosition();
    
    responseSystem.update(0.1f);
    
    auto newPos = player->getPosition();
    
    // Player should pass through trigger
    EXPECT_GT(newPos.x, oldPos.x);  // Movement continues
    EXPECT_NEAR(newPos.x, oldPos.x + 5.0f, 0.1f);  // Moved by velocity * time
    
    // But trigger collision should be reported
    auto triggerCollisions = responseSystem.getTriggerCollisions();
    EXPECT_EQ(triggerCollisions.size(), 1);
    EXPECT_TRUE((triggerCollisions[0].first == 1 && triggerCollisions[0].second == 2) ||
                (triggerCollisions[0].first == 2 && triggerCollisions[0].second == 1));
}

// Test 6: Continuous collision detection for fast objects
TEST_F(CollisionResponseTest, ContinuousCollisionDetection) {
    auto bullet = std::make_shared<CollisionEntity>(1, CollisionShape::CIRCLE);
    bullet->setPosition(glm::vec2(0.0f, 50.0f));
    bullet->setRadius(5.0f);
    bullet->setVelocity(glm::vec2(1000.0f, 0.0f));  // Very fast
    bullet->setContinuous(true);  // Enable CCD
    
    auto wall = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    wall->setPosition(glm::vec2(100.0f, 0.0f));
    wall->setSize(glm::vec2(20.0f, 100.0f));
    wall->setStatic(true);
    
    collisionSystem.addEntity(bullet);
    collisionSystem.addEntity(wall);
    
    responseSystem.update(0.2f);  // Large timestep where bullet would pass through
    
    auto bulletPos = bullet->getPosition();
    
    // Bullet should be stopped at wall, not pass through
    EXPECT_LT(bulletPos.x, 95.0f);  // Should be before the wall
}

// Test 7: Collision callbacks
TEST_F(CollisionResponseTest, CollisionCallbacks) {
    bool collisionDetected = false;
    int collidedEntity1 = -1;
    int collidedEntity2 = -1;
    
    responseSystem.setCollisionCallback([&](int id1, int id2, const CollisionInfo& info) {
        collisionDetected = true;
        collidedEntity1 = id1;
        collidedEntity2 = id2;
    });
    
    auto entity1 = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    entity1->setPosition(glm::vec2(0.0f, 0.0f));
    entity1->setSize(glm::vec2(40.0f, 40.0f));
    
    auto entity2 = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    entity2->setPosition(glm::vec2(30.0f, 0.0f));
    entity2->setSize(glm::vec2(40.0f, 40.0f));
    
    collisionSystem.addEntity(entity1);
    collisionSystem.addEntity(entity2);
    
    responseSystem.resolveCollisions();
    
    EXPECT_TRUE(collisionDetected);
    EXPECT_TRUE((collidedEntity1 == 1 && collidedEntity2 == 2) ||
                (collidedEntity1 == 2 && collidedEntity2 == 1));
}

// Test 8: Damage on collision
TEST_F(CollisionResponseTest, DamageOnCollision) {
    auto projectile = std::make_shared<CollisionEntity>(1, CollisionShape::CIRCLE);
    projectile->setPosition(glm::vec2(0.0f, 0.0f));
    projectile->setRadius(10.0f);
    projectile->setVelocity(glm::vec2(100.0f, 0.0f));
    projectile->setDamageOnCollision(25.0f);
    projectile->setLayer(CollisionLayer::PROJECTILE);
    
    auto monster = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    monster->setPosition(glm::vec2(5.0f, -10.0f));  // Will collide with projectile
    monster->setSize(glm::vec2(40.0f, 40.0f));
    monster->setHealth(100.0f);
    monster->setLayer(CollisionLayer::MONSTER);
    
    collisionSystem.addEntity(projectile);
    collisionSystem.addEntity(monster);
    
    responseSystem.update(0.1f);
    
    // Monster should take damage
    EXPECT_FLOAT_EQ(monster->getHealth(), 75.0f);
    
    // Projectile should be destroyed
    EXPECT_TRUE(projectile->isDestroyed());
}

// Test 9: Push force on collision
TEST_F(CollisionResponseTest, PushForceOnCollision) {
    auto player = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    player->setPosition(glm::vec2(0.0f, 0.0f));
    player->setSize(glm::vec2(30.0f, 30.0f));
    player->setVelocity(glm::vec2(50.0f, 0.0f));
    player->setMass(2.0f);
    
    auto crate = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    crate->setPosition(glm::vec2(3.0f, 0.0f));  // Will collide after player moves
    crate->setSize(glm::vec2(30.0f, 30.0f));
    crate->setMass(1.0f);
    crate->setPushable(true);
    
    collisionSystem.addEntity(player);
    collisionSystem.addEntity(crate);
    
    responseSystem.update(0.1f);
    
    // Crate should be pushed by player
    auto crateVel = crate->getVelocity();
    EXPECT_GT(crateVel.x, 0.0f);  // Crate moves in player's direction
}

// Test 10: Collision group filtering
TEST_F(CollisionResponseTest, CollisionGroupFiltering) {
    auto ally1 = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    ally1->setPosition(glm::vec2(0.0f, 0.0f));
    ally1->setSize(glm::vec2(30.0f, 30.0f));
    ally1->setCollisionGroup(1);  // Group 1 = allies
    
    auto ally2 = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    ally2->setPosition(glm::vec2(20.0f, 0.0f));
    ally2->setSize(glm::vec2(30.0f, 30.0f));
    ally2->setCollisionGroup(1);  // Same group
    
    auto enemy = std::make_shared<CollisionEntity>(3, CollisionShape::AABB);
    enemy->setPosition(glm::vec2(40.0f, 0.0f));
    enemy->setSize(glm::vec2(30.0f, 30.0f));
    enemy->setCollisionGroup(2);  // Group 2 = enemies
    
    collisionSystem.addEntity(ally1);
    collisionSystem.addEntity(ally2);
    collisionSystem.addEntity(enemy);
    
    // Set group collision rules - allies don't collide with each other
    responseSystem.setGroupCollision(1, 1, false);
    responseSystem.setGroupCollision(1, 2, true);
    
    auto collisions = responseSystem.detectAndResolveCollisions();
    
    // Should only detect ally vs enemy collision, not ally vs ally
    // Note: Only ally2 overlaps with enemy, so we expect 1 collision
    EXPECT_EQ(collisions.size(), 1);  // ally2-enemy
    
    bool foundAllyCollision = false;
    for (const auto& collision : collisions) {
        if ((collision.first == 1 && collision.second == 2) ||
            (collision.first == 2 && collision.second == 1)) {
            foundAllyCollision = true;
        }
    }
    EXPECT_FALSE(foundAllyCollision);  // Allies should not collide
}