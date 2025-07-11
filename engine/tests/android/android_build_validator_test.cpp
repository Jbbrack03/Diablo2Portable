#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <regex>
#include <cstdlib>

namespace fs = std::filesystem;

class AndroidBuildValidator {
public:
    struct BuildConfiguration {
        std::string sdkPath;
        std::string ndkVersion;
        int compileSdk = 0;
        int minSdk = 0;
        int targetSdk = 0;
        bool sdkExists = false;
        bool ndkExists = false;
        bool gradleWrapperExists = false;
        std::string error;
    };
    
    static BuildConfiguration validateAndroidBuild(const std::string& projectRoot) {
        BuildConfiguration config;
        
        // Check local.properties for SDK path
        std::string localPropsPath = projectRoot + "/android/local.properties";
        if (fs::exists(localPropsPath)) {
            std::ifstream file(localPropsPath);
            std::string line;
            while (std::getline(file, line)) {
                if (line.find("sdk.dir=") == 0) {
                    config.sdkPath = line.substr(8); // Remove "sdk.dir="
                    break;
                }
            }
        }
        
        // Check if SDK path exists
        if (!config.sdkPath.empty()) {
            config.sdkExists = fs::exists(config.sdkPath);
        }
        
        // Parse app/build.gradle for configuration
        std::string buildGradlePath = projectRoot + "/android/app/build.gradle";
        if (fs::exists(buildGradlePath)) {
            std::ifstream file(buildGradlePath);
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Extract SDK versions
            std::regex compileSdkRegex(R"(compileSdk\s+(\d+))");
            std::regex minSdkRegex(R"(minSdk\s+(\d+))");
            std::regex targetSdkRegex(R"(targetSdk\s+(\d+))");
            std::regex ndkVersionRegex("ndkVersion\\s+\"([^\"]+)\"");
            
            std::smatch match;
            if (std::regex_search(content, match, compileSdkRegex)) {
                config.compileSdk = std::stoi(match[1]);
            }
            if (std::regex_search(content, match, minSdkRegex)) {
                config.minSdk = std::stoi(match[1]);
            }
            if (std::regex_search(content, match, targetSdkRegex)) {
                config.targetSdk = std::stoi(match[1]);
            }
            if (std::regex_search(content, match, ndkVersionRegex)) {
                config.ndkVersion = match[1];
            }
        }
        
        // Check NDK existence
        if (!config.sdkPath.empty() && !config.ndkVersion.empty()) {
            std::string ndkPath = config.sdkPath + "/ndk/" + config.ndkVersion;
            config.ndkExists = fs::exists(ndkPath);
        }
        
        // Check gradle wrapper
        std::string gradlewPath = projectRoot + "/android/gradlew";
        config.gradleWrapperExists = fs::exists(gradlewPath);
        
        // Generate error messages if needed
        if (config.sdkPath.empty()) {
            config.error = "SDK path not configured in local.properties";
        } else if (!config.sdkExists) {
            config.error = "SDK path does not exist: " + config.sdkPath;
        } else if (!config.ndkExists) {
            config.error = "NDK not found at: " + config.sdkPath + "/ndk/" + config.ndkVersion;
        }
        
        return config;
    }
    
    static bool isValidBuildConfiguration(const BuildConfiguration& config) {
        return config.sdkExists && 
               config.ndkExists && 
               config.gradleWrapperExists && 
               config.compileSdk > 0 && 
               config.minSdk > 0 && 
               config.targetSdk > 0;
    }
    
    static std::string generateBuildReport(const BuildConfiguration& config) {
        std::string report = "# Android Build Configuration Report\n\n";
        
        report += "## SDK Configuration\n";
        report += "- **SDK Path**: " + config.sdkPath + "\n";
        report += "- **SDK Exists**: " + std::string(config.sdkExists ? "✅" : "❌") + "\n";
        report += "- **NDK Version**: " + config.ndkVersion + "\n";
        report += "- **NDK Exists**: " + std::string(config.ndkExists ? "✅" : "❌") + "\n\n";
        
        report += "## Build Configuration\n";
        report += "- **Compile SDK**: " + std::to_string(config.compileSdk) + "\n";
        report += "- **Min SDK**: " + std::to_string(config.minSdk) + "\n";
        report += "- **Target SDK**: " + std::to_string(config.targetSdk) + "\n";
        report += "- **Gradle Wrapper**: " + std::string(config.gradleWrapperExists ? "✅" : "❌") + "\n\n";
        
        report += "## Status\n";
        if (isValidBuildConfiguration(config)) {
            report += "- **Overall**: ✅ READY TO BUILD\n";
        } else {
            report += "- **Overall**: ❌ BUILD CONFIGURATION INCOMPLETE\n";
            if (!config.error.empty()) {
                report += "- **Error**: " + config.error + "\n";
            }
        }
        
        return report;
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

// Test Android build configuration validation
TEST(AndroidBuildValidatorTest, ValidateCurrentBuildConfiguration) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto config = AndroidBuildValidator::validateAndroidBuild(projectRoot);
    
    // Basic configuration should be present
    EXPECT_FALSE(config.sdkPath.empty()) << "SDK path should be configured";
    EXPECT_GT(config.compileSdk, 0) << "Compile SDK should be set";
    EXPECT_GT(config.minSdk, 0) << "Min SDK should be set";
    EXPECT_GT(config.targetSdk, 0) << "Target SDK should be set";
    EXPECT_FALSE(config.ndkVersion.empty()) << "NDK version should be specified";
    EXPECT_TRUE(config.gradleWrapperExists) << "Gradle wrapper should exist";
    
    // SDK and NDK existence depends on local setup
    if (!config.sdkExists) {
        GTEST_SKIP() << "Android SDK not installed at: " << config.sdkPath;
    }
    if (!config.ndkExists) {
        GTEST_SKIP() << "Android NDK not installed: " << config.ndkVersion;
    }
}

// Test build configuration parsing
TEST(AndroidBuildValidatorTest, ParseBuildGradleConfiguration) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto config = AndroidBuildValidator::validateAndroidBuild(projectRoot);
    
    // Verify expected values from our build.gradle
    EXPECT_EQ(config.compileSdk, 33) << "Should use compile SDK 33";
    EXPECT_EQ(config.minSdk, 26) << "Should use min SDK 26 (Android 8.0)";
    EXPECT_EQ(config.targetSdk, 33) << "Should use target SDK 33";
    EXPECT_EQ(config.ndkVersion, "27.0.12077973") << "Should use specified NDK version";
}

// Test build report generation
TEST(AndroidBuildValidatorTest, GenerateBuildReport) {
    AndroidBuildValidator::BuildConfiguration testConfig;
    testConfig.sdkPath = "/test/sdk/path";
    testConfig.sdkExists = true;
    testConfig.ndkVersion = "27.0.12077973";
    testConfig.ndkExists = true;
    testConfig.compileSdk = 33;
    testConfig.minSdk = 26;
    testConfig.targetSdk = 33;
    testConfig.gradleWrapperExists = true;
    
    std::string report = AndroidBuildValidator::generateBuildReport(testConfig);
    
    EXPECT_NE(report.find("SDK Path"), std::string::npos) << "Report should contain SDK path";
    EXPECT_NE(report.find("NDK Version"), std::string::npos) << "Report should contain NDK version";
    EXPECT_NE(report.find("✅ READY TO BUILD"), std::string::npos) << "Should show ready status";
}

// Test configuration validation
TEST(AndroidBuildValidatorTest, ValidateConfiguration) {
    // Test valid configuration
    AndroidBuildValidator::BuildConfiguration validConfig;
    validConfig.sdkExists = true;
    validConfig.ndkExists = true;
    validConfig.gradleWrapperExists = true;
    validConfig.compileSdk = 33;
    validConfig.minSdk = 26;
    validConfig.targetSdk = 33;
    
    EXPECT_TRUE(AndroidBuildValidator::isValidBuildConfiguration(validConfig));
    
    // Test invalid configuration (missing SDK)
    AndroidBuildValidator::BuildConfiguration invalidConfig = validConfig;
    invalidConfig.sdkExists = false;
    
    EXPECT_FALSE(AndroidBuildValidator::isValidBuildConfiguration(invalidConfig));
}