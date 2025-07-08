#!/bin/bash

# Deploy to Android device script for Diablo II Portable
# This script builds and deploys the Android application to a connected device

set -e  # Exit on any error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
ANDROID_DIR="$PROJECT_ROOT/android"

echo "🚀 Deploying Diablo II Portable to Android device..."

# Check if adb is available
if ! command -v adb &> /dev/null; then
    echo "❌ Error: adb not found. Please install Android SDK platform-tools."
    exit 1
fi

# Check if any devices are connected
DEVICES=$(adb devices | grep -v "List of devices" | grep "device$" | wc -l)
if [ "$DEVICES" -eq 0 ]; then
    echo "❌ Error: No Android devices connected. Please connect a device with USB debugging enabled."
    exit 1
fi

echo "📱 Found $DEVICES connected device(s)"

# Navigate to Android project directory
cd "$ANDROID_DIR"

# Check if gradlew exists
if [ ! -f "./gradlew" ]; then
    echo "❌ Error: gradlew not found in $ANDROID_DIR"
    exit 1
fi

# Make gradlew executable
chmod +x ./gradlew

# Build and install debug APK
echo "🔨 Building and installing debug APK..."
./gradlew installDebug

# Check if installation was successful
if [ $? -eq 0 ]; then
    echo "✅ Successfully deployed Diablo II Portable to Android device!"
    echo ""
    echo "📋 Next steps:"
    echo "  1. Find 'Diablo II Portable' app on your device"
    echo "  2. Ensure you have copied your legally-owned D2 MPQ files"
    echo "  3. Launch the app and follow the onboarding wizard"
    echo ""
    echo "🎮 Happy gaming!"
else
    echo "❌ Failed to deploy to device. Check the error messages above."
    exit 1
fi