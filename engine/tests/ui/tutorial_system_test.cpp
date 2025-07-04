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

// Test 3: Navigate between tutorial steps - This test MUST fail first
TEST_F(TutorialSystemTest, NavigateTutorialSteps) {
    TutorialSystem tutorial;
    tutorial.initialize(testDir.string());
    tutorial.loadTutorial("getting_started");
    
    // Initially at step 0
    EXPECT_EQ(tutorial.getCurrentStep(), 0);
    auto currentStep = tutorial.getCurrentStepInfo();
    EXPECT_EQ(currentStep.id, "welcome");
    
    // Move to next step
    EXPECT_TRUE(tutorial.nextStep());
    EXPECT_EQ(tutorial.getCurrentStep(), 1);
    currentStep = tutorial.getCurrentStepInfo();
    EXPECT_EQ(currentStep.id, "controls");
    
    // Can't go beyond last step
    EXPECT_FALSE(tutorial.nextStep());
    EXPECT_EQ(tutorial.getCurrentStep(), 1);
    
    // Move to previous step
    EXPECT_TRUE(tutorial.previousStep());
    EXPECT_EQ(tutorial.getCurrentStep(), 0);
    
    // Can't go before first step
    EXPECT_FALSE(tutorial.previousStep());
    EXPECT_EQ(tutorial.getCurrentStep(), 0);
}

// Test 4: Mark tutorial steps as completed - This test MUST fail first
TEST_F(TutorialSystemTest, MarkStepsCompleted) {
    TutorialSystem tutorial;
    tutorial.initialize(testDir.string());
    tutorial.loadTutorial("getting_started");
    
    // Initially step is not completed
    auto step = tutorial.getCurrentStepInfo();
    EXPECT_FALSE(step.completed);
    
    // Mark current step as completed
    EXPECT_TRUE(tutorial.markCurrentStepCompleted());
    
    // Verify step is now completed
    step = tutorial.getCurrentStepInfo();
    EXPECT_TRUE(step.completed);
    
    // Move to next step
    tutorial.nextStep();
    step = tutorial.getCurrentStepInfo();
    EXPECT_FALSE(step.completed);
    
    // Check if tutorial is complete
    EXPECT_FALSE(tutorial.isComplete());
    
    // Mark last step as completed
    tutorial.markCurrentStepCompleted();
    EXPECT_TRUE(tutorial.isComplete());
    
    // Get completion percentage
    EXPECT_EQ(tutorial.getCompletionPercentage(), 100);
}

// Test 5: Save and load tutorial progress - This test MUST fail first
TEST_F(TutorialSystemTest, SaveAndLoadProgress) {
    // Create and setup first tutorial instance
    {
        TutorialSystem tutorial;
        tutorial.initialize(testDir.string());
        tutorial.loadTutorial("getting_started");
        
        // Complete first step and move to second
        tutorial.markCurrentStepCompleted();
        tutorial.nextStep();
        
        // Save progress
        EXPECT_TRUE(tutorial.saveProgress());
    }
    
    // Create new instance and load saved progress
    {
        TutorialSystem tutorial;
        tutorial.initialize(testDir.string());
        tutorial.loadTutorial("getting_started");
        
        // Load saved progress
        EXPECT_TRUE(tutorial.loadProgress());
        
        // Verify we're on the second step
        EXPECT_EQ(tutorial.getCurrentStep(), 1);
        
        // Verify first step is completed
        auto firstStep = tutorial.getStep(0);
        EXPECT_TRUE(firstStep.completed);
        
        // Verify second step is not completed
        auto secondStep = tutorial.getStep(1);
        EXPECT_FALSE(secondStep.completed);
        
        // Verify completion percentage
        EXPECT_EQ(tutorial.getCompletionPercentage(), 50);
    }
}

} // namespace
} // namespace d2