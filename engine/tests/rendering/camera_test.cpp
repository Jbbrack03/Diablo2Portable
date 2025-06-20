#include <gtest/gtest.h>
#include <memory>
#include "rendering/camera.h"
#include "game/player.h"
#include "game/character.h"

using namespace d2::rendering;
using namespace d2::game;

class CameraTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Screen size 800x600
        camera = std::make_unique<Camera>(800, 600);
    }
    
    std::unique_ptr<Camera> camera;
};

// Test 1: Camera should follow player
TEST_F(CameraTest, FollowPlayer) {
    // Create a player
    Character character(CharacterClass::AMAZON);
    Player player(character);
    player.setPosition(glm::vec2(1000.0f, 1000.0f));
    
    // Set camera to follow player
    camera->followTarget(&player);
    camera->update();
    
    // Camera center should be at player position
    glm::vec2 center = camera->getCenter();
    EXPECT_FLOAT_EQ(center.x, 1000.0f);
    EXPECT_FLOAT_EQ(center.y, 1000.0f);
}