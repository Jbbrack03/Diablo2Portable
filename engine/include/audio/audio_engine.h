#pragma once

#include <string>
#include <unordered_set>
#include <glm/glm.hpp>

namespace d2::audio {

struct ChannelLevels {
    float left = 0.0f;
    float right = 0.0f;
};

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
    
    // 3D positional audio
    void setListenerPosition(const glm::vec3& position);
    bool playPositional(SoundId soundId, const glm::vec3& position);
    ChannelLevels getChannelLevels() const;

private:
    bool initialized_ = false;
    SoundId nextSoundId_ = 1;
    std::unordered_set<SoundId> loadedSounds_;
    glm::vec3 listenerPosition_{0.0f, 0.0f, 0.0f};
    ChannelLevels currentLevels_;
};

} // namespace d2::audio