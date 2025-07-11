#include <iostream>
#include <vector>
#include <cstdint>
#include <bzlib.h>
#include <cstring>

int main() {
    std::cout << "Testing BZip2 decompression\n";
    
    // BZip2 compressed "Hello, World!"
    std::vector<uint8_t> compressed = {
        0x42, 0x5a, 0x68, 0x39,  // BZh9 header
        0x31, 0x41, 0x59, 0x26,  // BZ signature
        0x53, 0x59, 0xe6, 0xd8,
        0xfe, 0xdf, 0x00, 0x00,
        0x01, 0x97, 0x80, 0x60,
        0x04, 0x00, 0x40, 0x00,
        0x80, 0x06, 0x04, 0x90,
        0x00, 0x20, 0x00, 0x22,
        0x03, 0x23, 0x21, 0x00,
        0x30, 0xb2, 0x80, 0x5a,
        0xde, 0x43, 0xef, 0x17,
        0x72, 0x45, 0x38, 0x50,
        0x90, 0xe6, 0xd8, 0xfe,
        0xdf
    };
    
    std::cout << "Compressed size: " << compressed.size() << " bytes\n";
    std::cout << "Expected output: \"Hello, World!\" (13 bytes)\n\n";
    
    // Initialize BZip2 stream
    bz_stream stream;
    memset(&stream, 0, sizeof(stream));
    
    // Set input
    stream.next_in = (char*)compressed.data();
    stream.avail_in = compressed.size();
    
    // Initialize decompression
    int ret = BZ2_bzDecompressInit(&stream, 0, 0);
    std::cout << "BZ2_bzDecompressInit: " << ret << " (BZ_OK=" << BZ_OK << ")\n";
    
    if (ret != BZ_OK) {
        std::cout << "Failed to initialize\n";
        return 1;
    }
    
    // Prepare output buffer
    std::vector<uint8_t> output(100);
    stream.next_out = (char*)output.data();
    stream.avail_out = output.size();
    
    // Decompress
    ret = BZ2_bzDecompress(&stream);
    std::cout << "BZ2_bzDecompress: " << ret << " (BZ_STREAM_END=" << BZ_STREAM_END << ")\n";
    std::cout << "Output bytes: " << stream.total_out_lo32 << "\n";
    
    if (ret == BZ_STREAM_END) {
        output.resize(stream.total_out_lo32);
        std::cout << "Success! Output: \"";
        for (char c : output) std::cout << c;
        std::cout << "\"\n";
    } else {
        std::cout << "Failed with error: " << ret << "\n";
    }
    
    BZ2_bzDecompressEnd(&stream);
    
    return 0;
}