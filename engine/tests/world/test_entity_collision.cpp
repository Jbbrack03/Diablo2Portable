#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "world/collision_system.h"
#include "world/collision_entity.h"

class EntityCollisionTest : public ::testing::Test {
protected:
    CollisionSystem collisionSystem;
    
    void SetUp() override {
        collisionSystem.clear();
    }
};

// Test 1: Add entity to collision system
TEST_F(EntityCollisionTest, AddEntityToSystem) {
    auto entity = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    entity->setPosition(glm::vec2(50.0f, 50.0f));
    entity->setSize(glm::vec2(32.0f, 32.0f));
    
    collisionSystem.addEntity(entity);
    
    EXPECT_EQ(collisionSystem.getEntityCount(), 1);
}

// Test 2: Remove entity from collision system
TEST_F(EntityCollisionTest, RemoveEntityFromSystem) {
    auto entity = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    collisionSystem.addEntity(entity);
    
    EXPECT_EQ(collisionSystem.getEntityCount(), 1);
    
    collisionSystem.removeEntity(entity->getId());
    EXPECT_EQ(collisionSystem.getEntityCount(), 0);
}

// Test 3: Detect collision between two entities
TEST_F(EntityCollisionTest, DetectEntityCollision) {
    auto entity1 = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    entity1->setPosition(glm::vec2(0.0f, 0.0f));
    entity1->setSize(glm::vec2(50.0f, 50.0f));
    
    auto entity2 = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    entity2->setPosition(glm::vec2(40.0f, 40.0f));
    entity2->setSize(glm::vec2(50.0f, 50.0f));
    
    collisionSystem.addEntity(entity1);
    collisionSystem.addEntity(entity2);
    
    auto collisions = collisionSystem.checkCollisions();
    EXPECT_EQ(collisions.size(), 1);
    EXPECT_TRUE((collisions[0].first == 1 && collisions[0].second == 2) ||
                (collisions[0].first == 2 && collisions[0].second == 1));
}

// Test 4: No collision when entities are separate
TEST_F(EntityCollisionTest, NoCollisionWhenEntitiesSeparate) {
    auto entity1 = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    entity1->setPosition(glm::vec2(0.0f, 0.0f));
    entity1->setSize(glm::vec2(30.0f, 30.0f));
    
    auto entity2 = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    entity2->setPosition(glm::vec2(100.0f, 100.0f));
    entity2->setSize(glm::vec2(30.0f, 30.0f));
    
    collisionSystem.addEntity(entity1);
    collisionSystem.addEntity(entity2);
    
    auto collisions = collisionSystem.checkCollisions();
    EXPECT_EQ(collisions.size(), 0);
}

// Test 5: Circle entity collision
TEST_F(EntityCollisionTest, CircleEntityCollision) {
    auto entity1 = std::make_shared<CollisionEntity>(1, CollisionShape::CIRCLE);
    entity1->setPosition(glm::vec2(0.0f, 0.0f));
    entity1->setRadius(25.0f);
    
    auto entity2 = std::make_shared<CollisionEntity>(2, CollisionShape::CIRCLE);
    entity2->setPosition(glm::vec2(40.0f, 0.0f));
    entity2->setRadius(25.0f);
    
    collisionSystem.addEntity(entity1);
    collisionSystem.addEntity(entity2);
    
    auto collisions = collisionSystem.checkCollisions();
    EXPECT_EQ(collisions.size(), 1);
}

// Test 6: Mixed shape collision (AABB vs Circle)
TEST_F(EntityCollisionTest, MixedShapeCollision) {
    auto boxEntity = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    boxEntity->setPosition(glm::vec2(0.0f, 0.0f));
    boxEntity->setSize(glm::vec2(40.0f, 40.0f));
    
    auto circleEntity = std::make_shared<CollisionEntity>(2, CollisionShape::CIRCLE);
    circleEntity->setPosition(glm::vec2(50.0f, 20.0f));
    circleEntity->setRadius(15.0f);
    
    collisionSystem.addEntity(boxEntity);
    collisionSystem.addEntity(circleEntity);
    
    auto collisions = collisionSystem.checkCollisions();
    EXPECT_EQ(collisions.size(), 1);
}

// Test 7: Collision layers (entities on different layers don't collide)
TEST_F(EntityCollisionTest, CollisionLayers) {
    auto entity1 = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    entity1->setPosition(glm::vec2(0.0f, 0.0f));
    entity1->setSize(glm::vec2(50.0f, 50.0f));
    entity1->setLayer(CollisionLayer::PLAYER);
    
    auto entity2 = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    entity2->setPosition(glm::vec2(25.0f, 25.0f));
    entity2->setSize(glm::vec2(50.0f, 50.0f));
    entity2->setLayer(CollisionLayer::PLAYER);  // Same layer, no collision
    
    collisionSystem.addEntity(entity1);
    collisionSystem.addEntity(entity2);
    
    // Players shouldn't collide with each other in Diablo II
    collisionSystem.setLayerCollision(static_cast<int>(CollisionLayer::PLAYER), 
                                     static_cast<int>(CollisionLayer::PLAYER), false);
    
    auto collisions = collisionSystem.checkCollisions();
    EXPECT_EQ(collisions.size(), 0);
}

// Test 8: Static entities (walls) collide with dynamic entities
TEST_F(EntityCollisionTest, StaticEntityCollision) {
    auto wall = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    wall->setPosition(glm::vec2(50.0f, 50.0f));
    wall->setSize(glm::vec2(100.0f, 20.0f));
    wall->setStatic(true);
    wall->setLayer(CollisionLayer::WALL);
    
    auto player = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    player->setPosition(glm::vec2(60.0f, 40.0f));
    player->setSize(glm::vec2(30.0f, 30.0f));
    player->setLayer(CollisionLayer::PLAYER);
    
    collisionSystem.addEntity(wall);
    collisionSystem.addEntity(player);
    
    auto collisions = collisionSystem.checkCollisions();
    EXPECT_EQ(collisions.size(), 1);
}

// Test 9: Get specific entity collision info
TEST_F(EntityCollisionTest, GetEntityCollisionInfo) {
    auto entity1 = std::make_shared<CollisionEntity>(1, CollisionShape::AABB);
    entity1->setPosition(glm::vec2(0.0f, 0.0f));
    entity1->setSize(glm::vec2(40.0f, 40.0f));
    
    auto entity2 = std::make_shared<CollisionEntity>(2, CollisionShape::AABB);
    entity2->setPosition(glm::vec2(30.0f, 0.0f));
    entity2->setSize(glm::vec2(40.0f, 40.0f));
    
    collisionSystem.addEntity(entity1);
    collisionSystem.addEntity(entity2);
    
    auto collidingEntities = collisionSystem.getCollidingEntities(1);
    EXPECT_EQ(collidingEntities.size(), 1);
    EXPECT_EQ(collidingEntities[0], 2);
}

// Test 10: Spatial optimization with grid
TEST_F(EntityCollisionTest, SpatialGridOptimization) {
    // Set up spatial grid for optimization
    collisionSystem.initializeSpatialGrid(1000.0f, 1000.0f, 100.0f);  // 10x10 grid cells
    
    // Add many entities
    for (int i = 0; i < 100; ++i) {
        auto entity = std::make_shared<CollisionEntity>(i, CollisionShape::AABB);
        entity->setPosition(glm::vec2(static_cast<float>((i * 50) % 1000), 
                                      static_cast<float>((i * 50) / 1000) * 50.0f));
        entity->setSize(glm::vec2(30.0f, 30.0f));
        collisionSystem.addEntity(entity);
    }
    
    // Entities should only check collision with nearby entities
    auto entity = std::make_shared<CollisionEntity>(100, CollisionShape::AABB);
    entity->setPosition(glm::vec2(500.0f, 500.0f));
    entity->setSize(glm::vec2(30.0f, 30.0f));
    collisionSystem.addEntity(entity);
    
    // This should be efficient - only check nearby entities
    auto nearbyEntities = collisionSystem.getNearbyEntities(100, 150.0f);
    EXPECT_LT(nearbyEntities.size(), 20);  // Should find less than 20 nearby entities
}