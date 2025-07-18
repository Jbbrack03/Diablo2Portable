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

// Phase 18 Task 4 Tests - Real Audio Implementation
TEST_F(AudioEngineTest, LoadRealAudioFile) {
    AudioEngine engine;
    engine.initialize();
    
    // Test loading a real audio file (should fail until implemented)
    auto soundId = engine.loadSound("test_data/sword_swing.ogg");
    EXPECT_NE(soundId, AudioEngine::INVALID_SOUND_ID);
    
    // The sound should actually contain audio data
    EXPECT_TRUE(engine.hasAudioData(soundId));
    EXPECT_GT(engine.getAudioDuration(soundId), 0.0f);
}

TEST_F(AudioEngineTest, DecodeOggVorbisFile) {
    AudioEngine engine;
    engine.initialize();
    
    // Test loading and decoding an OGG Vorbis file
    auto soundId = engine.loadSound("test_data/test_sound.ogg");
    EXPECT_NE(soundId, AudioEngine::INVALID_SOUND_ID);
    
    // Check that we have decoded PCM data
    auto pcmData = engine.getDecodedPCMData(soundId);
    EXPECT_FALSE(pcmData.empty());
    
    // Verify audio properties
    auto properties = engine.getAudioProperties(soundId);
    EXPECT_EQ(properties.sampleRate, 44100);
    EXPECT_EQ(properties.channels, 2);
    EXPECT_EQ(properties.bitsPerSample, 16);
}

TEST_F(AudioEngineTest, AudioDevicePlayback) {
    AudioEngine engine;
    engine.initialize();  // Need to initialize first
    
    // Test that we can open an audio device for playback
    EXPECT_TRUE(engine.openAudioDevice());
    EXPECT_TRUE(engine.isAudioDeviceOpen());
    
    // Test that we can query device capabilities
    auto deviceCaps = engine.getDeviceCapabilities();
    EXPECT_GT(deviceCaps.sampleRate, 0);
    EXPECT_GT(deviceCaps.bufferSize, 0);
    EXPECT_GE(deviceCaps.channels, 2);
    
    // Test closing the device
    engine.closeAudioDevice();
    EXPECT_FALSE(engine.isAudioDeviceOpen());
}

TEST_F(AudioEngineTest, AudioStreamingForMusic) {
    AudioEngine engine;
    engine.initialize();
    
    // Test loading a music file for streaming (larger files)
    auto musicId = engine.loadMusic("test_data/background_music.ogg");
    EXPECT_NE(musicId, AudioEngine::INVALID_SOUND_ID);
    
    // Music should be marked for streaming, not loaded entirely into memory
    EXPECT_TRUE(engine.isStreamingAudio(musicId));
    EXPECT_FALSE(engine.hasAudioData(musicId)); // Should not have all data in memory
    
    // Test playing streaming audio
    EXPECT_TRUE(engine.playMusic(musicId));
    EXPECT_TRUE(engine.isMusicPlaying());
    
    // Test stopping music
    engine.stopMusic();
    EXPECT_FALSE(engine.isMusicPlaying());
}

// Test for real audio device initialization using platform-specific APIs
TEST_F(AudioEngineTest, InitializesRealAudioDevice) {
    AudioEngine engine;
    engine.initialize();
    
    // The engine should use a real audio backend (e.g., Oboe on Android)
    EXPECT_TRUE(engine.hasRealAudioBackend());
    
    // Should be able to query actual device properties
    auto deviceInfo = engine.getAudioDeviceInfo();
    EXPECT_FALSE(deviceInfo.deviceName.empty());
    EXPECT_GT(deviceInfo.maxChannels, 0);
    EXPECT_TRUE(deviceInfo.supportsLowLatency);
}