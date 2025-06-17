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
    
    enum class Priority {
        LOW = 0,
        NORMAL = 1,
        HIGH = 2
    };

    AudioEngine() = default;
    ~AudioEngine() = default;

    bool initialize();
    bool isInitialized() const;
    
    SoundId loadSound(const std::string& filename);
    bool playSound(SoundId soundId);
    bool playSound(SoundId soundId, Priority priority);
    
    // Volume controls
    void setMasterVolume(float volume);
    float getMasterVolume() const;
    void setSoundEffectVolume(float volume);
    float getSoundEffectVolume() const;
    void setMusicVolume(float volume);
    float getMusicVolume() const;
    
    // 3D positional audio
    void setListenerPosition(const glm::vec3& position);
    bool playPositional(SoundId soundId, const glm::vec3& position);
    ChannelLevels getChannelLevels() const;
    float getLastPlayedVolume() const;
    
    // Sound management
    bool playLooping(SoundId soundId);
    void stopLooping(SoundId soundId);
    bool isLooping(SoundId soundId) const;
    uint32_t getActiveSoundCount() const;
    bool isSoundPlaying(SoundId soundId) const;

private:
    bool initialized_ = false;
    SoundId nextSoundId_ = 1;
    std::unordered_set<SoundId> loadedSounds_;
    std::unordered_set<SoundId> loopingSounds_;
    std::unordered_set<SoundId> playingSounds_;
    glm::vec3 listenerPosition_{0.0f, 0.0f, 0.0f};
    ChannelLevels currentLevels_;
    
    // Volume controls
    float masterVolume_ = 1.0f;
    float soundEffectVolume_ = 1.0f;
    float musicVolume_ = 1.0f;
    float lastPlayedVolume_ = 1.0f;
};

} // namespace d2::audio