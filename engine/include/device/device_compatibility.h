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
    
    // Check if OpenGL ES version meets minimum requirements
    bool isOpenGLESVersionCompatible(int major, int minor) const {
        // Minimum OpenGL ES 3.0 required
        if (major > 3) return true;
        if (major == 3 && minor >= 0) return true;
        return false;
    }
};

} // namespace d2::device