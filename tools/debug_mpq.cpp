#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>

struct MPQHeader {
    char signature[4];
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_offset;
    uint32_t block_table_offset;
    uint32_t hash_table_entries;
    uint32_t block_table_entries;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>\n";
        return 1;
    }
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << "\n";
        return 1;
    }
    
    MPQHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    std::cout << "MPQ Header Information:\n";
    std::cout << "======================\n";
    std::cout << "Signature: " << std::string(header.signature, 4) << "\n";
    std::cout << "Header size: " << header.header_size << " bytes\n";
    std::cout << "Archive size: " << header.archive_size << " bytes\n";
    std::cout << "Format version: " << header.format_version << "\n";
    std::cout << "Block size: " << header.block_size << " (2^" << header.block_size << " = " << (1 << header.block_size) << " bytes)\n";
    std::cout << "Hash table offset: 0x" << std::hex << header.hash_table_offset << std::dec << " (" << header.hash_table_offset << ")\n";
    std::cout << "Block table offset: 0x" << std::hex << header.block_table_offset << std::dec << " (" << header.block_table_offset << ")\n";
    std::cout << "Hash table entries: " << header.hash_table_entries << "\n";
    std::cout << "Block table entries: " << header.block_table_entries << "\n";
    
    // Check if tables are within file bounds
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    std::cout << "\nFile size: " << file_size << " bytes\n";
    
    if (header.hash_table_offset + (header.hash_table_entries * 16) > file_size) {
        std::cout << "WARNING: Hash table extends beyond file!\n";
    }
    if (header.block_table_offset + (header.block_table_entries * 16) > file_size) {
        std::cout << "WARNING: Block table extends beyond file!\n";
    }
    
    return 0;
}