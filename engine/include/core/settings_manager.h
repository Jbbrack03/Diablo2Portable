#pragma once

#include <string>
#include <memory>

namespace d2 {

class SettingsManager {
public:
    enum class GraphicsQuality {
        LOW = 0,
        MEDIUM = 1,
        HIGH = 2,
        ULTRA = 3
    };

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

    // Video settings
    int getResolutionWidth() const;
    int getResolutionHeight() const;
    void setResolution(int width, int height);
    
    GraphicsQuality getGraphicsQuality() const;
    void setGraphicsQuality(GraphicsQuality quality);
    
    bool isFullscreen() const;
    void setFullscreen(bool fullscreen);

    // Control settings
    float getGamepadSensitivity() const;
    void setGamepadSensitivity(float sensitivity);
    
    float getGamepadDeadzone() const;
    void setGamepadDeadzone(float deadzone);
    
    bool isVibrationEnabled() const;
    void setVibrationEnabled(bool enabled);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace d2