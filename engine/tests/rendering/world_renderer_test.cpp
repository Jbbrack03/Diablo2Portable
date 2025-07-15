#include <gtest/gtest.h>
#include <memory>
#include "rendering/world_renderer.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"
#include "game/monster.h"
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/camera.h"
#include "rendering/sprite_animation.h"
#include "map/map_loader.h"
#include "core/asset_manager.h"

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

// Test 4: WorldRenderer should use actual sprite textures from AssetManager
TEST_F(WorldRendererTest, UseActualSprites) {
    // Set up asset manager with texture IDs
    d2portable::core::AssetManager assetManager;
    assetManager.initialize("test_assets");
    
    // Register some test textures
    const uint32_t PLAYER_TEXTURE_ID = 100;
    const uint32_t TILE_TEXTURE_ID = 200;
    
    // Initialize world renderer with asset manager
    worldRenderer->initialize(assetManager);
    
    // Create player
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(100.0f, 100.0f));
    gameState->setPlayer(player);
    
    // Create map
    MapLoader loader;
    auto map = loader.loadMap("test_map.ds1");
    gameState->setMap(std::move(map));
    
    // Render the world
    worldRenderer->render(*gameState, *testSpriteRenderer);
    
    // Verify proper texture IDs are used (not placeholder IDs)
    bool foundPlayerSprite = false;
    bool foundTileSprite = false;
    
    for (const auto& drawCall : testSpriteRenderer->drawCalls) {
        if (drawCall.texture_id >= 100) { // Real texture IDs, not placeholders
            if (drawCall.size.x == 64.0f && drawCall.size.y == 64.0f) {
                foundPlayerSprite = true;
            } else if (drawCall.size.x == 32.0f && drawCall.size.y == 32.0f) {
                foundTileSprite = true;
            }
        }
    }
    
    EXPECT_TRUE(foundPlayerSprite) << "Player should use real sprite texture";
    EXPECT_TRUE(foundTileSprite) << "Tiles should use real sprite textures";
}

// Test 5: WorldRenderer should render HUD elements
TEST_F(WorldRendererTest, RenderHUD) {
    // Create player with specific stats
    Character character(CharacterClass::SORCERESS);
    character.setLevel(10);
    character.setStat(StatType::VITALITY, 50); // Will affect life
    character.setStat(StatType::ENERGY, 40);   // Will affect mana
    
    auto player = std::make_shared<Player>(character);
    gameState->setPlayer(player);
    
    // Enable HUD rendering
    worldRenderer->setHUDEnabled(true);
    
    // Render the world
    worldRenderer->render(*gameState, *testSpriteRenderer);
    
    // Should have rendered HUD elements in addition to player
    // Expect at least 2 draw calls for health/mana orbs or bars
    EXPECT_GE(testSpriteRenderer->drawCalls.size(), 3u); // Player + at least 2 HUD elements
    
    // Check for HUD-specific draw calls (different sizes/positions)
    int hudElements = 0;
    for (const auto& drawCall : testSpriteRenderer->drawCalls) {
        // HUD elements are typically at fixed screen positions
        if (drawCall.position.y >= 500.0f) { // Bottom of screen
            hudElements++;
        }
    }
    
    EXPECT_GE(hudElements, 2) << "Should render at least health and mana HUD elements";
}

// Test 6: WorldRenderer should render monsters
TEST_F(WorldRendererTest, RenderMonsters) {
    // Create a player
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(50.0f, 50.0f));
    gameState->setPlayer(player);
    
    // Add some monsters to the game state
    auto skeleton = std::make_shared<Monster>(MonsterType::SKELETON, 5);
    skeleton->setPosition(100, 100);
    
    auto zombie = std::make_shared<Monster>(MonsterType::ZOMBIE, 7);
    zombie->setPosition(200, 150);
    
    auto demon = std::make_shared<Monster>(MonsterType::DEMON, 10);
    demon->setPosition(300, 200);
    
    gameState->addMonster(skeleton);
    gameState->addMonster(zombie);
    gameState->addMonster(demon);
    
    // Render the world
    worldRenderer->render(*gameState, *testSpriteRenderer);
    
    // Verify sprite renderer was called correctly
    EXPECT_TRUE(testSpriteRenderer->beginFrameCalled);
    EXPECT_TRUE(testSpriteRenderer->endFrameCalled);
    
    // Should have rendered player + 3 monsters = 4 entities
    EXPECT_EQ(testSpriteRenderer->drawCalls.size(), 4u);
    
    // Verify correct positions (player first, then monsters)
    if (testSpriteRenderer->drawCalls.size() >= 4) {
        // Player at (50, 50)
        EXPECT_FLOAT_EQ(testSpriteRenderer->drawCalls[0].position.x, 50.0f);
        EXPECT_FLOAT_EQ(testSpriteRenderer->drawCalls[0].position.y, 50.0f);
        
        // Check that monsters are rendered (order may vary)
        bool foundSkeleton = false;
        bool foundZombie = false;
        bool foundDemon = false;
        
        for (size_t i = 1; i < testSpriteRenderer->drawCalls.size(); ++i) {
            const auto& pos = testSpriteRenderer->drawCalls[i].position;
            if (pos.x == 100.0f && pos.y == 100.0f) foundSkeleton = true;
            if (pos.x == 200.0f && pos.y == 150.0f) foundZombie = true;
            if (pos.x == 300.0f && pos.y == 200.0f) foundDemon = true;
        }
        
        EXPECT_TRUE(foundSkeleton) << "Skeleton should be rendered at (100, 100)";
        EXPECT_TRUE(foundZombie) << "Zombie should be rendered at (200, 150)";
        EXPECT_TRUE(foundDemon) << "Demon should be rendered at (300, 200)";
    }
}

TEST_F(WorldRendererTest, RenderAnimatedEntities) {
    // Create game state with animated player
    GameState gameState;
    
    // Create player with character
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(100.0f, 150.0f));
    
    // Add animated player to game state
    gameState.setPlayer(player);
    
    // Create a test sprite renderer
    auto testSpriteRenderer = std::make_unique<TestSpriteRenderer>();
    
    // Create world renderer with animation support
    WorldRenderer worldRenderer;
    
    // Initialize with asset manager (needed for animation logic)
    d2portable::core::AssetManager assetManager;
    assetManager.initialize("./test_assets");
    worldRenderer.initialize(assetManager);
    
    // Create a test animation for the player
    SpriteAnimation playerAnimation("player_walk");
    playerAnimation.setFrameCount(8);  // 8-frame walk cycle
    playerAnimation.setFrameRate(10.0f);  // 10 FPS
    playerAnimation.setDirection(0);   // Facing south
    playerAnimation.play();
    
    // Advance animation to frame 2
    playerAnimation.update(0.2f);  // 2 frames at 10 FPS
    EXPECT_EQ(playerAnimation.getCurrentFrame(), 2);
    
    // Set the animation for the player entity in the world renderer
    worldRenderer.setEntityAnimation(player->getId(), playerAnimation);
    
    // Render the world
    worldRenderer.render(gameState, *testSpriteRenderer);
    
    // Verify rendering was called
    EXPECT_TRUE(testSpriteRenderer->beginFrameCalled);
    EXPECT_TRUE(testSpriteRenderer->endFrameCalled);
    
    // Should have rendered the animated player
    EXPECT_EQ(testSpriteRenderer->drawCalls.size(), 1u);
    
    // Verify player position and animated texture
    if (testSpriteRenderer->drawCalls.size() >= 1) {
        EXPECT_FLOAT_EQ(testSpriteRenderer->drawCalls[0].position.x, 100.0f);
        EXPECT_FLOAT_EQ(testSpriteRenderer->drawCalls[0].position.y, 150.0f);
        
        // Should use dynamically loaded sprite texture ID
        // With the new sprite loading system, barbarian_walk gets ID 1000+
        EXPECT_GE(testSpriteRenderer->drawCalls[0].texture_id, 1000u);
        
        // Also verify the texture was cached
        EXPECT_TRUE(worldRenderer.hasLoadedSprite("barbarian_walk"));
    }
}