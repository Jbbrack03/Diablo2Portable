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

// Test 3: Get topics by category
TEST_F(HelpSystemTest, GetTopicsByCategory) {
    helpSystem->loadHelpTopics();
    
    // Get all topics in "basics" category
    auto basicTopics = helpSystem->getTopicsByCategory("basics");
    EXPECT_GE(basicTopics.size(), 2); // getting-started and controls
    
    // Verify we have the expected topics
    bool foundGettingStarted = false;
    bool foundControls = false;
    
    for (const auto& topic : basicTopics) {
        if (topic.id == "getting-started") {
            foundGettingStarted = true;
        } else if (topic.id == "controls") {
            foundControls = true;
        }
    }
    
    EXPECT_TRUE(foundGettingStarted);
    EXPECT_TRUE(foundControls);
    
    // Get support category
    auto supportTopics = helpSystem->getTopicsByCategory("support");
    EXPECT_GE(supportTopics.size(), 1); // troubleshooting
    
    bool foundTroubleshootingInSupport = false;
    for (const auto& topic : supportTopics) {
        if (topic.id == "troubleshooting") {
            foundTroubleshootingInSupport = true;
            break;
        }
    }
    EXPECT_TRUE(foundTroubleshootingInSupport);
}

// Test 4: Get all available categories
TEST_F(HelpSystemTest, GetAllCategories) {
    helpSystem->loadHelpTopics();
    
    // Get all categories
    auto categories = helpSystem->getAllCategories();
    EXPECT_GE(categories.size(), 2); // At least "basics" and "support"
    
    // Verify we have the expected categories
    bool foundBasics = false;
    bool foundSupport = false;
    
    for (const auto& category : categories) {
        if (category == "basics") {
            foundBasics = true;
        } else if (category == "support") {
            foundSupport = true;
        }
    }
    
    EXPECT_TRUE(foundBasics);
    EXPECT_TRUE(foundSupport);
}

// Test 5: Verify comprehensive help content
TEST_F(HelpSystemTest, ComprehensiveHelpContent) {
    helpSystem->loadHelpTopics();
    
    // Check we have help for all major game features
    EXPECT_TRUE(helpSystem->hasTopic("asset-extraction"));
    EXPECT_TRUE(helpSystem->hasTopic("performance-tips"));
    EXPECT_TRUE(helpSystem->hasTopic("multiplayer-setup"));
    EXPECT_TRUE(helpSystem->hasTopic("save-management"));
    
    // Verify asset extraction help has proper content
    const auto* assetHelp = helpSystem->getTopic("asset-extraction");
    EXPECT_NE(assetHelp, nullptr);
    EXPECT_EQ(assetHelp->category, "setup");
    EXPECT_FALSE(assetHelp->content.empty());
    
    // Search for performance-related help
    auto perfResults = helpSystem->searchByKeyword("performance");
    EXPECT_GE(perfResults.size(), 1);
    
    // Verify we have at least 3 categories now
    auto categories = helpSystem->getAllCategories();
    EXPECT_GE(categories.size(), 3); // basics, support, setup
}

} // namespace d2