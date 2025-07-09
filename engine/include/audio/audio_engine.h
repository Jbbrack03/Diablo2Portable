#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
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
    
    // Audio data queries
    bool hasAudioData(SoundId soundId) const;
    float getAudioDuration(SoundId soundId) const;
    
    struct AudioProperties {
        int sampleRate = 44100;
        int channels = 2;
        int bitsPerSample = 16;
    };
    
    std::vector<uint8_t> getDecodedPCMData(SoundId soundId) const;
    AudioProperties getAudioProperties(SoundId soundId) const;
    
    // Audio device management
    struct DeviceCapabilities {
        int sampleRate = 0;
        int bufferSize = 0;
        int channels = 0;
    };
    
    bool openAudioDevice();
    bool isAudioDeviceOpen() const;
    void closeAudioDevice();
    DeviceCapabilities getDeviceCapabilities() const;
    
    // Music streaming
    SoundId loadMusic(const std::string& filename);
    bool isStreamingAudio(SoundId soundId) const;
    bool playMusic(SoundId soundId);
    void stopMusic();
    bool isMusicPlaying() const;
    
    // Real audio backend support
    bool hasRealAudioBackend() const;
    struct AudioDeviceInfo {
        std::string deviceName;
        int maxChannels = 0;
        bool supportsLowLatency = false;
    };
    AudioDeviceInfo getAudioDeviceInfo() const;

private:
    struct AudioData {
        std::vector<uint8_t> data;
        float duration = 0.0f;
        int sampleRate = 44100;
        int channels = 2;
        int bitsPerSample = 16;
    };

    bool initialized_ = false;
    SoundId nextSoundId_ = 1;
    std::unordered_set<SoundId> loadedSounds_;
    std::unordered_set<SoundId> loopingSounds_;
    std::unordered_set<SoundId> playingSounds_;
    std::unordered_map<SoundId, std::unique_ptr<AudioData>> audioDataMap_;
    glm::vec3 listenerPosition_{0.0f, 0.0f, 0.0f};
    ChannelLevels currentLevels_;
    
    // Volume controls
    float masterVolume_ = 1.0f;
    float soundEffectVolume_ = 1.0f;
    float musicVolume_ = 1.0f;
    float lastPlayedVolume_ = 1.0f;
    
    // Audio device state
    bool deviceOpen_ = false;
    DeviceCapabilities deviceCapabilities_;
    
    // Music streaming state
    std::unordered_set<SoundId> streamingSounds_;
    SoundId currentMusicId_ = INVALID_SOUND_ID;
    bool musicPlaying_ = false;
};

} // namespace d2::audio