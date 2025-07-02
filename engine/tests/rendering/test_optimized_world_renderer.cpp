#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/optimized_world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "rendering/camera.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/character.h"
#include "core/asset_manager.h"

using namespace d2::rendering;
using namespace d2::game;
using namespace d2portable::core;
using ::testing::_;

// Mock sprite renderer for testing
class MockSpriteRenderer : public SpriteRenderer {
public:
    MOCK_METHOD(void, beginFrame, (), (override));
    MOCK_METHOD(void, endFrame, (), (override));
    MOCK_METHOD(void, drawSprite, (uint32_t textureId, const glm::vec2& position, const glm::vec2& size), (override));
};

// Unit tests for OptimizedWorldRenderer class
// Following strict TDD principles - one test at a time

// TEST 1: Create optimized world renderer
TEST(OptimizedWorldRendererTest, CreateOptimizedWorldRenderer) {
    OptimizedWorldRenderer renderer;
    // Renderer should exist and be in initial state
    EXPECT_TRUE(renderer.areOptimizationsEnabled());
    EXPECT_EQ(renderer.getRenderedEntityCount(), 0);
    EXPECT_EQ(renderer.getCulledEntityCount(), 0);
}

// TEST 2: Configure optimization settings
TEST(OptimizedWorldRendererTest, ConfigureOptimizationSettings) {
    OptimizedWorldRenderer renderer;
    
    // Disable optimizations
    renderer.setOptimizationsEnabled(false);
    EXPECT_FALSE(renderer.areOptimizationsEnabled());
    
    // Enable optimizations
    renderer.setOptimizationsEnabled(true);
    EXPECT_TRUE(renderer.areOptimizationsEnabled());
}

// TEST 3: Render empty game state
TEST(OptimizedWorldRendererTest, RenderEmptyGameState) {
    OptimizedWorldRenderer renderer;
    GameState gameState;
    MockSpriteRenderer mockSpriteRenderer;
    
    // Expect beginFrame and endFrame to be called
    EXPECT_CALL(mockSpriteRenderer, beginFrame())
        .Times(1);
    EXPECT_CALL(mockSpriteRenderer, endFrame())
        .Times(1);
    
    // Should handle empty game state without crashing
    renderer.render(gameState, mockSpriteRenderer);
    
    // No entities should be rendered
    EXPECT_EQ(renderer.getRenderedEntityCount(), 0);
}

// TEST 4: Render with camera
TEST(OptimizedWorldRendererTest, RenderWithCamera) {
    OptimizedWorldRenderer renderer;
    GameState gameState;
    MockSpriteRenderer mockSpriteRenderer;
    Camera camera(800, 600);
    
    // Expect beginFrame and endFrame to be called
    EXPECT_CALL(mockSpriteRenderer, beginFrame())
        .Times(1);
    EXPECT_CALL(mockSpriteRenderer, endFrame())
        .Times(1);
    
    // Should handle camera rendering without crashing
    renderer.renderWithCamera(gameState, mockSpriteRenderer, camera);
    
    // No entities should be rendered in empty state
    EXPECT_EQ(renderer.getRenderedEntityCount(), 0);
}

// TEST 5: Test optimizations enabled vs disabled
TEST(OptimizedWorldRendererTest, OptimizationsEnabledVsDisabled) {
    OptimizedWorldRenderer renderer;
    GameState gameState;
    MockSpriteRenderer mockSpriteRenderer;
    
    // Create a player for testing
    auto character = Character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(100, 100));
    gameState.setPlayer(player);
    
    // Test with optimizations enabled
    renderer.setOptimizationsEnabled(true);
    EXPECT_CALL(mockSpriteRenderer, beginFrame()).Times(1);
    EXPECT_CALL(mockSpriteRenderer, endFrame()).Times(1);
    EXPECT_CALL(mockSpriteRenderer, drawSprite(_, _, _)).Times(1); // Player sprite
    
    renderer.render(gameState, mockSpriteRenderer);
    EXPECT_EQ(renderer.getRenderedEntityCount(), 1);
    
    // Reset mock
    ::testing::Mock::VerifyAndClearExpectations(&mockSpriteRenderer);
    
    // Test with optimizations disabled (falls back to base class)
    renderer.setOptimizationsEnabled(false);
    // Base class will handle rendering, but we can't easily test that here
    // So we just verify it doesn't crash
    EXPECT_CALL(mockSpriteRenderer, beginFrame()).Times(1);
    EXPECT_CALL(mockSpriteRenderer, endFrame()).Times(1);
    EXPECT_CALL(mockSpriteRenderer, drawSprite(_, _, _)).Times(1); // Player sprite
    
    renderer.render(gameState, mockSpriteRenderer);
}