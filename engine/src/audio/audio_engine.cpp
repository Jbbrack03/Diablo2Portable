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

} // namespace d2::audio