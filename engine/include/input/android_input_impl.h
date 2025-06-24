#pragma once
#include "input/android_input.h"
#include <vector>

namespace d2::input {

class AndroidInputImpl : public AndroidInput {
public:
    AndroidInputImpl() = default;
    ~AndroidInputImpl() override = default;
    
    std::vector<int> getConnectedDevices() const override;
    bool isGamepad(int deviceId) const override;
    
    // Add a device with its input source type
    void addDevice(int deviceId, int inputSource);
    
    // Remove a device
    void removeDevice(int deviceId);
    
private:
    struct DeviceInfo {
        int deviceId;
        int inputSource;
    };
    
    std::vector<DeviceInfo> devices_;
};

} // namespace d2::input