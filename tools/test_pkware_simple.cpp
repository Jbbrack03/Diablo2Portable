#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

// Simple bit reader
struct BitReader {
    const uint8_t* data;
    const uint8_t* end;
    uint32_t bitbuf;
    int bitcnt;
    
    BitReader(const uint8_t* d, size_t len) 
        : data(d), end(d + len), bitbuf(0), bitcnt(0) {}
    
    int getBit() {
        if (bitcnt == 0) {
            if (data >= end) return -1;
            bitbuf = *data++;
            bitcnt = 8;
        }
        int bit = bitbuf & 1;
        bitbuf >>= 1;
        bitcnt--;
        return bit;
    }
    
    int getBits(int n) {
        int val = 0;
        for (int i = 0; i < n; i++) {
            int bit = getBit();
            if (bit < 0) return -1;
            val |= bit << i;
        }
        return val;
    }
};

int main() {
    // Test the documented example: 00 04 82 24 25 8f 80 7f
    std::vector<uint8_t> data = {0x00, 0x04, 0x82, 0x24, 0x25, 0x8f, 0x80, 0x7f};
    
    std::cout << "Testing PKWARE DCL bit stream\n";
    std::cout << "Input: ";
    for (auto b : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    std::cout << "\n\n";
    
    // First two bytes are header
    int lit_type = data[0];  // 0 = coded literals
    int dict_bits = data[1]; // 4 = 10 bits total (4 + 6)
    
    std::cout << "Literal type: " << lit_type << " (0=coded, 1=uncoded)\n";
    std::cout << "Dictionary bits: " << dict_bits << " (total: " << (dict_bits + 6) << ")\n\n";
    
    // Create bit reader for the compressed data
    BitReader reader(data.data() + 2, data.size() - 2);
    
    // Try to read first few bits
    std::cout << "Reading bits from stream:\n";
    for (int i = 0; i < 32 && reader.data < reader.end; i++) {
        int bit = reader.getBit();
        if (bit < 0) break;
        std::cout << bit;
        if ((i + 1) % 8 == 0) std::cout << " ";
    }
    std::cout << "\n\n";
    
    // Reset reader
    BitReader reader2(data.data() + 2, data.size() - 2);
    
    // Try to decode as PKWARE
    std::cout << "Attempting decode:\n";
    std::vector<char> output;
    
    while (output.size() < 20) {  // Limit output
        int flag = reader2.getBit();
        if (flag < 0) break;
        
        std::cout << "Flag bit: " << flag << " ";
        
        if (flag == 0) {
            // Literal - but it's Huffman coded!
            // For now, let's just try to read 8 bits
            int byte = reader2.getBits(8);
            if (byte < 0) break;
            std::cout << "-> Literal byte: 0x" << std::hex << byte << " '" << (char)byte << "'\n";
            output.push_back(byte);
        } else {
            // Match - length/distance pair
            std::cout << "-> Match (not implemented)\n";
            break;
        }
    }
    
    std::cout << "\nOutput: \"";
    for (char c : output) std::cout << c;
    std::cout << "\"\n";
    
    return 0;
}