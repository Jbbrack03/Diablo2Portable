#ifndef D2PORTABLE_UTILS_BZIP2_DECOMPRESS_H
#define D2PORTABLE_UTILS_BZIP2_DECOMPRESS_H

#include <vector>
#include <cstdint>

namespace d2portable {
namespace utils {

/**
 * Decompress data using BZip2 algorithm
 * 
 * @param compressed_data Input compressed data
 * @param output Output buffer for decompressed data
 * @param expected_size Expected size of decompressed data (hint)
 * @return true if decompression successful, false otherwise
 */
bool bzip2_decompress(const std::vector<uint8_t>& compressed_data,
                     std::vector<uint8_t>& output,
                     size_t expected_size);

} // namespace utils
} // namespace d2portable

#endif // D2PORTABLE_UTILS_BZIP2_DECOMPRESS_H