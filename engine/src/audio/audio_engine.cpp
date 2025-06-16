#include "audio/audio_engine.h"

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
    
    return loadedSounds_.find(soundId) != loadedSounds_.end();
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
    
    currentLevels_.left = leftGain;
    currentLevels_.right = rightGain;
    return true;
}

ChannelLevels AudioEngine::getChannelLevels() const {
    return currentLevels_;
}

} // namespace d2::audio