#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

// Test to validate MPQ files before running integration tests
class MPQFileValidationTest : public ::testing::Test {
protected:
    bool isValidMPQFile(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            return false;
        }
        
        // Check file size - empty files are not valid MPQs
        auto file_size = std::filesystem::file_size(path);
        if (file_size == 0) {
            return false;
        }
        
        // Check MPQ header signature
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // MPQ files should start with 'MPQ\x1A' (0x1A51504D in little endian)
        uint32_t signature;
        file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
        
        return signature == 0x1A51504D;
    }
};

// Test that checks if vendor MPQ files are valid
TEST_F(MPQFileValidationTest, CheckVendorMPQFiles) {
    std::string vendor_mpq_dir = "/Users/jbbrack03/Diablo2Portable/vendor/mpq";
    
    // List of required MPQ files
    std::vector<std::string> required_mpqs = {
        "d2data.mpq",
        "d2exp.mpq",
        "d2sfx.mpq"
    };
    
    bool all_valid = true;
    for (const auto& mpq_name : required_mpqs) {
        std::string mpq_path = vendor_mpq_dir + "/" + mpq_name;
        bool is_valid = isValidMPQFile(mpq_path);
        
        if (!is_valid) {
            std::cout << "Invalid or empty MPQ file: " << mpq_path;
            auto file_size = std::filesystem::exists(mpq_path) ? 
                std::filesystem::file_size(mpq_path) : 0;
            std::cout << " (size: " << file_size << " bytes)" << std::endl;
            all_valid = false;
        }
    }
    
    if (!all_valid) {
        GTEST_SKIP() << "Valid Diablo II MPQ files not found. "
                     << "Please copy your legally owned MPQ files to vendor/mpq/";
    }
    
    SUCCEED() << "All required MPQ files are valid";
}