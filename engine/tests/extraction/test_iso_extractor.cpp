#include <gtest/gtest.h>
#include "extraction/iso_extractor.h"
#include <filesystem>
#include <fstream>

using namespace d2;
namespace fs = std::filesystem;

class ISOExtractorTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir = fs::temp_directory_path() / "iso_extractor_test";
        fs::create_directories(test_dir);
    }
    
    void TearDown() override {
        fs::remove_all(test_dir);
    }
    
    fs::path test_dir;
};

// Test 1: Create ISOExtractor instance
TEST_F(ISOExtractorTest, CreateISOExtractor) {
    ISOExtractor extractor;
    EXPECT_FALSE(extractor.isOpen());
}

// Test 2: Open non-existent ISO file should fail
TEST_F(ISOExtractorTest, OpenNonExistentISOFails) {
    ISOExtractor extractor;
    fs::path non_existent = test_dir / "does_not_exist.iso";
    
    EXPECT_FALSE(extractor.open(non_existent.string()));
    EXPECT_FALSE(extractor.isOpen());
    EXPECT_FALSE(extractor.getLastError().empty());
}

// Test 3: List files in ISO should return empty for unopened ISO
TEST_F(ISOExtractorTest, ListFilesWhenNotOpen) {
    ISOExtractor extractor;
    
    auto files = extractor.listFiles();
    EXPECT_TRUE(files.empty());
}