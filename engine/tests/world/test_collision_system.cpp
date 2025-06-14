#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "world/collision_system.h"

class CollisionSystemTest : public ::testing::Test {
protected:
    CollisionSystem collisionSystem;
};

// Test 1: Basic AABB collision detection
TEST_F(CollisionSystemTest, DetectAABBCollision) {
    // Create two axis-aligned bounding boxes
    AABB box1{glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f)};  // position (0,0), size (10,10)
    AABB box2{glm::vec2(5.0f, 5.0f), glm::vec2(10.0f, 10.0f)};  // position (5,5), size (10,10)
    
    // They should collide (overlapping)
    EXPECT_TRUE(collisionSystem.checkAABBCollision(box1, box2));
}

// Test 2: No collision when boxes are separate
TEST_F(CollisionSystemTest, NoCollisionWhenSeparate) {
    AABB box1{glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f)};   // position (0,0), size (10,10)
    AABB box2{glm::vec2(20.0f, 20.0f), glm::vec2(10.0f, 10.0f)}; // position (20,20), size (10,10)
    
    // They should not collide (too far apart)
    EXPECT_FALSE(collisionSystem.checkAABBCollision(box1, box2));
}

// Test 3: Edge-touching boxes should collide
TEST_F(CollisionSystemTest, EdgeTouchingCollision) {
    AABB box1{glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f)};   // position (0,0), size (10,10)
    AABB box2{glm::vec2(10.0f, 0.0f), glm::vec2(10.0f, 10.0f)};  // position (10,0), size (10,10)
    
    // Touching edges should be considered collision in Diablo II style
    EXPECT_TRUE(collisionSystem.checkAABBCollision(box1, box2));
}

// Test 4: Circle collision detection
TEST_F(CollisionSystemTest, DetectCircleCollision) {
    Circle circle1{glm::vec2(0.0f, 0.0f), 5.0f};   // center (0,0), radius 5
    Circle circle2{glm::vec2(8.0f, 0.0f), 5.0f};   // center (8,0), radius 5
    
    // Distance between centers is 8, sum of radii is 10, so they collide
    EXPECT_TRUE(collisionSystem.checkCircleCollision(circle1, circle2));
}

// Test 5: No circle collision when too far
TEST_F(CollisionSystemTest, NoCircleCollisionWhenDistant) {
    Circle circle1{glm::vec2(0.0f, 0.0f), 5.0f};    // center (0,0), radius 5
    Circle circle2{glm::vec2(15.0f, 0.0f), 5.0f};   // center (15,0), radius 5
    
    // Distance between centers is 15, sum of radii is 10, so no collision
    EXPECT_FALSE(collisionSystem.checkCircleCollision(circle1, circle2));
}

// Test 6: AABB vs Circle collision
TEST_F(CollisionSystemTest, DetectAABBCircleCollision) {
    AABB box{glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f)};  // position (0,0), size (10,10)
    Circle circle{glm::vec2(15.0f, 5.0f), 6.0f};               // center (15,5), radius 6
    
    // Circle overlaps with box edge
    EXPECT_TRUE(collisionSystem.checkAABBCircleCollision(box, circle));
}

// Test 7: Collision with world boundaries
TEST_F(CollisionSystemTest, CollisionWithWorldBounds) {
    // Set world bounds (0,0) to (100,100)
    collisionSystem.setWorldBounds(glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 100.0f));
    
    // Entity trying to go outside bounds
    AABB entity{glm::vec2(-5.0f, 50.0f), glm::vec2(10.0f, 10.0f)};  // Partially outside left edge
    
    EXPECT_TRUE(collisionSystem.isOutOfBounds(entity));
}

// Test 8: No collision with world bounds when inside
TEST_F(CollisionSystemTest, NoCollisionWhenInsideBounds) {
    collisionSystem.setWorldBounds(glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 100.0f));
    
    AABB entity{glm::vec2(45.0f, 45.0f), glm::vec2(10.0f, 10.0f)};  // Fully inside bounds
    
    EXPECT_FALSE(collisionSystem.isOutOfBounds(entity));
}

// Test 9: Collision detection with tile-based map obstacles
TEST_F(CollisionSystemTest, CollisionWithMapTiles) {
    // Create a simple collision map (1 = wall, 0 = walkable)
    std::vector<std::vector<int>> collisionMap = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    
    collisionSystem.setCollisionMap(collisionMap, 32.0f);  // 32 pixels per tile
    
    // Entity at position that would overlap with wall tile
    AABB entity{glm::vec2(32.0f, 32.0f), glm::vec2(20.0f, 20.0f)};  // Position overlaps with tile (1,1) which is walkable
    EXPECT_FALSE(collisionSystem.checkMapCollision(entity));
    
    // Entity overlapping with wall
    AABB wallEntity{glm::vec2(60.0f, 60.0f), glm::vec2(20.0f, 20.0f)};  // Overlaps with wall at (2,2)
    EXPECT_TRUE(collisionSystem.checkMapCollision(wallEntity));
}

// Test 10: Get penetration depth for collision resolution
TEST_F(CollisionSystemTest, CalculatePenetrationDepth) {
    AABB box1{glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f)};
    AABB box2{glm::vec2(8.0f, 0.0f), glm::vec2(10.0f, 10.0f)};
    
    glm::vec2 penetration = collisionSystem.getPenetrationVector(box1, box2);
    
    // Box2 overlaps box1 by 2 units on the X axis
    EXPECT_FLOAT_EQ(penetration.x, -2.0f);
    EXPECT_FLOAT_EQ(penetration.y, 0.0f);
}