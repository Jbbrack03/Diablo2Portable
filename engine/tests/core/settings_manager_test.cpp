#include <gtest/gtest.h>
#include "core/settings_manager.h"
#include <filesystem>

using namespace d2;

class SettingsManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use a temporary test file
        testFilePath = "test_settings.json";
    }

    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testFilePath)) {
            std::filesystem::remove(testFilePath);
        }
    }

    std::string testFilePath;
};

// Test 1: Create and initialize settings manager
TEST_F(SettingsManagerTest, CreateSettingsManager) {
    SettingsManager settings;
    
    // Should have default values
    EXPECT_FLOAT_EQ(settings.getMasterVolume(), 1.0f);
    EXPECT_FLOAT_EQ(settings.getSoundEffectVolume(), 1.0f);
    EXPECT_FLOAT_EQ(settings.getMusicVolume(), 1.0f);
}