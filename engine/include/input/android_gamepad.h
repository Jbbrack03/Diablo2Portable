#pragma once

#include "input/gamepad.h"
#include <unordered_map>

namespace d2::input {

class AndroidGamepad : public Gamepad {
public:
    explicit AndroidGamepad(int deviceId);
    ~AndroidGamepad() override = default;
    
    bool getButton(int buttonId) const override;
    float getAxis(int axisId) const override;
    bool isConnected() const override;
    
    // Methods to update state (called by Android input system)
    void setButton(int buttonId, bool pressed);
    void setAxis(int axisId, float value);
    void setConnected(bool connected);
    
    // Deadzone configuration
    void setDeadzone(float deadzone);
    float getDeadzone() const { return deadzone_; }
    
private:
    float applyDeadzone(float value) const;
    
    int deviceId_;
    bool connected_;
    float deadzone_ = 0.1f; // Default 10% deadzone
    std::unordered_map<int, bool> buttonStates_;
    std::unordered_map<int, float> axisValues_;
};

} // namespace d2::input