#include <gtest/gtest.h>
#include "audio/audio_engine.h"
#include <glm/glm.hpp>

using namespace d2::audio;

class AudioEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

TEST_F(AudioEngineTest, InitializeAudioEngine) {
    AudioEngine engine;
    
    // Test that the audio engine can be initialized
    bool success = engine.initialize();
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(engine.isInitialized());
}

TEST_F(AudioEngineTest, LoadAndPlaySound) {
    AudioEngine engine;
    engine.initialize();
    
    // Test loading a sound file
    auto soundId = engine.loadSound("sword_swing.ogg");
    EXPECT_NE(soundId, AudioEngine::INVALID_SOUND_ID);
    
    // Test playing the loaded sound
    bool playSuccess = engine.playSound(soundId);
    EXPECT_TRUE(playSuccess);
}

TEST_F(AudioEngineTest, PositionalAudio3D) {
    AudioEngine engine;
    engine.initialize();
    
    // Set listener position (e.g., player position)
    engine.setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    
    auto soundId = engine.loadSound("monster_growl.ogg");
    EXPECT_NE(soundId, AudioEngine::INVALID_SOUND_ID);
    
    // Play sound at a position in 3D space (e.g., monster to the right)
    bool playSuccess = engine.playPositional(soundId, glm::vec3(10.0f, 0.0f, 0.0f));
    EXPECT_TRUE(playSuccess);
    
    // Verify that we can get the channel levels for 3D audio verification
    auto levels = engine.getChannelLevels();
    
    // For a sound to the right, the right channel should be louder
    EXPECT_GT(levels.right, levels.left);
}