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

} // namespace
} // namespace d2