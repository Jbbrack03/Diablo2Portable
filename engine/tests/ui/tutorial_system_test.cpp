#include <gtest/gtest.h>
#include "ui/tutorial_system.h"
#include <filesystem>
#include <fstream>

namespace d2 {
namespace {

class TutorialSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory for tutorial data
        testDir = std::filesystem::temp_directory_path() / "tutorial_test";
        std::filesystem::create_directories(testDir);
    }
    
    void TearDown() override {
        std::filesystem::remove_all(testDir);
    }
    
    std::filesystem::path testDir;
};

// Test 1: Create tutorial system - This test MUST fail first
TEST_F(TutorialSystemTest, CreateTutorialSystem) {
    TutorialSystem tutorial;
    tutorial.initialize(testDir.string());
    
    EXPECT_TRUE(tutorial.isInitialized());
    EXPECT_EQ(tutorial.getTotalSteps(), 0);
    EXPECT_EQ(tutorial.getCurrentStep(), 0);
}

// Test 2: Load tutorial steps from JSON configuration - This test MUST fail first
TEST_F(TutorialSystemTest, LoadTutorialSteps) {
    // Create a test tutorial configuration file
    std::string configFile = (testDir / "tutorials.json").string();
    std::ofstream out(configFile);
    out << R"({
        "tutorials": {
            "getting_started": {
                "name": "Getting Started",
                "steps": [
                    {
                        "id": "welcome",
                        "title": "Welcome to Diablo II",
                        "description": "Welcome to your adventure in Sanctuary!",
                        "image": "welcome.png"
                    },
                    {
                        "id": "controls",
                        "title": "Basic Controls",
                        "description": "Use the left stick to move your character",
                        "image": "controls.png"
                    }
                ]
            }
        }
    })";
    out.close();
    
    TutorialSystem tutorial;
    tutorial.initialize(testDir.string());
    
    EXPECT_TRUE(tutorial.loadTutorial("getting_started"));
    EXPECT_EQ(tutorial.getTotalSteps(), 2);
    
    // Check first step details
    auto step = tutorial.getStep(0);
    EXPECT_EQ(step.id, "welcome");
    EXPECT_EQ(step.title, "Welcome to Diablo II");
    EXPECT_EQ(step.description, "Welcome to your adventure in Sanctuary!");
    EXPECT_EQ(step.imagePath, "welcome.png");
    EXPECT_FALSE(step.completed);
}

} // namespace
} // namespace d2