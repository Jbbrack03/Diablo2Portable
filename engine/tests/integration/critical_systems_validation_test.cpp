#include <gtest/gtest.h>
#include "game/game_engine.h"
#include "game/character.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/game_state.h"
#include "game/combat_engine.h"
#include "game/item.h"
#include "game/inventory.h"
#include "save/save_manager.h"
#include "performance/performance_monitor.h"
#include "performance/memory_monitor.h"
#include "rendering/world_renderer.h"
#include "audio/audio_engine.h"
#include "network/network_manager.h"
#include <glm/glm.hpp>
#include <filesystem>

// Task 28.3: Verify all critical systems are functional
class CriticalSystemsValidationTest : public ::testing::Test {
protected:
    void SetUp() override {
        testSaveDir_ = std::filesystem::temp_directory_path() / "d2_critical_test";
        std::filesystem::create_directories(testSaveDir_);
    }
    
    void TearDown() override {
        std::filesystem::remove_all(testSaveDir_);
    }
    
    std::filesystem::path testSaveDir_;
};

// Test: Comprehensive critical systems validation
TEST_F(CriticalSystemsValidationTest, ValidateAllCriticalSystemsWorking) {
    // This test validates that all critical systems mentioned in CLAUDE.md are functional
    
    // System 1: GameEngine - Central game system integration
    d2::GameEngine engine;
    EXPECT_TRUE(engine.initialize("vendor/mpq")) << "GameEngine initialization failed";
    EXPECT_TRUE(engine.start()) << "GameEngine start failed";
    EXPECT_TRUE(engine.isRunning()) << "GameEngine should be running";
    
    // System 2: Character System - Stats, leveling, and progression
    d2::game::Character character(d2::game::CharacterClass::SORCERESS);
    character.setLevel(5);
    character.setStat(d2::game::StatType::STRENGTH, 25);
    character.setStat(d2::game::StatType::VITALITY, 30);
    
    EXPECT_EQ(character.getLevel(), 5);
    EXPECT_EQ(character.getStrength(), 25);
    EXPECT_EQ(character.getVitality(), 30);
    EXPECT_GT(character.getLife(), 0) << "Character should have positive life";
    
    // System 3: Player Entity - Position tracking, movement, character association
    auto player = std::make_shared<d2::game::Player>(character);
    player->setPosition({100.0f, 200.0f});
    glm::vec2 playerPos = player->getPosition();
    EXPECT_EQ(playerPos.x, 100.0f);
    EXPECT_EQ(playerPos.y, 200.0f);
    
    // System 4: GameState Management - Player management, world state tracking
    engine.getGameState()->setPlayer(player);
    auto retrievedPlayer = engine.getGameState()->getPlayer();
    EXPECT_NE(retrievedPlayer, nullptr);
    EXPECT_EQ(retrievedPlayer->getPosition().x, 100.0f);
    
    // System 5: Monster System - AI behaviors and group mechanics
    auto monster = std::make_shared<d2::game::Monster>(d2::game::MonsterType::FALLEN, 3);
    monster->setPosition(150, 250);
    engine.getGameState()->addMonster(monster);
    
    auto monsters = engine.getGameState()->getAllMonsters();
    EXPECT_EQ(monsters.size(), 1);
    EXPECT_EQ(monster->getLevel(), 3);
    
    // System 6: Combat Engine - Damage calculation and resistances
    d2::game::CombatEngine combat;
    int damage = combat.calculateDamage(5, 10, 2); // min, max, elemental
    EXPECT_GT(damage, 0) << "Combat should calculate positive damage";
    
    // System 7: Item System - Complete with affixes and rarity
    auto weapon = std::make_shared<d2::game::Item>("Iron Sword", d2::game::ItemType::WEAPON);
    weapon->setDamage(5, 10);
    weapon->setRarity(d2::game::ItemRarity::NORMAL);
    
    EXPECT_EQ(weapon->getName(), "Iron Sword");
    EXPECT_EQ(weapon->getMinDamage(), 5);
    EXPECT_EQ(weapon->getMaxDamage(), 10);
    EXPECT_EQ(weapon->getRarity(), d2::game::ItemRarity::NORMAL);
    
    // System 8: Inventory System - Grid-based with equipment slots
    d2::game::Inventory inventory(10, 4);
    EXPECT_TRUE(inventory.addItem(weapon, 0, 0));
    EXPECT_EQ(inventory.getItemAt(0, 0), weapon);
    EXPECT_FALSE(inventory.isEmpty());
    
    // System 9: Save/Load System - D2S format, checksum validation, inventory persistence
    d2::save::SaveManager saves(testSaveDir_.string());
    EXPECT_TRUE(saves.saveCharacter(character, "TestSorc.d2s"));
    
    auto loadedChar = saves.loadCharacter("TestSorc.d2s");
    EXPECT_NE(loadedChar, nullptr);
    EXPECT_EQ(loadedChar->getLevel(), 5);
    EXPECT_EQ(loadedChar->getCharacterClass(), d2::game::CharacterClass::SORCERESS);
    
    // System 10: Performance Monitor - Real-time FPS tracking, frame time statistics
    d2::performance::PerformanceMonitor perfMon;
    perfMon.startFrame();
    // Simulate some work
    for (int i = 0; i < 1000; ++i) {
        engine.update(0.001f);
    }
    perfMon.endFrame();
    
    EXPECT_GT(perfMon.getAverageFPS(), 0.0) << "Performance monitor should track FPS";
    EXPECT_GT(perfMon.getAverageFrameTime(), 0.0) << "Performance monitor should track frame time";
    
    // System 11: Memory Monitor - Memory allocation tracking, budget enforcement
    d2::MemoryMonitor memMon;
    memMon.setMemoryBudget(1024 * 1024 * 1024); // 1GB
    EXPECT_TRUE(memMon.tryRecordAllocation("test", 100 * 1024 * 1024)); // 100MB
    EXPECT_TRUE(memMon.isWithinBudget());
    
    // System 12: Input System - Input processing and movement
    engine.processInput({1.0f, 0.0f}); // Move right
    engine.update(0.016f);
    glm::vec2 newPos = engine.getGameState()->getPlayer()->getPosition();
    EXPECT_NE(newPos.x, playerPos.x) << "Player should have moved after input";
    
    // System 13: Audio System - Volume controls, 3D positioning (basic validation)
    d2::audio::AudioEngine audio;
    audio.initialize();
    audio.setMasterVolume(0.8f);
    EXPECT_FLOAT_EQ(audio.getMasterVolume(), 0.8f);
    
    // System 14: Network System - LAN game hosting/joining (basic validation)
    d2::network::NetworkManager network;
    EXPECT_TRUE(network.initialize()) << "Network manager should initialize";
    
    // System 15: World Rendering - Basic validation (requires SpriteRenderer)
    // Skip detailed rendering test - would need full renderer setup
    
    // System 18: Performance targets validation
    EXPECT_GT(perfMon.getAverageFPS(), 30.0) << "Should maintain at least 30 FPS";
    EXPECT_LT(memMon.getCurrentMemoryUsage(), memMon.getMemoryBudget()) << "Should stay within memory budget";
    
    // Cleanup
    engine.stop();
    EXPECT_FALSE(engine.isRunning()) << "Engine should stop cleanly";
}