#include <gtest/gtest.h>
#include <StormLib.h>
#include <filesystem>
#include <fstream>

/**
 * StormLib Stack Overflow Fix Test
 * 
 * Tests that MPQ opening doesn't cause stack overflow issues.
 * Error 1000 indicates stack overflow, so we test different approaches.
 */
class StormLibStackFixTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Get the path relative to the test executable
        std::filesystem::path exe_dir = std::filesystem::path(__FILE__).parent_path();
        std::filesystem::path project_root = exe_dir.parent_path().parent_path().parent_path();
        mpq_path = (project_root / "vendor" / "mpq" / "d2data.mpq").string();
        
        // Skip test if file doesn't exist
        if (!std::filesystem::exists(mpq_path)) {
            GTEST_SKIP() << "Test MPQ file not found: " << mpq_path;
        }
        
        // Skip test if file is empty
        auto file_size = std::filesystem::file_size(mpq_path);
        if (file_size == 0) {
            GTEST_SKIP() << "MPQ file is empty (0 bytes). Please copy valid Diablo II MPQ files to vendor/mpq/";
        }
        
        // Check if file has valid MPQ header
        std::ifstream file(mpq_path, std::ios::binary);
        if (file.is_open()) {
            uint32_t signature;
            file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
            file.close();
            
            // MPQ files start with 'MPQ\x1a' (0x1A51504D in little-endian)
            if (signature != 0x1A51504D) {
                GTEST_SKIP() << "File is not a valid MPQ (invalid header). Please copy valid Diablo II MPQ files to vendor/mpq/";
            }
        }
    }
    
    std::string mpq_path;
};

// Test 1: Try opening MPQ with different flags to avoid stack overflow
TEST_F(StormLibStackFixTest, OpenMPQWithSpecialFlags) {
    HANDLE hMpq = nullptr;
    
    // Try opening with priority 0 and specific flags that might avoid stack issues
    // Use MPQ_OPEN_NO_LISTFILE | MPQ_OPEN_NO_ATTRIBUTES to avoid recursive parsing
    bool result = SFileOpenArchive(mpq_path.c_str(), 0, 
                                   MPQ_OPEN_READ_ONLY | MPQ_OPEN_NO_LISTFILE | MPQ_OPEN_NO_ATTRIBUTES, 
                                   &hMpq);
    
    if (!result) {
        DWORD error = GetLastError();
        if (error == 1000) {
            FAIL() << "Stack overflow still occurs with special flags. Error: " << error;
        } else {
            // Different error - this means we've made progress fixing the stack issue
            std::cout << "Different error (not stack overflow): " << error << std::endl;
            // This test should pass if we get any error other than 1000
        }
    } else {
        EXPECT_TRUE(result);
        EXPECT_NE(hMpq, nullptr);
    }
    
    // Clean up
    if (hMpq) {
        SFileCloseArchive(hMpq);
    }
}