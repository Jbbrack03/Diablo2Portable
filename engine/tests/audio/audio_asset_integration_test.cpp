#include <gtest/gtest.h>
#include <memory>
#include "audio/audio_engine.h"
#include "core/asset_manager.h"

class AudioAssetIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        assetManager_ = std::make_shared<d2portable::core::AssetManager>();
        assetManager_->initialize("test_assets");
        
        audioEngine_ = std::make_unique<d2::audio::AudioEngine>();
        audioEngine_->initialize();
        audioEngine_->setAssetManager(assetManager_);
    }
    
    std::shared_ptr<d2portable::core::AssetManager> assetManager_;
    std::unique_ptr<d2::audio::AudioEngine> audioEngine_;
};

TEST_F(AudioAssetIntegrationTest, LoadsSoundFromAssetManager) {
    // This test should load a sound file through the asset manager
    auto soundId = audioEngine_->loadSound("audio/effects/button.wav");
    
    EXPECT_NE(soundId, d2::audio::AudioEngine::INVALID_SOUND_ID);
    EXPECT_TRUE(audioEngine_->isSoundLoaded(soundId));
}

TEST_F(AudioAssetIntegrationTest, PlaysExtractedAudioFiles) {
    // Load actual game sounds from extracted assets
    auto soundId = audioEngine_->loadSound("audio/sfx/item/gem1.wav");
    
    EXPECT_NE(soundId, d2::audio::AudioEngine::INVALID_SOUND_ID);
    
    // Test playing the sound
    EXPECT_TRUE(audioEngine_->playSound(soundId));
}

TEST_F(AudioAssetIntegrationTest, LoadsMusicFromAssetManager) {
    // Load background music
    auto musicId = audioEngine_->loadMusic("audio/music/town1.wav");
    
    EXPECT_NE(musicId, d2::audio::AudioEngine::INVALID_SOUND_ID);
    EXPECT_TRUE(audioEngine_->isMusicLoaded(musicId));
}

TEST_F(AudioAssetIntegrationTest, CachesLoadedAudioFiles) {
    // First load
    auto soundId1 = audioEngine_->loadSound("audio/effects/button.wav");
    
    // Second load should return the same ID (cached)
    auto soundId2 = audioEngine_->loadSound("audio/effects/button.wav");
    
    EXPECT_EQ(soundId1, soundId2);
}

TEST_F(AudioAssetIntegrationTest, HandlesNonExistentFiles) {
    // Try to load a file that doesn't exist
    auto soundId = audioEngine_->loadSound("audio/effects/nonexistent.wav");
    
    EXPECT_EQ(soundId, d2::audio::AudioEngine::INVALID_SOUND_ID);
}