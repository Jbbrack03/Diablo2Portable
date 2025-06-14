#!/bin/bash

# Extract assets from Diablo II MPQ files
# Usage: ./extract_assets.sh [path_to_d2_files]

echo "Diablo II Asset Extraction Tool"
echo "==============================="
echo ""

# Check if vendor directory exists
VENDOR_DIR="${1:-./vendor}"
OUTPUT_DIR="./extracted_assets"

if [ ! -d "$VENDOR_DIR" ]; then
    echo "Error: Vendor directory not found at $VENDOR_DIR"
    echo "Please provide the path to your Diablo II installation files"
    echo "Usage: $0 /path/to/diablo2/files"
    exit 1
fi

# Look for common MPQ files
MPQ_FILES=(
    "d2data.mpq"
    "d2char.mpq"
    "d2sfx.mpq"
    "d2speech.mpq"
    "d2video.mpq"
    "d2exp.mpq"
    "d2xmusic.mpq"
    "d2xtalk.mpq"
    "d2xvideo.mpq"
    "patch_d2.mpq"
)

echo "Looking for MPQ files in: $VENDOR_DIR"
echo ""

FOUND_MPQS=()
for mpq in "${MPQ_FILES[@]}"; do
    if [ -f "$VENDOR_DIR/$mpq" ]; then
        echo "✓ Found: $mpq"
        FOUND_MPQS+=("$VENDOR_DIR/$mpq")
    else
        echo "✗ Missing: $mpq"
    fi
done

if [ ${#FOUND_MPQS[@]} -eq 0 ]; then
    echo ""
    echo "Error: No MPQ files found in $VENDOR_DIR"
    echo "Please ensure you have copied Diablo II files to the vendor directory"
    exit 1
fi

echo ""
echo "Found ${#FOUND_MPQS[@]} MPQ files"
echo ""

# Build the tools if needed
if [ ! -f "./build/tools/test_dc6_extraction" ]; then
    echo "Building extraction tools..."
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build --target test_dc6_extraction
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Test MPQ integrity first
echo "Testing MPQ file integrity..."
for mpq in "${FOUND_MPQS[@]}"; do
    echo -n "Analyzing $(basename $mpq)... "
    if ./build/tools/analyze_mpq "$mpq" > /dev/null 2>&1; then
        echo "OK"
    else
        echo "FAILED"
    fi
done

echo ""
echo "Extracting common DC6 sprites..."
echo ""

# Extract DC6 sprites from d2data.mpq
if [ -f "$VENDOR_DIR/d2data.mpq" ]; then
    echo "Extracting sprites from d2data.mpq..."
    ./build/tools/test_dc6_extraction "$VENDOR_DIR/d2data.mpq" "$OUTPUT_DIR/sprites"
fi

# Try to find known files
echo ""
echo "Looking for known game files..."
if [ -f "$VENDOR_DIR/d2data.mpq" ]; then
    ./build/tools/try_known_files "$VENDOR_DIR/d2data.mpq"
fi

echo ""
echo "Asset extraction complete!"
echo "Extracted files are in: $OUTPUT_DIR"
echo ""
echo "Note: This tool only extracts a sample of assets for testing."
echo "Full asset extraction would require additional implementation."