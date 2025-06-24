#include "input/android_input_impl.h"
#include <algorithm>

namespace d2::input {

// Android input source types - these match Android's InputDevice constants
constexpr int AINPUT_SOURCE_GAMEPAD = 0x00000401;
constexpr int AINPUT_SOURCE_JOYSTICK = 0x01000010;

std::vector<int> AndroidInputImpl::getConnectedDevices() const {
    std::vector<int> deviceIds;
    for (const auto& device : devices_) {
        deviceIds.push_back(device.deviceId);
    }
    return deviceIds;
}

bool AndroidInputImpl::isGamepad(int deviceId) const {
    auto it = std::find_if(devices_.begin(), devices_.end(),
        [deviceId](const DeviceInfo& device) {
            return device.deviceId == deviceId;
        });
    
    if (it != devices_.end()) {
        // A device is a gamepad if it has either GAMEPAD or JOYSTICK flag
        return ((it->inputSource & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD) ||
               ((it->inputSource & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK);
    }
    
    return false;
}

void AndroidInputImpl::addDevice(int deviceId, int inputSource) {
    // Check if device already exists
    auto it = std::find_if(devices_.begin(), devices_.end(),
        [deviceId](const DeviceInfo& device) {
            return device.deviceId == deviceId;
        });
    
    if (it == devices_.end()) {
        devices_.push_back({deviceId, inputSource});
    } else {
        // Update input source if device already exists
        it->inputSource = inputSource;
    }
}

void AndroidInputImpl::removeDevice(int deviceId) {
    devices_.erase(
        std::remove_if(devices_.begin(), devices_.end(),
            [deviceId](const DeviceInfo& device) {
                return device.deviceId == deviceId;
            }),
        devices_.end()
    );
}

} // namespace d2::input