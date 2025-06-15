#include <iostream>
#include <vector>
#include <cstdint>

// Forward declaration
bool huffman_decode_literals(const std::vector<uint8_t>& compressed_data,
                           std::vector<uint8_t>& output,
                           size_t num_symbols);

int main() {
    // Test decoding of simple Huffman-encoded literals
    std::vector<uint8_t> compressed = {
        // Huffman tree structure (simplified for testing)
        0x02,  // Number of symbols at depth 1
        0x41, 0x42,  // Symbols 'A' and 'B' at depth 1
        0x01,  // Number of symbols at depth 2
        0x43,  // Symbol 'C' at depth 2
        // Encoded data: "ABCAB"
        0b10110010  // A(0) B(1) C(10) A(0) B(1) = 01100 1
    };
    
    std::cout << "Compressed data size: " << compressed.size() << std::endl;
    std::cout << "Data: ";
    for (auto b : compressed) {
        std::cout << std::hex << (int)b << " ";
    }
    std::cout << std::dec << std::endl;
    
    std::vector<uint8_t> output;
    bool result = huffman_decode_literals(compressed, output, 256);
    
    std::cout << "Result: " << (result ? "true" : "false") << std::endl;
    std::cout << "Output size: " << output.size() << std::endl;
    std::cout << "Output: ";
    for (auto b : output) {
        std::cout << (char)b;
    }
    std::cout << std::endl;
    
    return 0;
}