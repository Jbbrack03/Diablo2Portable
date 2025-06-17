#include "audio/audio_engine.h"
#include <algorithm>

namespace d2::audio {

bool AudioEngine::initialize() {
    // Minimal implementation to make test pass (Green phase)
    initialized_ = true;
    return true;
}

bool AudioEngine::isInitialized() const {
    return initialized_;
}

AudioEngine::SoundId AudioEngine::loadSound(const std::string& filename) {
    // Minimal implementation to make test pass (Green phase)
    if (!initialized_) {
        return INVALID_SOUND_ID;
    }
    
    (void)filename; // We don't actually load files yet - just assign IDs
    SoundId soundId = nextSoundId_++;
    loadedSounds_.insert(soundId);
    return soundId;
}

bool AudioEngine::playSound(SoundId soundId) {
    // Minimal implementation to make test pass (Green phase)
    if (!initialized_) {
        return false;
    }
    
    bool exists = loadedSounds_.find(soundId) != loadedSounds_.end();
    if (exists) {
        playingSounds_.insert(soundId);
    }
    return exists;
}

bool AudioEngine::playSound(SoundId soundId, Priority priority) {
    // Green phase implementation - priority not fully implemented yet
    (void)priority; // Suppress unused parameter warning
    return playSound(soundId);
}

void AudioEngine::setListenerPosition(const glm::vec3& position) {
    listenerPosition_ = position;
}

bool AudioEngine::playPositional(SoundId soundId, const glm::vec3& position) {
    if (!initialized_ || loadedSounds_.find(soundId) == loadedSounds_.end()) {
        return false;
    }
    
    // Simple 3D positioning calculation (Green phase)
    glm::vec3 soundDirection = position - listenerPosition_;
    
    // For simplicity, use X position for left/right panning
    float distance = glm::length(soundDirection);
    float panFactor = (distance > 0.0f) ? soundDirection.x / distance : 0.0f;
    
    // Calculate stereo balance: -1.0 = full left, 0.0 = center, +1.0 = full right
    float rightGain = 0.5f + (panFactor * 0.5f);  // 0.0 to 1.0
    float leftGain = 1.0f - rightGain;             // 1.0 to 0.0
    
    // Distance attenuation - sounds get quieter with distance
    float maxDistance = 100.0f; // Maximum hearing distance
    float volumeFactor = 1.0f;
    if (distance > 0.0f) {
        volumeFactor = std::max(0.0f, 1.0f - (distance / maxDistance));
    }
    
    lastPlayedVolume_ = volumeFactor;
    currentLevels_.left = leftGain * volumeFactor;
    currentLevels_.right = rightGain * volumeFactor;
    
    playingSounds_.insert(soundId);
    return true;
}

ChannelLevels AudioEngine::getChannelLevels() const {
    return currentLevels_;
}

// Volume control methods
void AudioEngine::setMasterVolume(float volume) {
    masterVolume_ = volume;
}

float AudioEngine::getMasterVolume() const {
    return masterVolume_;
}

void AudioEngine::setSoundEffectVolume(float volume) {
    soundEffectVolume_ = volume;
}

float AudioEngine::getSoundEffectVolume() const {
    return soundEffectVolume_;
}

void AudioEngine::setMusicVolume(float volume) {
    musicVolume_ = volume;
}

float AudioEngine::getMusicVolume() const {
    return musicVolume_;
}

float AudioEngine::getLastPlayedVolume() const {
    return lastPlayedVolume_;
}

// Sound looping methods
bool AudioEngine::playLooping(SoundId soundId) {
    if (!initialized_ || loadedSounds_.find(soundId) == loadedSounds_.end()) {
        return false;
    }
    
    loopingSounds_.insert(soundId);
    playingSounds_.insert(soundId);
    return true;
}

void AudioEngine::stopLooping(SoundId soundId) {
    loopingSounds_.erase(soundId);
    playingSounds_.erase(soundId);
}

bool AudioEngine::isLooping(SoundId soundId) const {
    return loopingSounds_.find(soundId) != loopingSounds_.end();
}

uint32_t AudioEngine::getActiveSoundCount() const {
    return static_cast<uint32_t>(playingSounds_.size());
}

bool AudioEngine::isSoundPlaying(SoundId soundId) const {
    return playingSounds_.find(soundId) != playingSounds_.end();
}

} // namespace d2::audio