#include "core/settings_manager.h"
#include <fstream>
#include <sstream>

namespace d2 {

struct SettingsManager::Impl {
    // Audio settings
    float masterVolume = 1.0f;
    float soundEffectVolume = 1.0f;
    float musicVolume = 1.0f;
    
    // Video settings
    int resolutionWidth = 1280;
    int resolutionHeight = 720;
    GraphicsQuality graphicsQuality = GraphicsQuality::MEDIUM;
    bool fullscreen = true;
    
    // Control settings
    float gamepadSensitivity = 1.0f;
    float gamepadDeadzone = 0.2f;
    bool vibrationEnabled = true;
};

SettingsManager::SettingsManager() : impl_(std::make_unique<Impl>()) {}

SettingsManager::~SettingsManager() = default;

float SettingsManager::getMasterVolume() const {
    return impl_->masterVolume;
}

void SettingsManager::setMasterVolume(float volume) {
    impl_->masterVolume = volume;
}

float SettingsManager::getSoundEffectVolume() const {
    return impl_->soundEffectVolume;
}

void SettingsManager::setSoundEffectVolume(float volume) {
    impl_->soundEffectVolume = volume;
}

float SettingsManager::getMusicVolume() const {
    return impl_->musicVolume;
}

void SettingsManager::setMusicVolume(float volume) {
    impl_->musicVolume = volume;
}

bool SettingsManager::save(const std::string& filepath) const {
    // Minimal implementation to pass test (GREEN phase)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple text format for now
    // Audio settings
    file << "masterVolume=" << impl_->masterVolume << "\n";
    file << "soundEffectVolume=" << impl_->soundEffectVolume << "\n";
    file << "musicVolume=" << impl_->musicVolume << "\n";
    
    // Video settings
    file << "resolutionWidth=" << impl_->resolutionWidth << "\n";
    file << "resolutionHeight=" << impl_->resolutionHeight << "\n";
    file << "graphicsQuality=" << static_cast<int>(impl_->graphicsQuality) << "\n";
    file << "fullscreen=" << (impl_->fullscreen ? 1 : 0) << "\n";
    
    // Control settings
    file << "gamepadSensitivity=" << impl_->gamepadSensitivity << "\n";
    file << "gamepadDeadzone=" << impl_->gamepadDeadzone << "\n";
    file << "vibrationEnabled=" << (impl_->vibrationEnabled ? 1 : 0) << "\n";
    
    file.close();
    return true;
}

bool SettingsManager::load(const std::string& filepath) {
    // Minimal implementation to pass test (GREEN phase)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string valueStr = line.substr(equalPos + 1);
            
            // Audio settings
            if (key == "masterVolume") {
                impl_->masterVolume = std::stof(valueStr);
            } else if (key == "soundEffectVolume") {
                impl_->soundEffectVolume = std::stof(valueStr);
            } else if (key == "musicVolume") {
                impl_->musicVolume = std::stof(valueStr);
            }
            // Video settings
            else if (key == "resolutionWidth") {
                impl_->resolutionWidth = std::stoi(valueStr);
            } else if (key == "resolutionHeight") {
                impl_->resolutionHeight = std::stoi(valueStr);
            } else if (key == "graphicsQuality") {
                impl_->graphicsQuality = static_cast<GraphicsQuality>(std::stoi(valueStr));
            } else if (key == "fullscreen") {
                impl_->fullscreen = (std::stoi(valueStr) != 0);
            }
            // Control settings
            else if (key == "gamepadSensitivity") {
                impl_->gamepadSensitivity = std::stof(valueStr);
            } else if (key == "gamepadDeadzone") {
                impl_->gamepadDeadzone = std::stof(valueStr);
            } else if (key == "vibrationEnabled") {
                impl_->vibrationEnabled = (std::stoi(valueStr) != 0);
            }
        }
    }
    
    file.close();
    return true;
}

// Video settings implementation
int SettingsManager::getResolutionWidth() const {
    return impl_->resolutionWidth;
}

int SettingsManager::getResolutionHeight() const {
    return impl_->resolutionHeight;
}

void SettingsManager::setResolution(int width, int height) {
    impl_->resolutionWidth = width;
    impl_->resolutionHeight = height;
}

SettingsManager::GraphicsQuality SettingsManager::getGraphicsQuality() const {
    return impl_->graphicsQuality;
}

void SettingsManager::setGraphicsQuality(GraphicsQuality quality) {
    impl_->graphicsQuality = quality;
}

bool SettingsManager::isFullscreen() const {
    return impl_->fullscreen;
}

void SettingsManager::setFullscreen(bool fullscreen) {
    impl_->fullscreen = fullscreen;
}

// Control settings implementation
float SettingsManager::getGamepadSensitivity() const {
    return impl_->gamepadSensitivity;
}

void SettingsManager::setGamepadSensitivity(float sensitivity) {
    impl_->gamepadSensitivity = sensitivity;
}

float SettingsManager::getGamepadDeadzone() const {
    return impl_->gamepadDeadzone;
}

void SettingsManager::setGamepadDeadzone(float deadzone) {
    impl_->gamepadDeadzone = deadzone;
}

bool SettingsManager::isVibrationEnabled() const {
    return impl_->vibrationEnabled;
}

void SettingsManager::setVibrationEnabled(bool enabled) {
    impl_->vibrationEnabled = enabled;
}

} // namespace d2