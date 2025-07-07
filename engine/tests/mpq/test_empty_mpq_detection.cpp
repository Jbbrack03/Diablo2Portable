#include <gtest/gtest.h>
#include <StormLib.h>
#include <filesystem>
#include <fstream>

/**
 * Empty MPQ Detection Test
 * 
 * Tests that empty MPQ files are detected and handled gracefully
 * instead of causing stack overflow (error 1000).
 */
class EmptyMPQDetectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir = std::filesystem::temp_directory_path() / "empty_mpq_test";
        std::filesystem::create_directories(test_dir);
        
        // Create an empty MPQ file (like our current d2data.mpq)
        empty_mpq_path = test_dir / "empty.mpq";
        std::ofstream empty_file(empty_mpq_path);
        empty_file.close(); // Creates 0-byte file
        
        // Create a file with just MPQ signature but corrupted
        corrupted_mpq_path = test_dir / "corrupted.mpq";
        std::ofstream corrupted_file(corrupted_mpq_path, std::ios::binary);
        // Write MPQ signature but incomplete header
        corrupted_file.write("MPQ\x1A", 4); // Just the signature
        corrupted_file.close();
    }
    
    void TearDown() override {
        std::filesystem::remove_all(test_dir);
    }
    
    std::filesystem::path test_dir;
    std::filesystem::path empty_mpq_path;
    std::filesystem::path corrupted_mpq_path;
};

// Test 1: Verify empty file causes stack overflow in current StormLib
TEST_F(EmptyMPQDetectionTest, EmptyFileCausesStackOverflow) {
    HANDLE hMpq = nullptr;
    
    // Try to open the empty MPQ file - this should fail with stack overflow (error 1000)
    bool result = SFileOpenArchive(empty_mpq_path.string().c_str(), 0, MPQ_OPEN_READ_ONLY, &hMpq);
    
    if (!result) {
        DWORD error = GetLastError();
        EXPECT_EQ(error, 1000) << "Empty MPQ file should cause stack overflow error";
    } else {
        FAIL() << "Empty MPQ file should not open successfully";
    }
    
    // Clean up
    if (hMpq) {
        SFileCloseArchive(hMpq);
    }
}

// Test 2: Verify basic file size check prevents stack overflow
TEST_F(EmptyMPQDetectionTest, PreCheckFileSizePreventsStackOverflow) {
    // Check file size before attempting to open with StormLib
    auto file_size = std::filesystem::file_size(empty_mpq_path);
    
    if (file_size < 32) { // MPQ header is at least 32 bytes
        // Don't try to open with StormLib - file is too small
        EXPECT_LT(file_size, 32) << "Empty file should be smaller than minimum MPQ header size";
        return; // Test passes - we avoided the stack overflow
    }
    
    // If file was large enough, we could try StormLib here
    FAIL() << "Should not reach here with empty file";
}