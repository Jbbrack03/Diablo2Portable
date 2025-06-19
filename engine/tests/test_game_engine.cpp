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

} // namespace d2::test