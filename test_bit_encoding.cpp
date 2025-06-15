#include <iostream>
#include <bitset>

int main() {
    // ABCAB with codes: A=0, B=1, C=10
    // Sequence: 0 1 10 0 1
    
    // Write bits LSB first into byte
    uint8_t byte = 0;
    int bit_pos = 0;
    
    // A = 0
    byte |= (0 << bit_pos++);
    
    // B = 1
    byte |= (1 << bit_pos++);
    
    // C = 10 (binary: 1 0, but read LSB first so 0 then 1)
    byte |= (0 << bit_pos++);
    byte |= (1 << bit_pos++);
    
    // A = 0
    byte |= (0 << bit_pos++);
    
    // B = 1
    byte |= (1 << bit_pos++);
    
    std::cout << "Encoded byte: 0x" << std::hex << (int)byte << std::endl;
    std::cout << "Binary: " << std::bitset<8>(byte) << std::endl;
    std::cout << "Decimal: " << std::dec << (int)byte << std::endl;
    
    return 0;
}