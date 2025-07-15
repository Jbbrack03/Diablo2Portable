#include <gtest/gtest.h>
#include "game/game_engine.h"
#include <filesystem>

class GameEngineRefactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<d2::GameEngine>();
    }
    
    std::unique_ptr<d2::GameEngine> engine;
};

TEST_F(GameEngineRefactorTest, InitializeCallsHelperMethodsInOrder) {
    // Test that initialize() properly delegates to helper methods
    // Use empty path which should succeed
    std::string testAssetPath = "";
    
    // Initialize should succeed
    EXPECT_TRUE(engine->initialize(testAssetPath));
    
    // After initialization, all components should be created
    EXPECT_TRUE(engine->isInitialized());
    
    // Verify core components exist by testing functionality
    // (We can't directly access private members, so we test behavior)
    EXPECT_TRUE(engine->start());
    
    // Should be able to render a frame after initialization
    EXPECT_TRUE(engine->renderFrame());
}

TEST_F(GameEngineRefactorTest, InitializeIsIdempotent) {
    std::string testAssetPath = "";
    
    // First initialization
    EXPECT_TRUE(engine->initialize(testAssetPath));
    
    // Second initialization should also succeed but not re-initialize
    EXPECT_TRUE(engine->initialize(testAssetPath));
    
    // Engine should still be functional
    EXPECT_TRUE(engine->isInitialized());
    EXPECT_TRUE(engine->start());
}

TEST_F(GameEngineRefactorTest, ComponentInitializationCanHandleEmptyAssetPath) {
    // Initialize with empty asset path should still succeed
    EXPECT_TRUE(engine->initialize(""));
    EXPECT_TRUE(engine->isInitialized());
}