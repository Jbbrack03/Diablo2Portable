#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "utils/stormlib_mpq_loader.h"

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

// AnalyzeRealPKWAREFile test removed - was optional test requiring TEST_MPQ_PATH environment variable

// AnalyzeMultiplePKWAREFiles test removed - was optional test requiring TEST_MPQ_PATH environment variable