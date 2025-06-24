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

// Test 2: Save and load settings
TEST_F(SettingsManagerTest, SaveAndLoadSettings) {
    SettingsManager settings;
    
    // Change some settings
    settings.setMasterVolume(0.7f);
    settings.setSoundEffectVolume(0.5f);
    settings.setMusicVolume(0.3f);
    
    // Save to file
    EXPECT_TRUE(settings.save(testFilePath));
    
    // Create new settings manager and load
    SettingsManager loadedSettings;
    EXPECT_TRUE(loadedSettings.load(testFilePath));
    
    // Verify loaded values
    EXPECT_FLOAT_EQ(loadedSettings.getMasterVolume(), 0.7f);
    EXPECT_FLOAT_EQ(loadedSettings.getSoundEffectVolume(), 0.5f);
    EXPECT_FLOAT_EQ(loadedSettings.getMusicVolume(), 0.3f);
}

// Test 3: Video settings
TEST_F(SettingsManagerTest, VideoSettings) {
    SettingsManager settings;
    
    // Default video settings
    EXPECT_EQ(settings.getResolutionWidth(), 1280);
    EXPECT_EQ(settings.getResolutionHeight(), 720);
    EXPECT_EQ(settings.getGraphicsQuality(), SettingsManager::GraphicsQuality::MEDIUM);
    EXPECT_TRUE(settings.isFullscreen());
    
    // Change video settings
    settings.setResolution(1920, 1080);
    settings.setGraphicsQuality(SettingsManager::GraphicsQuality::HIGH);
    settings.setFullscreen(false);
    
    // Verify changes
    EXPECT_EQ(settings.getResolutionWidth(), 1920);
    EXPECT_EQ(settings.getResolutionHeight(), 1080);
    EXPECT_EQ(settings.getGraphicsQuality(), SettingsManager::GraphicsQuality::HIGH);
    EXPECT_FALSE(settings.isFullscreen());
}

// Test 4: Control settings
TEST_F(SettingsManagerTest, ControlSettings) {
    SettingsManager settings;
    
    // Default control settings
    EXPECT_FLOAT_EQ(settings.getGamepadSensitivity(), 1.0f);
    EXPECT_FLOAT_EQ(settings.getGamepadDeadzone(), 0.2f);
    EXPECT_TRUE(settings.isVibrationEnabled());
    
    // Change control settings
    settings.setGamepadSensitivity(1.5f);
    settings.setGamepadDeadzone(0.15f);
    settings.setVibrationEnabled(false);
    
    // Verify changes
    EXPECT_FLOAT_EQ(settings.getGamepadSensitivity(), 1.5f);
    EXPECT_FLOAT_EQ(settings.getGamepadDeadzone(), 0.15f);
    EXPECT_FALSE(settings.isVibrationEnabled());
}

// Test 5: Save and load all settings
TEST_F(SettingsManagerTest, SaveAndLoadAllSettings) {
    SettingsManager settings;
    
    // Set all settings
    settings.setMasterVolume(0.8f);
    settings.setSoundEffectVolume(0.6f);
    settings.setMusicVolume(0.4f);
    settings.setResolution(1920, 1080);
    settings.setGraphicsQuality(SettingsManager::GraphicsQuality::HIGH);
    settings.setFullscreen(false);
    settings.setGamepadSensitivity(1.2f);
    settings.setGamepadDeadzone(0.25f);
    settings.setVibrationEnabled(false);
    
    // Save settings
    EXPECT_TRUE(settings.save(testFilePath));
    
    // Create new settings and load
    SettingsManager loaded;
    EXPECT_TRUE(loaded.load(testFilePath));
    
    // Verify all settings loaded correctly
    EXPECT_FLOAT_EQ(loaded.getMasterVolume(), 0.8f);
    EXPECT_FLOAT_EQ(loaded.getSoundEffectVolume(), 0.6f);
    EXPECT_FLOAT_EQ(loaded.getMusicVolume(), 0.4f);
    EXPECT_EQ(loaded.getResolutionWidth(), 1920);
    EXPECT_EQ(loaded.getResolutionHeight(), 1080);
    EXPECT_EQ(loaded.getGraphicsQuality(), SettingsManager::GraphicsQuality::HIGH);
    EXPECT_FALSE(loaded.isFullscreen());
    EXPECT_FLOAT_EQ(loaded.getGamepadSensitivity(), 1.2f);
    EXPECT_FLOAT_EQ(loaded.getGamepadDeadzone(), 0.25f);
    EXPECT_FALSE(loaded.isVibrationEnabled());
}