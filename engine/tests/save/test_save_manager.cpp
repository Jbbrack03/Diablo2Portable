#include <gtest/gtest.h>
#include "save/save_manager.h"
#include "game/character.h"
#include "game/inventory.h"
#include <filesystem>
#include <fstream>

using namespace d2::game;
using namespace d2::save;
namespace fs = std::filesystem;

// Phase 20: Save System - Following strict TDD
// TEST 1: Can we create a SaveManager?
TEST(SaveManagerTest, CreateSaveManager) {
    SaveManager saveManager("test_saves");
    // A save manager should exist - that's all we test for now
}

// TEST 2: Can we save a character to a file?
TEST(SaveManagerTest, SaveCharacterCreatesFile) {
    SaveManager saveManager("test_saves");
    Character character(CharacterClass::BARBARIAN);
    
    std::string filename = "test_character.d2s";
    std::string fullPath = "test_saves/" + filename;
    
    // Clean up any existing file
    if (fs::exists(fullPath)) {
        fs::remove(fullPath);
    }
    
    // Save should succeed
    bool result = saveManager.saveCharacter(character, filename);
    EXPECT_TRUE(result);
    
    // File should exist in the save directory
    EXPECT_TRUE(fs::exists(fullPath));
    
    // Clean up
    if (fs::exists(fullPath)) {
        fs::remove(fullPath);
    }
}

// TEST 3: Saved file should have D2S signature
TEST(SaveManagerTest, SavedFileHasD2SSignature) {
    SaveManager saveManager("test_saves");
    Character character(CharacterClass::SORCERESS);
    
    std::string filename = "test_signature.d2s";
    std::string fullPath = "test_saves/" + filename;
    
    // Save character
    saveManager.saveCharacter(character, filename);
    
    // Read file and check signature
    std::ifstream file(fullPath, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    
    uint32_t signature;
    file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
    
    // D2S files must start with 0xAA55AA55
    EXPECT_EQ(signature, 0xAA55AA55);
    
    file.close();
    
    // Clean up
    fs::remove(fullPath);
}

// TEST 4: Save character level
TEST(SaveManagerTest, SaveCharacterLevel) {
    SaveManager saveManager("test_saves");
    Character character(CharacterClass::NECROMANCER);
    character.setLevel(42);
    
    std::string filename = "test_level.d2s";
    std::string fullPath = "test_saves/" + filename;
    
    // Save character
    saveManager.saveCharacter(character, filename);
    
    // Read file and check level
    std::ifstream file(fullPath, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    
    // Skip signature (4 bytes), version (4 bytes), filesize (4 bytes), 
    // checksum (4 bytes), character name (16 bytes), status (1 byte), 
    // progression (1 byte), unknown (2 bytes), class (1 byte) = 37 bytes
    file.seekg(37);
    
    uint8_t level;
    file.read(reinterpret_cast<char*>(&level), sizeof(level));
    
    EXPECT_EQ(level, 42);
    
    file.close();
    fs::remove(fullPath);
}

// TEST 5: Load character from file
TEST(SaveManagerTest, LoadCharacterFromFile) {
    SaveManager saveManager("test_saves");
    
    // Create and save a character
    Character original(CharacterClass::AMAZON);
    original.setLevel(25);
    
    std::string filename = "test_load.d2s";
    std::string fullPath = "test_saves/" + filename;
    saveManager.saveCharacter(original, filename);
    
    // Load the character
    auto loaded = saveManager.loadCharacter(filename);
    
    // Verify it loaded
    ASSERT_NE(loaded, nullptr);
    
    // Verify the data matches
    EXPECT_EQ(loaded->getCharacterClass(), CharacterClass::AMAZON);
    EXPECT_EQ(loaded->getLevel(), 25);
    
    // Clean up
    fs::remove(fullPath);
}

// TEST 6: Save character with inventory
TEST(SaveManagerTest, SaveCharacterWithInventory) {
    SaveManager saveManager("test_saves");
    Character character(CharacterClass::PALADIN);
    character.setLevel(10);
    
    // Create inventory with 10x4 grid (40 slots)
    Inventory inventory(10, 4);
    
    std::string filename = "test_character_inventory.d2s";
    std::string fullPath = "test_saves/" + filename;
    
    // Clean up any existing file
    if (fs::exists(fullPath)) {
        fs::remove(fullPath);
    }
    
    // Save should succeed
    bool result = saveManager.saveCharacterWithInventory(character, inventory, filename);
    EXPECT_TRUE(result);
    
    // File should exist
    EXPECT_TRUE(fs::exists(fullPath));
    
    // Clean up
    if (fs::exists(fullPath)) {
        fs::remove(fullPath);
    }
}