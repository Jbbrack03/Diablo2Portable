#pragma once

namespace d2::input {

class AndroidInput;

class GamepadManager {
public:
    explicit GamepadManager(AndroidInput* androidInput);
    
    bool hasController() const;

private:
    AndroidInput* androidInput_;
};

} // namespace d2::input