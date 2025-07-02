#include <gtest/gtest.h>
#include "device/device_compatibility.h"
#include <string>
#include <vector>

using namespace d2::device;

// Phase 20 Task 2: Device Testing - Following strict TDD
// These tests verify device compatibility and performance requirements

// TEST 1: Create device compatibility checker
TEST(DeviceCompatibilityTest, CreateDeviceChecker) {
    DeviceCompatibility checker;
    // Device compatibility checker should exist - that's all for now
}

// TEST 2: Check Android version compatibility
TEST(DeviceCompatibilityTest, CheckAndroidVersion) {
    DeviceCompatibility checker;
    
    // Android 8.0 (API 26) should be compatible
    bool result = checker.isAndroidVersionCompatible(26);
    EXPECT_TRUE(result);
    
    // Android 7.0 (API 25) should not be compatible
    result = checker.isAndroidVersionCompatible(25);
    EXPECT_FALSE(result);
}

// TEST 3: Check OpenGL ES version compatibility
TEST(DeviceCompatibilityTest, CheckOpenGLESVersion) {
    DeviceCompatibility checker;
    
    // OpenGL ES 3.0 should be compatible
    bool result = checker.isOpenGLESVersionCompatible(3, 0);
    EXPECT_TRUE(result);
    
    // OpenGL ES 3.1 should be compatible
    result = checker.isOpenGLESVersionCompatible(3, 1);
    EXPECT_TRUE(result);
    
    // OpenGL ES 2.0 should not be compatible
    result = checker.isOpenGLESVersionCompatible(2, 0);
    EXPECT_FALSE(result);
}