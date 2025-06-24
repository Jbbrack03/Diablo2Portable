#include "core/settings_manager.h"

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

} // namespace d2