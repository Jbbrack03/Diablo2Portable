# Diablo II Android Port Project Overview

## Project Goal
Port the original Diablo II (2000) game with Lords of Destruction expansion to Android, targeting the Retroid Pocket Flip 2 handheld gaming device with full controller support and modern UI improvements.

## Target Specifications
- **Platform**: Android APK (personal use, not Play Store distribution)
- **Hardware**: Retroid Pocket Flip 2 with Snapdragon processor
- **Display**: 1080p widescreen without cropping or stretching
- **Performance**: 60 FPS target
- **Controls**: Full Xbox-style controller support with modern UI
- **Multiplayer**: LAN functionality preserved for local play

## Key Requirements
1. Users must provide their own game files (we do not distribute copyrighted content)
2. Modern controller-friendly UI similar to Diablo 2 Resurrected
3. Modified UI assets integrated into the game (not an overlay)
4. Full compatibility with patch 1.14d

## Technical Architecture

### Original Diablo II Engine
- **Release**: 2000 by Blizzard North
- **Architecture**: Modular Windows application with multiple DLLs
- **Graphics**: 2D sprite-based rendering with support for multiple APIs (DirectDraw, Direct3D, Glide, GDI)
- **File Format**: MPQ archives for game assets
- **Platform**: x86 Windows binary

### Engine Components
- `D2Client.dll` - Client functionality
- `D2Game.dll` - Core game logic
- `D2Common.dll` - Shared game code
- `D2CMP.dll` - Compression handling
- `D2Net.dll` - Networking
- `D2Sound.dll` - Audio system
- Graphics renderers: `D2DDraw.dll`, `D2Direct3D.dll`, `D2Glide.dll`, `D2Gdi.dll`

## Porting Approaches

### Option 1: Native Android Port (Recommended)
Build a native Android implementation using existing open-source projects as reference:

1. **OpenDiablo2** - Go-based reimplementation
   - Cross-platform architecture
   - Clean room implementation
   - MPQ file support
   - No existing Android port

2. **Diablerie** - Unity-based reimplementation
   - Built with Unity (C#)
   - Cross-platform potential
   - Could leverage Unity's Android build support
   - No completed Android port

### Option 2: Emulation Layer
Use Wine/ExaGear approach (limited viability):
- ExaGear provided x86 emulation + Wine
- No longer available on Play Store
- Performance limitations
- Online play issues reported

### Option 3: Custom Engine Implementation
Build a new engine specifically for Android:
- Full control over implementation
- Optimized for mobile hardware
- Native controller support
- Most development effort required

## Recommended Architecture

### Core Components

1. **Game Engine Core**
   - Native Android application (C++ with JNI)
   - OpenGL ES 3.0 for rendering
   - Custom MPQ loader for asset extraction
   - Game logic reimplementation based on reverse engineering

2. **Asset Pipeline**
   - MPQ file extraction and caching
   - Sprite sheet optimization for mobile GPUs
   - Audio format conversion (to Android-supported formats)
   - UI asset modification system

3. **Input System**
   - Native gamepad API integration
   - Custom control mapping system
   - UI navigation optimized for controller
   - Touch input support (optional)

4. **Networking**
   - TCP/IP stack for LAN play
   - Compatible with original protocol
   - No Battle.net integration (LAN only)

## Development Phases

### Phase 1: Foundation (Weeks 1-4)
- Set up Android development environment
- Implement MPQ file reader
- Basic OpenGL ES renderer
- Load and display game sprites

### Phase 2: Core Systems (Weeks 5-12)
- Game state management
- Character system implementation
- Basic gameplay mechanics
- Save/load functionality

### Phase 3: Controller Integration (Weeks 13-16)
- Android gamepad API integration
- Control mapping system
- UI navigation system
- Controller-friendly menus

### Phase 4: UI Modernization (Weeks 17-20)
- Design new UI assets
- Implement UI rendering system
- Create controller-friendly inventory
- Skill tree navigation

### Phase 5: Audio & Networking (Weeks 21-24)
- Audio playback system
- Sound effect management
- LAN multiplayer protocol
- Network game discovery

### Phase 6: Optimization & Polish (Weeks 25-28)
- Performance profiling
- Memory optimization
- Bug fixes and stability
- Final testing on target hardware

## Technical Challenges

1. **x86 to ARM Translation**
   - Game logic must be reimplemented
   - Cannot directly run original binaries
   - Behavior must match original exactly

2. **Widescreen Support**
   - Original game was 4:3 aspect ratio
   - UI elements need repositioning
   - Gameplay area expansion considerations

3. **Controller Mapping**
   - Original game designed for mouse/keyboard
   - Complex inventory management
   - Skill targeting system adaptation

4. **Performance Optimization**
   - Mobile GPU limitations
   - Battery life considerations
   - Memory constraints

## Next Steps

1. Evaluate existing open-source implementations for code reuse
2. Set up Android development environment with NDK
3. Create MPQ extraction tool for game assets
4. Prototype basic renderer with sprite loading
5. Design controller mapping scheme

## Resources

- [OpenDiablo2 GitHub](https://github.com/OpenDiablo2/OpenDiablo2)
- [Diablerie GitHub](https://github.com/mofr/Diablerie)
- [MPQ Format Documentation](https://github.com/ladislav-zezula/StormLib)
- [Android Game Development Guide](https://developer.android.com/games)