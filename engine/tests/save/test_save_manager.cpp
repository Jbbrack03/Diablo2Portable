#include <gtest/gtest.h>
#include "game/save_manager.h"
#include "game/character.h"
#include <filesystem>

using namespace d2::game;
namespace fs = std::filesystem;

// Phase 20: Save System - Following strict TDD
// TEST 1: Can we create a SaveManager?
TEST(SaveManagerTest, CreateSaveManager) {
    SaveManager saveManager;
    // A save manager should exist - that's all we test for now
}

// TEST 2: Can we save a character to a file?
TEST(SaveManagerTest, SaveCharacterCreatesFile) {
    SaveManager saveManager;
    Character character(CharacterClass::BARBARIAN);
    
    std::string filename = "test_character.d2s";
    
    // Clean up any existing file
    if (fs::exists(filename)) {
        fs::remove(filename);
    }
    
    // Save should succeed
    bool result = saveManager.saveCharacter(character, filename);
    EXPECT_TRUE(result);
    
    // File should exist
    EXPECT_TRUE(fs::exists(filename));
    
    // Clean up
    if (fs::exists(filename)) {
        fs::remove(filename);
    }
}