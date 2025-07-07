#include <gtest/gtest.h>
#include <StormLib.h>
#include <filesystem>

/**
 * Direct StormLib Integration Test
 * 
 * This test bypasses our wrapper and tests StormLib directly to isolate
 * the root cause of MPQ opening failures.
 */
class StormLibDirectTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use the real MPQ file that exists in our project
        mpq_path = "/Users/jbbrack03/Diablo2Portable/vendor/mpq/d2data.mpq";
        
        // Skip test if file doesn't exist
        if (!std::filesystem::exists(mpq_path)) {
            GTEST_SKIP() << "Test MPQ file not found: " << mpq_path;
        }
    }
    
    std::string mpq_path;
};

// Test 1: Direct StormLib call to SFileOpenArchive
TEST_F(StormLibDirectTest, OpenMPQWithStormLibDirect) {
    HANDLE hMpq = nullptr;
    
    // Try to open MPQ with StormLib directly - this should succeed
    bool result = SFileOpenArchive(mpq_path.c_str(), 0, MPQ_OPEN_READ_ONLY, &hMpq);
    
    if (!result) {
        DWORD error = GetLastError();
        FAIL() << "SFileOpenArchive failed with error code: " << error 
               << " for file: " << mpq_path;
    }
    
    EXPECT_TRUE(result);
    EXPECT_NE(hMpq, nullptr);
    
    // Clean up
    if (hMpq) {
        SFileCloseArchive(hMpq);
    }
}