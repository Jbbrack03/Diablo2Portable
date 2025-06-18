#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class AndroidAssetsTest : public ::testing::Test {
protected:
    void SetUp() override {
        project_root = fs::current_path();
        android_assets_path = project_root / "android" / "app" / "src" / "main" / "assets";
        android_cpp_path = project_root / "android" / "app" / "src" / "main" / "cpp";
    }
    
    fs::path project_root;
    fs::path android_assets_path;
    fs::path android_cpp_path;
    
    bool fileContains(const fs::path& file_path, const std::string& content) {
        if (!fs::exists(file_path)) {
            return false;
        }
        
        std::ifstream file(file_path);
        std::string file_content((std::istreambuf_iterator<char>(file)), 
                                 std::istreambuf_iterator<char>());
        return file_content.find(content) != std::string::npos;
    }
};

TEST_F(AndroidAssetsTest, AssetsDirectoryExists) {
    EXPECT_TRUE(fs::exists(android_assets_path)) << "Android assets directory should exist";
}

TEST_F(AndroidAssetsTest, AndroidCMakeListsExists) {
    fs::path cmake_path = android_cpp_path / "CMakeLists.txt";
    EXPECT_TRUE(fs::exists(cmake_path)) << "Android CMakeLists.txt should exist";
}

TEST_F(AndroidAssetsTest, AndroidCMakeConfiguresNativeLibrary) {
    fs::path cmake_path = android_cpp_path / "CMakeLists.txt";
    EXPECT_TRUE(fileContains(cmake_path, "add_library(d2engine SHARED")) 
        << "CMakeLists.txt should configure shared library for Android";
}

TEST_F(AndroidAssetsTest, AndroidCMakeLinksRequiredLibraries) {
    fs::path cmake_path = android_cpp_path / "CMakeLists.txt";
    EXPECT_TRUE(fileContains(cmake_path, "target_link_libraries")) 
        << "CMakeLists.txt should link required libraries";
}

TEST_F(AndroidAssetsTest, AndroidCMakeIncludesEngineSource) {
    fs::path cmake_path = android_cpp_path / "CMakeLists.txt";
    EXPECT_TRUE(fileContains(cmake_path, "../../../../engine/src")) 
        << "CMakeLists.txt should include engine source files";
}

TEST_F(AndroidAssetsTest, AndroidCMakeTargetsAndroidAPI) {
    fs::path cmake_path = android_cpp_path / "CMakeLists.txt";
    EXPECT_TRUE(fileContains(cmake_path, "android")) 
        << "CMakeLists.txt should target Android API";
}

TEST_F(AndroidAssetsTest, ProguardRulesExist) {
    fs::path proguard_path = project_root / "android" / "app" / "proguard-rules.pro";
    EXPECT_TRUE(fs::exists(proguard_path)) << "ProGuard rules should exist";
}

TEST_F(AndroidAssetsTest, StringResourcesExist) {
    fs::path strings_path = project_root / "android" / "app" / "src" / "main" / "res" / "values" / "strings.xml";
    EXPECT_TRUE(fs::exists(strings_path)) << "String resources should exist";
}

TEST_F(AndroidAssetsTest, AppIconResourcesExist) {
    fs::path drawable_path = project_root / "android" / "app" / "src" / "main" / "res" / "mipmap-hdpi" / "ic_launcher.png";
    EXPECT_TRUE(fs::exists(drawable_path)) << "App icon resources should exist";
}