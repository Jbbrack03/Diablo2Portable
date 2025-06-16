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

} // namespace d2::audio