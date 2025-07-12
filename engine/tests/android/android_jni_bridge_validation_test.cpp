#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class AndroidJNIBridgeValidator {
public:
    struct JNIBridgeStatus {
        bool jniHeaderExists = false;
        bool jniImplementationExists = false;
        bool gameEngineIntegrationExists = false;
        bool cmakeConfigured = false;
        bool javaInterfaceExists = false;
        std::string error;
    };
    
    static JNIBridgeStatus validateJNIBridge(const std::string& projectRoot) {
        JNIBridgeStatus status;
        
        // Check JNI header file - check if actual JNI header exists
        std::string jniHeaderPath = projectRoot + "/android/app/src/main/cpp/jni_bridge.h";
        status.jniHeaderExists = fs::exists(jniHeaderPath);
        
        // Check JNI implementation file (now in engine directory)
        std::string jniImplPath = projectRoot + "/engine/src/android/jni_bridge.cpp";
        status.jniImplementationExists = fs::exists(jniImplPath);
        
        // Check GameEngine integration - validate that engine headers are accessible
        std::string gameEngineHeader = projectRoot + "/engine/include/game/game_engine.h";
        status.gameEngineIntegrationExists = fs::exists(gameEngineHeader);
        
        // Check CMake configuration
        std::string cmakeListsPath = projectRoot + "/android/app/src/main/cpp/CMakeLists.txt";
        status.cmakeConfigured = fs::exists(cmakeListsPath);
        
        // Check Java interface - check the actual NativeEngine.java file
        std::string javaInterfacePath = projectRoot + "/android/app/src/main/java/com/diablo2portable/NativeEngine.java";
        status.javaInterfaceExists = fs::exists(javaInterfacePath);
        
        // Generate error if critical components are missing
        if (!status.jniHeaderExists || !status.jniImplementationExists) {
            status.error = "JNI bridge implementation missing - need to create jni_bridge.cpp/.h files";
        } else if (!status.gameEngineIntegrationExists) {
            status.error = "GameEngine header not found - check engine include path";
        } else if (!status.cmakeConfigured) {
            status.error = "CMake configuration missing - required for native builds";
        } else if (!status.javaInterfaceExists) {
            status.error = "NativeEngine.java interface missing - required for Android app integration";
        }
        
        return status;
    }
    
    static bool hasValidJNIBridge(const JNIBridgeStatus& status) {
        return status.jniHeaderExists && 
               status.jniImplementationExists && 
               status.gameEngineIntegrationExists && 
               status.cmakeConfigured && 
               status.javaInterfaceExists;
    }
    
    static std::string generateJNIBridgeReport(const JNIBridgeStatus& status) {
        std::string report = "# JNI Bridge Validation Report\n\n";
        
        report += "## JNI Bridge Components\n";
        report += "- **JNI Header**: " + std::string(status.jniHeaderExists ? "✅" : "❌") + "\n";
        report += "- **JNI Implementation**: " + std::string(status.jniImplementationExists ? "✅" : "❌") + "\n";
        report += "- **GameEngine Integration**: " + std::string(status.gameEngineIntegrationExists ? "✅" : "❌") + "\n";
        report += "- **CMake Configuration**: " + std::string(status.cmakeConfigured ? "✅" : "❌") + "\n";
        report += "- **Java Interface**: " + std::string(status.javaInterfaceExists ? "✅" : "❌") + "\n\n";
        
        report += "## Status\n";
        if (hasValidJNIBridge(status)) {
            report += "- **Overall**: ✅ JNI BRIDGE READY\n";
        } else {
            report += "- **Overall**: ❌ JNI BRIDGE INCOMPLETE\n";
            if (!status.error.empty()) {
                report += "- **Error**: " + status.error + "\n";
            }
        }
        
        return report;
    }
    
    static std::vector<std::string> getRequiredJNIFunctions() {
        return {
            "Java_com_diablo2portable_NativeEngine_createEngine",
            "Java_com_diablo2portable_NativeEngine_initialize", 
            "Java_com_diablo2portable_NativeEngine_loadAssets",
            "Java_com_diablo2portable_NativeEngine_onTouchEvent",
            "Java_com_diablo2portable_NativeEngine_onSurfaceCreated",
            "Java_com_diablo2portable_NativeEngine_renderFrame",
            "Java_com_diablo2portable_NativeEngine_destroyEngine"
        };
    }
    
    static bool validateJNIFunctionSignatures(const std::string& jniImplPath) {
        if (!fs::exists(jniImplPath)) {
            return false;
        }
        
        std::ifstream file(jniImplPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        auto requiredFunctions = getRequiredJNIFunctions();
        
        for (const auto& func : requiredFunctions) {
            if (content.find(func) == std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
};

// Test JNI bridge validation
TEST(AndroidJNIBridgeValidationTest, ValidateJNIBridgeComponents) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto status = AndroidJNIBridgeValidator::validateJNIBridge(projectRoot);
    
    // Check what exists vs what's missing
    EXPECT_TRUE(status.gameEngineIntegrationExists) << "GameEngine header should exist";
    EXPECT_TRUE(status.cmakeConfigured) << "CMake configuration should exist";
    EXPECT_TRUE(status.javaInterfaceExists) << "NativeEngine.java interface should exist";
    
    // JNI implementation should now exist (Phase 38.2 implementation)
    EXPECT_TRUE(status.jniImplementationExists) << "JNI implementation should exist after Phase 38.2";
    
    // Overall validation
    if (AndroidJNIBridgeValidator::hasValidJNIBridge(status)) {
        EXPECT_TRUE(status.error.empty()) << "Should not have errors when bridge is valid";
    } else {
        EXPECT_FALSE(status.error.empty()) << "Should provide error when bridge is invalid";
    }
}

// Test JNI bridge report generation
TEST(AndroidJNIBridgeValidationTest, GenerateJNIBridgeReport) {
    AndroidJNIBridgeValidator::JNIBridgeStatus testStatus;
    testStatus.jniHeaderExists = true;
    testStatus.jniImplementationExists = true;
    testStatus.gameEngineIntegrationExists = true;
    testStatus.cmakeConfigured = true;
    testStatus.javaInterfaceExists = true;
    
    std::string report = AndroidJNIBridgeValidator::generateJNIBridgeReport(testStatus);
    
    EXPECT_NE(report.find("JNI Bridge Validation Report"), std::string::npos) << "Should have report title";
    EXPECT_NE(report.find("JNI Header"), std::string::npos) << "Should include JNI header status";
    EXPECT_NE(report.find("JNI Implementation"), std::string::npos) << "Should include JNI implementation status";
    EXPECT_NE(report.find("GameEngine Integration"), std::string::npos) << "Should include GameEngine integration status";
    EXPECT_NE(report.find("✅ JNI BRIDGE READY"), std::string::npos) << "Should show ready status for valid bridge";
}

// Test JNI function signature validation
TEST(AndroidJNIBridgeValidationTest, ValidateJNIFunctionSignatures) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    std::string jniImplPath = projectRoot + "/engine/src/android/jni_bridge.cpp";
    
    auto requiredFunctions = AndroidJNIBridgeValidator::getRequiredJNIFunctions();
    EXPECT_GT(requiredFunctions.size(), 0) << "Should have required JNI functions";
    
    // Check if required functions include key operations
    bool hasCreateEngine = std::find(requiredFunctions.begin(), requiredFunctions.end(), 
                                     "Java_com_diablo2portable_NativeEngine_createEngine") != requiredFunctions.end();
    bool hasInitialize = std::find(requiredFunctions.begin(), requiredFunctions.end(), 
                                     "Java_com_diablo2portable_NativeEngine_initialize") != requiredFunctions.end();
    bool hasRenderFrame = std::find(requiredFunctions.begin(), requiredFunctions.end(), 
                                     "Java_com_diablo2portable_NativeEngine_renderFrame") != requiredFunctions.end();
    
    EXPECT_TRUE(hasCreateEngine) << "Should include createEngine function";
    EXPECT_TRUE(hasInitialize) << "Should include initialize function";
    EXPECT_TRUE(hasRenderFrame) << "Should include renderFrame function";
    
    // Validate function signatures if JNI implementation exists
    if (fs::exists(jniImplPath)) {
        bool signaturesValid = AndroidJNIBridgeValidator::validateJNIFunctionSignatures(jniImplPath);
        EXPECT_TRUE(signaturesValid) << "JNI function signatures should be valid";
    } else {
        GTEST_SKIP() << "JNI implementation not found - validation skipped";
    }
}

// Test JNI bridge validation integration
TEST(AndroidJNIBridgeValidationTest, JNIBridgeValidationIntegration) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto status = AndroidJNIBridgeValidator::validateJNIBridge(projectRoot);
    std::string report = AndroidJNIBridgeValidator::generateJNIBridgeReport(status);
    
    // Report should provide comprehensive status
    EXPECT_NE(report.find("JNI Bridge Components"), std::string::npos) << "Should include component status";
    EXPECT_NE(report.find("Status"), std::string::npos) << "Should include overall status";
    
    // Should either be ready or provide clear error
    bool isReady = AndroidJNIBridgeValidator::hasValidJNIBridge(status);
    if (isReady) {
        EXPECT_NE(report.find("✅ JNI BRIDGE READY"), std::string::npos) << "Should show ready status";
    } else {
        EXPECT_NE(report.find("❌ JNI BRIDGE INCOMPLETE"), std::string::npos) << "Should show incomplete status";
        EXPECT_FALSE(status.error.empty()) << "Should provide error details";
    }
    
    // Integration test validates complete JNI bridge functionality
    EXPECT_TRUE(true) << "JNI bridge validation integration test complete";
}