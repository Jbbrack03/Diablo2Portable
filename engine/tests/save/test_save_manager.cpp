#include <gtest/gtest.h>
#include "game/save_manager.h"
#include "game/character.h"
#include "game/character_inventory.h"
#include "game/item.h"
#include <filesystem>
#include <fstream>
#include <thread>

using namespace d2::game;
namespace fs = std::filesystem;

// Phase 20 Task 20.3: Save System Implementation
// Following TDD principles - write failing test first
class SaveManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir = "test_saves/";
        if (!fs::exists(test_dir)) {
            fs::create_directories(test_dir);
        }
    }
    
    void TearDown() override {
        // Clean up test files
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
    }
    
    std::string test_dir;
};

// Test creating a save manager instance
TEST_F(SaveManagerTest, CreateSaveManager) {
    SaveManager saveManager;
    
    // Save manager should be ready to use
    EXPECT_TRUE(saveManager.isReady());
}

// Test saving a character to D2S format
TEST_F(SaveManagerTest, SaveCharacterToD2S) {
    SaveManager saveManager;
    
    // Create a test character
    Character testChar(CharacterClass::SORCERESS);
    testChar.setLevel(42);
    testChar.addStatPoint(StatType::STRENGTH, 20);
    testChar.addStatPoint(StatType::VITALITY, 30);
    
    // Save the character
    std::string savePath = test_dir + "TestSorc.d2s";
    bool result = saveManager.saveCharacter(testChar, savePath);
    
    EXPECT_TRUE(result) << "Failed to save character";
    EXPECT_TRUE(fs::exists(savePath)) << "Save file not created";
    
    // Verify file has proper D2S header
    if (fs::exists(savePath)) {
        std::ifstream file(savePath, std::ios::binary);
        uint32_t signature;
        file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
        
        // D2S files start with 0xAA55AA55
        EXPECT_EQ(signature, 0xAA55AA55) << "Invalid D2S file signature";
    }
}

// Test loading a character from D2S format
TEST_F(SaveManagerTest, LoadCharacterFromD2S) {
    SaveManager saveManager;
    
    // First save a character
    Character original(CharacterClass::BARBARIAN);
    original.setLevel(25);
    original.addExperience(500000);
    
    std::string savePath = test_dir + "TestBarb.d2s";
    saveManager.saveCharacter(original, savePath);
    
    // Now load it back
    auto loaded = saveManager.loadCharacter(savePath);
    
    ASSERT_NE(loaded, nullptr) << "Failed to load character";
    EXPECT_EQ(loaded->getLevel(), 25);
    EXPECT_EQ(loaded->getCharacterClass(), CharacterClass::BARBARIAN);
}


// Test save file validation
TEST_F(SaveManagerTest, ValidateSaveFile) {
    SaveManager saveManager;
    
    // Create a valid save
    Character testChar(CharacterClass::AMAZON);
    std::string validPath = test_dir + "valid.d2s";
    saveManager.saveCharacter(testChar, validPath);
    
    // Test validation
    EXPECT_TRUE(saveManager.isValidSaveFile(validPath));
    
    // Create an invalid file
    std::string invalidPath = test_dir + "invalid.d2s";
    std::ofstream invalidFile(invalidPath);
    invalidFile << "This is not a valid D2S file";
    invalidFile.close();
    
    EXPECT_FALSE(saveManager.isValidSaveFile(invalidPath));
    
    // Test non-existent file
    EXPECT_FALSE(saveManager.isValidSaveFile("non_existent.d2s"));
}

// Test checksum calculation for save integrity
TEST_F(SaveManagerTest, SaveFileChecksum) {
    SaveManager saveManager;
    
    // Create and save a character
    Character testChar(CharacterClass::NECROMANCER);
    std::string savePath = test_dir + "checksum.d2s";
    
    saveManager.saveCharacter(testChar, savePath);
    
    // Verify checksum is calculated and stored
    uint32_t checksum = saveManager.calculateChecksum(savePath);
    EXPECT_NE(checksum, 0) << "Checksum should not be zero";
    
    // Verify save has valid checksum
    EXPECT_TRUE(saveManager.verifyChecksum(savePath)) << "Save file checksum verification failed";
    
    // Corrupt the file
    std::fstream file(savePath, std::ios::binary | std::ios::in | std::ios::out);
    file.seekp(100); // Skip header
    char corruption = 0xFF;
    file.write(&corruption, 1);
    file.close();
    
    // Checksum should now fail
    EXPECT_FALSE(saveManager.verifyChecksum(savePath)) << "Corrupted file should fail checksum";
}

