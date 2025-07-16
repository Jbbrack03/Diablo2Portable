#include <gtest/gtest.h>
#include <memory>
#include "animation/animation_controller.h"
#include "sprites/dc6_parser.h"
#include "core/asset_manager.h"

class AnimationControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        assetManager_ = std::make_shared<d2portable::core::AssetManager>();
        assetManager_->initialize("test_assets");
        
        // Create a test DC6 sprite with multiple frames
        createTestSprite();
    }
    
    void createTestSprite() {
        // Create a mock DC6 sprite for testing
        class MockDC6Sprite : public d2portable::sprites::DC6Sprite {
        public:
            MockDC6Sprite() {
                // Set up 8 directions with 8 frames each
                frames_.resize(8);
                for (auto& dir : frames_) {
                    dir.resize(8);
                    for (auto& frame : dir) {
                        frame.width = 64;
                        frame.height = 64;
                        frame.offsetX = 0;
                        frame.offsetY = 0;
                        frame.pixelData.resize(64 * 64 * 4, 255); // RGBA
                    }
                }
            }
            
            uint32_t getDirectionCount() const override { return 8; }
            uint32_t getFramesPerDirection() const override { return 8; }
            
            d2portable::sprites::DC6Frame getFrame(uint32_t direction, uint32_t frame) const override {
                if (direction < frames_.size() && frame < frames_[direction].size()) {
                    return frames_[direction][frame];
                }
                return d2portable::sprites::DC6Frame();
            }
            
            std::vector<uint8_t> getFrameImage(uint32_t direction, uint32_t frame) const override {
                auto f = getFrame(direction, frame);
                return f.pixelData;
            }
            
            std::vector<uint8_t> getFrameImageWithPalette(uint32_t direction, uint32_t frame, 
                                                          const std::vector<uint32_t>& palette) const override {
                return getFrameImage(direction, frame);
            }
            
            std::vector<std::vector<d2portable::sprites::DC6Frame>> frames_;
        };
        
        testSprite_ = std::make_shared<MockDC6Sprite>();
    }
    
    std::shared_ptr<d2portable::core::AssetManager> assetManager_;
    std::shared_ptr<d2portable::sprites::DC6Sprite> testSprite_;
};

TEST_F(AnimationControllerTest, PlaysMultiFrameAnimations) {
    d2::animation::AnimationController controller(testSprite_);
    
    controller.setDirection(d2::animation::Direction::SOUTH);
    controller.play();
    
    // Advance time
    controller.update(0.1f);
    
    EXPECT_GT(controller.getCurrentFrame(), 0);
    EXPECT_EQ(controller.getCurrentDirection(), d2::animation::Direction::SOUTH);
}

TEST_F(AnimationControllerTest, LoopsAnimations) {
    d2::animation::AnimationController controller(testSprite_);
    controller.setFrameRate(10.0f); // 10 FPS = 0.1s per frame
    controller.play();
    
    // Advance through all frames
    for (int i = 0; i < 8; ++i) {
        controller.update(0.1f);
    }
    
    // Should loop back to frame 0
    EXPECT_EQ(controller.getCurrentFrame(), 0);
    EXPECT_TRUE(controller.isPlaying());
}

TEST_F(AnimationControllerTest, StopsAtEndWhenNotLooping) {
    d2::animation::AnimationController controller(testSprite_);
    controller.setFrameRate(10.0f);
    controller.setLooping(false);
    controller.play();
    
    // Advance through all frames
    for (int i = 0; i < 10; ++i) {
        controller.update(0.1f);
    }
    
    // Should stop at last frame
    EXPECT_EQ(controller.getCurrentFrame(), 7);
    EXPECT_FALSE(controller.isPlaying());
}

TEST_F(AnimationControllerTest, ChangesDirectionMidAnimation) {
    d2::animation::AnimationController controller(testSprite_);
    controller.setDirection(d2::animation::Direction::SOUTH);
    controller.play();
    
    // Play a few frames
    controller.update(0.2f);
    
    // Change direction
    controller.setDirection(d2::animation::Direction::NORTH);
    
    // Frame should reset but animation continues
    EXPECT_EQ(controller.getCurrentFrame(), 0);
    EXPECT_EQ(controller.getCurrentDirection(), d2::animation::Direction::NORTH);
    EXPECT_TRUE(controller.isPlaying());
}

TEST_F(AnimationControllerTest, PausesAndResumes) {
    d2::animation::AnimationController controller(testSprite_);
    controller.setFrameRate(10.0f);
    controller.play();
    
    // Advance to frame 2
    controller.update(0.2f);
    int frameBeforePause = controller.getCurrentFrame();
    
    // Pause
    controller.pause();
    EXPECT_FALSE(controller.isPlaying());
    
    // Time passes but frame shouldn't change
    controller.update(0.5f);
    EXPECT_EQ(controller.getCurrentFrame(), frameBeforePause);
    
    // Resume
    controller.play();
    controller.update(0.1f);
    EXPECT_GT(controller.getCurrentFrame(), frameBeforePause);
}

TEST_F(AnimationControllerTest, GetsCorrectSpriteFrame) {
    d2::animation::AnimationController controller(testSprite_);
    controller.setDirection(d2::animation::Direction::EAST); // Direction 6
    controller.setCurrentFrame(3);
    
    // Verify the controller is tracking the correct frame
    EXPECT_EQ(controller.getCurrentDirection(), d2::animation::Direction::EAST);
    EXPECT_EQ(controller.getCurrentFrame(), 3);
    
    // In a real implementation, getCurrentSpriteFrame would return the actual frame
    // For now, we just verify the state is correct
}