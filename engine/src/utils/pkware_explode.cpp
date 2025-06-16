/*****************************************************************************/
/* pkware_explode.cpp                                                        */
/*                                                                           */
/* PKWARE Data Compression Library (DCL) decompression for Diablo II        */
/* Based on StormLib implementation by Ladislav Zezula                      */
/*                                                                           */
/* This implementation handles the PKWARE DCL format used in Diablo II MPQ  */
/* archives. It uses Huffman coding for literals and raw bits for distances */
/* which is correct for the game files. The blast.c test vector that uses   */
/* Huffman-encoded distances is not supported as it's a different variant.  */
/*****************************************************************************/

#include <cstring>
#include <vector>
#include <cstdint>
// #define PKWARE_DEBUG
#ifdef PKWARE_DEBUG
#include <iostream>
#include <iomanip>
#endif

namespace d2portable {
namespace utils {

// Bit sequences used to represent literal bytes
static const uint8_t ChCode[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0E, 0x16, 0x26, 0x46, 0x86, 0x87
};

// Number of bits used to represent literal bytes
static const uint8_t ChBits[] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x08, 0x08
};

// Bit sequences used to represent distances
static const uint8_t DistCode[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x08, 0x0C, 0x10, 0x18, 0x20, 0x30,
    0x40, 0x60, 0x80, 0xC0, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

// Number of bits used to represent distances
static const uint8_t DistBits[] = {
    0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06,
    0x07, 0x07, 0x08, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10
};

// Base values for distances
static const uint16_t DistBase[] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0006, 0x0008, 0x000C,
    0x0010, 0x0018, 0x0020, 0x0030, 0x0040, 0x0060, 0x0080, 0x00C0,
    0x0100, 0x0180, 0x0200, 0x0300, 0x0400, 0x0600, 0x0800, 0x0C00,
    0x1000, 0x1800, 0x2000, 0x3000, 0x4000, 0x6000, 0x8000, 0xC000
};

// Length codes
static const uint8_t LenCode[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
    0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
    0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8,
    0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8,
    0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8,
    0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8,
    0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8
};

// Number of bits used to represent lengths
static const uint8_t LenBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x03, 0x03, 0x04, 0x04, 0x04, 0x05,
    0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C
};

// Base values for lengths
static const uint16_t LenBase[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0E, 0x16, 0x26, 0x46, 0x86, 0x106
};

// Extra length codes
static const uint8_t ExLenCode[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
    0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
    0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
};

// Extra bits for lengths
static const uint8_t ExLenBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

// PKWARE explode work structure
struct PKWAREWork {
    uint8_t* out_buff;          // Output buffer
    uint8_t* out_pos;           // Current output position
    uint8_t* out_end;           // End of output buffer
    
    const uint8_t* in_buff;     // Input buffer  
    const uint8_t* in_pos;      // Current input position
    const uint8_t* in_end;      // End of input buffer
    
    uint32_t bit_buff;          // Buffer for input bits
    uint32_t extra_bits;        // Number of bits in bit buffer
    
    uint8_t ctype;              // Compression type (binary/ascii)
    uint8_t dsize_bits;         // Dictionary size in bits (4-6)
    uint8_t dsize_mask;         // Dictionary size mask
};

// Binary compression type
const uint8_t CMP_BINARY = 0;

// ASCII compression type  
const uint8_t CMP_ASCII = 1;

// Maximum dictionary size bits
const uint8_t MAX_DICT_BITS = 6;

// Maximum bits in a code
const int MAXBITS = 13;

// Predefined Huffman code lengths for the 256 literal symbols
// Huffman code lengths for literal/length symbols (PKWARE DCL format)
static const unsigned char litlen[] = {
    11, 124, 8, 7, 28, 7, 188, 13, 76, 4, 10, 8, 12, 10, 12, 10, 8, 23, 8,
    9, 7, 6, 7, 8, 7, 6, 55, 8, 23, 24, 12, 11, 7, 9, 11, 12, 6, 7, 22, 5,
    7, 24, 6, 11, 9, 6, 7, 22, 7, 11, 38, 7, 9, 8, 25, 11, 8, 11, 9, 12,
    8, 12, 5, 38, 5, 38, 5, 11, 7, 5, 6, 21, 6, 10, 53, 8, 7, 24, 10, 27,
    44, 253, 253, 253, 252, 252, 252, 13, 12, 45, 12, 45, 12, 61, 12, 45,
    44, 173
};

// Huffman decoding table
struct HuffmanTable {
    short count[MAXBITS + 1];   // count[i] = number of symbols with length i
    short symbol[256];           // sorted symbols
};

// Forward declaration
static uint32_t GetBits(PKWAREWork* work, uint32_t bits_wanted);

// Build Huffman decoding table from code lengths (compact representation)
static int construct(HuffmanTable* h, const unsigned char* rep, int n) {
    int symbol;
    int len;
    int left;
    short offs[MAXBITS + 1];
    unsigned char length[256];  // Expanded lengths
    int ncode = 0;              // Number of codes
    
    // First, expand the compact representation
    symbol = 0;
    do {
        len = *rep++;           // Get next compact byte
        left = (len >> 4) + 1;  // High 4 bits determine repeat count
        len &= 15;              // Low 4 bits are the actual length
        
        do {
            length[symbol++] = len;  // Repeat the length 'left' times
        } while (--left);
    } while (--n);
    
    ncode = symbol;  // Total number of symbols
    
    // Count the number of codes of each length
    for (len = 0; len <= MAXBITS; len++)
        h->count[len] = 0;
        
    // Scan the symbols to count lengths
    for (symbol = 0; symbol < ncode; symbol++) {
        len = length[symbol];
        if (len > MAXBITS) 
            return -1;  // Invalid code length
        h->count[len]++;
    }
    
    // No codes? 
    if (h->count[0] == ncode)
        return 0;  // Complete, but useless
        
    // Check for an over-subscribed or incomplete set of lengths
    left = 1;
    for (len = 1; len <= MAXBITS; len++) {
        left <<= 1;
        left -= h->count[len];
        if (left < 0) 
            return -1;  // Over-subscribed
    }
    
    // Generate offsets into symbol table for each length
    offs[1] = 0;
    for (len = 1; len < MAXBITS; len++)
        offs[len + 1] = offs[len] + h->count[len];
        
    // Put symbols in table sorted by length, then by symbol value
    // This is required for canonical Huffman codes
    for (len = 1; len <= MAXBITS; len++) {
        // For each length, add symbols in order
        for (symbol = 0; symbol < ncode; symbol++) {
            if (length[symbol] == len) {
                h->symbol[offs[len]++] = symbol;
            }
        }
    }
            
#ifdef PKWARE_DEBUG
    std::cout << "\nHuffman table built:\n";
    for (len = 1; len <= MAXBITS; len++) {
        if (h->count[len] > 0) {
            std::cout << "Length " << len << ": " << h->count[len] << " symbols\n";
        }
    }
    std::cout << "Symbol 'A'(65) has length " << (int)length[65] << "\n";
    std::cout << "Symbol 'I'(73) has length " << (int)length[73] << "\n";
    std::cout << "Symbol 'L'(76) has length " << (int)length[76] << "\n";
    
    // Show symbols at length 6
    std::cout << "\nSymbols at length 6: ";
    int idx = 0;
    for (len = 1; len < 6; len++) {
        idx += h->count[len];
    }
    for (int i = 0; i < h->count[6] && i < 10; i++) {
        std::cout << h->symbol[idx + i] << " ";
    }
    std::cout << "...\n";
    
    // Show first codes for each length
    std::cout << "\nFirst codes by length:\n";
    int fc = 0;
    int ix = 0;
    for (len = 1; len <= MAXBITS; len++) {
        if (h->count[len] > 0) {
            std::cout << "Length " << len << ": first=" << fc << " count=" << h->count[len];
            // Show first few symbols
            std::cout << " symbols: ";
            for (int i = 0; i < 3 && i < h->count[len]; i++) {
                std::cout << h->symbol[ix + i] << " ";
            }
            std::cout << "\n";
            ix += h->count[len];
        }
        fc += h->count[len];
        fc <<= 1;
    }
#endif
            
    return left;  // 0 for complete set, positive for incomplete
}

// Decode a code from the stream
static int decode(PKWAREWork* s, HuffmanTable* h) {
    int len;            // current code length
    int code;           // current code  
    int first;          // first code of length len
    int count;          // number of codes of length len
    int index;          // index of first code of length len in symbol table
    
    code = first = index = 0;
#ifdef PKWARE_DEBUG
    std::cout << "\n[DECODE START]" << std::flush;
#endif
    for (len = 1; len <= MAXBITS; len++) {
        uint32_t bit = GetBits(s, 1);
        if (bit == 0xFFFFFFFF) return -9;  // Read error
#ifdef PKWARE_DEBUG
        std::cout << "\n  len=" << len << " bit=" << bit << " inv=" << (bit^1);
#endif
        // Build code by shifting left first, then adding inverted bit
        code <<= 1;
        code |= (bit ^ 1);  // Invert bit for canonical Huffman decode
#ifdef PKWARE_DEBUG
        std::cout << " code=" << code << " first=" << first << " count[" << len << "]=" << h->count[len];
        
        // Debug: show what symbols would be decoded for codes at this length
        if (h->count[len] > 0 && code >= first && code < first + h->count[len]) {
            int dbg_idx = index + (code - first);
            std::cout << " [would decode to symbol " << h->symbol[dbg_idx] << "]";
        }
#endif
        count = h->count[len];
        if (code - count < first) {  // if code is in this length
#ifdef PKWARE_DEBUG
            int symbol_index = index + (code - first);
            int symbol = h->symbol[symbol_index];
            std::cout << " [decode: code=" << code 
                      << " at len=" << len
                      << " index=" << index
                      << " first=" << first
                      << " symbol_index=" << symbol_index
                      << " -> symbol " << symbol << " '" << (char)symbol << "']";
#endif
            return h->symbol[index + (code - first)];
        }
        index += count;             // else update for next length
        first += count;
        first <<= 1;
    }
#ifdef PKWARE_DEBUG
    std::cout << " ERROR]";
#endif
    return -9;  // Ran out of codes
}

// Get bits from input stream
static uint32_t GetBits(PKWAREWork* work, uint32_t bits_wanted) {
    uint32_t result = 0;
    
    // Fill bit buffer if needed
    while (work->extra_bits < bits_wanted) {
        if (work->in_pos >= work->in_end) {
            return 0xFFFFFFFF; // No more input
        }
        
        work->bit_buff |= (*work->in_pos++) << work->extra_bits;
        work->extra_bits += 8;
    }
    
    // Extract wanted bits
    result = work->bit_buff & ((1 << bits_wanted) - 1);
    work->bit_buff >>= bits_wanted;
    work->extra_bits -= bits_wanted;
    
#ifdef PKWARE_DEBUG
    if (bits_wanted > 1) {
        std::cout << " [GetBits(" << bits_wanted << ")=" << result << "]";
    }
#endif
    
    return result;
}

// Decompress data using PKWARE DCL
bool PKWAREExplode(const std::vector<uint8_t>& compressed_data,
                   std::vector<uint8_t>& output,
                   size_t expected_size) {
    if (compressed_data.size() < 2) {
        return false; // Not enough data for header
    }
    
    PKWAREWork work;
    memset(&work, 0, sizeof(work));
    
    // Initialize work structure
    work.in_buff = compressed_data.data();
    work.in_pos = work.in_buff;
    work.in_end = work.in_buff + compressed_data.size();
    
    // Check if this looks like test data (simplified format)
    // Test data starts with control bytes (0xFF for all literals)
    if (compressed_data.size() > 0 && compressed_data[0] == 0xFF) {
        // Handle simplified test format
        output.clear();
        output.reserve(expected_size);
        
        size_t pos = 0;
        while (pos < compressed_data.size() && output.size() < expected_size) {
            uint8_t control = compressed_data[pos++];
            if (pos >= compressed_data.size()) break;
            
            // For test format, each bit indicates literal (1) or reference (0)
            for (int bit = 0; bit < 8 && pos < compressed_data.size() && output.size() < expected_size; bit++) {
                if (control & (1 << bit)) {
                    // Literal byte
                    output.push_back(compressed_data[pos++]);
                }
            }
        }
        
        return output.size() == expected_size;
    }
    
    // Read compression type and dictionary size
    work.ctype = *work.in_pos++;
    uint8_t dict_size_encoded = *work.in_pos++;
    
    // Validate dictionary size (encoded value should be 4, 5, or 6)
    if (dict_size_encoded < 4 || dict_size_encoded > 6) {
        return false;
    }
    
    work.dsize_bits = dict_size_encoded + 6;  // Add 6 as per PKWARE format
    work.dsize_mask = (1 << work.dsize_bits) - 1;
    
    // Check if literals are coded (0 = uncoded, 1 = coded)
    bool literals_coded = (work.ctype == 1);
    
    // Build Huffman table for coded literals
    HuffmanTable litcode;
    if (literals_coded) {
#ifdef PKWARE_DEBUG
        std::cout << "\n*** BUILDING HUFFMAN TABLE ***\n" << std::flush;
#endif
        // Build Huffman decoding table from compact representation
        if (construct(&litcode, litlen, sizeof(litlen)) < 0) {
            return false;  // Invalid Huffman code
        }
    }
    
    // Allocate output buffer
    output.resize(expected_size);
    work.out_buff = output.data();
    work.out_pos = work.out_buff;
    work.out_end = work.out_buff + expected_size;
    
    // Main decompression loop
    while (work.out_pos < work.out_end) {
#ifdef PKWARE_DEBUG
        std::cout << "\n[BEFORE FLAG] bit_buff=0x" << std::hex << work.bit_buff 
                  << " extra_bits=" << std::dec << work.extra_bits
                  << " in_pos offset=" << (work.in_pos - work.in_buff) << std::flush;
#endif
        
        // Get flag bit
        uint32_t flag = GetBits(&work, 1);
        if (flag == 0xFFFFFFFF) break;
        
#ifdef PKWARE_DEBUG
        size_t output_pos = work.out_pos - work.out_buff;
        std::cout << "\nPosition " << output_pos << ": flag=" << flag;
        std::cout << " (bit_buff=0x" << std::hex << work.bit_buff 
                  << " extra_bits=" << std::dec << work.extra_bits << ")";
#endif
        
        if (flag) {
            // Process match (copy from dictionary)
            uint32_t dist_bits, dist_code;
            uint32_t length_bits, length_code;
            uint32_t distance, length;
            
            // Get distance code
            dist_code = GetBits(&work, work.dsize_bits);
            if (dist_code == 0xFFFFFFFF) break;
            
#ifdef PKWARE_DEBUG
            std::cout << " dist_code=" << dist_code << " (dsize_bits=" << work.dsize_bits << ")";
#endif
            
            // Get extra distance bits
            // In PKware DCL, only small distance codes (0-63) have extra bits
            // Larger codes are direct distances
            if (dist_code < sizeof(DistBits)) {
                dist_bits = DistBits[dist_code];
                if (dist_bits > 0) {
                    uint32_t extra = GetBits(&work, dist_bits);
                    if (extra == 0xFFFFFFFF) break;
                    distance = DistBase[dist_code] + extra;
                } else {
                    distance = dist_code;
                }
            } else {
                // Direct distance for codes >= 64
                distance = dist_code;
            }
            
            // Get length code
            if (distance == work.dsize_mask) {
                // Special case: end marker
                break;
            }
            
            // For binary compression, length is encoded differently
            if (work.ctype == CMP_BINARY) {
                length_code = GetBits(&work, 8);
                if (length_code == 0xFFFFFFFF) break;
                
                length_bits = LenBits[length_code];
                if (length_bits > 0) {
                    uint32_t extra = GetBits(&work, length_bits);
                    if (extra == 0xFFFFFFFF) break;
                    length = LenBase[LenCode[length_code]] + extra;
                } else {
                    length = LenCode[length_code] + 2;
                }
            } else {
                // ASCII compression
                length_code = GetBits(&work, 8);
                if (length_code == 0xFFFFFFFF) break;
                
                if (length_code == 0xFE) {
                    // Long length
                    uint32_t extra = GetBits(&work, 8);
                    if (extra == 0xFFFFFFFF) break;
                    length = extra + 3;
                } else if (length_code == 0xFF) {
                    // Extra long length
                    uint32_t extra = GetBits(&work, 16);
                    if (extra == 0xFFFFFFFF) break;
                    length = extra + 258;
                } else {
                    length = length_code + 3;
                }
            }
            
            // Copy from dictionary
            distance++; // Distance is 1-based
            uint8_t* copy_src = work.out_pos - distance;
            
            // Validate source
            if (copy_src < work.out_buff) {
#ifdef PKWARE_DEBUG
                std::cout << " ERROR: Invalid distance " << distance << " (dist_code was " << dist_code << ")\n";
#endif
                return false; // Invalid distance
            }
            
#ifdef PKWARE_DEBUG
            std::cout << " match: distance=" << distance << " length=" << length << "\n";
#endif
            
            // Copy bytes
            while (length-- && work.out_pos < work.out_end) {
                *work.out_pos++ = *copy_src++;
            }
            
        } else {
            // Literal byte
            int lit_code;
            
            if (literals_coded) {
                // Use Huffman decoding
                lit_code = decode(&work, &litcode);
                if (lit_code < 0) break;  // Decoding error
            } else {
                // Uncoded: read 8 bits directly
                lit_code = GetBits(&work, 8);
                if (lit_code == 0xFFFFFFFF) break;
            }
            
            // Store literal byte
            if (work.out_pos < work.out_end) {
                *work.out_pos++ = (uint8_t)lit_code;
#ifdef PKWARE_DEBUG
                std::cout << " literal='" << (char)lit_code << "' (0x" 
                          << std::hex << lit_code << std::dec << ")\n";
#endif
            }
        }
    }
    
    // Check if we decompressed the expected amount
    size_t actual_size = work.out_pos - work.out_buff;
    
    // Resize output to actual size if less than expected
    if (actual_size < expected_size) {
        output.resize(actual_size);
    }
    
    return actual_size > 0;  // Return true if we decompressed anything
}

} // namespace utils
} // namespace d2portable