#include <gtest/gtest.h>
#include "extraction/iso_extractor.h"
#include <filesystem>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>

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
    
    // Helper to create an ISO with files
    void createISOWithFiles(const fs::path& iso_path) {
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
        
        // Root directory record location (offset 156) - put at sector 20
        uint32_t root_dir_sector = 20;
        // Location of extent (LBA) - offset 2 in directory record
        pvd[156 + 2] = root_dir_sector & 0xFF;
        pvd[156 + 3] = (root_dir_sector >> 8) & 0xFF;
        pvd[156 + 4] = (root_dir_sector >> 16) & 0xFF;
        pvd[156 + 5] = (root_dir_sector >> 24) & 0xFF;
        // Big-endian
        pvd[156 + 6] = (root_dir_sector >> 24) & 0xFF;
        pvd[156 + 7] = (root_dir_sector >> 16) & 0xFF;
        pvd[156 + 8] = (root_dir_sector >> 8) & 0xFF;
        pvd[156 + 9] = root_dir_sector & 0xFF;
        
        // Data length of root directory - offset 10 in directory record
        uint32_t root_dir_size = 2048; // One sector
        pvd[156 + 10] = root_dir_size & 0xFF;
        pvd[156 + 11] = (root_dir_size >> 8) & 0xFF;
        pvd[156 + 12] = (root_dir_size >> 16) & 0xFF;
        pvd[156 + 13] = (root_dir_size >> 24) & 0xFF;
        // Big-endian
        pvd[156 + 14] = (root_dir_size >> 24) & 0xFF;
        pvd[156 + 15] = (root_dir_size >> 16) & 0xFF;
        pvd[156 + 16] = (root_dir_size >> 8) & 0xFF;
        pvd[156 + 17] = root_dir_size & 0xFF;
        
        // Directory record length
        pvd[156 + 0] = 34; // Minimum directory record size
        
        // Flags (offset 25) - 0x02 for directory
        pvd[156 + 25] = 0x02;
        
        // File identifier length (offset 32)
        pvd[156 + 32] = 1; // Root directory has identifier length 1
        // File identifier (offset 33)
        pvd[156 + 33] = 0x00; // Root directory identifier
        
        file.write(reinterpret_cast<char*>(pvd.data()), pvd.size());
        
        // Write Volume Descriptor Set Terminator at sector 17
        std::vector<uint8_t> terminator(2048, 0);
        terminator[0] = 0xFF; // Type code for terminator
        std::memcpy(terminator.data() + 1, "CD001", 5);
        terminator[6] = 0x01;
        
        file.write(reinterpret_cast<char*>(terminator.data()), terminator.size());
        
        // Skip to sector 20 for root directory
        file.seekp(20 * 2048);
        
        // Write root directory at sector 20
        std::vector<uint8_t> root_dir(2048, 0);
        size_t offset = 0;
        
        // Self entry "."
        root_dir[offset + 0] = 34; // Record length
        root_dir[offset + 2] = 20; // Location (sector 20)
        root_dir[offset + 10] = 0x08; // Data length (2048 bytes)
        root_dir[offset + 25] = 0x02; // Directory flag
        root_dir[offset + 32] = 1; // Identifier length
        root_dir[offset + 33] = 0x00; // "." identifier
        offset += 34;
        
        // Parent entry ".."
        root_dir[offset + 0] = 34; // Record length
        root_dir[offset + 2] = 20; // Location (same as self for root)
        root_dir[offset + 10] = 0x08; // Data length
        root_dir[offset + 25] = 0x02; // Directory flag
        root_dir[offset + 32] = 1; // Identifier length
        root_dir[offset + 33] = 0x01; // ".." identifier
        offset += 34;
        
        // Add a file entry: "D2DATA.MPQ"
        std::string filename = "D2DATA.MPQ";
        uint8_t record_len = 33 + filename.length() + (filename.length() % 2 == 0 ? 1 : 0);
        root_dir[offset + 0] = record_len; // Record length
        root_dir[offset + 2] = 21; // Location (sector 21)
        root_dir[offset + 10] = 0x00; // Data length (0 for empty file)
        root_dir[offset + 11] = 0x10; // 4096 bytes
        root_dir[offset + 25] = 0x00; // File flag (not directory)
        root_dir[offset + 32] = filename.length(); // Identifier length
        std::memcpy(root_dir.data() + offset + 33, filename.c_str(), filename.length());
        offset += record_len;
        
        // Add another file: "D2EXP.MPQ"
        filename = "D2EXP.MPQ";
        record_len = 33 + filename.length() + (filename.length() % 2 == 0 ? 1 : 0);
        root_dir[offset + 0] = record_len; // Record length
        root_dir[offset + 2] = 22; // Location (sector 22)
        root_dir[offset + 10] = 0x00; // Data length
        root_dir[offset + 11] = 0x20; // 8192 bytes
        root_dir[offset + 25] = 0x00; // File flag
        root_dir[offset + 32] = filename.length(); // Identifier length
        std::memcpy(root_dir.data() + offset + 33, filename.c_str(), filename.length());
        
        file.write(reinterpret_cast<char*>(root_dir.data()), root_dir.size());
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

// Test 7: List files in ISO should return file list
TEST_F(ISOExtractorTest, ListFilesInISO) {
    fs::path iso_path = test_dir / "test_with_files.iso";
    createISOWithFiles(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    auto files = extractor.listFiles();
    EXPECT_EQ(files.size(), 2u);
    EXPECT_TRUE(std::find(files.begin(), files.end(), "D2DATA.MPQ") != files.end());
    EXPECT_TRUE(std::find(files.begin(), files.end(), "D2EXP.MPQ") != files.end());
}

// Test 8: Extract file from ISO should succeed
TEST_F(ISOExtractorTest, ExtractFileFromISO) {
    // Create ISO with actual file content
    fs::path iso_path = test_dir / "test_extract.iso";
    {
        std::ofstream file(iso_path, std::ios::binary);
        
        // Write 16 sectors of zeros (system area)
        std::vector<uint8_t> sector(2048, 0);
        for (int i = 0; i < 16; ++i) {
            file.write(reinterpret_cast<char*>(sector.data()), sector.size());
        }
        
        // Write Primary Volume Descriptor at sector 16
        std::vector<uint8_t> pvd(2048, 0);
        pvd[0] = 0x01;
        std::memcpy(pvd.data() + 1, "CD001", 5);
        pvd[6] = 0x01;
        
        // Volume Space Size
        uint32_t volume_size = 100;
        pvd[80] = volume_size & 0xFF;
        pvd[81] = (volume_size >> 8) & 0xFF;
        pvd[82] = (volume_size >> 16) & 0xFF;
        pvd[83] = (volume_size >> 24) & 0xFF;
        pvd[84] = (volume_size >> 24) & 0xFF;
        pvd[85] = (volume_size >> 16) & 0xFF;
        pvd[86] = (volume_size >> 8) & 0xFF;
        pvd[87] = volume_size & 0xFF;
        
        // Root directory at sector 20
        uint32_t root_dir_sector = 20;
        pvd[156 + 2] = root_dir_sector & 0xFF;
        pvd[156 + 3] = (root_dir_sector >> 8) & 0xFF;
        pvd[156 + 4] = (root_dir_sector >> 16) & 0xFF;
        pvd[156 + 5] = (root_dir_sector >> 24) & 0xFF;
        pvd[156 + 6] = (root_dir_sector >> 24) & 0xFF;
        pvd[156 + 7] = (root_dir_sector >> 16) & 0xFF;
        pvd[156 + 8] = (root_dir_sector >> 8) & 0xFF;
        pvd[156 + 9] = root_dir_sector & 0xFF;
        
        uint32_t root_dir_size = 2048;
        pvd[156 + 10] = root_dir_size & 0xFF;
        pvd[156 + 11] = (root_dir_size >> 8) & 0xFF;
        pvd[156 + 12] = (root_dir_size >> 16) & 0xFF;
        pvd[156 + 13] = (root_dir_size >> 24) & 0xFF;
        pvd[156 + 14] = (root_dir_size >> 24) & 0xFF;
        pvd[156 + 15] = (root_dir_size >> 16) & 0xFF;
        pvd[156 + 16] = (root_dir_size >> 8) & 0xFF;
        pvd[156 + 17] = root_dir_size & 0xFF;
        
        pvd[156 + 0] = 34;
        pvd[156 + 25] = 0x02;
        pvd[156 + 32] = 1;
        pvd[156 + 33] = 0x00;
        
        file.write(reinterpret_cast<char*>(pvd.data()), pvd.size());
        
        // Write Volume Descriptor Set Terminator at sector 17
        std::vector<uint8_t> terminator(2048, 0);
        terminator[0] = 0xFF;
        std::memcpy(terminator.data() + 1, "CD001", 5);
        terminator[6] = 0x01;
        file.write(reinterpret_cast<char*>(terminator.data()), terminator.size());
        
        // Skip to sector 20 for root directory
        file.seekp(20 * 2048);
        
        // Write root directory at sector 20
        std::vector<uint8_t> root_dir(2048, 0);
        size_t offset = 0;
        
        // Self entry "."
        root_dir[offset + 0] = 34;
        root_dir[offset + 2] = 20;
        root_dir[offset + 10] = 0x08;
        root_dir[offset + 25] = 0x02;
        root_dir[offset + 32] = 1;
        root_dir[offset + 33] = 0x00;
        offset += 34;
        
        // Parent entry ".."
        root_dir[offset + 0] = 34;
        root_dir[offset + 2] = 20;
        root_dir[offset + 10] = 0x08;
        root_dir[offset + 25] = 0x02;
        root_dir[offset + 32] = 1;
        root_dir[offset + 33] = 0x01;
        offset += 34;
        
        // Add a file entry: "TEST.MPQ" with actual content
        std::string filename = "TEST.MPQ";
        std::string fileContent = "This is test MPQ file content!";
        uint32_t fileSize = fileContent.length();
        
        uint8_t record_len = 33 + filename.length() + (filename.length() % 2 == 0 ? 1 : 0);
        root_dir[offset + 0] = record_len;
        root_dir[offset + 2] = 21; // File at sector 21
        
        // File size (both-endian)
        root_dir[offset + 10] = fileSize & 0xFF;
        root_dir[offset + 11] = (fileSize >> 8) & 0xFF;
        root_dir[offset + 12] = (fileSize >> 16) & 0xFF;
        root_dir[offset + 13] = (fileSize >> 24) & 0xFF;
        root_dir[offset + 14] = (fileSize >> 24) & 0xFF;
        root_dir[offset + 15] = (fileSize >> 16) & 0xFF;
        root_dir[offset + 16] = (fileSize >> 8) & 0xFF;
        root_dir[offset + 17] = fileSize & 0xFF;
        
        root_dir[offset + 25] = 0x00; // File flag
        root_dir[offset + 32] = filename.length();
        std::memcpy(root_dir.data() + offset + 33, filename.c_str(), filename.length());
        
        file.write(reinterpret_cast<char*>(root_dir.data()), root_dir.size());
        
        // Write file content at sector 21
        file.seekp(21 * 2048);
        file.write(fileContent.c_str(), fileContent.length());
    }
    
    // Now test extraction
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    fs::path output_path = test_dir / "extracted_test.mpq";
    EXPECT_TRUE(extractor.extractFile("TEST.MPQ", output_path.string()));
    
    // Verify the extracted file exists and has correct content
    EXPECT_TRUE(fs::exists(output_path));
    
    std::ifstream extracted(output_path);
    std::string content((std::istreambuf_iterator<char>(extracted)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "This is test MPQ file content!");
}