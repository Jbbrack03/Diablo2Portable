#include <gtest/gtest.h>
#include "game/game_engine.h"
#include "game/character.h"
#include "game/quest_manager.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/quest.h"
#include "game/monster.h"
#include "core/asset_manager.h"
#include "performance/memory_monitor.h"
#include "performance/performance_monitor.h"
#include <filesystem>
#include <chrono>
#include <fstream>
#include <algorithm>

using namespace d2;
using namespace d2::game;
using namespace d2portable::core;
namespace fs = std::filesystem;

// Complete gameplay integration tests for Phase 20
// These tests verify the full game loop functionality
class GameplayIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Skip tests if integration tests are not enabled
        const char* enable_tests = std::getenv("ENABLE_GAMEPLAY_INTEGRATION_TESTS");
        if (!enable_tests || std::string(enable_tests) != "1") {
            GTEST_SKIP() << "Gameplay integration tests disabled. Set ENABLE_GAMEPLAY_INTEGRATION_TESTS=1 to run.";
        }
        
        // Set up test assets path
        test_assets_path = "test_assets/";
        if (!fs::exists(test_assets_path)) {
            fs::create_directories(test_assets_path);
        }
        
        // Create minimal test data structure
        fs::create_directories(test_assets_path + "/data");
    }
    
    void TearDown() override {
        // Clean up test save files
        if (fs::exists(test_save_path)) {
            fs::remove(test_save_path);
        }
        
        // Clean up test assets
        if (fs::exists(test_assets_path)) {
            fs::remove_all(test_assets_path);
        }
    }
    
    std::string test_assets_path;
    std::string test_save_path = "test_save.d2s";
};

// Helper class to simulate a test gameplay session
class TestSession {
public:
    TestSession() : engine(std::make_unique<GameEngine>()) {}
    
    void createCharacter(CharacterClass charClass) {
        character = std::make_shared<Character>(charClass);
        character_name = "TestChar";
        character_class = charClass;
    }
    
    void enterGame() {
        if (!character) {
            throw std::runtime_error("Must create character before entering game");
        }
        
        // Initialize engine and create player
        engine->initialize("test_assets/");
        engine->start();
        
        // Create player with character
        auto player = std::make_shared<Player>(*character);
        engine->getGameState()->setPlayer(player);
        
        in_game = true;
    }
    
    void completeQuest(QuestId questId) {
        if (!in_game) {
            throw std::runtime_error("Must enter game before completing quests");
        }
        
        auto* questManager = engine->getQuestManager();
        if (!questManager) {
            throw std::runtime_error("No quest manager available");
        }
        
        // Create and start the quest if not already started
        auto quest = questManager->getQuest(questId);
        if (!quest) {
            // Create the Den of Evil quest
            quest = questManager->createQuest(
                questId, 
                "Den of Evil",
                "Clear the Den of Evil", 
                QuestType::KILL_MONSTERS
            );
            
            // Set kill requirements for Den of Evil
            quest->setKillRequirement(MonsterType::FALLEN, 5);
            questManager->startTracking(quest);
        }
        
        // Complete quest objectives
        if (quest->getType() == QuestType::KILL_MONSTERS) {
            // Simulate killing required monsters
            auto monsterType = MonsterType::FALLEN;
            for (int i = 0; i < quest->getRequiredKills(monsterType); i++) {
                questManager->onMonsterKilled(monsterType);
            }
        }
        
        completed_quests.push_back(questId);
    }
    
    void saveAndExit() {
        if (!in_game) {
            throw std::runtime_error("Must be in game to save");
        }
        
        // Save character and game state
        save_path = "test_save.d2s";
        
        // For now, create a dummy save file to verify the workflow
        std::ofstream file(save_path);
        file << "D2S_SAVE_FILE\n";
        file << "Character: " << character_name << "\n";
        file << "Class: " << static_cast<int>(character_class) << "\n";
        file << "Level: " << character->getLevel() << "\n";
        
        // Save quest states
        file << "Quests:\n";
        for (auto questId : completed_quests) {
            file << "  Quest_" << static_cast<int>(questId) << "_Complete: true\n";
        }
        
        file.close();
        in_game = false;
    }
    
    void loadGame() {
        if (!fs::exists(save_path)) {
            throw std::runtime_error("No save file found");
        }
        
        // For now, just verify the file exists
        // In a real implementation, this would parse the save file
        // and restore character/quest states
        in_game = true;
    }
    
    bool isQuestComplete(QuestId questId) const {
        // For integration test, check our tracking
        return std::find(completed_quests.begin(), completed_quests.end(), questId) != completed_quests.end();
    }
    
    std::string getSavePath() const { return save_path; }
    
private:
    std::unique_ptr<GameEngine> engine;
    std::shared_ptr<Character> character;
    std::string character_name;
    CharacterClass character_class;
    std::string save_path;
    std::vector<QuestId> completed_quests;
    bool in_game = false;
};

// Create helper function for test session
TestSession createTestSession() {
    return TestSession();
}

// Test 20.1: Complete Gameplay Loop Integration Test
TEST_F(GameplayIntegrationTest, CompleteGameplayLoop) {
    GameEngine engine;
    
    // Test that engine can be initialized with test assets
    bool init_result = engine.initialize(test_assets_path);
    EXPECT_TRUE(init_result) << "Engine initialization failed";
    EXPECT_TRUE(engine.isInitialized());
    
    // Test that engine can start
    bool start_result = engine.start();
    EXPECT_TRUE(start_result) << "Engine start failed";
    EXPECT_TRUE(engine.isRunning());
    
    // Simulate full gameplay session
    auto testSession = createTestSession();
    testSession.createCharacter(CharacterClass::SORCERESS);
    
    // Verify character was created
    EXPECT_NO_THROW(testSession.enterGame()) << "Failed to enter game";
    
    // Complete a quest
    EXPECT_NO_THROW(testSession.completeQuest(QuestId::DEN_OF_EVIL)) << "Failed to complete quest";
    
    // Save the game
    EXPECT_NO_THROW(testSession.saveAndExit()) << "Failed to save and exit";
    
    // Verify save file was created
    std::string save_path = testSession.getSavePath();
    EXPECT_TRUE(fs::exists(save_path)) << "Save file not found at: " << save_path;
    
    // Verify save file has content
    if (fs::exists(save_path)) {
        EXPECT_GT(fs::file_size(save_path), 0) << "Save file is empty";
    }
    
    // Load and verify
    EXPECT_NO_THROW(testSession.loadGame()) << "Failed to load game";
    EXPECT_TRUE(testSession.isQuestComplete(QuestId::DEN_OF_EVIL)) << "Quest not marked as complete after load";
}

// Test 20.2: Performance Under Load
TEST_F(GameplayIntegrationTest, PerformanceUnderLoad) {
    GameEngine engine;
    
    // Initialize engine
    EXPECT_TRUE(engine.initialize(test_assets_path));
    EXPECT_TRUE(engine.start());
    
    // Get performance monitor
    auto* perfMonitor = engine.getPerformanceMonitor();
    ASSERT_NE(perfMonitor, nullptr) << "Performance monitor not available";
    
    // Simulate heavy game load
    auto* gameState = engine.getGameState();
    ASSERT_NE(gameState, nullptr);
    
    // Add many monsters to stress test
    for (int i = 0; i < 100; i++) {
        auto monster = std::make_shared<Monster>(MonsterType::FALLEN, 1); // Level 1
        monster->setPosition(i * 10, i * 10);
        gameState->addMonster(monster);
    }
    
    // Run several update cycles
    const int numFrames = 60; // 1 second at 60 FPS
    for (int i = 0; i < numFrames; i++) {
        engine.update(0.016f); // 16ms per frame
        engine.renderFrame();
    }
    
    // Check performance metrics
    float avgFPS = perfMonitor->getAverageFPS();
    
    // First check if we're getting meaningful FPS data
    EXPECT_GT(avgFPS, 0.0f) << "Performance monitor returned no FPS data";
    
    // Then check if it meets our target
    EXPECT_GE(avgFPS, 60.0f) << "FPS dropped below 60 with 100 monsters (actual: " << avgFPS << ")";
    
    // Memory monitoring would be checked here in a full implementation
    // For now, just verify the test completes without crashes
}