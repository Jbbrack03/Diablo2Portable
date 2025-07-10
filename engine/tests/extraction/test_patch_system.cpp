#include <gtest/gtest.h>
#include "extraction/patch_system.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class PatchSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir = fs::temp_directory_path() / "patch_system_test";
        fs::create_directories(test_dir);
    }
    
    void TearDown() override {
        // Clean up test directory
        fs::remove_all(test_dir);
    }
    
    fs::path test_dir;
};

TEST_F(PatchSystemTest, DetectStandalonePatchMPQ) {
    // Create a mock patch MPQ file
    fs::path patch_path = test_dir / "patch.mpq";
    std::ofstream patch_file(patch_path, std::ios::binary);
    // Write minimal MPQ header (32 bytes)
    char header[32] = {'M', 'P', 'Q', 0x1A}; // MPQ signature
    patch_file.write(header, sizeof(header));
    patch_file.close();
    
    d2::PatchSystem patch_system;
    auto detected_patches = patch_system.detectPatches(test_dir);
    
    ASSERT_EQ(detected_patches.size(), 1);
    EXPECT_EQ(detected_patches[0].getFilename(), "patch.mpq");
    EXPECT_EQ(detected_patches[0].getType(), d2::PatchType::STANDALONE_MPQ);
}

TEST_F(PatchSystemTest, DetectPatchExecutable) {
    // Create a mock patch executable with embedded MPQ
    fs::path patch_path = test_dir / "LODPatch_114d.exe";
    std::ofstream patch_file(patch_path, std::ios::binary);
    
    // Write a simple PE header (DOS stub + PE signature)
    // This is a minimal valid PE file structure
    char dos_header[128] = {'M', 'Z'}; // DOS header signature
    dos_header[60] = static_cast<char>(128); // e_lfanew points to PE header
    patch_file.write(dos_header, sizeof(dos_header));
    
    // PE header
    char pe_header[4] = {'P', 'E', 0, 0};
    patch_file.write(pe_header, sizeof(pe_header));
    
    // Add some padding
    char padding[1024] = {0};
    patch_file.write(padding, sizeof(padding));
    
    // Embed an MPQ file at offset 2048
    patch_file.seekp(2048);
    char mpq_header[32] = {'M', 'P', 'Q', 0x1A};
    patch_file.write(mpq_header, sizeof(mpq_header));
    patch_file.close();
    
    d2::PatchSystem patch_system;
    auto detected_patches = patch_system.detectPatches(test_dir);
    
    ASSERT_EQ(detected_patches.size(), 1);
    EXPECT_EQ(detected_patches[0].getFilename(), "LODPatch_114d.exe");
    EXPECT_EQ(detected_patches[0].getType(), d2::PatchType::PATCH_EXECUTABLE);
}

TEST_F(PatchSystemTest, DetectPatchVersionFromFilename) {
    // Create patches with version in filename
    fs::path patch114d = test_dir / "LODPatch_114d.exe";
    fs::path patch113c = test_dir / "D2Patch_113c.exe";
    
    // Create minimal PE files with MPQ
    for (const auto& path : {patch114d, patch113c}) {
        std::ofstream file(path, std::ios::binary);
        char dos_header[128] = {'M', 'Z'};
        dos_header[60] = static_cast<char>(128);
        file.write(dos_header, sizeof(dos_header));
        
        char pe_header[4] = {'P', 'E', 0, 0};
        file.write(pe_header, sizeof(pe_header));
        
        // Add MPQ at offset 512
        file.seekp(512);
        char mpq_header[32] = {'M', 'P', 'Q', 0x1A};
        file.write(mpq_header, sizeof(mpq_header));
    }
    
    d2::PatchSystem patch_system;
    auto detected_patches = patch_system.detectPatches(test_dir);
    
    ASSERT_EQ(detected_patches.size(), 2);
    
    // Find and verify version detection
    for (const auto& patch : detected_patches) {
        if (patch.getFilename() == "LODPatch_114d.exe") {
            EXPECT_EQ(patch.getVersion(), "1.14d");
        } else if (patch.getFilename() == "D2Patch_113c.exe") {
            EXPECT_EQ(patch.getVersion(), "1.13c");
        }
    }
}

TEST_F(PatchSystemTest, FilePrioritySystem) {
    // Create a file priority system
    d2::FilePrioritySystem priority_system;
    
    // Add file sources in order
    priority_system.addSource("base", d2::FileSourcePriority::BASE_GAME);
    priority_system.addSource("expansion", d2::FileSourcePriority::EXPANSION);
    priority_system.addSource("patch", d2::FileSourcePriority::OFFICIAL_PATCH);
    priority_system.addSource("mod", d2::FileSourcePriority::USER_MOD);
    
    // Test file resolution with same file in multiple sources
    priority_system.addFile("base", "data/global/excel/armor.txt");
    priority_system.addFile("expansion", "data/global/excel/armor.txt");
    priority_system.addFile("patch", "data/global/excel/armor.txt");
    
    // Should return the highest priority source (patch)
    auto resolved = priority_system.resolveFile("data/global/excel/armor.txt");
    EXPECT_EQ(resolved.source, "patch");
    EXPECT_EQ(resolved.priority, d2::FileSourcePriority::OFFICIAL_PATCH);
}

TEST_F(PatchSystemTest, ExtractPatchFromExecutable) {
    // Create a simple executable with MPQ at a known offset
    fs::path patch_exe = test_dir / "LODPatch_114d.exe";
    
    // Create file content
    std::vector<uint8_t> file_content(2048, 0);
    
    // PE header
    file_content[0] = 'M';
    file_content[1] = 'Z';
    file_content[60] = 128; // PE offset
    file_content[128] = 'P';
    file_content[129] = 'E';
    
    // MPQ at offset 1024
    size_t mpq_offset = 1024;
    file_content[mpq_offset] = 'M';
    file_content[mpq_offset + 1] = 'P';
    file_content[mpq_offset + 2] = 'Q';
    file_content[mpq_offset + 3] = 0x1A;
    
    // Archive size at offset 8 (512 bytes)
    uint32_t archive_size = 512;
    *reinterpret_cast<uint32_t*>(&file_content[mpq_offset + 8]) = archive_size;
    
    // Write file
    std::ofstream file(patch_exe, std::ios::binary);
    file.write(reinterpret_cast<char*>(file_content.data()), file_content.size());
    file.close();
    
    // Extract the patch
    d2::PatchSystem patch_system;
    fs::path output_path = test_dir / "extracted_patch.mpq";
    
    bool result = patch_system.extractPatchFromExecutable(patch_exe, output_path);
    
    ASSERT_TRUE(result) << "Failed to extract patch from executable";
    EXPECT_TRUE(fs::exists(output_path)) << "Output file does not exist";
    
    // Verify the extracted MPQ
    if (fs::exists(output_path)) {
        // Get file size
        auto file_size = fs::file_size(output_path);
        EXPECT_EQ(file_size, archive_size) << "Extracted file size is wrong";
        
        std::ifstream extracted(output_path, std::ios::binary);
        ASSERT_TRUE(extracted.is_open());
        
        char header[4] = {0};
        extracted.read(header, 4);
        ASSERT_EQ(extracted.gcount(), 4) << "Failed to read 4 bytes";
        
        EXPECT_EQ(header[0], 'M') << "First byte should be 'M'";
        EXPECT_EQ(header[1], 'P') << "Second byte should be 'P'";
        EXPECT_EQ(header[2], 'Q') << "Third byte should be 'Q'";
        EXPECT_EQ(header[3], 0x1A) << "Fourth byte should be 0x1A";
    }
}