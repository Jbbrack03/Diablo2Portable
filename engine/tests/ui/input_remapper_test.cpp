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

} // namespace d2