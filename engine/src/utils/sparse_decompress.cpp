#include "utils/sparse_decompress.h"
#include <iostream>

namespace d2portable {
namespace utils {

/**
 * Decompress sparse (run-length encoded) data
 * 
 * Sparse compression format (based on MPQ documentation):
 * - Data is stored as runs of bytes
 * - Each run consists of:
 *   - Length byte: how many times to repeat the following byte
 *   - Value byte: the byte value to repeat
 * - Special cases:
 *   - Length 0: Copy next byte literally (no repetition)
 *   - Length 255: Extended length format
 */
bool SparseDecompress(const std::vector<uint8_t>& compressed_data,
                      std::vector<uint8_t>& output,
                      size_t expected_size) {
    
    if (compressed_data.empty()) {
        return false;
    }
    
    output.clear();
    output.reserve(expected_size);
    
    size_t input_pos = 0;
    
    while (input_pos < compressed_data.size() && output.size() < expected_size) {
        
        // Get run length
        uint8_t run_length = compressed_data[input_pos++];
        
        if (input_pos >= compressed_data.size()) {
            break; // Unexpected end of data
        }
        
        if (run_length == 0) {
            // Copy next byte literally
            output.push_back(compressed_data[input_pos++]);
            
        } else if (run_length == 255) {
            // Extended length format (if needed)
            // For now, treat as regular run of 255
            if (input_pos >= compressed_data.size()) {
                break;
            }
            
            uint8_t value = compressed_data[input_pos++];
            for (int i = 0; i < 255 && output.size() < expected_size; i++) {
                output.push_back(value);
            }
            
        } else {
            // Regular run: repeat the next byte run_length times
            uint8_t value = compressed_data[input_pos++];
            for (int i = 0; i < run_length && output.size() < expected_size; i++) {
                output.push_back(value);
            }
        }
    }
    
    // Verify we got the expected size
    return output.size() == expected_size;
}

} // namespace utils
} // namespace d2portable