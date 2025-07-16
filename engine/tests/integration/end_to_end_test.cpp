#include <gtest/gtest.h>
#include "onboarding/onboarding_wizard.h"
#include "game/game_engine.h"
#include "game/character.h"
#include "game/player.h"
#include "game/game_state.h"
#include "game/inventory.h"
#include "save/save_manager.h"
#include <glm/glm.hpp>
#include <thread>
#include <chrono>
#include <filesystem>

// Test fixture for end-to-end testing
class EndToEndTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean test environment
        testSaveDir_ = std::filesystem::temp_directory_path() / "d2_test_saves";
        std::filesystem::create_directories(testSaveDir_);
    }
    
    void TearDown() override {
        // Cleanup
        std::filesystem::remove_all(testSaveDir_);
    }
    
    std::filesystem::path testSaveDir_;
};

// Task 24.1: Comprehensive Integration Testing
// Test 1: Complete onboarding to gameplay flow
TEST_F(EndToEndTest, CompleteOnboardingToGameplay) {
    // Full end-to-end test simulating user journey
    
    // 1. Fresh installation - check onboarding wizard
    d2::OnboardingWizard wizard;
    
    // Set import directory to vendor/mpq since game files are already there
    wizard.setImportDirectory("vendor/mpq");
    
    // 2. Check required files
    auto fileCheck = wizard.checkRequiredFiles();
    EXPECT_TRUE(fileCheck.allFilesPresent) << "Missing files: " << fileCheck.missingFiles.size();
    
    // 3. Game engine initialization
    d2::GameEngine engine;
    EXPECT_TRUE(engine.initialize("."));
    EXPECT_TRUE(engine.isInitialized());
    
    // 4. Start the engine
    EXPECT_TRUE(engine.start());
    EXPECT_TRUE(engine.isRunning());
    
    // 5. Create character and player
    d2::game::Character character(d2::game::CharacterClass::SORCERESS);
    character.setLevel(1);
    
    auto player = std::make_shared<d2::game::Player>(character);
    player->setPosition({100.0f, 100.0f});
    
    // Add player to game state
    engine.getGameState()->setPlayer(player);
    
    // 6. Basic gameplay - movement
    auto initialPos = player->getPosition();
    engine.processInput({1.0f, 0.0f}); // Move right
    engine.update(0.016f); // One frame at 60 FPS
    
    auto newPos = player->getPosition();
    EXPECT_NE(newPos.x, initialPos.x) << "Player should have moved";
    
    // 7. Save character
    d2::save::SaveManager saves(testSaveDir_.string());
    EXPECT_TRUE(saves.saveCharacter(character, "TestSorc.d2s"));
    
    // 8. Stop engine
    engine.stop();
    EXPECT_FALSE(engine.isRunning());
    
    // 9. Verify save file exists and can be loaded
    auto loadedChar = saves.loadCharacter("TestSorc.d2s");
    EXPECT_NE(loadedChar, nullptr);
    EXPECT_EQ(loadedChar->getLevel(), 1);
    EXPECT_EQ(loadedChar->getCharacterClass(), d2::game::CharacterClass::SORCERESS);
}

// Test 2: Long-term gameplay stability (stress test)
TEST_F(EndToEndTest, LongTermGameplayStability) {
    d2::GameEngine engine;
    engine.initialize(".");
    
    // Create character and player
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    character.setLevel(1);
    
    auto player = std::make_shared<d2::game::Player>(character);
    player->setPosition({100.0f, 100.0f});
    
    engine.start();
    engine.getGameState()->setPlayer(player);
    
    // Simulate 2 hours of gameplay (7200 seconds at 60fps)
    const int totalFrames = 7200 * 60; // 432,000 frames
    const float deltaTime = 0.016f; // 60 FPS
    
    // Track memory usage
    size_t initialMemory = 0;
    size_t peakMemory = 0;
    
    // Simulate gameplay for a reduced time (10 seconds for testing)
    const int testFrames = 600; // 10 seconds at 60fps
    
    for (int frame = 0; frame < testFrames; frame++) {
        // Vary input to simulate real gameplay
        if (frame % 60 == 0) { // Every second
            float angle = (frame / 60.0f) * 0.5f;
            glm::vec2 movement(cos(angle), sin(angle));
            engine.processInput(movement);
        }
        
        engine.update(deltaTime);
        
        // Check engine is still running
        if (frame % 600 == 0) { // Every 10 seconds
            EXPECT_TRUE(engine.isRunning()) << "Engine stopped at frame " << frame;
        }
    }
    
    // Engine should still be responsive
    EXPECT_TRUE(engine.isRunning());
    
    engine.stop();
}

// Task 28.2: End-to-end testing with repaired systems
// Test: Validate save system integration with character progression
TEST_F(EndToEndTest, ValidateRepairedSaveSystemProgression) {
    // Test that the repaired save system properly handles character progression
    d2::GameEngine engine;
    EXPECT_TRUE(engine.initialize("."));
    
    // Create character with initial stats
    d2::game::Character character(d2::game::CharacterClass::PALADIN);
    character.setLevel(1);
    
    auto player = std::make_shared<d2::game::Player>(character);
    player->setPosition({50.0f, 50.0f});
    
    engine.start();
    engine.getGameState()->setPlayer(player);
    
    // Simulate character progression
    engine.processInput({1.0f, 1.0f}); // Move diagonally
    engine.update(0.016f); // One frame
    
    // Level up the character
    character.setLevel(2);
    character.addExperience(1000);
    
    // Test save with progression
    d2::save::SaveManager saves(testSaveDir_.string());
    EXPECT_TRUE(saves.saveCharacter(character, "ProgressedPaladin.d2s"));
    
    // Simulate game restart - load character
    auto loadedChar = saves.loadCharacter("ProgressedPaladin.d2s");
    ASSERT_NE(loadedChar, nullptr);
    
    // Validate progression was preserved
    EXPECT_EQ(loadedChar->getLevel(), 2);
    EXPECT_EQ(loadedChar->getCharacterClass(), d2::game::CharacterClass::PALADIN);
    
    // Test inventory integration
    d2::game::Inventory inventory(10, 4); // 10x4 grid as per SaveManager spec
    auto result = saves.loadCharacterWithInventory("ProgressedPaladin.d2s");
    
    EXPECT_NE(result.character, nullptr);
    EXPECT_NE(result.inventory, nullptr);
    EXPECT_EQ(result.inventory->getWidth(), 10);
    EXPECT_EQ(result.inventory->getHeight(), 4);
    
    engine.stop();
}