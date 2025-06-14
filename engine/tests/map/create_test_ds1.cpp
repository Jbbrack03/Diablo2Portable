#include <fstream>
#include <vector>
#include <cstdint>

int main() {
    std::vector<uint8_t> data;
    
    // Version (4 bytes)
    data.push_back(0x12); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // Version 18
    
    // Width + 1 (4 bytes)
    data.push_back(0x0A); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 10
    
    // Height + 1 (4 bytes)
    data.push_back(0x0A); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 10
    
    // Act (4 bytes) - 1-based (Act 1 = 1)
    data.push_back(0x01); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // Act 1
    
    // Layer type (4 bytes)
    data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // No shadow
    
    // Number of walls (4 bytes)
    data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 0 walls
    
    // Number of floors (4 bytes)
    data.push_back(0x01); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 1 floor
    
    // Write to file
    std::ofstream file("test_data/minimal.ds1", std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
    
    return 0;
}