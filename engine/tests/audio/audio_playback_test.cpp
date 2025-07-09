#include <gtest/gtest.h>
#include "audio/audio_engine.h"
#include <vector>
#include <cmath>

using namespace d2::audio;

class AudioPlaybackTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<AudioEngine>();
        engine->initialize();
    }
    
    void TearDown() override {
        engine.reset();
    }
    
    // Generate a simple sine wave for testing
    std::vector<float> generateSineWave(float frequency, float duration, int sampleRate = 44100) {
        int numSamples = static_cast<int>(duration * sampleRate);
        std::vector<float> samples(numSamples);
        
        for (int i = 0; i < numSamples; i++) {
            float t = static_cast<float>(i) / sampleRate;
            samples[i] = std::sin(2.0f * M_PI * frequency * t);
        }
        
        return samples;
    }
    
    std::unique_ptr<AudioEngine> engine;
};

// Test that AudioEngine can play audio data
TEST_F(AudioPlaybackTest, PlayAudioData) {
    // Generate a 440Hz sine wave for 0.1 seconds
    auto audioData = generateSineWave(440.0f, 0.1f);
    
    // Test playing audio data
    bool playResult = engine->playAudioData(audioData.data(), audioData.size(), 44100, 1);
    EXPECT_TRUE(playResult);
    
    // Check that audio is playing
    EXPECT_TRUE(engine->isPlaying());
    
    // Get playback position
    float position = engine->getPlaybackPosition();
    EXPECT_GE(position, 0.0f);
}

// Test stopping audio playback
TEST_F(AudioPlaybackTest, StopPlayback) {
    auto audioData = generateSineWave(440.0f, 0.5f);
    
    // Start playing
    engine->playAudioData(audioData.data(), audioData.size(), 44100, 1);
    EXPECT_TRUE(engine->isPlaying());
    
    // Stop playback
    engine->stopPlayback();
    EXPECT_FALSE(engine->isPlaying());
    
    // Position should reset to 0
    EXPECT_EQ(engine->getPlaybackPosition(), 0.0f);
}

// Test audio buffer management
TEST_F(AudioPlaybackTest, AudioBufferManagement) {
    // Test getting buffer information
    size_t bufferSize = engine->getBufferSize();
    EXPECT_GT(bufferSize, 0u);
    
    // Test getting sample rate
    int sampleRate = engine->getSampleRate();
    EXPECT_GT(sampleRate, 0);
    
    // Common sample rates
    EXPECT_TRUE(sampleRate == 44100 || sampleRate == 48000);
}