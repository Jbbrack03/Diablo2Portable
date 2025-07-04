#include <gtest/gtest.h>
#include "ui/help_system.h"

namespace d2 {

class HelpSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        helpSystem = std::make_unique<HelpSystem>();
    }

    std::unique_ptr<HelpSystem> helpSystem;
};

// Test 1: Create help system and load topics
TEST_F(HelpSystemTest, LoadHelpTopics) {
    // Hardcoded help topics for now (like TutorialSystem)
    helpSystem->loadHelpTopics();
    
    auto topics = helpSystem->getAllTopics();
    EXPECT_GT(topics.size(), 0);
    
    // Should have basic categories
    EXPECT_TRUE(helpSystem->hasTopic("getting-started"));
    EXPECT_TRUE(helpSystem->hasTopic("controls"));
    EXPECT_TRUE(helpSystem->hasTopic("troubleshooting"));
}

} // namespace d2