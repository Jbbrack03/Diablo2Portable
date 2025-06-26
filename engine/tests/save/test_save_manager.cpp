#include <gtest/gtest.h>
#include "game/save_manager.h"
#include "game/character.h"
#include <filesystem>
#include <fstream>

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

// TEST 3: Saved file should have D2S signature
TEST(SaveManagerTest, SavedFileHasD2SSignature) {
    SaveManager saveManager;
    Character character(CharacterClass::SORCERESS);
    
    std::string filename = "test_signature.d2s";
    
    // Save character
    saveManager.saveCharacter(character, filename);
    
    // Read file and check signature
    std::ifstream file(filename, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    
    uint32_t signature;
    file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
    
    // D2S files must start with 0xAA55AA55
    EXPECT_EQ(signature, 0xAA55AA55);
    
    file.close();
    
    // Clean up
    fs::remove(filename);
}