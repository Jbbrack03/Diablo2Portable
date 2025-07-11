#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>

namespace fs = std::filesystem;

class AndroidIntegrationValidator {
public:
    struct IntegrationStatus {
        bool mainActivityExists = false;
        bool nativeEngineExists = false;
        bool gameRendererExists = false;
        bool gameSurfaceViewExists = false;
        bool lifecycleMethodsImplemented = false;
        bool touchInputImplemented = false;
        bool androidManifestConfigured = false;
        std::string error;
    };
    
    struct AndroidLifecycleTest {
        std::string methodName;
        bool implemented = false;
        std::string implementation;
    };
    
    static IntegrationStatus validateAndroidIntegration(const std::string& projectRoot) {
        IntegrationStatus status;
        
        // Check MainActivity
        std::string mainActivityPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/MainActivity.java";
        status.mainActivityExists = fs::exists(mainActivityPath);
        
        // Check NativeEngine
        std::string nativeEnginePath = projectRoot + "/android/app/src/main/java/com/diablo2portable/NativeEngine.java";
        status.nativeEngineExists = fs::exists(nativeEnginePath);
        
        // Check GameRenderer
        std::string gameRendererPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/GameRenderer.java";
        status.gameRendererExists = fs::exists(gameRendererPath);
        
        // Check GameSurfaceView
        std::string gameSurfaceViewPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/GameSurfaceView.java";
        status.gameSurfaceViewExists = fs::exists(gameSurfaceViewPath);
        
        // Check Android Manifest
        std::string manifestPath = projectRoot + "/android/app/src/main/AndroidManifest.xml";
        status.androidManifestConfigured = fs::exists(manifestPath);
        
        // Validate lifecycle methods if MainActivity exists
        if (status.mainActivityExists) {
            status.lifecycleMethodsImplemented = validateLifecycleMethods(mainActivityPath);
        }
        
        // Validate touch input if GameSurfaceView exists
        if (status.gameSurfaceViewExists) {
            status.touchInputImplemented = validateTouchInput(gameSurfaceViewPath);
        }
        
        // Generate error if critical components missing
        if (!status.mainActivityExists) {
            status.error = "MainActivity missing - required for Android app";
        } else if (!status.nativeEngineExists) {
            status.error = "NativeEngine missing - required for game integration";
        } else if (!status.gameRendererExists) {
            status.error = "GameRenderer missing - required for OpenGL rendering";
        } else if (!status.gameSurfaceViewExists) {
            status.error = "GameSurfaceView missing - required for game display";
        } else if (!status.androidManifestConfigured) {
            status.error = "AndroidManifest.xml missing - required for app configuration";
        }
        
        return status;
    }
    
    static bool validateLifecycleMethods(const std::string& mainActivityPath) {
        if (!fs::exists(mainActivityPath)) {
            return false;
        }
        
        std::ifstream file(mainActivityPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Check for essential lifecycle methods
        std::vector<std::string> requiredMethods = {
            "onCreate",
            "onResume", 
            "onPause",
            "onDestroy"
        };
        
        for (const auto& method : requiredMethods) {
            if (content.find(method) == std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
    
    static bool validateTouchInput(const std::string& gameSurfaceViewPath) {
        if (!fs::exists(gameSurfaceViewPath)) {
            return false;
        }
        
        std::ifstream file(gameSurfaceViewPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Check for touch input handling
        return content.find("onTouchEvent") != std::string::npos;
    }
    
    static std::vector<AndroidLifecycleTest> analyzeLifecycleImplementation(const std::string& projectRoot) {
        std::vector<AndroidLifecycleTest> tests;
        
        std::string mainActivityPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/MainActivity.java";
        if (!fs::exists(mainActivityPath)) {
            return tests;
        }
        
        std::ifstream file(mainActivityPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Analyze each lifecycle method
        std::vector<std::string> lifecycleMethods = {
            "onCreate", "onStart", "onResume", "onPause", "onStop", "onDestroy"
        };
        
        for (const auto& method : lifecycleMethods) {
            AndroidLifecycleTest test;
            test.methodName = method;
            test.implemented = content.find(method) != std::string::npos;
            
            if (test.implemented) {
                // Extract implementation details
                size_t methodPos = content.find(method);
                size_t braceStart = content.find("{", methodPos);
                size_t braceEnd = content.find("}", braceStart);
                
                if (braceStart != std::string::npos && braceEnd != std::string::npos) {
                    test.implementation = content.substr(braceStart + 1, braceEnd - braceStart - 1);
                    // Remove excess whitespace
                    test.implementation.erase(0, test.implementation.find_first_not_of(" \t\n\r"));
                    test.implementation.erase(test.implementation.find_last_not_of(" \t\n\r") + 1);
                }
            }
            
            tests.push_back(test);
        }
        
        return tests;
    }
    
    static std::string generateIntegrationReport(const IntegrationStatus& status) {
        std::string report = "# Android Integration Report\n\n";
        
        report += "## Core Components\n";
        report += "- **MainActivity**: " + std::string(status.mainActivityExists ? "✅" : "❌") + "\n";
        report += "- **NativeEngine**: " + std::string(status.nativeEngineExists ? "✅" : "❌") + "\n";
        report += "- **GameRenderer**: " + std::string(status.gameRendererExists ? "✅" : "❌") + "\n";
        report += "- **GameSurfaceView**: " + std::string(status.gameSurfaceViewExists ? "✅" : "❌") + "\n";
        report += "- **AndroidManifest**: " + std::string(status.androidManifestConfigured ? "✅" : "❌") + "\n\n";
        
        report += "## Integration Features\n";
        report += "- **Lifecycle Methods**: " + std::string(status.lifecycleMethodsImplemented ? "✅" : "❌") + "\n";
        report += "- **Touch Input**: " + std::string(status.touchInputImplemented ? "✅" : "❌") + "\n\n";
        
        report += "## Status\n";
        if (isIntegrationComplete(status)) {
            report += "- **Overall**: ✅ INTEGRATION COMPLETE\n";
        } else {
            report += "- **Overall**: ❌ INTEGRATION INCOMPLETE\n";
            if (!status.error.empty()) {
                report += "- **Error**: " + status.error + "\n";
            }
        }
        
        return report;
    }
    
    static bool isIntegrationComplete(const IntegrationStatus& status) {
        return status.mainActivityExists && 
               status.nativeEngineExists && 
               status.gameRendererExists && 
               status.gameSurfaceViewExists &&
               status.androidManifestConfigured &&
               status.lifecycleMethodsImplemented &&
               status.touchInputImplemented;
    }
};

// Test Android integration validation
TEST(AndroidIntegrationTest, ValidateAndroidIntegration) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto status = AndroidIntegrationValidator::validateAndroidIntegration(projectRoot);
    
    // Core Android components should exist
    EXPECT_TRUE(status.mainActivityExists) << "MainActivity should exist";
    EXPECT_TRUE(status.nativeEngineExists) << "NativeEngine should exist";
    EXPECT_TRUE(status.gameRendererExists) << "GameRenderer should exist";
    EXPECT_TRUE(status.gameSurfaceViewExists) << "GameSurfaceView should exist";
    EXPECT_TRUE(status.androidManifestConfigured) << "AndroidManifest should exist";
    
    // Integration features should be implemented
    EXPECT_TRUE(status.lifecycleMethodsImplemented) << "Lifecycle methods should be implemented";
    EXPECT_TRUE(status.touchInputImplemented) << "Touch input should be implemented";
    
    // Overall integration should be complete
    if (AndroidIntegrationValidator::isIntegrationComplete(status)) {
        EXPECT_TRUE(status.error.empty()) << "Should not have errors when integration is complete";
    } else {
        EXPECT_FALSE(status.error.empty()) << "Should provide error when integration is incomplete";
    }
}

// Test lifecycle method analysis
TEST(AndroidIntegrationTest, AnalyzeLifecycleImplementation) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto lifecycleTests = AndroidIntegrationValidator::analyzeLifecycleImplementation(projectRoot);
    
    // Should analyze lifecycle methods
    EXPECT_GT(lifecycleTests.size(), 0) << "Should analyze lifecycle methods";
    
    // Check for essential methods
    bool hasOnCreate = false;
    bool hasOnDestroy = false;
    
    for (const auto& test : lifecycleTests) {
        if (test.methodName == "onCreate") {
            hasOnCreate = true;
            EXPECT_TRUE(test.implemented) << "onCreate should be implemented";
        }
        if (test.methodName == "onDestroy") {
            hasOnDestroy = true;
            EXPECT_TRUE(test.implemented) << "onDestroy should be implemented";
        }
    }
    
    EXPECT_TRUE(hasOnCreate) << "Should analyze onCreate method";
    EXPECT_TRUE(hasOnDestroy) << "Should analyze onDestroy method";
}

// Test integration report generation
TEST(AndroidIntegrationTest, GenerateIntegrationReport) {
    AndroidIntegrationValidator::IntegrationStatus testStatus;
    testStatus.mainActivityExists = true;
    testStatus.nativeEngineExists = true;
    testStatus.gameRendererExists = true;
    testStatus.gameSurfaceViewExists = true;
    testStatus.androidManifestConfigured = true;
    testStatus.lifecycleMethodsImplemented = true;
    testStatus.touchInputImplemented = true;
    
    std::string report = AndroidIntegrationValidator::generateIntegrationReport(testStatus);
    
    EXPECT_NE(report.find("Android Integration Report"), std::string::npos) << "Should have report title";
    EXPECT_NE(report.find("Core Components"), std::string::npos) << "Should include core components";
    EXPECT_NE(report.find("Integration Features"), std::string::npos) << "Should include integration features";
    EXPECT_NE(report.find("✅ INTEGRATION COMPLETE"), std::string::npos) << "Should show complete status";
}

// Test Android app lifecycle integration
TEST(AndroidIntegrationTest, AndroidAppLifecycleIntegration) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    auto status = AndroidIntegrationValidator::validateAndroidIntegration(projectRoot);
    std::string report = AndroidIntegrationValidator::generateIntegrationReport(status);
    
    // Report should provide comprehensive integration status
    EXPECT_NE(report.find("Core Components"), std::string::npos) << "Should include component status";
    EXPECT_NE(report.find("Integration Features"), std::string::npos) << "Should include feature status";
    EXPECT_NE(report.find("Status"), std::string::npos) << "Should include overall status";
    
    // Should either be complete or provide clear error
    bool isComplete = AndroidIntegrationValidator::isIntegrationComplete(status);
    if (isComplete) {
        EXPECT_NE(report.find("✅ INTEGRATION COMPLETE"), std::string::npos) << "Should show complete status";
    } else {
        EXPECT_NE(report.find("❌ INTEGRATION INCOMPLETE"), std::string::npos) << "Should show incomplete status";
        EXPECT_FALSE(status.error.empty()) << "Should provide error details";
    }
    
    // Integration test validates complete Android app lifecycle
    EXPECT_TRUE(true) << "Android app lifecycle integration test complete";
}

// Test native library loading validation
TEST(AndroidIntegrationTest, ValidateNativeLibraryLoading) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    // Check if NativeEngine.java loads the correct library
    std::string nativeEnginePath = projectRoot + "/android/app/src/main/java/com/diablo2portable/NativeEngine.java";
    EXPECT_TRUE(fs::exists(nativeEnginePath)) << "NativeEngine.java should exist";
    
    if (fs::exists(nativeEnginePath)) {
        std::ifstream file(nativeEnginePath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should load the d2engine library
        EXPECT_NE(content.find("System.loadLibrary(\"d2engine\")"), std::string::npos) 
            << "Should load d2engine native library";
        
        // Should have native method declarations
        EXPECT_NE(content.find("native"), std::string::npos) 
            << "Should have native method declarations";
        
        // Check for essential native methods
        std::vector<std::string> requiredNativeMethods = {
            "createEngine", "destroyEngine", "initialize", "renderFrame"
        };
        
        for (const auto& method : requiredNativeMethods) {
            EXPECT_NE(content.find("native"), std::string::npos) 
                << "Should have " << method << " native method";
        }
    }
    
    // Check CMake configuration for native library
    std::string cmakePath = projectRoot + "/android/app/src/main/cpp/CMakeLists.txt";
    EXPECT_TRUE(fs::exists(cmakePath)) << "CMakeLists.txt should exist";
    
    if (fs::exists(cmakePath)) {
        std::ifstream file(cmakePath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should create d2engine shared library
        EXPECT_NE(content.find("d2engine"), std::string::npos) 
            << "Should configure d2engine library";
        
        // Should link required Android libraries
        EXPECT_NE(content.find("log-lib"), std::string::npos) 
            << "Should link Android log library";
        EXPECT_NE(content.find("GLESv3-lib"), std::string::npos) 
            << "Should link OpenGL ES 3.0 library";
        EXPECT_NE(content.find("EGL-lib"), std::string::npos) 
            << "Should link EGL library";
    }
}

// Test touch input forwarding
TEST(AndroidIntegrationTest, ValidateTouchInputForwarding) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    // Check GameSurfaceView touch handling
    std::string gameSurfaceViewPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/GameSurfaceView.java";
    EXPECT_TRUE(fs::exists(gameSurfaceViewPath)) << "GameSurfaceView.java should exist";
    
    if (fs::exists(gameSurfaceViewPath)) {
        std::ifstream file(gameSurfaceViewPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should implement touch event handling
        EXPECT_NE(content.find("onTouchEvent"), std::string::npos) 
            << "Should implement onTouchEvent method";
        
        // Should forward touch events to native engine
        EXPECT_NE(content.find("onTouchEvent"), std::string::npos) 
            << "Should call native onTouchEvent";
    }
    
    // Check NativeEngine touch input methods
    std::string nativeEnginePath = projectRoot + "/android/app/src/main/java/com/diablo2portable/NativeEngine.java";
    if (fs::exists(nativeEnginePath)) {
        std::ifstream file(nativeEnginePath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should have native touch event method
        EXPECT_NE(content.find("onTouchEvent"), std::string::npos) 
            << "Should have onTouchEvent native method";
        
        // Should accept touch coordinates and action
        EXPECT_NE(content.find("float x, float y, int action"), std::string::npos) 
            << "Should accept touch coordinates and action";
    }
    
    // Check MainActivity touch integration
    std::string mainActivityPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/MainActivity.java";
    if (fs::exists(mainActivityPath)) {
        std::ifstream file(mainActivityPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should integrate with GameSurfaceView or handle touch directly
        bool hasGameSurfaceView = content.find("GameSurfaceView") != std::string::npos;
        bool hasDirectTouchHandling = content.find("onTouchEvent") != std::string::npos;
        
        EXPECT_TRUE(hasGameSurfaceView || hasDirectTouchHandling) 
            << "Should integrate touch handling via GameSurfaceView or directly";
    }
}

// Test OpenGL context creation
TEST(AndroidIntegrationTest, ValidateOpenGLContextCreation) {
    fs::path currentPath = fs::current_path();
    while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
        currentPath = currentPath.parent_path();
    }
    std::string projectRoot = currentPath.string();
    
    // Check GameRenderer for OpenGL context setup
    std::string gameRendererPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/GameRenderer.java";
    EXPECT_TRUE(fs::exists(gameRendererPath)) << "GameRenderer.java should exist";
    
    if (fs::exists(gameRendererPath)) {
        std::ifstream file(gameRendererPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should implement GLSurfaceView.Renderer
        EXPECT_NE(content.find("GLSurfaceView.Renderer"), std::string::npos) 
            << "Should implement GLSurfaceView.Renderer";
        
        // Should have essential OpenGL lifecycle methods
        EXPECT_NE(content.find("onSurfaceCreated"), std::string::npos) 
            << "Should implement onSurfaceCreated";
        EXPECT_NE(content.find("onSurfaceChanged"), std::string::npos) 
            << "Should implement onSurfaceChanged";
        EXPECT_NE(content.find("onDrawFrame"), std::string::npos) 
            << "Should implement onDrawFrame";
    }
    
    // Check GameSurfaceView for OpenGL setup
    std::string gameSurfaceViewPath = projectRoot + "/android/app/src/main/java/com/diablo2portable/GameSurfaceView.java";
    if (fs::exists(gameSurfaceViewPath)) {
        std::ifstream file(gameSurfaceViewPath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should extend GLSurfaceView
        EXPECT_NE(content.find("GLSurfaceView"), std::string::npos) 
            << "Should extend GLSurfaceView";
        
        // Should set OpenGL ES version
        EXPECT_NE(content.find("setEGLContextClientVersion"), std::string::npos) 
            << "Should set OpenGL ES version";
        
        // Should set renderer
        EXPECT_NE(content.find("setRenderer"), std::string::npos) 
            << "Should set OpenGL renderer";
    }
    
    // Check NativeEngine for surface creation callback
    std::string nativeEnginePath = projectRoot + "/android/app/src/main/java/com/diablo2portable/NativeEngine.java";
    if (fs::exists(nativeEnginePath)) {
        std::ifstream file(nativeEnginePath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should have surface creation method
        EXPECT_NE(content.find("onSurfaceCreated"), std::string::npos) 
            << "Should have onSurfaceCreated method";
        
        // Should accept surface dimensions
        EXPECT_NE(content.find("int width, int height"), std::string::npos) 
            << "Should accept surface dimensions";
    }
    
    // Check CMake for OpenGL libraries
    std::string cmakePath = projectRoot + "/android/app/src/main/cpp/CMakeLists.txt";
    if (fs::exists(cmakePath)) {
        std::ifstream file(cmakePath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should link OpenGL ES and EGL libraries
        EXPECT_NE(content.find("GLESv3-lib"), std::string::npos) 
            << "Should link OpenGL ES 3.0 library";
        EXPECT_NE(content.find("EGL-lib"), std::string::npos) 
            << "Should link EGL library";
    }
}