#pragma once

namespace d2::device {

// Device compatibility checker for Phase 20 Task 2
class DeviceCompatibility {
public:
    DeviceCompatibility() = default;
    ~DeviceCompatibility() = default;
    
    // Check if Android version meets minimum requirements
    bool isAndroidVersionCompatible(int apiLevel) const {
        // Minimum API level is 26 (Android 8.0)
        return apiLevel >= 26;
    }
};

} // namespace d2::device