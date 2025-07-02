#include <gtest/gtest.h>
#include "release/release_builder.h"
#include <string>
#include <filesystem>

using namespace d2::release;
namespace fs = std::filesystem;

// Phase 20 Task 3: Release Preparation - Following strict TDD
// These tests verify release build process

// TEST 1: Create release builder
TEST(ReleaseBuilderTest, CreateReleaseBuilder) {
    ReleaseBuilder builder;
    // Release builder should exist
}

// TEST 2: Configure release build
TEST(ReleaseBuilderTest, ConfigureReleaseBuild) {
    ReleaseBuilder builder;
    
    // Configure build settings
    builder.setProjectName("Diablo2Portable");
    builder.setVersion("1.0.0");
    builder.setTargetPlatform("Android");
    builder.setMinSDKVersion(26);
    
    // Verify configuration
    EXPECT_EQ(builder.getProjectName(), "Diablo2Portable");
    EXPECT_EQ(builder.getVersion(), "1.0.0");
    EXPECT_EQ(builder.getTargetPlatform(), "Android");
    EXPECT_EQ(builder.getMinSDKVersion(), 26);
}

// TEST 3: Generate build script
TEST(ReleaseBuilderTest, GenerateBuildScript) {
    ReleaseBuilder builder;
    
    // Configure
    builder.setProjectName("Diablo2Portable");
    builder.setVersion("1.0.0");
    
    // Generate build script
    std::string script = builder.generateBuildScript();
    
    // Script should contain key commands
    EXPECT_TRUE(script.find("#!/bin/bash") != std::string::npos);
    EXPECT_TRUE(script.find("cmake") != std::string::npos);
    EXPECT_TRUE(script.find("gradlew assembleRelease") != std::string::npos);
}

// TEST 4: Generate installation guide
TEST(ReleaseBuilderTest, GenerateInstallationGuide) {
    ReleaseBuilder builder;
    
    // Configure
    builder.setProjectName("Diablo2Portable");
    builder.setVersion("1.0.0");
    builder.setMinSDKVersion(26);
    
    // Generate installation guide
    std::string guide = builder.generateInstallationGuide();
    
    // Guide should contain key sections
    EXPECT_TRUE(guide.find("Installation Guide") != std::string::npos);
    EXPECT_TRUE(guide.find("Requirements") != std::string::npos);
    EXPECT_TRUE(guide.find("Android 8.0") != std::string::npos);
    EXPECT_TRUE(guide.find("Diablo II game files") != std::string::npos);
}