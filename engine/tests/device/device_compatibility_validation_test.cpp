#include <gtest/gtest.h>
#include "device/device_compatibility.h"
#include <string>
#include <vector>

// Task 24.4: Device Compatibility Validation
class DeviceCompatibilityValidationTest : public ::testing::Test {
protected:
    void SetUp() override {
        compatibility_ = std::make_unique<d2::device::DeviceCompatibility>();
    }
    
    std::unique_ptr<d2::device::DeviceCompatibility> compatibility_;
};

// Test 1: Retroid Pocket Flip 2 validation
TEST_F(DeviceCompatibilityValidationTest, RetroidPocketFlip2Validation) {
    // Simulate Retroid Pocket Flip 2 specifications
    int apiLevel = 30; // Android 11
    int openGLMajor = 3;
    int openGLMinor = 0;
    size_t ramMB = 4096; // 4GB RAM
    std::string controller = "Xbox Wireless Controller";
    
    // Check compatibility
    EXPECT_TRUE(compatibility_->isAndroidVersionCompatible(apiLevel)) 
        << "Retroid Pocket Flip 2 Android version should be compatible";
    EXPECT_TRUE(compatibility_->isOpenGLESVersionCompatible(openGLMajor, openGLMinor))
        << "Retroid Pocket Flip 2 OpenGL ES should be compatible";
    EXPECT_TRUE(compatibility_->isRAMSufficient(ramMB))
        << "Retroid Pocket Flip 2 RAM should be sufficient";
    EXPECT_TRUE(compatibility_->isControllerCompatible(controller))
        << "Retroid Pocket Flip 2 controller should be compatible";
}

// Test 2: Minimum requirements validation
TEST_F(DeviceCompatibilityValidationTest, MinimumRequirementsValidation) {
    // Test device at minimum specs
    int apiLevel = 26; // Android 8.0
    int openGLMajor = 3;
    int openGLMinor = 0;
    size_t ramMB = 1536; // 1.5GB RAM (minimum)
    std::string controller = ""; // No controller
    
    EXPECT_TRUE(compatibility_->isAndroidVersionCompatible(apiLevel)) 
        << "Minimum Android version should be compatible";
    EXPECT_TRUE(compatibility_->isOpenGLESVersionCompatible(openGLMajor, openGLMinor))
        << "Minimum OpenGL ES should be compatible";
    EXPECT_TRUE(compatibility_->isRAMSufficient(ramMB))
        << "Minimum RAM should be sufficient";
    EXPECT_TRUE(compatibility_->isControllerCompatible(controller))
        << "Controller is always compatible (external can be connected)";
}

// Test 3: Incompatible device detection
TEST_F(DeviceCompatibilityValidationTest, IncompatibleDeviceDetection) {
    // Old device that doesn't meet requirements
    int apiLevel = 24; // Android 7.0
    int openGLMajor = 2;
    int openGLMinor = 0;
    size_t ramMB = 1024; // 1GB RAM
    
    EXPECT_FALSE(compatibility_->isAndroidVersionCompatible(apiLevel)) 
        << "Android 7 should not be compatible";
    EXPECT_FALSE(compatibility_->isOpenGLESVersionCompatible(openGLMajor, openGLMinor))
        << "OpenGL ES 2.0 should not be compatible";
    EXPECT_FALSE(compatibility_->isRAMSufficient(ramMB))
        << "1GB RAM should not be sufficient";
}

// Test 4: Edge cases
TEST_F(DeviceCompatibilityValidationTest, EdgeCases) {
    // Exactly at minimum API level
    EXPECT_TRUE(compatibility_->isAndroidVersionCompatible(26));
    EXPECT_FALSE(compatibility_->isAndroidVersionCompatible(25));
    
    // OpenGL ES version checks
    EXPECT_TRUE(compatibility_->isOpenGLESVersionCompatible(3, 0));
    EXPECT_TRUE(compatibility_->isOpenGLESVersionCompatible(3, 1));
    EXPECT_TRUE(compatibility_->isOpenGLESVersionCompatible(3, 2));
    EXPECT_TRUE(compatibility_->isOpenGLESVersionCompatible(4, 0));
    EXPECT_FALSE(compatibility_->isOpenGLESVersionCompatible(2, 0));
    
    // RAM exactly at limit
    EXPECT_TRUE(compatibility_->isRAMSufficient(1536)); // Exactly 1.5GB
    EXPECT_FALSE(compatibility_->isRAMSufficient(1535)); // Just under
}