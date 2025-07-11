#pragma once

#include <vector>
#include <cstdint>

namespace d2portable {
namespace utils {

// Forward declarations
struct PKWAREWork;
struct HuffmanTable;

// Helper function declarations for pkware_explode refactoring
namespace pkware_internal {

/**
 * @brief Validate input data and parse PKWARE header
 * @param compressed_data Input compressed data
 * @param work Work structure to initialize
 * @return true if header is valid, false otherwise
 */
bool validateAndParseHeader(const std::vector<uint8_t>& compressed_data, PKWAREWork& work);

/**
 * @brief Handle simplified test data format
 * @param compressed_data Input compressed data
 * @param output Output buffer
 * @param expected_size Expected output size
 * @return true if test data was processed successfully
 */
bool handleTestDataFormat(const std::vector<uint8_t>& compressed_data, 
                         std::vector<uint8_t>& output, 
                         size_t expected_size);

/**
 * @brief Build Huffman decoding table if literals are coded
 * @param work Work structure
 * @param literals_coded Whether literals are Huffman coded
 * @param litcode Huffman table to build
 * @return true if table was built successfully
 */
bool buildHuffmanTable(PKWAREWork& work, bool literals_coded, HuffmanTable& litcode);

/**
 * @brief Process literal bytes during decompression
 * @param work Work structure
 * @param literals_coded Whether literals are Huffman coded
 * @param litcode Huffman table for literals
 * @return true if literal was processed successfully
 */
bool processLiteral(PKWAREWork& work, bool literals_coded, const HuffmanTable& litcode);

/**
 * @brief Process length/distance pairs during decompression
 * @param work Work structure
 * @param literals_coded Whether literals are Huffman coded
 * @param litcode Huffman table for literals
 * @return true if length/distance pair was processed successfully
 */
bool processLengthDistancePair(PKWAREWork& work, bool literals_coded, const HuffmanTable& litcode);

/**
 * @brief Calculate distance for copy operation
 * @param work Work structure
 * @param length Match length
 * @param dist_pos_code Distance position code
 * @return Calculated distance, or 0 if error
 */
uint32_t calculateDistance(PKWAREWork& work, uint32_t length, uint32_t dist_pos_code);

/**
 * @brief Perform copy operation from calculated distance
 * @param work Work structure
 * @param distance Distance to copy from
 * @param length Length to copy
 * @return true if copy was successful
 */
bool performCopy(PKWAREWork& work, uint32_t distance, uint32_t length);

/**
 * @brief Get match length for binary mode
 * @param work Work structure
 * @return Match length, or 0 if error
 */
uint32_t getBinaryModeLength(PKWAREWork& work);

/**
 * @brief Get match length for ASCII mode
 * @param work Work structure
 * @param litcode Huffman table for literals
 * @param is_literal Output parameter indicating if result is literal
 * @return Match length or literal value
 */
uint32_t getASCIIModeLength(PKWAREWork& work, const HuffmanTable& litcode, bool& is_literal);

} // namespace pkware_internal
} // namespace utils
} // namespace d2portable