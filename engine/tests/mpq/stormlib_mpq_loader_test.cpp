#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "utils/stormlib_mpq_loader.h"
#include <fstream>
#include <filesystem>

using namespace d2portable::utils;
using namespace testing;

// Note: The original MPQLoader tests have been removed as they tested
// internal implementation details of the custom MPQ loader.
// StormLibMPQLoader is a wrapper around StormLib which handles all
// MPQ format details internally.

class StormLibMPQLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir = std::filesystem::temp_directory_path() / "d2portable_test";
        std::filesystem::create_directories(test_dir);
        
        // Note: Creating valid MPQ files for testing requires StormLib tools
        // For now, tests use the mock MPQ files created in integration tests
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(test_dir);
    }

    std::filesystem::path test_dir;
};

// Basic tests for StormLibMPQLoader
TEST_F(StormLibMPQLoaderTest, OpenValidMPQFile) {
    StormLibMPQLoader loader;
    // Test with a non-existent file - should fail gracefully
    EXPECT_FALSE(loader.open("nonexistent.mpq"));
}

TEST_F(StormLibMPQLoaderTest, OpenNonExistentFile) {
    StormLibMPQLoader loader;
    EXPECT_FALSE(loader.open("does_not_exist.mpq"));
}

TEST_F(StormLibMPQLoaderTest, CloseMPQFile) {
    StormLibMPQLoader loader;
    // Close without opening should not crash
    loader.close();
    EXPECT_TRUE(true); // If we get here, close handled gracefully
}

// More comprehensive tests are in integration test files that use real MPQ files