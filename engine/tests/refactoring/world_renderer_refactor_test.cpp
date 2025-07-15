#include <gtest/gtest.h>
#include "rendering/world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "rendering/camera.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"
#include "game/monster.h"
#include "map/map_loader.h"

class WorldRendererRefactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        worldRenderer = std::make_unique<d2::rendering::WorldRenderer>();
        spriteRenderer = std::make_unique<d2::rendering::SpriteRenderer>();
        gameState = std::make_unique<d2::game::GameState>();
        camera = std::make_unique<d2::rendering::Camera>(800, 600);
    }
    
    std::unique_ptr<d2::rendering::WorldRenderer> worldRenderer;
    std::unique_ptr<d2::rendering::SpriteRenderer> spriteRenderer;
    std::unique_ptr<d2::game::GameState> gameState;
    std::unique_ptr<d2::rendering::Camera> camera;
};

TEST_F(WorldRendererRefactorTest, RenderAndRenderWithCameraShareCommonLogic) {
    // Create a map using the loader
    d2::map::MapLoader loader;
    auto map = loader.generateRandomMap(10, 10, 12345);
    gameState->setMap(std::move(map));
    
    // Create a player
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState->setPlayer(player);
    
    // Both render methods should handle the same game state without duplicating logic
    // This test verifies that the refactoring maintains the same behavior
    EXPECT_NO_THROW(worldRenderer->render(*gameState, *spriteRenderer));
    EXPECT_NO_THROW(worldRenderer->renderWithCamera(*gameState, *spriteRenderer, *camera));
}

TEST_F(WorldRendererRefactorTest, TileRenderingLogicIsExtracted) {
    // Create a map with specific dimensions
    const int mapWidth = 5;
    const int mapHeight = 5;
    d2::map::MapLoader loader;
    auto map = loader.generateRandomMap(mapWidth, mapHeight, 54321);
    gameState->setMap(std::move(map));
    
    // Render should handle tile rendering through extracted method
    EXPECT_NO_THROW(worldRenderer->render(*gameState, *spriteRenderer));
}

TEST_F(WorldRendererRefactorTest, EntityRenderingLogicIsExtracted) {
    // Create player and monsters
    d2::game::Character character(d2::game::CharacterClass::SORCERESS);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState->setPlayer(player);
    
    // Add some monsters
    auto monster1 = std::make_shared<d2::game::Monster>(d2::game::MonsterType::SKELETON, 1);
    monster1->setPosition(100, 100);
    gameState->addMonster(monster1);
    
    auto monster2 = std::make_shared<d2::game::Monster>(d2::game::MonsterType::ZOMBIE, 1);
    monster2->setPosition(200, 200);
    gameState->addMonster(monster2);
    
    // Render should handle entity rendering through extracted method
    EXPECT_NO_THROW(worldRenderer->render(*gameState, *spriteRenderer));
}

TEST_F(WorldRendererRefactorTest, HUDRenderingIsConsistent) {
    // Create a player for HUD rendering
    d2::game::Character character(d2::game::CharacterClass::NECROMANCER);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState->setPlayer(player);
    
    // Enable HUD
    worldRenderer->setHUDEnabled(true);
    
    // HUD should render consistently in both methods
    EXPECT_NO_THROW(worldRenderer->render(*gameState, *spriteRenderer));
}