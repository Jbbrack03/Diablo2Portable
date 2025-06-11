#include "input/gamepad_manager.h"
#include "input/android_input.h"

namespace d2::input {

GamepadManager::GamepadManager(AndroidInput* androidInput)
    : androidInput_(androidInput) {
}

bool GamepadManager::hasController() const {
    auto devices = androidInput_->getConnectedDevices();
    
    for (int deviceId : devices) {
        if (androidInput_->isGamepad(deviceId)) {
            return true;
        }
    }
    
    return false;
}

} // namespace d2::input