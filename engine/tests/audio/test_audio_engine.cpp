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

// NEW Phase 6 Tests - Written BEFORE implementation (RED phase)
TEST_F(AudioEngineTest, VolumeControl) {
    AudioEngine engine;
    engine.initialize();
    
    // Test master volume control
    engine.setMasterVolume(0.5f);
    EXPECT_FLOAT_EQ(engine.getMasterVolume(), 0.5f);
    
    // Test sound effect volume
    engine.setSoundEffectVolume(0.75f);
    EXPECT_FLOAT_EQ(engine.getSoundEffectVolume(), 0.75f);
    
    // Test music volume
    engine.setMusicVolume(0.3f);
    EXPECT_FLOAT_EQ(engine.getMusicVolume(), 0.3f);
}

TEST_F(AudioEngineTest, DistanceAttenuation) {
    AudioEngine engine;
    engine.initialize();
    
    engine.setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    auto soundId = engine.loadSound("footstep.ogg");
    
    // Test that sounds get quieter with distance
    engine.playPositional(soundId, glm::vec3(5.0f, 0.0f, 0.0f));
    float nearVolume = engine.getLastPlayedVolume();
    
    engine.playPositional(soundId, glm::vec3(50.0f, 0.0f, 0.0f));
    float farVolume = engine.getLastPlayedVolume();
    
    EXPECT_GT(nearVolume, farVolume);
    EXPECT_GT(nearVolume, 0.0f);
    EXPECT_GE(farVolume, 0.0f);
}

TEST_F(AudioEngineTest, SoundLooping) {
    AudioEngine engine;
    engine.initialize();
    
    auto soundId = engine.loadSound("ambient_wind.ogg");
    
    // Test playing a looping sound
    bool success = engine.playLooping(soundId);
    EXPECT_TRUE(success);
    EXPECT_TRUE(engine.isLooping(soundId));
    
    // Test stopping a looping sound
    engine.stopLooping(soundId);
    EXPECT_FALSE(engine.isLooping(soundId));
}

TEST_F(AudioEngineTest, MultipleSimultaneousSounds) {
    AudioEngine engine;
    engine.initialize();
    
    auto sword1 = engine.loadSound("sword1.ogg");
    auto sword2 = engine.loadSound("sword2.ogg");
    auto magic = engine.loadSound("magic_spell.ogg");
    
    // Test playing multiple sounds at once
    EXPECT_TRUE(engine.playSound(sword1));
    EXPECT_TRUE(engine.playSound(sword2));
    EXPECT_TRUE(engine.playSound(magic));
    
    // Verify all are playing
    EXPECT_EQ(engine.getActiveSoundCount(), 3);
}

TEST_F(AudioEngineTest, SoundPriority) {
    AudioEngine engine;
    engine.initialize();
    
    auto lowPriority = engine.loadSound("ambient.ogg");
    auto highPriority = engine.loadSound("critical_hit.ogg");
    
    // Test that high priority sounds can interrupt low priority ones
    engine.playSound(lowPriority, AudioEngine::Priority::LOW);
    engine.playSound(highPriority, AudioEngine::Priority::HIGH);
    
    // High priority sound should be playing
    EXPECT_TRUE(engine.isSoundPlaying(highPriority));
}