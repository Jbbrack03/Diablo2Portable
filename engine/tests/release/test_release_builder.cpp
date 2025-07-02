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