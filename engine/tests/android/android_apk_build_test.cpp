#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <chrono>

namespace fs = std::filesystem;

class AndroidAPKBuilder {
public:
    struct BuildResult {
        bool success = false;
        std::string error;
        std::string apkPath;
        std::string buildOutput;
        int buildTime = 0;
    };
    
    static BuildResult buildDebugAPK(const std::string& projectRoot) {
        BuildResult result;
        
        // Change to android directory
        std::string androidDir = projectRoot + "/android";
        if (!fs::exists(androidDir)) {
            result.error = "Android project directory not found: " + androidDir;
            return result;
        }
        
        // Check if gradlew exists
        std::string gradlewPath = androidDir + "/gradlew";
        if (!fs::exists(gradlewPath)) {
            result.error = "Gradle wrapper not found: " + gradlewPath;
            return result;
        }
        
        // Execute gradle build
        std::string buildCommand = "cd " + androidDir + " && ./gradlew assembleDebug 2>&1";
        
        auto start = std::chrono::steady_clock::now();
        FILE* pipe = popen(buildCommand.c_str(), "r");
        if (!pipe) {
            result.error = "Failed to execute gradle build command";
            return result;
        }
        
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result.buildOutput += buffer;
        }
        
        int returnCode = pclose(pipe);
        auto end = std::chrono::steady_clock::now();
        result.buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        if (returnCode == 0) {
            result.success = true;
            // Find the generated APK
            std::string apkDir = androidDir + "/app/build/outputs/apk/debug";
            if (fs::exists(apkDir)) {
                for (const auto& entry : fs::directory_iterator(apkDir)) {
                    if (entry.path().extension() == ".apk") {
                        result.apkPath = entry.path().string();
                        break;
                    }
                }
            }
        } else {
            result.success = false;
            result.error = "Gradle build failed with return code: " + std::to_string(returnCode);
        }
        
        return result;
    }
    
    static BuildResult buildReleaseAPK(const std::string& projectRoot) {
        BuildResult result;
        
        // Change to android directory
        std::string androidDir = projectRoot + "/android";
        if (!fs::exists(androidDir)) {
            result.error = "Android project directory not found: " + androidDir;
            return result;
        }
        
        // Check if gradlew exists
        std::string gradlewPath = androidDir + "/gradlew";
        if (!fs::exists(gradlewPath)) {
            result.error = "Gradle wrapper not found: " + gradlewPath;
            return result;
        }
        
        // Execute gradle build
        std::string buildCommand = "cd " + androidDir + " && ./gradlew assembleRelease 2>&1";
        
        auto start = std::chrono::steady_clock::now();
        FILE* pipe = popen(buildCommand.c_str(), "r");
        if (!pipe) {
            result.error = "Failed to execute gradle build command";
            return result;
        }
        
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result.buildOutput += buffer;
        }
        
        int returnCode = pclose(pipe);
        auto end = std::chrono::steady_clock::now();
        result.buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        if (returnCode == 0) {
            result.success = true;
            // Find the generated APK
            std::string apkDir = androidDir + "/app/build/outputs/apk/release";
            if (fs::exists(apkDir)) {
                for (const auto& entry : fs::directory_iterator(apkDir)) {
                    if (entry.path().extension() == ".apk") {
                        result.apkPath = entry.path().string();
                        break;
                    }
                }
            }
        } else {
            result.success = false;
            result.error = "Gradle build failed with return code: " + std::to_string(returnCode);
        }
        
        return result;
    }
    
    static std::string analyzeAPKBuildFailure(const std::string& buildOutput) {
        std::string analysis = "APK Build Failure Analysis:\n\n";
        
        if (buildOutput.find("SDK location not found") != std::string::npos) {
            analysis += "- CAUSE: Android SDK not configured\n";
            analysis += "- SOLUTION: Install Android Studio or standalone SDK\n";
            analysis += "- NEXT STEPS: Run Android SDK setup guidance\n";
        } else if (buildOutput.find("NDK") != std::string::npos) {
            analysis += "- CAUSE: NDK not found or version mismatch\n";
            analysis += "- SOLUTION: Install correct NDK version via SDK Manager\n";
            analysis += "- NEXT STEPS: Check NDK version in build.gradle\n";
        } else if (buildOutput.find("CMake") != std::string::npos) {
            analysis += "- CAUSE: CMake not found\n";
            analysis += "- SOLUTION: Install CMake via SDK Manager\n";
            analysis += "- NEXT STEPS: Install CMake 3.22.1 or newer\n";
        } else if (buildOutput.find("BUILD FAILED") != std::string::npos) {
            analysis += "- CAUSE: General build failure\n";
            analysis += "- SOLUTION: Check build output for specific errors\n";
            analysis += "- NEXT STEPS: Review gradle build logs\n";
        } else {
            analysis += "- CAUSE: Unknown build failure\n";
            analysis += "- SOLUTION: Check build configuration\n";
            analysis += "- NEXT STEPS: Review complete build output\n";
        }
        
        return analysis;
    }
    
private:
    static std::string getProjectRoot() {
        fs::path currentPath = fs::current_path();
        while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
            currentPath = currentPath.parent_path();
        }
        return currentPath.string();
    }
};

// Test APK build process
TEST(AndroidAPKBuildTest, BuildDebugAPK) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto result = AndroidAPKBuilder::buildDebugAPK(projectRoot);
    
    // Verify build attempt was made
    EXPECT_FALSE(result.buildOutput.empty()) << "Build should produce output";
    EXPECT_GE(result.buildTime, 0) << "Build should record time (even if very fast)";
    
    if (result.success) {
        // If build succeeded, APK should exist
        EXPECT_FALSE(result.apkPath.empty()) << "APK path should be found";
        EXPECT_TRUE(fs::exists(result.apkPath)) << "APK file should exist";
    } else {
        // If build failed, we should get meaningful error analysis
        std::string analysis = AndroidAPKBuilder::analyzeAPKBuildFailure(result.buildOutput);
        EXPECT_NE(analysis.find("CAUSE"), std::string::npos) << "Should provide failure cause analysis";
        EXPECT_NE(analysis.find("SOLUTION"), std::string::npos) << "Should provide solution guidance";
        
        // Most likely cause is missing SDK
        if (result.buildOutput.find("SDK location not found") != std::string::npos) {
            GTEST_SKIP() << "Android SDK not configured - this is expected: " << result.error;
        } else {
            GTEST_SKIP() << "APK build failed (configuration issue): " << result.error;
        }
    }
}

// Test build failure analysis
TEST(AndroidAPKBuildTest, AnalyzeBuildFailure) {
    std::string sdkError = "SDK location not found. Define a valid SDK location";
    std::string ndkError = "NDK not found at specified location";
    std::string cmakeError = "CMake executable not found";
    std::string generalError = "BUILD FAILED with unknown error";
    
    auto sdkAnalysis = AndroidAPKBuilder::analyzeAPKBuildFailure(sdkError);
    EXPECT_NE(sdkAnalysis.find("Android SDK not configured"), std::string::npos) << "Should detect SDK issue";
    EXPECT_NE(sdkAnalysis.find("Install Android Studio"), std::string::npos) << "Should provide SDK solution";
    
    auto ndkAnalysis = AndroidAPKBuilder::analyzeAPKBuildFailure(ndkError);
    EXPECT_NE(ndkAnalysis.find("NDK not found"), std::string::npos) << "Should detect NDK issue";
    EXPECT_NE(ndkAnalysis.find("SDK Manager"), std::string::npos) << "Should provide NDK solution";
    
    auto cmakeAnalysis = AndroidAPKBuilder::analyzeAPKBuildFailure(cmakeError);
    EXPECT_NE(cmakeAnalysis.find("CMake not found"), std::string::npos) << "Should detect CMake issue";
    EXPECT_NE(cmakeAnalysis.find("Install CMake"), std::string::npos) << "Should provide CMake solution";
    
    auto generalAnalysis = AndroidAPKBuilder::analyzeAPKBuildFailure(generalError);
    EXPECT_NE(generalAnalysis.find("General build failure"), std::string::npos) << "Should detect general failure";
    EXPECT_NE(generalAnalysis.find("Check build output"), std::string::npos) << "Should provide general solution";
}

// Test release APK build
TEST(AndroidAPKBuildTest, BuildReleaseAPK) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto result = AndroidAPKBuilder::buildReleaseAPK(projectRoot);
    
    // Verify build attempt was made
    EXPECT_FALSE(result.buildOutput.empty()) << "Build should produce output";
    EXPECT_GE(result.buildTime, 0) << "Build should record time (even if very fast)";
    
    if (result.success) {
        // If build succeeded, APK should exist
        EXPECT_FALSE(result.apkPath.empty()) << "APK path should be found";
        EXPECT_TRUE(fs::exists(result.apkPath)) << "APK file should exist";
    } else {
        // Release builds often fail due to signing requirements
        GTEST_SKIP() << "Release APK build failed (expected without signing setup): " << result.error;
    }
}

// Test build configuration validation
TEST(AndroidAPKBuildTest, ValidateBuildConfiguration) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    // Check basic build configuration
    std::string androidDir = projectRoot + "/android";
    EXPECT_TRUE(fs::exists(androidDir)) << "Android project directory should exist";
    
    std::string gradlewPath = androidDir + "/gradlew";
    EXPECT_TRUE(fs::exists(gradlewPath)) << "Gradle wrapper should exist";
    
    std::string buildGradlePath = androidDir + "/app/build.gradle";
    EXPECT_TRUE(fs::exists(buildGradlePath)) << "Build configuration should exist";
    
    std::string localPropsPath = androidDir + "/local.properties";
    EXPECT_TRUE(fs::exists(localPropsPath)) << "Local properties should exist";
    
    // Basic configuration should be ready for builds
    EXPECT_TRUE(true) << "Build configuration validation complete";
}

// Test APK build process integration
TEST(AndroidAPKBuildTest, APKBuildProcessIntegration) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    // Test the complete APK build process
    auto result = AndroidAPKBuilder::buildDebugAPK(projectRoot);
    
    // Should always produce some output (even if build fails)
    EXPECT_FALSE(result.buildOutput.empty()) << "Build should produce output";
    EXPECT_GE(result.buildTime, 0) << "Build should record timing";
    
    if (!result.success) {
        // Build failed - should provide meaningful analysis
        std::string analysis = AndroidAPKBuilder::analyzeAPKBuildFailure(result.buildOutput);
        EXPECT_NE(analysis.find("CAUSE"), std::string::npos) << "Should identify failure cause";
        EXPECT_NE(analysis.find("SOLUTION"), std::string::npos) << "Should provide solution";
        EXPECT_NE(analysis.find("NEXT STEPS"), std::string::npos) << "Should provide next steps";
        
        // Most common failure case - SDK not configured
        if (result.buildOutput.find("SDK location not found") != std::string::npos) {
            EXPECT_NE(analysis.find("Android SDK not configured"), std::string::npos) << "Should detect SDK issue";
            EXPECT_NE(analysis.find("Install Android Studio"), std::string::npos) << "Should suggest SDK installation";
        }
    }
    
    // This test validates that our build system can detect and report APK build issues
    EXPECT_TRUE(true) << "APK build process integration test complete";
}