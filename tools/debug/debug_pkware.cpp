#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include "../engine/include/utils/pkware_explode.h"

using namespace d2;

void printHex(const std::vector<uint8_t>& data, const std::string& label) {
    std::cout << label << " (" << data.size() << " bytes): ";
    for (size_t i = 0; i < data.size() && i < 32; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)data[i] << " ";
    }
    if (data.size() > 32) std::cout << "...";
    std::cout << std::dec << "\n";
}

int main() {
    std::cout << "PKWARE DCL Debugging\n";
    std::cout << "====================\n\n";
    
    // Test 1: The documented example from blast.c
    std::cout << "Test 1: Documented example\n";
    std::vector<uint8_t> example = {0x00, 0x04, 0x82, 0x24, 0x25, 0x8f, 0x80, 0x7f};
    printHex(example, "Input");
    
    std::vector<uint8_t> output;
    bool result = PKWAREExplode(example, output, 13);
    
    std::cout << "Result: " << (result ? "SUCCESS" : "FAILED") << "\n";
    if (result) {
        printHex(output, "Output");
        std::string str(output.begin(), output.end());
        std::cout << "As string: \"" << str << "\"\n";
        std::cout << "Expected: \"AIAIAIAIAIAIA\"\n";
    }
    
    // Test 2: Simple uncompressed data
    std::cout << "\nTest 2: Uncompressed literals\n";
    std::vector<uint8_t> uncompressed = {
        0x01,  // Uncoded literals
        0x04,  // Dictionary size
        // For uncoded literals, we need the bit stream
        // Flag bit 0 = literal follows
        0x00,  // First byte has bit 0 = 0 (literal)
        'H',   // Literal byte
        0x00,  // Next flag bit
        'i'    // Literal byte
    };
    printHex(uncompressed, "Input");
    
    output.clear();
    result = PKWAREExplode(uncompressed, output, 2);
    
    std::cout << "Result: " << (result ? "SUCCESS" : "FAILED") << "\n";
    if (result) {
        printHex(output, "Output");
        std::string str(output.begin(), output.end());
        std::cout << "As string: \"" << str << "\"\n";
    }
    
    // Test 3: Check what our implementation does with the test format
    std::cout << "\nTest 3: Test format (0xFF control bytes)\n";
    std::vector<uint8_t> testformat = {
        0xFF,  // Control byte (all literals)
        'T', 'e', 's', 't', ' ', 'd', 'a', 't',
        0xFF,  // Next control byte
        'a'
    };
    printHex(testformat, "Input");
    
    output.clear();
    result = PKWAREExplode(testformat, output, 9);
    
    std::cout << "Result: " << (result ? "SUCCESS" : "FAILED") << "\n";
    if (result) {
        printHex(output, "Output");
        std::string str(output.begin(), output.end());
        std::cout << "As string: \"" << str << "\"\n";
    }
    
    return 0;
}