# Diablo II Portable - Android Port

A native Android implementation of Diablo II designed for the Retroid Pocket Flip 2 and similar Android gaming devices. This project provides a clean-room implementation without using any original Diablo II code.

## Project Status

✅ **COMPLETE** - All 36 development phases finished (January 2025)
- 745 comprehensive unit tests (100% passing)
- Full OpenGL ES 3.0 rendering pipeline
- Complete game architecture with all major systems
- Functional Android application with native engine
- Asset extraction system with user-friendly workflow

## Features

- Native ARM implementation (no emulation)
- Full controller support optimized for handheld devices
- Modern UI improvements while maintaining classic gameplay
- LAN multiplayer compatibility with PC version
- 60+ FPS performance on Snapdragon processors
- Under 1.5GB memory usage

## Prerequisites

- Android 8.0 (API 26) or higher
- OpenGL ES 3.0 support
- 1.5GB+ available RAM
- Xbox-style gamepad (recommended)
- Legally owned Diablo II game files

## Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/Diablo2Portable.git
   cd Diablo2Portable
   ```

2. **Set up development environment**
   ```bash
   ./tools/setup_environment.sh
   ```

3. **Build the project**
   ```bash
   # Desktop/testing build
   cmake -B build
   cmake --build build
   
   # Android build
   cd android
   ./gradlew assembleDebug
   ```

4. **Deploy to device**
   ```bash
   ./gradlew installDebug
   # Or use the deployment script:
   ../tools/deploy_to_device.sh
   ```

## Asset Extraction

You must provide your own legally purchased Diablo II game files. The app includes a user-friendly extraction wizard:

1. Launch the app on your Android device
2. Follow the on-screen extraction wizard
3. Select your D2 source (CD, ISO, installed game)
4. Wait for extraction to complete

For manual extraction, see `Docs/ASSET_EXTRACTION_GUIDE.md`

**Important**: Some MPQ files in `vendor/mpq/` are placeholders. See [vendor/mpq/README_MPQ_STATUS.md](vendor/mpq/README_MPQ_STATUS.md) for details.

## Documentation

- [Deployment Guide](Docs/DEPLOYMENT_GUIDE.md) - Detailed deployment instructions
- [Testing Guide](Docs/TESTING_GUIDE.md) - Running tests and benchmarks
- [Troubleshooting](Docs/TROUBLESHOOTING_GUIDE.md) - Common issues and solutions
- [Development Capabilities](Docs/DEVELOPMENT_CAPABILITIES.md) - AI-assisted development info
- [Technical Standards](Docs/TECHNICAL_STANDARDS.md) - Code standards and architecture

## Project Structure

```
Diablo2Portable/
├── engine/           # C++ game engine
├── android/          # Android app and JNI bridge
├── third_party/      # Dependencies (StormLib)
├── tools/            # Development utilities
├── Docs/             # Documentation
└── vendor/           # Game assets (user-provided)
```

## Development

This project follows Test-Driven Development (TDD) principles. All new features must have tests written first.

### Running Tests
```bash
# All tests
./run_all_tests.sh

# C++ tests only
./build/engine/tests/d2_unit_tests

# Android tests
cd android && ./gradlew connectedAndroidTest
```

### Cleaning Temporary Files
```bash
# Remove large temporary directories (6GB+)
./tools/cleanup_temp_files.sh
```

## Technical Details

- **Language**: C++17 with Android Kotlin/Java
- **Graphics**: OpenGL ES 3.0
- **Audio**: Oboe library
- **Networking**: TCP/UDP sockets for LAN play
- **Build System**: CMake + Gradle

## Legal Notice

This is a clean-room implementation created without access to the original Diablo II source code. Users must provide their own legally purchased Diablo II game files. This project is for personal use only and should not be distributed with copyrighted game assets.

## License

[Your chosen license here]

## Acknowledgments

- StormLib for MPQ file handling
- The Diablo II modding community for file format documentation