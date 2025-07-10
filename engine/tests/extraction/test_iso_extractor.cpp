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