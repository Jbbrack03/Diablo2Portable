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

TEST_F(GameEngineTest, GetRenderer) {
    GameEngine engine;
    
    // Before initialization, renderer should be null
    EXPECT_EQ(engine.getRenderer(), nullptr);
    
    // After initialization, renderer should be available
    engine.initialize();
    auto* renderer = engine.getRenderer();
    EXPECT_NE(renderer, nullptr);
}

TEST_F(GameEngineTest, StartEngine) {
    GameEngine engine;
    
    // Cannot start without initialization
    EXPECT_FALSE(engine.start());
    EXPECT_FALSE(engine.isRunning());
    
    // Initialize then start
    engine.initialize();
    EXPECT_TRUE(engine.start());
    EXPECT_TRUE(engine.isRunning());
}

TEST_F(GameEngineTest, StopEngine) {
    GameEngine engine;
    engine.initialize();
    engine.start();
    
    EXPECT_TRUE(engine.isRunning());
    
    engine.stop();
    
    EXPECT_FALSE(engine.isRunning());
}

TEST_F(GameEngineTest, RenderFrame) {
    GameEngine engine;
    
    // Cannot render without initialization
    EXPECT_FALSE(engine.renderFrame());
    
    // Initialize but don't start - still can't render
    engine.initialize();
    EXPECT_FALSE(engine.renderFrame());
    
    // Start engine - now we can render
    engine.start();
    EXPECT_TRUE(engine.renderFrame());
}

} // namespace d2::test