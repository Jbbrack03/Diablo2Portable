#pragma once

#include <string>
#include <memory>

namespace d2 {

class SettingsManager {
public:
    SettingsManager();
    ~SettingsManager();

    // Audio settings
    float getMasterVolume() const;
    void setMasterVolume(float volume);
    
    float getSoundEffectVolume() const;
    void setSoundEffectVolume(float volume);
    
    float getMusicVolume() const;
    void setMusicVolume(float volume);

    // Save/Load functionality
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace d2