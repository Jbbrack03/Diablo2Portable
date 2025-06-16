#pragma once

#include <vector>
#include <cstdint>

namespace d2portable {
namespace utils {

/**
 * Decompress sparse (run-length encoded) data
 * This implements the sparse compression format used in MPQ archives
 * 
 * @param compressed_data The compressed data to decompress
 * @param output The decompressed output data
 * @param expected_size Expected size of decompressed data
 * @return true if decompression succeeded, false otherwise
 */
bool SparseDecompress(const std::vector<uint8_t>& compressed_data,
                      std::vector<uint8_t>& output,
                      size_t expected_size);

} // namespace utils
} // namespace d2portable