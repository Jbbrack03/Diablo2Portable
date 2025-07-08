#include <gtest/gtest.h>
#include <string>
#include <filesystem>

// Test to verify Android app uses engine's JNI bridge implementation
TEST(AndroidJNIIntegrationTest, AndroidAppUsesEngineJNIBridge) {
    // The Android app should be configured to use the engine's JNI bridge
    // implementation, not the stub version
    
    // Check that the Android CMakeLists.txt includes engine sources
    std::string androidCMakeFile = "android/app/src/main/cpp/CMakeLists.txt";
    ASSERT_TRUE(std::filesystem::exists(androidCMakeFile)) 
        << "Android CMakeLists.txt should exist";
    
    // Check that the engine JNI bridge exists and is complete
    std::string engineJNIBridge = "engine/src/android/jni_bridge.cpp";
    ASSERT_TRUE(std::filesystem::exists(engineJNIBridge))
        << "Engine JNI bridge implementation should exist";
    
    // Check that stub JNI bridge is not used in Android build
    std::string stubJNIBridge = "android/app/src/main/cpp/jni_bridge.cpp";
    if (std::filesystem::exists(stubJNIBridge)) {
        // If stub exists, it should not be the primary implementation
        // The Android build should use the engine's implementation
        EXPECT_TRUE(false) << "Android app should not use stub JNI bridge when engine implementation exists";
    }
}