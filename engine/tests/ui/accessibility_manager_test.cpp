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

} // namespace d2