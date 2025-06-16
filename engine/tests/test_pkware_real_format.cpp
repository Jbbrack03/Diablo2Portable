#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "utils/mpq_loader.h"

using namespace d2portable::utils;

class PKWAREFormatAnalysisTest : public ::testing::Test {
protected:
    void AnalyzePKWAREData(const std::vector<uint8_t>& compressed_data) {
        std::cout << "\n=== PKWARE Data Analysis ===" << std::endl;
        std::cout << "Size: " << compressed_data.size() << " bytes" << std::endl;
        
        if (compressed_data.size() < 4) {
            std::cout << "ERROR: Data too small for PKWARE header" << std::endl;
            return;
        }
        
        // Check for compression type byte (should be 0x08 for PKWARE)
        std::cout << "First 16 bytes (hex):" << std::endl;
        for (size_t i = 0; i < std::min(size_t(16), compressed_data.size()); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << static_cast<int>(compressed_data[i]) << " ";
            if (i % 8 == 7) std::cout << " ";
        }
        std::cout << std::dec << std::endl;
        
        // Check for literal/length indicator (first byte after compression type)
        if (compressed_data.size() > 0) {
            uint8_t first_byte = compressed_data[0];
            std::cout << "\nFirst byte analysis:" << std::endl;
            std::cout << "  Value: 0x" << std::hex << static_cast<int>(first_byte) << std::dec << std::endl;
            std::cout << "  Binary: ";
            for (int i = 7; i >= 0; --i) {
                std::cout << ((first_byte >> i) & 1);
            }
            std::cout << std::endl;
            
            // In PKWARE DCL, first byte typically indicates:
            // Bit 0: 0=literal, 1=length
            // Bits 1-2: Dictionary size (0=1024, 1=2048, 2=4096)
            bool is_literal = (first_byte & 0x01) == 0;
            int dict_size_bits = (first_byte >> 1) & 0x03;
            int dict_size = 1024 << dict_size_bits;
            
            std::cout << "  Literal/Length: " << (is_literal ? "LITERAL" : "LENGTH") << std::endl;
            std::cout << "  Dictionary size: " << dict_size << " bytes" << std::endl;
        }
        
        // Check for common PKWARE patterns
        std::cout << "\nChecking for PKWARE patterns:" << std::endl;
        
        // Count consecutive zero bytes (might indicate padding or header)
        int zero_count = 0;
        for (size_t i = 0; i < compressed_data.size() && compressed_data[i] == 0; ++i) {
            zero_count++;
        }
        if (zero_count > 0) {
            std::cout << "  Leading zeros: " << zero_count << std::endl;
        }
        
        // Look for potential header bytes
        if (compressed_data.size() >= 2) {
            uint16_t potential_size = (compressed_data[1] << 8) | compressed_data[0];
            std::cout << "  Potential size (LE): " << potential_size << std::endl;
            potential_size = (compressed_data[0] << 8) | compressed_data[1];
            std::cout << "  Potential size (BE): " << potential_size << std::endl;
        }
    }
};

TEST_F(PKWAREFormatAnalysisTest, AnalyzeRealPKWAREFile) {
    const char* mpq_path = std::getenv("TEST_MPQ_PATH");
    if (!mpq_path) {
        GTEST_SKIP() << "Set TEST_MPQ_PATH to test with real MPQ file";
    }
    
    MPQLoader loader;
    ASSERT_TRUE(loader.open(mpq_path));
    
    // Try to extract a known PKWARE compressed file
    const char* test_file = "data\\global\\palette\\act1\\pal.dat";
    
    // First, get the raw compressed data without decompression
    std::vector<uint8_t> compressed_data;
    
    // We need to access the raw block data - let's check if file exists first
    auto file_list = loader.listFiles();
    bool found = false;
    for (const auto& file : file_list) {
        if (file.filename == test_file) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::cout << "File not found: " << test_file << std::endl;
        std::cout << "Available files containing 'pal.dat':" << std::endl;
        for (const auto& file : file_list) {
            if (file.filename.find("pal.dat") != std::string::npos) {
                std::cout << "  " << file.filename << std::endl;
            }
        }
        FAIL() << "Test file not found";
    }
    
    // Try to extract normally first to see the error
    std::vector<uint8_t> output;
    bool success = loader.extractFile(test_file, output);
    
    if (!success) {
        std::cout << "Normal extraction failed as expected" << std::endl;
        
        // Now we need to get the raw compressed data
        // This would require modifying MPQLoader to expose raw block data
        // For now, let's analyze what we can
        std::cout << "\nFile info:" << std::endl;
        std::cout << "  Path: " << test_file << std::endl;
        std::cout << "  Expected: Palette data (typically 256 * 3 = 768 bytes uncompressed)" << std::endl;
        
        SUCCEED() << "Analysis complete - need MPQLoader modification to access raw data";
    } else {
        std::cout << "Extraction succeeded! Size: " << output.size() << " bytes" << std::endl;
        AnalyzePKWAREData(output);
    }
}

// Test with multiple known PKWARE files
TEST_F(PKWAREFormatAnalysisTest, AnalyzeMultiplePKWAREFiles) {
    const char* mpq_path = std::getenv("TEST_MPQ_PATH");
    if (!mpq_path) {
        GTEST_SKIP() << "Set TEST_MPQ_PATH to test with real MPQ file";
    }
    
    MPQLoader loader;
    ASSERT_TRUE(loader.open(mpq_path));
    
    // Test various file types known to use PKWARE
    const char* test_files[] = {
        "data\\global\\palette\\act1\\pal.dat",
        "data\\global\\palette\\act2\\pal.dat",
        "data\\global\\palette\\act3\\pal.dat",
        "data\\global\\palette\\loading\\pal.dat",
        "data\\global\\palette\\units\\pal.dat"
    };
    
    for (const auto& file : test_files) {
        std::cout << "\n=== Testing: " << file << " ===" << std::endl;
        
        std::vector<uint8_t> output;
        bool success = loader.extractFile(file, output);
        
        if (success) {
            std::cout << "Success! Size: " << output.size() << std::endl;
            if (output.size() == 768) {
                std::cout << "Correct palette size (256 colors * 3 bytes)" << std::endl;
            }
        } else {
            std::cout << "Failed - needs PKWARE fix" << std::endl;
        }
    }
}