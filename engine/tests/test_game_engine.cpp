#include <gtest/gtest.h>
#include "game/game_engine.h"

namespace d2::test {

class GameEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup if needed
    }
    
    void TearDown() override {
        // Test cleanup if needed
    }
};

TEST_F(GameEngineTest, CreateEngine) {
    GameEngine engine;
    
    EXPECT_FALSE(engine.isInitialized());
    EXPECT_FALSE(engine.isRunning());
}

TEST_F(GameEngineTest, InitializeEngine) {
    GameEngine engine;
    
    bool result = engine.initialize();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(engine.isInitialized());
    EXPECT_FALSE(engine.isRunning());
}

TEST_F(GameEngineTest, InitializeWithAssetPath) {
    GameEngine engine;
    
    // Use current directory which should exist
    bool result = engine.initialize(".");
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(engine.isInitialized());
    
    // Verify asset manager was initialized with the path
    auto* assetManager = engine.getAssetManager();
    EXPECT_NE(assetManager, nullptr);
}

TEST_F(GameEngineTest, InitializeWithInvalidPath) {
    GameEngine engine;
    
    // Use a path that doesn't exist
    bool result = engine.initialize("/non/existent/path");
    
    EXPECT_FALSE(result);
    EXPECT_FALSE(engine.isInitialized());
    
    // Asset manager should still be created but initialization failed
    auto* assetManager = engine.getAssetManager();
    EXPECT_NE(assetManager, nullptr);
}

} // namespace d2::test