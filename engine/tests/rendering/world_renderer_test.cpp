#include <gtest/gtest.h>
#include <memory>
#include "rendering/world_renderer.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/camera.h"
#include "map/map_loader.h"

using namespace d2::rendering;
using namespace d2::game;
using namespace d2::map;

// Test sprite renderer that tracks calls
class TestSpriteRenderer : public SpriteRenderer {
public:
    struct DrawCall {
        uint32_t texture_id;
        glm::vec2 position;
        glm::vec2 size;
    };
    
    void beginFrame() override {
        SpriteRenderer::beginFrame();
        beginFrameCalled = true;
        drawCalls.clear();
    }
    
    void drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size) override {
        SpriteRenderer::drawSprite(texture_id, position, size);
        drawCalls.push_back({texture_id, position, size});
    }
    
    void endFrame() override {
        SpriteRenderer::endFrame();
        endFrameCalled = true;
    }
    
    bool beginFrameCalled = false;
    bool endFrameCalled = false;
    std::vector<DrawCall> drawCalls;
};

class WorldRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        worldRenderer = std::make_unique<WorldRenderer>();
        testSpriteRenderer = std::make_unique<TestSpriteRenderer>();
        gameState = std::make_unique<GameState>();
        
        // Initialize the test sprite renderer
        Renderer renderer;
        TextureManager textureManager;
        testSpriteRenderer->initialize(renderer, textureManager);
    }
    
    std::unique_ptr<WorldRenderer> worldRenderer;
    std::unique_ptr<TestSpriteRenderer> testSpriteRenderer;
    std::unique_ptr<GameState> gameState;
};

// Test 1: WorldRenderer should render player in world
TEST_F(WorldRendererTest, RenderPlayerInWorld) {
    // Create a player
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(100.0f, 100.0f));
    gameState->setPlayer(player);
    
    // Render the world
    worldRenderer->render(*gameState, *testSpriteRenderer);
    
    // Verify sprite renderer was called correctly
    EXPECT_TRUE(testSpriteRenderer->beginFrameCalled);
    EXPECT_TRUE(testSpriteRenderer->endFrameCalled);
    EXPECT_EQ(testSpriteRenderer->drawCalls.size(), 1u);
    
    // Verify player was drawn at correct position
    if (!testSpriteRenderer->drawCalls.empty()) {
        const auto& drawCall = testSpriteRenderer->drawCalls[0];
        EXPECT_FLOAT_EQ(drawCall.position.x, 100.0f);
        EXPECT_FLOAT_EQ(drawCall.position.y, 100.0f);
    }
}

// Test 2: WorldRenderer should render map tiles
TEST_F(WorldRendererTest, RenderMapTiles) {
    // Create a simple map using MapLoader
    MapLoader loader;
    auto map = loader.loadMap("test_map.ds1");
    ASSERT_NE(map, nullptr);
    
    // Add map to game state
    gameState->setMap(std::move(map));
    
    // Render the world
    worldRenderer->render(*gameState, *testSpriteRenderer);
    
    // Verify sprite renderer was called correctly
    EXPECT_TRUE(testSpriteRenderer->beginFrameCalled);
    EXPECT_TRUE(testSpriteRenderer->endFrameCalled);
    
    // Should render map tiles (10x10 = 100 tiles from default test map)
    EXPECT_GE(testSpriteRenderer->drawCalls.size(), 100u);
}

// Test 3: WorldRenderer should only render visible tiles (viewport culling)
TEST_F(WorldRendererTest, ViewportCulling) {
    // Create a large map
    MapLoader loader;
    auto map = loader.loadMap("large_map.ds1"); // This should create a larger map
    ASSERT_NE(map, nullptr);
    
    // Add map to game state
    gameState->setMap(std::move(map));
    
    // Set up a camera with limited viewport
    Camera camera(800, 600); // 800x600 viewport
    
    // Render with camera viewport
    worldRenderer->renderWithCamera(*gameState, *testSpriteRenderer, camera);
    
    // Should only render visible tiles, not all tiles
    // With 32x32 tiles, 800x600 viewport should show about 25x19 = 475 tiles
    // Plus margin tiles, expect around 540 tiles
    EXPECT_LE(testSpriteRenderer->drawCalls.size(), 600u);
    EXPECT_GT(testSpriteRenderer->drawCalls.size(), 400u);
    
    // Verify we're not rendering all tiles (100x100 = 10000)
    EXPECT_LT(testSpriteRenderer->drawCalls.size(), 1000u);
}