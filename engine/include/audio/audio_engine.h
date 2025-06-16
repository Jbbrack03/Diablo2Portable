#pragma once

#include <string>
#include <unordered_set>

namespace d2::audio {

class AudioEngine {
public:
    using SoundId = uint32_t;
    static constexpr SoundId INVALID_SOUND_ID = 0;

    AudioEngine() = default;
    ~AudioEngine() = default;

    bool initialize();
    bool isInitialized() const;
    
    SoundId loadSound(const std::string& filename);
    bool playSound(SoundId soundId);

private:
    bool initialized_ = false;
    SoundId nextSoundId_ = 1;
    std::unordered_set<SoundId> loadedSounds_;
};

} // namespace d2::audio