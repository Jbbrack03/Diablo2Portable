#include <gtest/gtest.h>
#include "ui/input_remapper.h"
#include "android/gamepad_handler.h"

namespace d2 {

class InputRemapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        remapper = std::make_unique<InputRemapper>();
    }

    std::unique_ptr<InputRemapper> remapper;
};

// Test 1: Default mapping returns original button
TEST_F(InputRemapperTest, DefaultMappingReturnsOriginalButton) {
    // By default, each button should map to itself
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::A), GamepadButton::A);
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::B), GamepadButton::B);
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::X), GamepadButton::X);
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::Y), GamepadButton::Y);
}

// Test 2: Remap buttons
TEST_F(InputRemapperTest, RemapButtons) {
    // Swap A and B buttons
    remapper->setButtonMapping(GamepadButton::A, GamepadButton::B);
    remapper->setButtonMapping(GamepadButton::B, GamepadButton::A);
    
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::A), GamepadButton::B);
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::B), GamepadButton::A);
    
    // X and Y should remain unchanged
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::X), GamepadButton::X);
    EXPECT_EQ(remapper->getMappedButton(GamepadButton::Y), GamepadButton::Y);
}

// Test 3: Save and load button mappings
TEST_F(InputRemapperTest, SaveAndLoadMappings) {
    // Set custom mappings
    remapper->setButtonMapping(GamepadButton::X, GamepadButton::Y);
    remapper->setButtonMapping(GamepadButton::Y, GamepadButton::X);
    remapper->setButtonMapping(GamepadButton::START, GamepadButton::SELECT);
    
    // Save mappings
    EXPECT_TRUE(remapper->saveMappings("/tmp/test_button_mappings.cfg"));
    
    // Create new remapper and load mappings
    auto newRemapper = std::make_unique<InputRemapper>();
    EXPECT_TRUE(newRemapper->loadMappings("/tmp/test_button_mappings.cfg"));
    
    // Verify loaded mappings
    EXPECT_EQ(newRemapper->getMappedButton(GamepadButton::X), GamepadButton::Y);
    EXPECT_EQ(newRemapper->getMappedButton(GamepadButton::Y), GamepadButton::X);
    EXPECT_EQ(newRemapper->getMappedButton(GamepadButton::START), GamepadButton::SELECT);
    
    // Other buttons should remain default
    EXPECT_EQ(newRemapper->getMappedButton(GamepadButton::A), GamepadButton::A);
    EXPECT_EQ(newRemapper->getMappedButton(GamepadButton::B), GamepadButton::B);
}

} // namespace d2