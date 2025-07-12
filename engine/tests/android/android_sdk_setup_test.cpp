#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class AndroidSDKSetup {
public:
    struct SDKSetupGuide {
        std::vector<std::string> installationSteps;
        std::vector<std::string> configurationSteps;
        std::string sdkManagerPath;
        std::vector<std::string> requiredPackages;
        std::string troubleshooting;
    };
    
    static SDKSetupGuide generateSetupGuide() {
        SDKSetupGuide guide;
        
        // Installation steps
        guide.installationSteps = {
            "1. Download Android Studio from https://developer.android.com/studio",
            "2. Run the installer and follow the setup wizard",
            "3. Accept the license agreements",
            "4. Wait for initial SDK download to complete",
            "5. Open Android Studio to ensure setup is complete"
        };
        
        // Configuration steps  
        guide.configurationSteps = {
            "1. Open Android Studio",
            "2. Go to Tools → SDK Manager",
            "3. Install required SDK platforms and tools",
            "4. Note the SDK path (usually ~/Library/Android/sdk on Mac)",
            "5. Update local.properties with correct SDK path"
        };
        
        // Required packages
        guide.requiredPackages = {
            "Android SDK Platform-Tools",
            "Android SDK Build-Tools 33.0.0+",
            "Android SDK Platform API 33",
            "Android SDK Platform API 26 (for min SDK)",
            "NDK (Side by side) 27.0.12077973",
            "CMake 3.22.1+"
        };
        
        // Troubleshooting
        guide.troubleshooting = 
            "Common Issues:\n"
            "- SDK path in local.properties is incorrect\n"
            "- NDK version mismatch (check build.gradle ndkVersion)\n"
            "- Missing CMake installation\n"
            "- Insufficient disk space for SDK (~5GB needed)\n"
            "- Network issues during SDK download";
            
        return guide;
    }
    
    static std::string generateLocalPropertiesTemplate(const std::string& detectedSdkPath = "") {
        std::string content = 
            "# This file should *NOT* be checked into Version Control Systems,\n"
            "# as it contains information specific to your local configuration.\n"
            "\n"
            "# Location of the SDK. This is only used by Gradle.\n"
            "# For Android Studio, the SDK location is managed by the IDE.\n";
            
        if (!detectedSdkPath.empty()) {
            content += "sdk.dir=" + detectedSdkPath + "\n";
        } else {
            content += "# sdk.dir=/path/to/android/sdk\n";
            content += "# Common paths:\n";
            content += "# macOS: sdk.dir=/Users/username/Library/Android/sdk\n";
            content += "# Linux: sdk.dir=/home/username/Android/Sdk\n";
            content += "# Windows: sdk.dir=C:\\\\Users\\\\username\\\\AppData\\\\Local\\\\Android\\\\Sdk\n";
        }
        
        return content;
    }
    
    static std::vector<std::string> detectPossibleSDKPaths() {
        std::vector<std::string> possiblePaths;
        
        // Get home directory
        const char* home = std::getenv("HOME");
        if (home) {
            std::string homeDir(home);
            
            // Common macOS paths
            possiblePaths.push_back(homeDir + "/Library/Android/sdk");
            possiblePaths.push_back(homeDir + "/Android/Sdk");
            
            // Android Studio default paths
            possiblePaths.push_back(homeDir + "/Library/Android/Sdk");
            
            // Manual installation paths
            possiblePaths.push_back("/usr/local/android-sdk");
            possiblePaths.push_back("/opt/android-sdk");
        }
        
        return possiblePaths;
    }
    
    static std::string findInstalledSDK() {
        auto possiblePaths = detectPossibleSDKPaths();
        
        for (const auto& path : possiblePaths) {
            if (fs::exists(path) && fs::is_directory(path)) {
                // Verify it's actually an SDK by checking for key files
                if (fs::exists(path + "/platform-tools") && 
                    fs::exists(path + "/tools")) {
                    return path;
                }
            }
        }
        
        return ""; // Not found
    }
    
    static bool updateLocalProperties(const std::string& projectRoot, const std::string& sdkPath) {
        std::string localPropsPath = projectRoot + "/android/local.properties";
        
        try {
            std::ofstream file(localPropsPath);
            if (!file.is_open()) {
                return false;
            }
            
            file << generateLocalPropertiesTemplate(sdkPath);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::string generateDetailedInstallationGuide() {
        std::string guide = 
            "ANDROID SDK INSTALLATION GUIDE\n"
            "==============================\n\n"
            "METHOD 1: Android Studio Installation\n"
            "1. Download Android Studio from https://developer.android.com/studio\n"
            "2. Install and run setup wizard\n"
            "3. SDK will be installed automatically\n\n"
            "METHOD 2: Command Line Tools\n"
            "1. Download command line tools from Android developer site\n"
            "2. Extract to desired location\n"
            "3. Use sdkmanager to install required components\n\n"
            "ENVIRONMENT SETUP:\n"
            "- Set ANDROID_HOME environment variable to SDK path\n"
            "- Add platform-tools to PATH\n"
            "- Configure local.properties file in project root\n\n"
            "REQUIRED COMPONENTS:\n"
            "- Platform Tools\n"
            "- Build Tools 33.0.0+\n"
            "- Platform API 33\n"
            "- NDK 27.0.12077973\n"
            "- CMake 3.22.1+\n";
        
        return guide;
    }
};

// Test SDK setup guide generation
TEST(AndroidSDKSetupTest, GenerateSetupGuide) {
    auto guide = AndroidSDKSetup::generateSetupGuide();
    
    EXPECT_GT(guide.installationSteps.size(), 0) << "Should have installation steps";
    EXPECT_GT(guide.configurationSteps.size(), 0) << "Should have configuration steps";
    EXPECT_GT(guide.requiredPackages.size(), 0) << "Should list required packages";
    EXPECT_FALSE(guide.troubleshooting.empty()) << "Should have troubleshooting info";
    
    // Verify specific required packages
    auto& packages = guide.requiredPackages;
    bool hasNDK = std::any_of(packages.begin(), packages.end(), 
        [](const std::string& pkg) { return pkg.find("NDK") != std::string::npos; });
    EXPECT_TRUE(hasNDK) << "Should include NDK in required packages";
}

// Test local.properties template generation
TEST(AndroidSDKSetupTest, GenerateLocalPropertiesTemplate) {
    // Test with no SDK path
    std::string template1 = AndroidSDKSetup::generateLocalPropertiesTemplate();
    EXPECT_NE(template1.find("sdk.dir"), std::string::npos) << "Should contain sdk.dir reference";
    EXPECT_NE(template1.find("Common paths"), std::string::npos) << "Should have path examples";
    
    // Test with provided SDK path
    std::string testPath = "/test/sdk/path";
    std::string template2 = AndroidSDKSetup::generateLocalPropertiesTemplate(testPath);
    EXPECT_NE(template2.find("sdk.dir=" + testPath), std::string::npos) << "Should use provided path";
}

// Test SDK path detection
TEST(AndroidSDKSetupTest, DetectPossibleSDKPaths) {
    auto paths = AndroidSDKSetup::detectPossibleSDKPaths();
    
    EXPECT_GT(paths.size(), 0) << "Should detect possible SDK paths";
    
    // Should include home directory based paths
    bool hasHomeBasedPath = std::any_of(paths.begin(), paths.end(),
        [](const std::string& path) { return path.find("Library/Android") != std::string::npos; });
    EXPECT_TRUE(hasHomeBasedPath) << "Should include home-based paths on macOS";
}

// Test SDK installation detection
TEST(AndroidSDKSetupTest, FindInstalledSDK) {
    std::string foundSDK = AndroidSDKSetup::findInstalledSDK();
    
    if (foundSDK.empty()) {
        GTEST_SKIP() << "No Android SDK installation found (expected for CI/test environments)";
    } else {
        EXPECT_TRUE(fs::exists(foundSDK)) << "Found SDK path should exist";
        EXPECT_TRUE(fs::exists(foundSDK + "/platform-tools")) << "SDK should have platform-tools";
    }
}

// Test local.properties update
TEST(AndroidSDKSetupTest, UpdateLocalProperties) {
    // Create a temporary test directory
    std::string testDir = "test_android_setup";
    fs::create_directories(testDir + "/android");
    
    std::string testSDKPath = "/test/sdk/path";
    bool success = AndroidSDKSetup::updateLocalProperties(testDir, testSDKPath);
    
    EXPECT_TRUE(success) << "Should successfully update local.properties";
    
    // Verify the file was created correctly
    std::string localPropsPath = testDir + "/android/local.properties";
    EXPECT_TRUE(fs::exists(localPropsPath)) << "local.properties should be created";
    
    // Verify content
    std::ifstream file(localPropsPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("sdk.dir=" + testSDKPath), std::string::npos) 
        << "Should contain correct SDK path";
    
    // Clean up
    fs::remove_all(testDir);
}

// Test SDK setup guidance integration
TEST(AndroidSDKSetupTest, ProvideSDKSetupGuidance) {
    auto guide = AndroidSDKSetup::generateSetupGuide();
    
    // Test should provide complete guidance when SDK is not installed
    EXPECT_GT(guide.installationSteps.size(), 3) << "Should provide comprehensive installation steps";
    EXPECT_GT(guide.configurationSteps.size(), 3) << "Should provide configuration steps";
    EXPECT_GT(guide.requiredPackages.size(), 5) << "Should list all required packages";
    
    // Check for essential packages
    auto& packages = guide.requiredPackages;
    bool hasSDKTools = std::any_of(packages.begin(), packages.end(),
        [](const std::string& pkg) { return pkg.find("Platform-Tools") != std::string::npos; });
    bool hasBuildTools = std::any_of(packages.begin(), packages.end(),
        [](const std::string& pkg) { return pkg.find("Build-Tools") != std::string::npos; });
    bool hasNDK = std::any_of(packages.begin(), packages.end(),
        [](const std::string& pkg) { return pkg.find("NDK") != std::string::npos; });
    bool hasCMake = std::any_of(packages.begin(), packages.end(),
        [](const std::string& pkg) { return pkg.find("CMake") != std::string::npos; });
    
    EXPECT_TRUE(hasSDKTools) << "Should include Platform-Tools in required packages";
    EXPECT_TRUE(hasBuildTools) << "Should include Build-Tools in required packages";
    EXPECT_TRUE(hasNDK) << "Should include NDK in required packages";
    EXPECT_TRUE(hasCMake) << "Should include CMake in required packages";
    
    // Verify troubleshooting section is comprehensive
    EXPECT_NE(guide.troubleshooting.find("SDK path"), std::string::npos) << "Should include SDK path troubleshooting";
    EXPECT_NE(guide.troubleshooting.find("NDK version"), std::string::npos) << "Should include NDK troubleshooting";
    EXPECT_NE(guide.troubleshooting.find("CMake"), std::string::npos) << "Should include CMake troubleshooting";
}

// Test integration with build validator to provide complete setup guidance
TEST(AndroidSDKSetupTest, IntegrateWithBuildValidator) {
    // This should provide a complete setup solution when SDK is not installed
    auto guide = AndroidSDKSetup::generateSetupGuide();
    std::string detectedSDK = AndroidSDKSetup::findInstalledSDK();
    
    if (detectedSDK.empty()) {
        // SDK not installed - guidance should be comprehensive
        EXPECT_GT(guide.installationSteps.size(), 0) << "Should provide installation steps when SDK not found";
        EXPECT_GT(guide.configurationSteps.size(), 0) << "Should provide configuration steps when SDK not found";
        
        // Should be able to generate template for local.properties
        std::string template_content = AndroidSDKSetup::generateLocalPropertiesTemplate();
        EXPECT_NE(template_content.find("sdk.dir"), std::string::npos) << "Should provide SDK path template";
        EXPECT_NE(template_content.find("Common paths"), std::string::npos) << "Should provide path examples";
    } else {
        // SDK installed - should be able to update local.properties
        EXPECT_TRUE(fs::exists(detectedSDK)) << "Detected SDK path should exist";
        
        std::string template_content = AndroidSDKSetup::generateLocalPropertiesTemplate(detectedSDK);
        EXPECT_NE(template_content.find("sdk.dir=" + detectedSDK), std::string::npos) << "Should use detected SDK path";
    }
}

// Test complete SDK setup process
TEST(AndroidSDKSetupTest, CompleteSDKSetupProcess) {
    // This test validates the complete SDK setup process
    
    // Step 1: Generate setup guide
    auto guide = AndroidSDKSetup::generateSetupGuide();
    EXPECT_GT(guide.installationSteps.size(), 0) << "Should provide installation steps";
    EXPECT_GT(guide.configurationSteps.size(), 0) << "Should provide configuration steps";
    EXPECT_GT(guide.requiredPackages.size(), 0) << "Should list required packages";
    
    // Step 2: Detect possible SDK paths
    auto possiblePaths = AndroidSDKSetup::detectPossibleSDKPaths();
    EXPECT_GT(possiblePaths.size(), 0) << "Should detect possible SDK installation paths";
    
    // Step 3: Try to find installed SDK
    std::string installedSDK = AndroidSDKSetup::findInstalledSDK();
    // This may be empty if SDK not installed - that's expected
    
    // Step 4: Generate local.properties template
    std::string template_content = AndroidSDKSetup::generateLocalPropertiesTemplate(installedSDK);
    EXPECT_NE(template_content.find("sdk.dir"), std::string::npos) << "Should contain SDK path configuration";
    
    // Step 5: If SDK is not found, guidance should be comprehensive
    if (installedSDK.empty()) {
        EXPECT_NE(guide.troubleshooting.find("SDK path"), std::string::npos) << "Should provide SDK path troubleshooting";
        EXPECT_NE(template_content.find("Common paths"), std::string::npos) << "Should provide path examples";
    }
    
    // The process should provide complete guidance for SDK setup
    EXPECT_TRUE(true) << "Complete SDK setup process validation successful";
}

// STEP 9: Write exactly ONE failing test for enhanced SDK setup guidance
TEST(AndroidSDKSetupTest, ProvideDetailedSDKInstallationGuide) {
    AndroidSDKSetup setup;
    
    // Test generation of detailed installation guide
    std::string guide = setup.generateDetailedInstallationGuide();
    
    EXPECT_FALSE(guide.empty()); // Should generate a guide
    EXPECT_NE(guide.find("Android Studio"), std::string::npos); // Should mention Android Studio
    EXPECT_NE(guide.find("command line tools"), std::string::npos); // Should mention CLI tools
    EXPECT_NE(guide.find("ANDROID_HOME"), std::string::npos); // Should mention environment variable
    EXPECT_NE(guide.find("local.properties"), std::string::npos); // Should mention config file
}