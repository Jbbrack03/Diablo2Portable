#include <gtest/gtest.h>
#include "audio/audio_engine.h"

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