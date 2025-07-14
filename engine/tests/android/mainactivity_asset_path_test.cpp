#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

// Following TDD methodology - write test first to identify the problem
namespace fs = std::filesystem;

class MainActivityAssetPathTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test
        android_activity_path = fs::current_path() / "android/app/src/main/java/com/diablo2portable/MainActivity.java";
        native_engine_path = fs::current_path() / "android/app/src/main/java/com/diablo2portable/NativeEngine.java";
    }
    
    fs::path android_activity_path;
    fs::path native_engine_path;
};

// STEP 1: Write exactly ONE failing test
TEST_F(MainActivityAssetPathTest, MainActivityInitializesNativeEngineWithContext) {
    // Test that MainActivity properly initializes NativeEngine with Context
    ASSERT_TRUE(fs::exists(android_activity_path)) << "MainActivity.java should exist";
    
    std::ifstream mainActivityFile(android_activity_path);
    ASSERT_TRUE(mainActivityFile.is_open()) << "Should be able to read MainActivity.java";
    
    std::string content((std::istreambuf_iterator<char>(mainActivityFile)),
                        std::istreambuf_iterator<char>());
    
    // Check that NativeEngine is initialized with context
    EXPECT_TRUE(content.find("nativeEngine = new NativeEngine()") != std::string::npos);
    
    // Check that initialize() is called with context
    EXPECT_TRUE(content.find("nativeEngine.initialize(this)") != std::string::npos ||
                content.find("nativeEngine.initialize(MainActivity.this)") != std::string::npos) 
                << "NativeEngine should be initialized with context";
}

// STEP 2: Write test for proper asset path handling
TEST_F(MainActivityAssetPathTest, MainActivityHandlesAssetPathCorrectly) {
    // Test that MainActivity handles asset path through OnboardingHelper
    ASSERT_TRUE(fs::exists(android_activity_path));
    
    std::ifstream mainActivityFile(android_activity_path);
    ASSERT_TRUE(mainActivityFile.is_open());
    
    std::string content((std::istreambuf_iterator<char>(mainActivityFile)),
                        std::istreambuf_iterator<char>());
    
    // Check that OnboardingHelper is used to get asset path
    EXPECT_TRUE(content.find("OnboardingHelper") != std::string::npos);
    
    // Check that asset path is passed to native engine
    EXPECT_TRUE(content.find("nativeEngine.initialize") != std::string::npos);
    
    // Check that we handle the case where assets are already extracted
    EXPECT_TRUE(content.find("onboardingHelper.isFirstRun()") != std::string::npos);
}

// STEP 3: Write test for NativeEngine initialization
TEST_F(MainActivityAssetPathTest, NativeEngineProperlyInitialized) {
    // Test that NativeEngine is properly initialized in MainActivity
    ASSERT_TRUE(fs::exists(android_activity_path));
    
    std::ifstream mainActivityFile(android_activity_path);
    ASSERT_TRUE(mainActivityFile.is_open());
    
    std::string content((std::istreambuf_iterator<char>(mainActivityFile)),
                        std::istreambuf_iterator<char>());
    
    // Check that NativeEngine initialization is called
    EXPECT_TRUE(content.find("nativeEngine.initialize") != std::string::npos) 
                << "NativeEngine should be properly initialized";
    
    // Check that the initialization happens after onboarding check
    size_t onboardingPos = content.find("onboardingHelper.isFirstRun()");
    size_t initPos = content.find("nativeEngine.initialize");
    
    if (onboardingPos != std::string::npos && initPos != std::string::npos) {
        EXPECT_GT(initPos, onboardingPos) << "NativeEngine should be initialized after onboarding check";
    }
}

// STEP 4: Write test for error handling
TEST_F(MainActivityAssetPathTest, MainActivityHandlesInitializationFailure) {
    // Test that MainActivity handles NativeEngine initialization failure
    ASSERT_TRUE(fs::exists(android_activity_path));
    
    std::ifstream mainActivityFile(android_activity_path);
    ASSERT_TRUE(mainActivityFile.is_open());
    
    std::string content((std::istreambuf_iterator<char>(mainActivityFile)),
                        std::istreambuf_iterator<char>());
    
    // Check that initialization result is checked
    EXPECT_TRUE(content.find("if (") != std::string::npos &&
                content.find("nativeEngine.initialize") != std::string::npos) 
                << "Should check initialization result";
}

// STEP 5: Write test for GameSurfaceView integration
TEST_F(MainActivityAssetPathTest, GameSurfaceViewUsesInitializedEngine) {
    // Test that GameSurfaceView receives properly initialized NativeEngine
    ASSERT_TRUE(fs::exists(android_activity_path));
    
    std::ifstream mainActivityFile(android_activity_path);
    ASSERT_TRUE(mainActivityFile.is_open());
    
    std::string content((std::istreambuf_iterator<char>(mainActivityFile)),
                        std::istreambuf_iterator<char>());
    
    // Check that GameSurfaceView is created with initialized NativeEngine
    EXPECT_TRUE(content.find("GameSurfaceView") != std::string::npos);
    EXPECT_TRUE(content.find("nativeEngine") != std::string::npos);
    
    // Check that NativeEngine is passed to GameSurfaceView
    EXPECT_TRUE(content.find("GameSurfaceView(this, nativeEngine)") != std::string::npos ||
                content.find("new GameSurfaceView(this, nativeEngine)") != std::string::npos);
}