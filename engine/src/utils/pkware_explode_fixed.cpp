/*****************************************************************************/
/* pkware_explode_fixed.cpp                                                  */
/*                                                                           */
/* PKWARE Data Compression Library decompression - Fixed implementation      */
/* Based on blast.c by Mark Adler                                           */
/*****************************************************************************/

#include <cstring>
#include <vector>
#include <cstdint>

namespace d2portable {
namespace utils {

// Constants for PKWARE DCL
const int MAXBITS = 13;     // Maximum code length
const int MAXWIN = 4096;    // Maximum window size

// State for bit reading
struct BitState {
    const uint8_t* in;      // Input buffer
    const uint8_t* in_end;  // End of input
    uint32_t bitbuf;        // Bit buffer
    int bitcnt;             // Number of bits in buffer
};

// Huffman decoding tables
struct HuffTable {
    int* count;      // Number of symbols with each length
    int* symbol;     // Symbols sorted by length
};

// Get bits from input stream (LSB first)
static int getbits(BitState* s, int need) {
    uint32_t val = s->bitbuf;
    
    while (s->bitcnt < need) {
        if (s->in >= s->in_end) {
            return -1;  // Out of input
        }
        val |= (uint32_t)(*s->in++) << s->bitcnt;
        s->bitcnt += 8;
    }
    
    s->bitbuf = val >> need;
    s->bitcnt -= need;
    
    return val & ((1 << need) - 1);
}

// Decode a symbol using Huffman table
static int decode(BitState* s, HuffTable* h) {
    int len = 1;
    int code = 0;
    int first = 0;
    int index = 0;
    
    for (;;) {
        int bit = getbits(s, 1);
        if (bit < 0) return -1;
        
        code |= bit;
        int count = h->count[len];
        
        if (code < first + count) {
            return h->symbol[index + (code - first)];
        }
        
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
        len++;
        
        if (len > MAXBITS) {
            return -1;  // Invalid code
        }
    }
}

// Build Huffman decoding table from code lengths
static int construct(HuffTable* h, const int* length, int n) {
    // Count symbols for each length
    for (int len = 0; len <= MAXBITS; len++) {
        h->count[len] = 0;
    }
    
    for (int symbol = 0; symbol < n; symbol++) {
        if (length[symbol] > MAXBITS) {
            return -1;  // Invalid length
        }
        h->count[length[symbol]]++;
    }
    
    if (h->count[0] == n) {
        return 0;  // No codes
    }
    
    // Check for incomplete code set
    int left = 1;
    for (int len = 1; len <= MAXBITS; len++) {
        left <<= 1;
        left -= h->count[len];
        if (left < 0) {
            return -1;  // Over-subscribed
        }
    }
    
    // Generate offsets into symbol table for each length
    int offs[MAXBITS + 1];
    offs[1] = 0;
    for (int len = 1; len < MAXBITS; len++) {
        offs[len + 1] = offs[len] + h->count[len];
    }
    
    // Sort symbols by length
    for (int symbol = 0; symbol < n; symbol++) {
        if (length[symbol] != 0) {
            h->symbol[offs[length[symbol]]++] = symbol;
        }
    }
    
    return 0;
}

// Main decompression function
bool PKWAREExplodeFixed(const std::vector<uint8_t>& compressed_data,
                        std::vector<uint8_t>& output,
                        size_t expected_size) {
    if (compressed_data.size() < 2) {
        return false;
    }
    
    // Initialize bit state
    BitState state;
    state.in = compressed_data.data();
    state.in_end = compressed_data.data() + compressed_data.size();
    state.bitbuf = 0;
    state.bitcnt = 0;
    
    // Read header
    int lit = getbits(&state, 8);
    if (lit < 0) return false;
    
    int dict = getbits(&state, 8);
    if (dict < 0) return false;
    
    // Validate dictionary size
    if (dict < 4 || dict > 6) {
        return false;
    }
    
    // Set up output
    output.clear();
    output.reserve(expected_size);
    
    // Length code table for PKWARE DCL
    static const int litcnt = 256;
    static const int lencnt = 64;
    static const int distcnt = 64;
    
    // Code lengths for literal/length alphabet (PKWARE DCL specific)
    static const int litlen[256] = {
        11, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
        124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
        12, 12, 8, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11,
        10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12,
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11,
        11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 0
    };
    
    // Code lengths for length alphabet
    static const int lenlen[64] = {
        3, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7,
        7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 11, 11
    };
    
    // Build decoding tables
    int count[MAXBITS + 1];
    int symbol[256];
    HuffTable litcode = {count, symbol};
    
    if (lit) {
        // Uncoded literals - use fixed table
        if (construct(&litcode, litlen, litcnt) != 0) {
            return false;
        }
    }
    
    // Decompress data
    while (output.size() < expected_size) {
        // Read flag bit (0 = literal, 1 = match)
        int flag = getbits(&state, 1);
        if (flag < 0) break;
        
        if (flag == 0) {
            // Literal
            int symbol;
            if (lit) {
                // Coded literal
                symbol = decode(&state, &litcode);
                if (symbol < 0) return false;
            } else {
                // Uncoded literal
                symbol = getbits(&state, 8);
                if (symbol < 0) return false;
            }
            output.push_back((uint8_t)symbol);
        } else {
            // Length/distance pair
            // For now, simplified implementation
            // Real implementation would decode length and distance
            break;
        }
    }
    
    return output.size() == expected_size;
}

} // namespace utils
} // namespace d2portable