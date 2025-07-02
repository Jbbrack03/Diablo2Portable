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

// TEST 4: Check RAM requirements
TEST(DeviceCompatibilityTest, CheckRAMRequirements) {
    DeviceCompatibility checker;
    
    // 2GB RAM should be compatible (plenty for 1.5GB requirement)
    bool result = checker.isRAMSufficient(2048); // MB
    EXPECT_TRUE(result);
    
    // 1.5GB RAM should be compatible (exact minimum)
    result = checker.isRAMSufficient(1536);
    EXPECT_TRUE(result);
    
    // 1GB RAM should not be compatible
    result = checker.isRAMSufficient(1024);
    EXPECT_FALSE(result);
}