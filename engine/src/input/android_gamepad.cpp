#include "input/android_gamepad.h"
#include <cmath>

namespace d2::input {

AndroidGamepad::AndroidGamepad(int deviceId)
    : deviceId_(deviceId), connected_(true) {
}

bool AndroidGamepad::getButton(int buttonId) const {
    auto it = buttonStates_.find(buttonId);
    return it != buttonStates_.end() ? it->second : false;
}

float AndroidGamepad::getAxis(int axisId) const {
    auto it = axisValues_.find(axisId);
    if (it != axisValues_.end()) {
        return applyDeadzone(it->second);
    }
    return 0.0f;
}

bool AndroidGamepad::isConnected() const {
    return connected_;
}

void AndroidGamepad::setButton(int buttonId, bool pressed) {
    buttonStates_[buttonId] = pressed;
}

void AndroidGamepad::setAxis(int axisId, float value) {
    axisValues_[axisId] = value;
}

void AndroidGamepad::setConnected(bool connected) {
    connected_ = connected;
}

void AndroidGamepad::setDeadzone(float deadzone) {
    deadzone_ = std::max(0.0f, std::min(1.0f, deadzone)); // Clamp to [0, 1]
}

float AndroidGamepad::applyDeadzone(float value) const {
    float absValue = std::abs(value);
    
    if (absValue < deadzone_) {
        return 0.0f;
    }
    
    // Scale the value to use the full range outside the deadzone
    float sign = value < 0.0f ? -1.0f : 1.0f;
    float scaledValue = (absValue - deadzone_) / (1.0f - deadzone_);
    
    return sign * scaledValue;
}

} // namespace d2::input