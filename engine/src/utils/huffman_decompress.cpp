/*****************************************************************************/
/* huffman_decompress.cpp                                                    */
/*                                                                           */
/* Huffman decompression for MPQ files                                      */
/* Based on StormLib implementation                                          */
/*****************************************************************************/

#include <cstring>
#include <vector>
#include <cstdint>

namespace d2portable {
namespace utils {

// Huffman tree node
struct HuffmanNode {
    HuffmanNode* child0;    // Child for bit 0
    HuffmanNode* child1;    // Child for bit 1
    uint32_t value;         // Value if this is a leaf node
    bool is_leaf;           // True if this is a leaf node
    
    HuffmanNode() : child0(nullptr), child1(nullptr), value(0), is_leaf(false) {}
};

// Huffman decompression context
struct HuffmanContext {
    const uint8_t* in_buff;     // Input buffer
    const uint8_t* in_pos;      // Current input position
    const uint8_t* in_end;      // End of input buffer
    
    uint8_t* out_buff;          // Output buffer
    uint8_t* out_pos;           // Current output position
    uint8_t* out_end;           // End of output buffer
    
    uint32_t bit_buff;          // Bit buffer
    uint32_t bits_avail;        // Number of bits available in bit buffer
    
    HuffmanNode* root;          // Root of Huffman tree
    std::vector<HuffmanNode> nodes; // Node storage
};

// Read bits from input stream
static uint32_t ReadBits(HuffmanContext* ctx, uint32_t num_bits) {
    uint32_t result = 0;
    
    while (num_bits > 0) {
        // Refill bit buffer if needed
        if (ctx->bits_avail == 0) {
            if (ctx->in_pos >= ctx->in_end) {
                return 0xFFFFFFFF; // No more input
            }
            ctx->bit_buff = *ctx->in_pos++;
            ctx->bits_avail = 8;
        }
        
        // Extract one bit
        uint32_t bit = ctx->bit_buff & 1;
        ctx->bit_buff >>= 1;
        ctx->bits_avail--;
        
        result = (result << 1) | bit;
        num_bits--;
    }
    
    return result;
}

// Build a simple Huffman tree for MPQ decompression
// This is a simplified version - real MPQ Huffman uses predefined weight tables
static bool BuildHuffmanTree(HuffmanContext* ctx) {
    // For MPQ Huffman, we need to read the tree structure from the compressed data
    // This is a simplified implementation that handles basic cases
    
    // Reserve space for nodes (worst case: 512 nodes for 256 symbols)
    ctx->nodes.reserve(512);
    
    // Create root node
    ctx->nodes.emplace_back();
    ctx->root = &ctx->nodes[0];
    
    // In a real implementation, we would:
    // 1. Read the compression type indicator
    // 2. Build the tree based on predefined weight tables
    // 3. Handle different compression types (0-8)
    
    // For now, create a simple fixed tree for testing
    // This handles uncompressed data passed through Huffman wrapper
    for (int i = 0; i < 256; i++) {
        HuffmanNode* current = ctx->root;
        
        // Use the byte value as a path in the tree (8 bits)
        for (int bit = 7; bit >= 0; bit--) {
            if (i & (1 << bit)) {
                if (!current->child1) {
                    ctx->nodes.emplace_back();
                    current->child1 = &ctx->nodes.back();
                }
                current = current->child1;
            } else {
                if (!current->child0) {
                    ctx->nodes.emplace_back();
                    current->child0 = &ctx->nodes.back();
                }
                current = current->child0;
            }
        }
        
        // Make it a leaf node
        current->is_leaf = true;
        current->value = i;
    }
    
    return true;
}

// Decode one byte using Huffman tree
static uint32_t DecodeSymbol(HuffmanContext* ctx) {
    HuffmanNode* current = ctx->root;
    
    // Traverse tree based on input bits
    while (!current->is_leaf) {
        uint32_t bit = ReadBits(ctx, 1);
        if (bit == 0xFFFFFFFF) {
            return 0xFFFFFFFF; // No more input
        }
        
        if (bit) {
            current = current->child1;
        } else {
            current = current->child0;
        }
        
        if (!current) {
            return 0xFFFFFFFF; // Invalid tree
        }
    }
    
    return current->value;
}

// Main Huffman decompression function
bool HuffmanDecompress(const std::vector<uint8_t>& compressed_data,
                      std::vector<uint8_t>& output,
                      size_t expected_size) {
    if (compressed_data.empty()) {
        return false;
    }
    
    HuffmanContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    
    // Initialize context
    ctx.in_buff = compressed_data.data();
    ctx.in_pos = ctx.in_buff;
    ctx.in_end = ctx.in_buff + compressed_data.size();
    
    // Allocate output buffer
    output.resize(expected_size);
    ctx.out_buff = output.data();
    ctx.out_pos = ctx.out_buff;
    ctx.out_end = ctx.out_buff + expected_size;
    
    // Build Huffman tree
    if (!BuildHuffmanTree(&ctx)) {
        return false;
    }
    
    // Check for compression type byte (first byte in MPQ Huffman)
    if (ctx.in_pos < ctx.in_end) {
        uint8_t comp_type = *ctx.in_pos++;
        
        // For now, handle only simple cases
        // Type 0-8 are different weight tables in real implementation
        if (comp_type > 8) {
            // Unknown compression type, try simple copy
            if (compressed_data.size() - 1 == expected_size) {
                memcpy(output.data(), compressed_data.data() + 1, expected_size);
                return true;
            }
            return false;
        }
    }
    
    // Decompress data
    while (ctx.out_pos < ctx.out_end) {
        uint32_t symbol = DecodeSymbol(&ctx);
        if (symbol == 0xFFFFFFFF) {
            break; // End of input or error
        }
        
        *ctx.out_pos++ = (uint8_t)symbol;
    }
    
    // Check if we decompressed the expected amount
    return (ctx.out_pos - ctx.out_buff) == expected_size;
}

} // namespace utils
} // namespace d2portable