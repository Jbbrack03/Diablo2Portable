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
        std::string fileContent1 = "D2DATA.MPQ test content";
        uint32_t fileSize1 = fileContent1.length();
        
        uint8_t record_len = 33 + filename.length() + (filename.length() % 2 == 0 ? 1 : 0);
        root_dir[offset + 0] = record_len; // Record length
        root_dir[offset + 2] = 21; // Location (sector 21)
        root_dir[offset + 10] = fileSize1 & 0xFF; // Data length
        root_dir[offset + 11] = (fileSize1 >> 8) & 0xFF;
        root_dir[offset + 12] = (fileSize1 >> 16) & 0xFF;
        root_dir[offset + 13] = (fileSize1 >> 24) & 0xFF;
        root_dir[offset + 14] = (fileSize1 >> 24) & 0xFF;
        root_dir[offset + 15] = (fileSize1 >> 16) & 0xFF;
        root_dir[offset + 16] = (fileSize1 >> 8) & 0xFF;
        root_dir[offset + 17] = fileSize1 & 0xFF;
        root_dir[offset + 25] = 0x00; // File flag (not directory)
        root_dir[offset + 32] = filename.length(); // Identifier length
        std::memcpy(root_dir.data() + offset + 33, filename.c_str(), filename.length());
        offset += record_len;
        
        // Add another file: "D2EXP.MPQ"
        filename = "D2EXP.MPQ";
        std::string fileContent2 = "D2EXP.MPQ test content";
        uint32_t fileSize2 = fileContent2.length();
        
        record_len = 33 + filename.length() + (filename.length() % 2 == 0 ? 1 : 0);
        root_dir[offset + 0] = record_len; // Record length
        root_dir[offset + 2] = 22; // Location (sector 22)
        root_dir[offset + 10] = fileSize2 & 0xFF; // Data length
        root_dir[offset + 11] = (fileSize2 >> 8) & 0xFF;
        root_dir[offset + 12] = (fileSize2 >> 16) & 0xFF;
        root_dir[offset + 13] = (fileSize2 >> 24) & 0xFF;
        root_dir[offset + 14] = (fileSize2 >> 24) & 0xFF;
        root_dir[offset + 15] = (fileSize2 >> 16) & 0xFF;
        root_dir[offset + 16] = (fileSize2 >> 8) & 0xFF;
        root_dir[offset + 17] = fileSize2 & 0xFF;
        root_dir[offset + 25] = 0x00; // File flag
        root_dir[offset + 32] = filename.length(); // Identifier length
        std::memcpy(root_dir.data() + offset + 33, filename.c_str(), filename.length());
        
        file.write(reinterpret_cast<char*>(root_dir.data()), root_dir.size());
        
        // Write file content at sector 21
        file.seekp(21 * 2048);
        file.write(fileContent1.c_str(), fileContent1.length());
        
        // Write file content at sector 22
        file.seekp(22 * 2048);
        file.write(fileContent2.c_str(), fileContent2.length());
    }
    
    // Helper to create an ISO with subdirectories
    void createISOWithSubdirectories(const fs::path& iso_path) {
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
        
        // Write root directory at sector 20
        file.seekp(20 * 2048);
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
        
        // Add a subdirectory: "DATA"
        std::string dirname = "DATA";
        uint8_t record_len = 33 + dirname.length() + (dirname.length() % 2 == 0 ? 1 : 0);
        root_dir[offset + 0] = record_len;
        root_dir[offset + 2] = 21; // DATA directory at sector 21
        root_dir[offset + 3] = 0;
        root_dir[offset + 4] = 0;
        root_dir[offset + 5] = 0;
        root_dir[offset + 6] = 0;
        root_dir[offset + 7] = 0;
        root_dir[offset + 8] = 0;
        root_dir[offset + 9] = 21;
        root_dir[offset + 10] = 0x00; // Size low byte
        root_dir[offset + 11] = 0x08; // Size 2048 bytes (0x0800)
        root_dir[offset + 12] = 0;
        root_dir[offset + 13] = 0;
        root_dir[offset + 14] = 0;
        root_dir[offset + 15] = 0;
        root_dir[offset + 16] = 0x08;
        root_dir[offset + 17] = 0x00;
        root_dir[offset + 25] = 0x02; // Directory flag
        root_dir[offset + 32] = dirname.length();
        std::memcpy(root_dir.data() + offset + 33, dirname.c_str(), dirname.length());
        offset += record_len;
        
        file.write(reinterpret_cast<char*>(root_dir.data()), root_dir.size());
        
        // Write DATA directory at sector 21
        file.seekp(21 * 2048);
        std::vector<uint8_t> data_dir(2048, 0);
        offset = 0;
        
        // Self entry "."
        data_dir[offset + 0] = 34;
        data_dir[offset + 2] = 21;
        data_dir[offset + 10] = 0x00;
        data_dir[offset + 11] = 0x08;
        data_dir[offset + 25] = 0x02;
        data_dir[offset + 32] = 1;
        data_dir[offset + 33] = 0x00;
        offset += 34;
        
        // Parent entry ".."
        data_dir[offset + 0] = 34;
        data_dir[offset + 2] = 20;
        data_dir[offset + 10] = 0x00;
        data_dir[offset + 11] = 0x08;
        data_dir[offset + 25] = 0x02;
        data_dir[offset + 32] = 1;
        data_dir[offset + 33] = 0x01;
        offset += 34;
        
        // Add a file in subdirectory: "GLOBAL.MPQ"
        std::string filename = "GLOBAL.MPQ";
        std::string fileContent = "Global MPQ content";
        uint32_t fileSize = fileContent.length();
        
        record_len = 33 + filename.length() + (filename.length() % 2 == 0 ? 1 : 0);
        data_dir[offset + 0] = record_len;
        data_dir[offset + 2] = 22; // File at sector 22
        data_dir[offset + 10] = fileSize & 0xFF;
        data_dir[offset + 11] = (fileSize >> 8) & 0xFF;
        data_dir[offset + 12] = (fileSize >> 16) & 0xFF;
        data_dir[offset + 13] = (fileSize >> 24) & 0xFF;
        data_dir[offset + 14] = (fileSize >> 24) & 0xFF;
        data_dir[offset + 15] = (fileSize >> 16) & 0xFF;
        data_dir[offset + 16] = (fileSize >> 8) & 0xFF;
        data_dir[offset + 17] = fileSize & 0xFF;
        data_dir[offset + 25] = 0x00; // File flag
        data_dir[offset + 32] = filename.length();
        std::memcpy(data_dir.data() + offset + 33, filename.c_str(), filename.length());
        
        file.write(reinterpret_cast<char*>(data_dir.data()), data_dir.size());
        
        // Write file content at sector 22
        file.seekp(22 * 2048);
        file.write(fileContent.c_str(), fileContent.length());
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

// Test 9: Extract non-existent file should fail
TEST_F(ISOExtractorTest, ExtractNonExistentFileFails) {
    fs::path iso_path = test_dir / "test_with_files.iso";
    createISOWithFiles(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    fs::path output_path = test_dir / "nonexistent.mpq";
    EXPECT_FALSE(extractor.extractFile("NONEXISTENT.MPQ", output_path.string()));
    EXPECT_FALSE(fs::exists(output_path));
    EXPECT_FALSE(extractor.getLastError().empty());
    EXPECT_TRUE(extractor.getLastError().find("File not found") != std::string::npos);
}

// Test 10: Extract all files from ISO should succeed
TEST_F(ISOExtractorTest, ExtractAllFilesFromISO) {
    fs::path iso_path = test_dir / "test_with_files.iso";
    createISOWithFiles(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    fs::path output_dir = test_dir / "extracted_all";
    fs::create_directories(output_dir);
    
    // Debug: Check what files are listed
    auto files = extractor.listFiles();
    EXPECT_EQ(files.size(), 2u);
    
    EXPECT_TRUE(extractor.extractAll(output_dir.string())) << "Error: " << extractor.getLastError();
    
    // Verify both files were extracted
    EXPECT_TRUE(fs::exists(output_dir / "D2DATA.MPQ"));
    EXPECT_TRUE(fs::exists(output_dir / "D2EXP.MPQ"));
}

// Test 11: Get file info should return correct size and location
TEST_F(ISOExtractorTest, GetFileInfo) {
    fs::path iso_path = test_dir / "test_with_files.iso";
    createISOWithFiles(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    auto info = extractor.getFileInfo("D2DATA.MPQ");
    EXPECT_TRUE(info.exists);
    EXPECT_EQ(info.size, 23u); // "D2DATA.MPQ test content" is 23 bytes
    EXPECT_EQ(info.sector, 21u);
    
    info = extractor.getFileInfo("D2EXP.MPQ");
    EXPECT_TRUE(info.exists);
    EXPECT_EQ(info.size, 22u); // "D2EXP.MPQ test content" is 22 bytes
    EXPECT_EQ(info.sector, 22u);
    
    // Non-existent file
    info = extractor.getFileInfo("NOTEXIST.MPQ");
    EXPECT_FALSE(info.exists);
}

// Test 12: Extract from real ISO file should work with actual Diablo II ISOs
TEST_F(ISOExtractorTest, ExtractFromRealISO) {
    // Check if real ISO files are available
    fs::path real_iso_dir = "/Users/jbbrack03/Diablo2Portable/vendor/Diablo II CD Images";
    fs::path install_iso = real_iso_dir / "Diablo II install.iso";
    
    if (!fs::exists(install_iso)) {
        GTEST_SKIP() << "Real Diablo II ISO not available for testing";
    }
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(install_iso.string()));
    
    // List files to see what's in the ISO
    auto files = extractor.listFiles();
    EXPECT_GT(files.size(), 0u);
    
    // Debug: Print some files found
    std::cout << "Found " << files.size() << " files in ISO:\n";
    for (size_t i = 0; i < std::min(size_t(10), files.size()); ++i) {
        std::cout << "  " << files[i] << "\n";
    }
    
    // Look for common Diablo II files
    bool found_mpq = false;
    std::string mpq_file;
    for (const auto& file : files) {
        if (file.find(".MPQ") != std::string::npos || 
            file.find(".mpq") != std::string::npos) {
            found_mpq = true;
            mpq_file = file;
            break;
        }
    }
    EXPECT_TRUE(found_mpq) << "No MPQ files found in Diablo II ISO";
    
    if (found_mpq) {
        // Try to extract an MPQ file
        fs::path extract_dir = test_dir / "real_iso_extract";
        fs::create_directories(extract_dir);
        
        fs::path output_file = extract_dir / mpq_file;
        EXPECT_TRUE(extractor.extractFile(mpq_file, output_file.string()));
        EXPECT_TRUE(fs::exists(output_file));
        EXPECT_GT(fs::file_size(output_file), 0u);
    }
}

// Test 13: List files with subdirectories should show all files recursively
TEST_F(ISOExtractorTest, ListFilesRecursively) {
    fs::path iso_path = test_dir / "test_with_subdirs.iso";
    createISOWithSubdirectories(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    // Call the recursive version
    auto files = extractor.listFilesRecursive();
    
    // Should find the file in the subdirectory
    EXPECT_EQ(files.size(), 1u);
    EXPECT_EQ(files[0], "DATA/GLOBAL.MPQ");
}

// Test 14: Extract file from subdirectory
TEST_F(ISOExtractorTest, ExtractFileFromSubdirectory) {
    fs::path iso_path = test_dir / "test_with_subdirs.iso";
    createISOWithSubdirectories(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    // Extract file from subdirectory
    fs::path output_path = test_dir / "extracted_global.mpq";
    EXPECT_TRUE(extractor.extractFile("DATA/GLOBAL.MPQ", output_path.string()));
    
    // Verify the file was extracted
    EXPECT_TRUE(fs::exists(output_path));
    
    // Verify content
    std::ifstream extracted(output_path);
    std::string content((std::istreambuf_iterator<char>(extracted)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Global MPQ content");
}

// Test 15: Extract all files including subdirectories
TEST_F(ISOExtractorTest, ExtractAllIncludingSubdirectories) {
    fs::path iso_path = test_dir / "test_with_subdirs.iso";
    createISOWithSubdirectories(iso_path);
    
    ISOExtractor extractor;
    EXPECT_TRUE(extractor.open(iso_path.string()));
    
    // Extract all files
    fs::path output_dir = test_dir / "extracted_all_subdirs";
    fs::create_directories(output_dir);
    
    EXPECT_TRUE(extractor.extractAll(output_dir.string()));
    
    // Verify subdirectory was created
    EXPECT_TRUE(fs::exists(output_dir / "DATA"));
    EXPECT_TRUE(fs::is_directory(output_dir / "DATA"));
    
    // Verify file was extracted to subdirectory
    EXPECT_TRUE(fs::exists(output_dir / "DATA" / "GLOBAL.MPQ"));
    
    // Verify content
    std::ifstream extracted(output_dir / "DATA" / "GLOBAL.MPQ");
    std::string content((std::istreambuf_iterator<char>(extracted)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Global MPQ content");
}