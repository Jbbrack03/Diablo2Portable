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