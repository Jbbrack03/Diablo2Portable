#include <iostream>
#include <bitset>

int main() {
    // A=0 (1 bit), B=10 (2 bits), C=11 (2 bits)
    // ABCAB = 0 10 11 0 10
    
    uint8_t byte = 0;
    int pos = 0;
    
    // A = 0 (1 bit)
    byte |= (0 << pos++);
    
    // B = 10 (read LSB first: 0 then 1)
    byte |= (0 << pos++);
    byte |= (1 << pos++);
    
    // C = 11 (read LSB first: 1 then 1)
    byte |= (1 << pos++);
    byte |= (1 << pos++);
    
    // A = 0 (1 bit)
    byte |= (0 << pos++);
    
    // B = 10 (read LSB first: 0 then 1)
    byte |= (0 << pos++);
    byte |= (1 << pos++);
    
    std::cout << "Result: 0x" << std::hex << (int)byte << std::endl;
    std::cout << "Binary: " << std::bitset<8>(byte) << std::endl;
    
    return 0;
}