#!/bin/bash

# Diablo II Portable - Development Environment Setup Script
# This script sets up the development environment for building the Diablo II Android port

set -e  # Exit on error

echo "=== Diablo II Portable Development Environment Setup ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

# Check operating system
OS_TYPE=$(uname -s)
print_status "Detected OS: $OS_TYPE"

# Check for required tools
check_tool() {
    if command -v $1 &> /dev/null; then
        print_status "$1 is installed ($(command -v $1))"
        return 0
    else
        print_error "$1 is not installed"
        return 1
    fi
}

echo
echo "Checking for required tools..."

MISSING_TOOLS=()

# Check essential tools
check_tool "git" || MISSING_TOOLS+=("git")
check_tool "cmake" || MISSING_TOOLS+=("cmake")
check_tool "make" || MISSING_TOOLS+=("make")
check_tool "python3" || MISSING_TOOLS+=("python3")

# Check for Java (required for Android development)
if command -v java &> /dev/null; then
    JAVA_VERSION=$(java -version 2>&1 | head -n 1)
    print_status "Java is installed: $JAVA_VERSION"
else
    print_error "Java is not installed"
    MISSING_TOOLS+=("java")
fi

# Report missing tools
if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
    echo
    print_error "Missing required tools: ${MISSING_TOOLS[*]}"
    echo "Please install the missing tools before continuing."
    exit 1
fi

# Check for Android SDK
echo
echo "Checking for Android development tools..."

if [ -n "$ANDROID_HOME" ]; then
    print_status "ANDROID_HOME is set: $ANDROID_HOME"
    
    # Verify SDK tools exist
    if [ -d "$ANDROID_HOME/platform-tools" ]; then
        print_status "Android platform-tools found"
    else
        print_warning "Android platform-tools not found in ANDROID_HOME"
    fi
else
    print_warning "ANDROID_HOME is not set"
    echo "Please set ANDROID_HOME to your Android SDK location:"
    echo "  export ANDROID_HOME=/path/to/android-sdk"
fi

# Check for Android NDK
if [ -n "$ANDROID_NDK" ] || [ -n "$ANDROID_NDK_HOME" ]; then
    NDK_PATH="${ANDROID_NDK:-$ANDROID_NDK_HOME}"
    print_status "Android NDK found: $NDK_PATH"
    
    # Check NDK version
    if [ -f "$NDK_PATH/source.properties" ]; then
        NDK_VERSION=$(grep "Pkg.Revision" "$NDK_PATH/source.properties" | cut -d'=' -f2 | tr -d ' ')
        print_status "NDK Version: $NDK_VERSION"
    fi
else
    print_warning "Android NDK not found"
    echo "Please set ANDROID_NDK or ANDROID_NDK_HOME to your NDK location:"
    echo "  export ANDROID_NDK=/path/to/android-ndk"
fi

# Create necessary directories
echo
echo "Creating project directories..."

DIRS=(
    "build"
    "assets"
    "extracted_assets"
    "android/app/libs"
)

for dir in "${DIRS[@]}"; do
    if [ ! -d "$dir" ]; then
        mkdir -p "$dir"
        print_status "Created directory: $dir"
    else
        print_status "Directory exists: $dir"
    fi
done

# Initialize git submodules if any
if [ -f ".gitmodules" ]; then
    echo
    echo "Initializing git submodules..."
    git submodule update --init --recursive
    print_status "Git submodules initialized"
fi

# Download Google Test if not already present
if [ ! -d "build/_deps/googletest-src" ]; then
    echo
    echo "Google Test will be downloaded during first build..."
fi

# Create local.properties for Android build
if [ -n "$ANDROID_HOME" ] && [ ! -f "android/local.properties" ]; then
    echo
    echo "Creating android/local.properties..."
    echo "sdk.dir=$ANDROID_HOME" > android/local.properties
    
    if [ -n "$ANDROID_NDK" ] || [ -n "$ANDROID_NDK_HOME" ]; then
        NDK_PATH="${ANDROID_NDK:-$ANDROID_NDK_HOME}"
        echo "ndk.dir=$NDK_PATH" >> android/local.properties
    fi
    
    print_status "Created android/local.properties"
fi

# Environment setup script
ENV_SCRIPT="env_setup.sh"
echo
echo "Creating environment setup script: $ENV_SCRIPT"

cat > "$ENV_SCRIPT" << 'EOF'
#!/bin/bash
# Source this file to set up the Diablo II Portable development environment

# Add any custom environment variables here
export D2PORTABLE_ROOT="$(pwd)"

# Android SDK/NDK paths (update these to match your system)
# export ANDROID_HOME=/path/to/android-sdk
# export ANDROID_NDK=/path/to/android-ndk

# Add tools to PATH
export PATH="$D2PORTABLE_ROOT/tools:$PATH"

echo "Diablo II Portable development environment loaded"
echo "Project root: $D2PORTABLE_ROOT"
EOF

chmod +x "$ENV_SCRIPT"
print_status "Created $ENV_SCRIPT"

# Summary
echo
echo "=== Setup Summary ==="
echo
print_status "Development environment check complete!"
echo
echo "Next steps:"
echo "1. Set up Android SDK/NDK environment variables if not already done"
echo "2. Source the environment setup script: source $ENV_SCRIPT"
echo "3. Build the project: cmake -B build && cmake --build build"
echo "4. Run tests: cd build && ctest"
echo
echo "For Android builds, see the documentation in Docs/"

# Check if we can do a test build
if [ -n "$ANDROID_NDK" ] || [ -n "$ANDROID_NDK_HOME" ]; then
    echo
    read -p "Would you like to configure a test build? (y/N) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        NDK_PATH="${ANDROID_NDK:-$ANDROID_NDK_HOME}"
        cmake -B build \
            -DCMAKE_TOOLCHAIN_FILE="$NDK_PATH/build/cmake/android.toolchain.cmake" \
            -DANDROID_ABI=arm64-v8a \
            -DANDROID_PLATFORM=android-26 \
            -DBUILD_TESTS=ON
        
        if [ $? -eq 0 ]; then
            print_status "CMake configuration successful!"
            echo "You can now build with: cmake --build build"
        else
            print_error "CMake configuration failed"
        fi
    fi
fi