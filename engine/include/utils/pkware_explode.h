#ifndef D2PORTABLE_UTILS_PKWARE_EXPLODE_H
#define D2PORTABLE_UTILS_PKWARE_EXPLODE_H

#include <vector>
#include <cstdint>

namespace d2portable {
namespace utils {

/**
 * Decompress data using PKWARE Data Compression Library (DCL) algorithm
 * This is the "explode" algorithm used in MPQ files
 * 
 * @param compressed_data Input compressed data
 * @param output Output buffer for decompressed data
 * @param expected_size Expected size of decompressed data
 * @return true if decompression successful, false otherwise
 */
bool PKWAREExplode(const std::vector<uint8_t>& compressed_data,
                   std::vector<uint8_t>& output,
                   size_t expected_size);

} // namespace utils
} // namespace d2portable

#endif // D2PORTABLE_UTILS_PKWARE_EXPLODE_H