#include "utils/bzip2_decompress.h"
#include <bzlib.h>
#include <cstring>
#include <iostream>

namespace d2portable {
namespace utils {

bool bzip2_decompress(const std::vector<uint8_t>& compressed_data,
                     std::vector<uint8_t>& output,
                     size_t expected_size) {
    if (compressed_data.empty()) {
        return false;
    }
    
    // Note: In MPQ files, BZip2 compressed data might not have the standard 
    // "BZh" header. Some MPQ implementations strip this header.
    // We'll try to decompress anyway and let BZ2_bzDecompressInit decide.
    
    // Optional: Check for BZip2 header "BZh" but don't require it
    bool has_header = (compressed_data.size() >= 4 && 
                      compressed_data[0] == 0x42 &&  // 'B'
                      compressed_data[1] == 0x5a &&  // 'Z'  
                      compressed_data[2] == 0x68);   // 'h'
    
    // Initialize BZip2 stream
    bz_stream stream;
    memset(&stream, 0, sizeof(stream));
    
    // Set input
    stream.next_in = (char*)compressed_data.data();
    stream.avail_in = compressed_data.size();
    
    // Initialize decompression
    int ret = BZ2_bzDecompressInit(&stream, 0, 0);
    if (ret != BZ_OK) {
        std::cerr << "BZ2_bzDecompressInit failed with code: " << ret << std::endl;
        if (!has_header && compressed_data.size() >= 4) {
            std::cerr << "First 4 bytes: " << std::hex 
                      << (int)compressed_data[0] << " " 
                      << (int)compressed_data[1] << " "
                      << (int)compressed_data[2] << " "
                      << (int)compressed_data[3] << std::dec << std::endl;
        }
        return false;
    }
    
    // Prepare output buffer
    output.clear();
    output.resize(expected_size > 0 ? expected_size : compressed_data.size() * 4);
    
    stream.next_out = (char*)output.data();
    stream.avail_out = output.size();
    
    // Decompress
    ret = BZ_OK;  // Initialize for loop
    while (ret == BZ_OK) {
        ret = BZ2_bzDecompress(&stream);
        
        if (ret == BZ_STREAM_END) {
            // Decompression complete
            output.resize(stream.total_out_lo32);
            BZ2_bzDecompressEnd(&stream);
            return true;
        }
        
        if (ret != BZ_OK && ret != BZ_STREAM_END) {
            // Error occurred
            BZ2_bzDecompressEnd(&stream);
            return false;
        }
        
        // Need more output space
        if (stream.avail_out == 0) {
            size_t old_size = output.size();
            output.resize(old_size * 2);
            stream.next_out = (char*)output.data() + old_size;
            stream.avail_out = output.size() - old_size;
        }
        
        // Check if we've consumed all input without reaching end
        if (stream.avail_in == 0 && ret == BZ_OK) {
            // Truncated data - not enough input
            BZ2_bzDecompressEnd(&stream);
            return false;
        }
    }
    
    BZ2_bzDecompressEnd(&stream);
    return false;
}

} // namespace utils
} // namespace d2portable