#include "core/settings_manager.h"
#include <fstream>
#include <sstream>

namespace d2 {

struct SettingsManager::Impl {
    float masterVolume = 1.0f;
    float soundEffectVolume = 1.0f;
    float musicVolume = 1.0f;
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
    file << "masterVolume=" << impl_->masterVolume << "\n";
    file << "soundEffectVolume=" << impl_->soundEffectVolume << "\n";
    file << "musicVolume=" << impl_->musicVolume << "\n";
    
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
            float value = std::stof(line.substr(equalPos + 1));
            
            if (key == "masterVolume") {
                impl_->masterVolume = value;
            } else if (key == "soundEffectVolume") {
                impl_->soundEffectVolume = value;
            } else if (key == "musicVolume") {
                impl_->musicVolume = value;
            }
        }
    }
    
    file.close();
    return true;
}

} // namespace d2