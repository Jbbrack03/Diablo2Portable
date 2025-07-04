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

// Test 2: Search help topics by keywords
TEST_F(HelpSystemTest, SearchByKeywords) {
    helpSystem->loadHelpTopics();
    
    // Search for "controller" should find controls topic
    auto results = helpSystem->searchByKeyword("controller");
    EXPECT_GE(results.size(), 1);
    
    bool foundControls = false;
    for (const auto& topic : results) {
        if (topic.id == "controls") {
            foundControls = true;
            break;
        }
    }
    EXPECT_TRUE(foundControls);
    
    // Search for "error" should find troubleshooting
    results = helpSystem->searchByKeyword("error");
    EXPECT_GE(results.size(), 1);
    
    bool foundTroubleshooting = false;
    for (const auto& topic : results) {
        if (topic.id == "troubleshooting") {
            foundTroubleshooting = true;
            break;
        }
    }
    EXPECT_TRUE(foundTroubleshooting);
}

} // namespace d2