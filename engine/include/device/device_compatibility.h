#pragma once

#include <string>

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
    
    // Check if OpenGL ES version meets minimum requirements
    bool isOpenGLESVersionCompatible(int major, int minor) const {
        // Minimum OpenGL ES 3.0 required
        if (major > 3) return true;
        if (major == 3 && minor >= 0) return true;
        return false;
    }
    
    // Check if RAM meets minimum requirements
    bool isRAMSufficient(size_t ramMB) const {
        // Minimum 1.5GB (1536MB) required
        return ramMB >= 1536;
    }
    
    // Check if controller is compatible
    bool isControllerCompatible(const std::string& controllerName) const {
        // We support any gamepad/controller
        return true;
    }
};

} // namespace d2::device