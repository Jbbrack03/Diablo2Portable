#include <gtest/gtest.h>
#include "rendering/sprite_animation.h"
#include <functional>

namespace d2 {
namespace rendering {

class SpriteAnimationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
};

TEST_F(SpriteAnimationTest, CreateBasicAnimation) {
    SpriteAnimation animation("test_sprite");
    
    EXPECT_EQ(animation.getSpriteName(), "test_sprite");
    EXPECT_EQ(animation.getCurrentFrame(), 0);
    EXPECT_EQ(animation.getCurrentDirection(), 0);
    EXPECT_FALSE(animation.isPlaying());
}

TEST_F(SpriteAnimationTest, StartAndStopAnimation) {
    SpriteAnimation animation("walk_animation");
    
    // Should be able to start animation
    animation.play();
    EXPECT_TRUE(animation.isPlaying());
    
    // Should be able to stop animation
    animation.stop();
    EXPECT_FALSE(animation.isPlaying());
}

TEST_F(SpriteAnimationTest, UpdateAdvancesFrames) {
    SpriteAnimation animation("walk_animation");
    animation.setFrameCount(4);  // 4 frames total
    animation.setFrameRate(10.0f);  // 10 FPS
    animation.play();
    
    // Initial state
    EXPECT_EQ(animation.getCurrentFrame(), 0);
    
    // Update with 0.1 seconds (1 frame at 10 FPS)
    animation.update(0.1f);
    EXPECT_EQ(animation.getCurrentFrame(), 1);
    
    // Another update
    animation.update(0.1f);
    EXPECT_EQ(animation.getCurrentFrame(), 2);
}

TEST_F(SpriteAnimationTest, AnimationLoopsCorrectly) {
    SpriteAnimation animation("loop_animation");
    animation.setFrameCount(3);  // 3 frames: 0, 1, 2
    animation.setFrameRate(10.0f);  // 10 FPS
    animation.play();
    
    // Advance to last frame
    animation.update(0.2f);  // 2 frames
    EXPECT_EQ(animation.getCurrentFrame(), 2);
    
    // One more update should loop back to frame 0
    animation.update(0.1f);  // 1 more frame
    EXPECT_EQ(animation.getCurrentFrame(), 0);
    
    // Continue looping
    animation.update(0.1f);
    EXPECT_EQ(animation.getCurrentFrame(), 1);
}

TEST_F(SpriteAnimationTest, DirectionControl) {
    SpriteAnimation animation("character_walk");
    
    // Test setting different directions
    animation.setDirection(2);
    EXPECT_EQ(animation.getCurrentDirection(), 2);
    
    animation.setDirection(7);
    EXPECT_EQ(animation.getCurrentDirection(), 7);
    
    // Direction should persist through animation updates
    animation.setFrameCount(4);
    animation.setFrameRate(10.0f);
    animation.play();
    animation.update(0.1f);
    
    EXPECT_EQ(animation.getCurrentDirection(), 7);  // Should still be direction 7
    EXPECT_EQ(animation.getCurrentFrame(), 1);      // But frame should advance
}

TEST_F(SpriteAnimationTest, FrameInterpolation) {
    SpriteAnimation animation("smooth_animation");
    animation.setFrameCount(4);  // 4 frames: 0, 1, 2, 3
    animation.setFrameRate(10.0f);  // 10 FPS (0.1s per frame)
    animation.play();
    
    // At start, interpolation should be 0.0 (exactly on frame 0)
    EXPECT_FLOAT_EQ(animation.getFrameInterpolation(), 0.0f);
    
    // After 0.05s (half a frame duration), should be 0.5 interpolation
    animation.update(0.05f);
    EXPECT_EQ(animation.getCurrentFrame(), 0);  // Still on frame 0
    EXPECT_FLOAT_EQ(animation.getFrameInterpolation(), 0.5f);  // But halfway to next frame
    
    // After another 0.05s (complete frame), should be on frame 1 with 0.0 interpolation
    animation.update(0.05f);
    EXPECT_EQ(animation.getCurrentFrame(), 1);
    EXPECT_FLOAT_EQ(animation.getFrameInterpolation(), 0.0f);
    
    // Test partial progress towards frame 2
    animation.update(0.03f);  // 30% towards next frame
    EXPECT_EQ(animation.getCurrentFrame(), 1);
    EXPECT_FLOAT_EQ(animation.getFrameInterpolation(), 0.3f);
}

TEST_F(SpriteAnimationTest, AnimationCompleteCallback) {
    SpriteAnimation animation("callback_animation");
    animation.setFrameCount(3);  // 3 frames: 0, 1, 2
    animation.setFrameRate(10.0f);  // 10 FPS (0.1s per frame)
    
    // Set up callback to track when animation completes a full cycle
    bool callbackCalled = false;
    std::string callbackSpriteName;
    animation.setOnAnimationComplete([&callbackCalled, &callbackSpriteName](const std::string& spriteName) {
        callbackCalled = true;
        callbackSpriteName = spriteName;
    });
    
    animation.play();
    
    // Animation should not have completed yet
    EXPECT_FALSE(callbackCalled);
    
    // Update through most of the animation (2 frames)
    animation.update(0.2f);  
    EXPECT_EQ(animation.getCurrentFrame(), 2);  // On last frame
    EXPECT_FALSE(callbackCalled);  // Should not have completed yet
    
    // One more update should complete the animation and trigger callback
    animation.update(0.1f);  // Complete the cycle
    EXPECT_EQ(animation.getCurrentFrame(), 0);  // Should loop back to start
    EXPECT_TRUE(callbackCalled);  // Callback should have been called
    EXPECT_EQ(callbackSpriteName, "callback_animation");  // With correct sprite name
}

} // namespace rendering
} // namespace d2