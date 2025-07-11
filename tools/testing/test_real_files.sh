#!/bin/bash

# Test script for real Diablo II game files
# This script validates our MPQ and DC6 implementation with actual game assets

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Directories
BUILD_DIR="build"
TEST_DIR="temp/d2_extract"
RESULTS_DIR="temp/test_results"

# Create results directory
mkdir -p "$RESULTS_DIR"

# Log file
LOG_FILE="$RESULTS_DIR/test_real_files_$(date +%Y%m%d_%H%M%S).log"

# Function to log messages
log() {
    echo -e "$1" | tee -a "$LOG_FILE"
}

# Function to run a test and capture output
run_test() {
    local test_name="$1"
    local command="$2"
    
    log "${BLUE}Running: $test_name${NC}"
    if eval "$command" >> "$LOG_FILE" 2>&1; then
        log "${GREEN}✓ $test_name passed${NC}"
        return 0
    else
        log "${RED}✗ $test_name failed${NC}"
        return 1
    fi
}

# Header
log "${BLUE}=== Diablo II Real File Test Suite ===${NC}"
log "Date: $(date)"
log "Build directory: $BUILD_DIR"
log "Test directory: $TEST_DIR"
log ""

# Check if build exists
if [ ! -d "$BUILD_DIR" ]; then
    log "${RED}Error: Build directory not found. Please run cmake and build first.${NC}"
    exit 1
fi

# Check if test directory exists
if [ ! -d "$TEST_DIR" ]; then
    log "${RED}Error: Test directory '$TEST_DIR' not found.${NC}"
    log "${YELLOW}Please extract Diablo II files to this directory first.${NC}"
    exit 1
fi

# Build the test tools
log "${BLUE}Building test tools...${NC}"
if ! cmake --build "$BUILD_DIR" --target d2_unit_tests 2>&1 | tee -a "$LOG_FILE"; then
    log "${RED}Failed to build test tools${NC}"
    exit 1
fi

# Create test programs
log "${BLUE}Creating MPQ test program...${NC}"
cat > "$BUILD_DIR/mpq_test.cpp" << 'EOF'
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include "utils/mpq_loader.h"

void printFileInfo(const std::string& filename, const std::vector<uint8_t>& data) {
    std::cout << "  File: " << filename << std::endl;
    std::cout << "  Size: " << data.size() << " bytes" << std::endl;
    
    // Show first 16 bytes in hex
    std::cout << "  First 16 bytes: ";
    for (size_t i = 0; i < std::min(size_t(16), data.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
    
    // Check for common file signatures
    if (data.size() >= 4) {
        uint32_t sig = *reinterpret_cast<const uint32_t*>(data.data());
        if (sig == 0x1A51504D) std::cout << "  Type: MPQ Archive" << std::endl;
        else if (sig == 0xAA55AA55) std::cout << "  Type: DC6 Sprite" << std::endl;
        else if (data[0] == 0x50 && data[1] == 0x4B) std::cout << "  Type: ZIP/JAR" << std::endl;
        else if (data[0] == 0x0A && data[1] == 0x0D) std::cout << "  Type: Text file" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file> [file_to_extract]" << std::endl;
        return 1;
    }
    
    d2::MPQLoader loader;
    std::string mpqPath = argv[1];
    
    std::cout << "\n=== Testing MPQ: " << mpqPath << " ===" << std::endl;
    
    // Try to open the MPQ
    if (!loader.open(mpqPath)) {
        std::cerr << "Failed to open MPQ file" << std::endl;
        return 1;
    }
    
    std::cout << "Successfully opened MPQ file!" << std::endl;
    
    // If a specific file was requested, try to extract it
    if (argc >= 3) {
        std::string targetFile = argv[2];
        std::vector<uint8_t> data;
        
        std::cout << "\nAttempting to extract: " << targetFile << std::endl;
        if (loader.extractFile(targetFile, data)) {
            std::cout << "Success!" << std::endl;
            printFileInfo(targetFile, data);
            
            // Save to file
            std::string outputPath = "temp/test_results/" + targetFile;
            size_t lastSlash = outputPath.find_last_of('/');
            if (lastSlash != std::string::npos) {
                std::string dir = outputPath.substr(0, lastSlash);
                system(("mkdir -p " + dir).c_str());
            }
            
            std::ofstream out(outputPath, std::ios::binary);
            out.write(reinterpret_cast<const char*>(data.data()), data.size());
            out.close();
            std::cout << "Saved to: " << outputPath << std::endl;
        } else {
            std::cerr << "Failed to extract file" << std::endl;
            return 1;
        }
    } else {
        // Try to extract common test files
        std::vector<std::string> testFiles = {
            "(listfile)",
            "(attributes)",
            "(signature)",
            "data\\global\\ui\\Loading\\loadingscreen.dc6",
            "data\\global\\ui\\CURSOR\\cursorhand.dc6",
            "data\\global\\ui\\PANEL\\800ctrlpnl7.dc6",
            "data\\global\\chars\\BA\\COF\\BACOF.COF",
            "data\\global\\excel\\armor.txt",
            "data\\global\\excel\\weapons.txt",
            "data\\global\\excel\\misc.txt"
        };
        
        std::cout << "\nTrying to extract common files..." << std::endl;
        int successCount = 0;
        
        for (const auto& file : testFiles) {
            std::cout << "\nTesting: " << file << std::endl;
            std::vector<uint8_t> data;
            
            if (loader.extractFile(file, data)) {
                successCount++;
                std::cout << "  ✓ Extracted successfully" << std::endl;
                printFileInfo(file, data);
            } else {
                std::cout << "  ✗ Failed to extract" << std::endl;
            }
        }
        
        std::cout << "\n=== Summary ===" << std::endl;
        std::cout << "Successfully extracted: " << successCount << "/" << testFiles.size() << " files" << std::endl;
    }
    
    return 0;
}
EOF

# Compile the MPQ test program
log "${BLUE}Compiling MPQ test program...${NC}"
if ! g++ -std=c++17 -I"$BUILD_DIR/../engine/include" -L"$BUILD_DIR/engine/src" \
    "$BUILD_DIR/mpq_test.cpp" -ld2_engine -lz -o "$BUILD_DIR/mpq_test" 2>&1 | tee -a "$LOG_FILE"; then
    log "${RED}Failed to compile MPQ test program${NC}"
fi

# Create DC6 test program
log "${BLUE}Creating DC6 test program...${NC}"
cat > "$BUILD_DIR/dc6_test.cpp" << 'EOF'
#include <iostream>
#include <fstream>
#include <vector>
#include "graphics/dc6_parser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dc6_file>" << std::endl;
        return 1;
    }
    
    std::string dc6Path = argv[1];
    std::cout << "\n=== Testing DC6: " << dc6Path << " ===" << std::endl;
    
    // Read file
    std::ifstream file(dc6Path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    
    size_t size = file.tellg();
    file.seekg(0);
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    file.close();
    
    // Parse DC6
    d2::DC6Parser parser;
    if (!parser.parse(data)) {
        std::cerr << "Failed to parse DC6 file" << std::endl;
        return 1;
    }
    
    std::cout << "Successfully parsed DC6!" << std::endl;
    std::cout << "Directions: " << static_cast<int>(parser.getDirectionCount()) << std::endl;
    std::cout << "Frames per direction: " << static_cast<int>(parser.getFramesPerDirection()) << std::endl;
    
    // Try to get first frame
    auto frame = parser.getFrame(0, 0);
    if (frame) {
        std::cout << "First frame size: " << frame->width << "x" << frame->height << std::endl;
        std::cout << "First frame data size: " << frame->data.size() << " bytes" << std::endl;
    }
    
    return 0;
}
EOF

# Compile the DC6 test program
log "${BLUE}Compiling DC6 test program...${NC}"
if ! g++ -std=c++17 -I"$BUILD_DIR/../engine/include" -L"$BUILD_DIR/engine/src" \
    "$BUILD_DIR/dc6_test.cpp" -ld2_engine -lz -o "$BUILD_DIR/dc6_test" 2>&1 | tee -a "$LOG_FILE"; then
    log "${RED}Failed to compile DC6 test program${NC}"
fi

# Run unit tests first
log ""
log "${BLUE}=== Running Unit Tests ===${NC}"
if run_test "Unit Tests" "$BUILD_DIR/engine/tests/d2_unit_tests"; then
    UNIT_TESTS_PASSED=1
else
    UNIT_TESTS_PASSED=0
fi

# Find all MPQ files
log ""
log "${BLUE}=== Scanning for MPQ files ===${NC}"
MPQ_FILES=$(find "$TEST_DIR" -name "*.mpq" -o -name "*.MPQ" 2>/dev/null | sort)

if [ -z "$MPQ_FILES" ]; then
    log "${RED}No MPQ files found in $TEST_DIR${NC}"
    exit 1
fi

log "Found MPQ files:"
echo "$MPQ_FILES" | while read mpq; do
    log "  - $(basename "$mpq") ($(du -h "$mpq" | cut -f1))"
done

# Test each MPQ file
log ""
log "${BLUE}=== Testing MPQ Files ===${NC}"
MPQ_SUCCESS=0
MPQ_TOTAL=0

echo "$MPQ_FILES" | while read mpq; do
    if [ -z "$mpq" ]; then continue; fi
    
    MPQ_TOTAL=$((MPQ_TOTAL + 1))
    log ""
    log "${YELLOW}Testing: $(basename "$mpq")${NC}"
    
    if [ -x "$BUILD_DIR/mpq_test" ]; then
        if "$BUILD_DIR/mpq_test" "$mpq" >> "$LOG_FILE" 2>&1; then
            MPQ_SUCCESS=$((MPQ_SUCCESS + 1))
            log "${GREEN}✓ MPQ test passed${NC}"
            
            # Try to extract a DC6 file for testing
            DC6_FILE="data\\global\\ui\\CURSOR\\cursorhand.dc6"
            if "$BUILD_DIR/mpq_test" "$mpq" "$DC6_FILE" >> "$LOG_FILE" 2>&1; then
                log "${GREEN}✓ Extracted DC6 file${NC}"
                
                # Test the DC6 parser
                EXTRACTED_DC6="$RESULTS_DIR/data/global/ui/CURSOR/cursorhand.dc6"
                if [ -f "$EXTRACTED_DC6" ] && [ -x "$BUILD_DIR/dc6_test" ]; then
                    if "$BUILD_DIR/dc6_test" "$EXTRACTED_DC6" >> "$LOG_FILE" 2>&1; then
                        log "${GREEN}✓ DC6 parser test passed${NC}"
                    else
                        log "${RED}✗ DC6 parser test failed${NC}"
                    fi
                fi
            fi
        else
            log "${RED}✗ MPQ test failed${NC}"
        fi
    else
        log "${RED}✗ MPQ test program not available${NC}"
    fi
done

# Test specific known files
log ""
log "${BLUE}=== Testing Specific File Extraction ===${NC}"
declare -A TEST_FILES=(
    ["d2data.mpq"]="data\\global\\excel\\armor.txt"
    ["d2exp.mpq"]="data\\global\\excel\\misc.txt"
    ["d2char.mpq"]="data\\global\\chars\\BA\\COF\\BACOF.COF"
)

for mpq in "${!TEST_FILES[@]}"; do
    MPQ_PATH="$TEST_DIR/$mpq"
    if [ -f "$MPQ_PATH" ] && [ -x "$BUILD_DIR/mpq_test" ]; then
        TARGET_FILE="${TEST_FILES[$mpq]}"
        log ""
        log "${YELLOW}Extracting $TARGET_FILE from $mpq${NC}"
        
        if "$BUILD_DIR/mpq_test" "$MPQ_PATH" "$TARGET_FILE" >> "$LOG_FILE" 2>&1; then
            log "${GREEN}✓ Extraction successful${NC}"
        else
            log "${RED}✗ Extraction failed${NC}"
        fi
    fi
done

# Summary
log ""
log "${BLUE}=== Test Summary ===${NC}"
log "Unit Tests: $([ $UNIT_TESTS_PASSED -eq 1 ] && echo "${GREEN}PASSED${NC}" || echo "${RED}FAILED${NC}")"
log "MPQ Files Tested: $MPQ_TOTAL"
log "MPQ Tests Passed: $MPQ_SUCCESS"
log ""
log "Results saved to: $LOG_FILE"
log "Extracted files saved to: $RESULTS_DIR"

# Check for common issues
log ""
log "${BLUE}=== Diagnostics ===${NC}"

# Check for encrypted files
if grep -q "encrypted" "$LOG_FILE" 2>/dev/null; then
    log "${YELLOW}⚠ Some files appear to be encrypted${NC}"
    log "  Our implementation currently doesn't support file-level encryption"
fi

# Check for compression issues
if grep -q "compression" "$LOG_FILE" 2>/dev/null; then
    log "${YELLOW}⚠ Some files use compression methods we might not support${NC}"
    log "  Supported: Zlib, PKWARE DCL, Huffman"
fi

# Recommendations
log ""
log "${BLUE}=== Recommendations ===${NC}"
log "1. Files like (listfile) and text files are usually unencrypted - try these first"
log "2. UI elements (DC6 files) are often uncompressed or use simple compression"
log "3. For encrypted files, we need to implement the file decryption algorithm"
log "4. Check $RESULTS_DIR for any successfully extracted files"

# Exit status
if [ $UNIT_TESTS_PASSED -eq 1 ] && [ $MPQ_SUCCESS -gt 0 ]; then
    exit 0
else
    exit 1
fi