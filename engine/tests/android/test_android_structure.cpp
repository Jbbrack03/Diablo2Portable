#include <gtest/gtest.h>
#include <filesystem>

namespace fs = std::filesystem;

class AndroidStructureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test is already running from project root
        project_root = fs::current_path();
    }
    
    fs::path project_root;
};

TEST_F(AndroidStructureTest, AndroidDirectoryStructureExists) {
    // Test that the required Android project structure exists
    EXPECT_TRUE(fs::exists(project_root / "android"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "app"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "app" / "src"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "app" / "src" / "main"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "app" / "src" / "main" / "java"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "app" / "src" / "main" / "java" / "com"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "app" / "src" / "main" / "java" / "com" / "diablo2portable"));
}

TEST_F(AndroidStructureTest, AndroidManifestExists) {
    fs::path manifest_path = project_root / "android" / "app" / "src" / "main" / "AndroidManifest.xml";
    EXPECT_TRUE(fs::exists(manifest_path));
}

TEST_F(AndroidStructureTest, GradleBuildFilesExist) {
    EXPECT_TRUE(fs::exists(project_root / "android" / "app" / "build.gradle"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "settings.gradle"));
    EXPECT_TRUE(fs::exists(project_root / "android" / "gradle.properties"));
}

TEST_F(AndroidStructureTest, NativeLibDirectoryExists) {
    EXPECT_TRUE(fs::exists(project_root / "android" / "app" / "src" / "main" / "cpp"));
}