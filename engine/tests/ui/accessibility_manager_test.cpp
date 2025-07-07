#include <gtest/gtest.h>
#include "ui/accessibility_manager.h"

namespace d2 {

class AccessibilityManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<AccessibilityManager>();
    }

    std::unique_ptr<AccessibilityManager> manager;
};

// Test 1: Create AccessibilityManager and get default text scale
TEST_F(AccessibilityManagerTest, CreateManagerWithDefaultTextScale) {
    EXPECT_EQ(manager->getTextScale(), 1.0f);
}

// Test 2: Set and get text scale
TEST_F(AccessibilityManagerTest, SetAndGetTextScale) {
    manager->setTextScale(1.5f);
    EXPECT_EQ(manager->getTextScale(), 1.5f);
    
    manager->setTextScale(2.0f);
    EXPECT_EQ(manager->getTextScale(), 2.0f);
}

// Test 3: Default colorblind mode is None
TEST_F(AccessibilityManagerTest, DefaultColorblindModeIsNone) {
    EXPECT_EQ(manager->getColorblindMode(), ColorblindMode::None);
}

// Test 4: Set and get colorblind mode
TEST_F(AccessibilityManagerTest, SetAndGetColorblindMode) {
    manager->setColorblindMode(ColorblindMode::Protanopia);
    EXPECT_EQ(manager->getColorblindMode(), ColorblindMode::Protanopia);
    
    manager->setColorblindMode(ColorblindMode::Deuteranopia);
    EXPECT_EQ(manager->getColorblindMode(), ColorblindMode::Deuteranopia);
    
    manager->setColorblindMode(ColorblindMode::Tritanopia);
    EXPECT_EQ(manager->getColorblindMode(), ColorblindMode::Tritanopia);
}

// Test 5: Save and load accessibility settings
TEST_F(AccessibilityManagerTest, SaveAndLoadAccessibilitySettings) {
    // Set custom values
    manager->setTextScale(1.5f);
    manager->setColorblindMode(ColorblindMode::Protanopia);
    
    // Save settings
    EXPECT_TRUE(manager->saveSettings("/tmp/test_accessibility.cfg"));
    
    // Create new manager and load settings
    auto newManager = std::make_unique<AccessibilityManager>();
    EXPECT_TRUE(newManager->loadSettings("/tmp/test_accessibility.cfg"));
    
    // Verify loaded values
    EXPECT_EQ(newManager->getTextScale(), 1.5f);
    EXPECT_EQ(newManager->getColorblindMode(), ColorblindMode::Protanopia);
}

} // namespace d2