#!/bin/bash

# Diablo2Portable Temporary Files Cleanup Script
# This script removes large temporary directories that are not needed for development

echo "Diablo2Portable Temporary Files Cleanup"
echo "======================================="
echo ""
echo "This script will remove the following temporary directories:"
echo "- temp/ (2.0GB)"
echo "- vendor/Diablo II CD Images/ (2.2GB)"
echo "- vendor/mpq_temp/ (1.9GB)"
echo ""
echo "Total space to be freed: ~6.1GB"
echo ""
read -p "Do you want to proceed? (y/N) " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Removing temporary directories..."
    
    # Remove temp directory
    if [ -d "temp/" ]; then
        echo "Removing temp/..."
        rm -rf temp/
    fi
    
    # Remove CD images
    if [ -d "vendor/Diablo II CD Images/" ]; then
        echo "Removing vendor/Diablo II CD Images/..."
        rm -rf "vendor/Diablo II CD Images/"
    fi
    
    # Remove mpq_temp
    if [ -d "vendor/mpq_temp/" ]; then
        echo "Removing vendor/mpq_temp/..."
        rm -rf "vendor/mpq_temp/"
    fi
    
    echo "Cleanup complete!"
else
    echo "Cleanup cancelled."
fi