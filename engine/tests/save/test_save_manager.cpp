#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "save/save_manager.h"

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

} // namespace d2::save