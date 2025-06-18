#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "save/save_manager.h"
#include "game/character.h"

namespace d2::save {

class SaveManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for test saves
        m_testSaveDir = std::filesystem::temp_directory_path() / "d2_test_saves";
        std::filesystem::create_directories(m_testSaveDir);
    }
    
    void TearDown() override {
        // Clean up test directory
        std::filesystem::remove_all(m_testSaveDir);
    }
    
    std::filesystem::path m_testSaveDir;
};

// Test 1: SaveManager basic initialization
TEST_F(SaveManagerTest, BasicInitialization) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Verify save directory is set correctly
    EXPECT_EQ(saveManager.getSaveDirectory(), m_testSaveDir.string());
    
    // Verify save directory exists
    EXPECT_TRUE(std::filesystem::exists(m_testSaveDir));
}

// Test 2: Save character data to D2S format
TEST_F(SaveManagerTest, SaveCharacterToD2S) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create a test character
    d2::game::Character testChar(d2::game::CharacterClass::SORCERESS);
    testChar.setLevel(42);
    
    // Save the character
    std::string saveFileName = "TestSorc.d2s";
    EXPECT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Verify the save file exists
    auto savePath = m_testSaveDir / saveFileName;
    EXPECT_TRUE(std::filesystem::exists(savePath));
    
    // Verify basic D2S file structure
    std::ifstream file(savePath, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    
    // D2S files start with a 4-byte signature (0xAA55AA55)
    uint32_t signature;
    file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
    EXPECT_EQ(signature, 0xAA55AA55);
}

// Test 3: Load character data from D2S format
TEST_F(SaveManagerTest, LoadCharacterFromD2S) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // First save a character
    d2::game::Character originalChar(d2::game::CharacterClass::BARBARIAN);
    originalChar.setLevel(25);
    
    std::string saveFileName = "TestBarb.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(originalChar, saveFileName));
    
    // Now load it back
    auto loadedChar = saveManager.loadCharacter(saveFileName);
    ASSERT_NE(loadedChar, nullptr);
    
    // Verify loaded character data matches
    EXPECT_EQ(loadedChar->getCharacterClass(), d2::game::CharacterClass::BARBARIAN);
    EXPECT_EQ(loadedChar->getLevel(), 25);
}

} // namespace d2::save