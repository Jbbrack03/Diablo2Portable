#include <gtest/gtest.h>
#include "extraction/iso_extractor.h"
#include <filesystem>
#include <fstream>
#include <cstring>
#include <vector>

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
    
    // Helper to create a minimal valid ISO 9660 file
    void createMinimalISO(const fs::path& iso_path) {
        std::ofstream file(iso_path, std::ios::binary);
        
        // Write 16 sectors of zeros (system area)
        std::vector<uint8_t> sector(2048, 0);
        for (int i = 0; i < 16; ++i) {
            file.write(reinterpret_cast<char*>(sector.data()), sector.size());
        }
        
        // Write Primary Volume Descriptor at sector 16
        std::vector<uint8_t> pvd(2048, 0);
        pvd[0] = 0x01; // Type code for Primary Volume Descriptor
        std::memcpy(pvd.data() + 1, "CD001", 5); // Standard identifier
        pvd[6] = 0x01; // Version
        
        // Volume Space Size (both-endian format at offset 80)
        uint32_t volume_size = 100; // 100 sectors
        // Little-endian
        pvd[80] = volume_size & 0xFF;
        pvd[81] = (volume_size >> 8) & 0xFF;
        pvd[82] = (volume_size >> 16) & 0xFF;
        pvd[83] = (volume_size >> 24) & 0xFF;
        // Big-endian
        pvd[84] = (volume_size >> 24) & 0xFF;
        pvd[85] = (volume_size >> 16) & 0xFF;
        pvd[86] = (volume_size >> 8) & 0xFF;
        pvd[87] = volume_size & 0xFF;
        
        file.write(reinterpret_cast<char*>(pvd.data()), pvd.size());
        
        // Write Volume Descriptor Set Terminator at sector 17
        std::vector<uint8_t> terminator(2048, 0);
        terminator[0] = 0xFF; // Type code for terminator
        std::memcpy(terminator.data() + 1, "CD001", 5);
        terminator[6] = 0x01;
        
        file.write(reinterpret_cast<char*>(terminator.data()), terminator.size());
    }
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

// Test 4: Extract file from unopened ISO should fail
TEST_F(ISOExtractorTest, ExtractFileWhenNotOpen) {
    ISOExtractor extractor;
    fs::path output_path = test_dir / "extracted_file.mpq";
    
    EXPECT_FALSE(extractor.extractFile("some_file.mpq", output_path.string()));
    EXPECT_FALSE(fs::exists(output_path));
}

// Test 5: Open valid ISO file should succeed
TEST_F(ISOExtractorTest, OpenValidISO) {
    fs::path iso_path = test_dir / "test.iso";
    createMinimalISO(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    EXPECT_TRUE(extractor.isOpen());
    EXPECT_TRUE(extractor.getLastError().empty());
}

// Test 6: Close ISO file should work
TEST_F(ISOExtractorTest, CloseISO) {
    fs::path iso_path = test_dir / "test.iso";
    createMinimalISO(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    EXPECT_TRUE(extractor.isOpen());
    
    extractor.close();
    EXPECT_FALSE(extractor.isOpen());
}