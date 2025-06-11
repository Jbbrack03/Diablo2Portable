#pragma once

namespace d2::input {

class Gamepad {
public:
    virtual ~Gamepad() = default;
    
    virtual bool getButton(int buttonId) const = 0;
    virtual float getAxis(int axisId) const = 0;
    virtual bool isConnected() const = 0;
};

} // namespace d2::input