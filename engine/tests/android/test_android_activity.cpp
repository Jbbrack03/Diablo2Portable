#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class AndroidActivityTest : public ::testing::Test {
protected:
    void SetUp() override {
        project_root = fs::current_path();
        java_src_path = project_root / "android" / "app" / "src" / "main" / "java" / "com" / "diablo2portable";
    }
    
    fs::path project_root;
    fs::path java_src_path;
    
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

TEST_F(AndroidActivityTest, MainActivityExists) {
    fs::path main_activity_path = java_src_path / "MainActivity.java";
    EXPECT_TRUE(fs::exists(main_activity_path)) << "MainActivity.java should exist";
}

TEST_F(AndroidActivityTest, MainActivityExtendsAppCompatActivity) {
    fs::path main_activity_path = java_src_path / "MainActivity.java";
    EXPECT_TRUE(fileContains(main_activity_path, "extends AppCompatActivity")) 
        << "MainActivity should extend AppCompatActivity";
}

TEST_F(AndroidActivityTest, MainActivityHasNativeEngineField) {
    fs::path main_activity_path = java_src_path / "MainActivity.java";
    EXPECT_TRUE(fileContains(main_activity_path, "private NativeEngine nativeEngine")) 
        << "MainActivity should have NativeEngine field";
}

TEST_F(AndroidActivityTest, MainActivityHasOnCreateMethod) {
    fs::path main_activity_path = java_src_path / "MainActivity.java";
    EXPECT_TRUE(fileContains(main_activity_path, "protected void onCreate")) 
        << "MainActivity should have onCreate method";
}

TEST_F(AndroidActivityTest, MainActivityHasOnDestroyMethod) {
    fs::path main_activity_path = java_src_path / "MainActivity.java";
    EXPECT_TRUE(fileContains(main_activity_path, "protected void onDestroy")) 
        << "MainActivity should have onDestroy method";
}

TEST_F(AndroidActivityTest, NativeEngineClassExists) {
    fs::path native_engine_path = java_src_path / "NativeEngine.java";
    EXPECT_TRUE(fs::exists(native_engine_path)) << "NativeEngine.java should exist";
}

TEST_F(AndroidActivityTest, NativeEngineHasCreateEngineMethod) {
    fs::path native_engine_path = java_src_path / "NativeEngine.java";
    EXPECT_TRUE(fileContains(native_engine_path, "public static native long createEngine")) 
        << "NativeEngine should have createEngine native method";
}

TEST_F(AndroidActivityTest, NativeEngineHasDestroyEngineMethod) {
    fs::path native_engine_path = java_src_path / "NativeEngine.java";
    EXPECT_TRUE(fileContains(native_engine_path, "public static native void destroyEngine")) 
        << "NativeEngine should have destroyEngine native method";
}

TEST_F(AndroidActivityTest, NativeEngineLoadsLibrary) {
    fs::path native_engine_path = java_src_path / "NativeEngine.java";
    EXPECT_TRUE(fileContains(native_engine_path, "System.loadLibrary")) 
        << "NativeEngine should load native library";
}

TEST_F(AndroidActivityTest, GameSurfaceViewExists) {
    fs::path game_surface_path = java_src_path / "GameSurfaceView.java";
    EXPECT_TRUE(fs::exists(game_surface_path)) << "GameSurfaceView.java should exist";
}

TEST_F(AndroidActivityTest, GameSurfaceViewExtendsGLSurfaceView) {
    fs::path game_surface_path = java_src_path / "GameSurfaceView.java";
    EXPECT_TRUE(fileContains(game_surface_path, "extends GLSurfaceView")) 
        << "GameSurfaceView should extend GLSurfaceView";
}

TEST_F(AndroidActivityTest, GameRendererExists) {
    fs::path game_renderer_path = java_src_path / "GameRenderer.java";
    EXPECT_TRUE(fs::exists(game_renderer_path)) << "GameRenderer.java should exist";
}

TEST_F(AndroidActivityTest, GameRendererImplementsRenderer) {
    fs::path game_renderer_path = java_src_path / "GameRenderer.java";
    EXPECT_TRUE(fileContains(game_renderer_path, "implements GLSurfaceView.Renderer")) 
        << "GameRenderer should implement GLSurfaceView.Renderer";
}