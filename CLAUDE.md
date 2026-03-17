# CLAUDE.md

This file provides guidance to Claude Code when working with this repository.

## Project Overview

Diablo II Android port targeting the Retroid Pocket Flip 2. Native ARM implementation (no emulation) with controller support and LAN multiplayer. Users must provide their own legally purchased game files.

## Architecture

### Rendering Abstraction (`IRenderBackend`)

All OpenGL calls go through `IRenderBackend` (defined in `engine/include/rendering/render_backend.h`). Two implementations:

- **`MockRenderBackend`** -- Used on desktop/tests. Tracks draw calls, validates parameters, generates random IDs. No GPU operations.
- **`GLES3RenderBackend`** -- Used on Android. Thin wrapper around real `<GLES3/gl3.h>` calls.

Access via `RenderContext::getBackend()`. Tests install MockRenderBackend in `engine/tests/main.cpp` via a global test environment. GameEngine accepts an optional `IRenderBackend*` via `initialize(assetPath, backend)`.

### Key Components

| Layer | Location | Notes |
|-------|----------|-------|
| Core Engine (C++17) | `engine/src/` | Game logic, rendering, input |
| Android Framework | `android/app/src/` | JNI bridge, lifecycle, gamepad |
| Asset Pipeline | `engine/src/tools/`, `engine/src/utils/` | MPQ extraction, sprite optimization |
| Network Protocol | `engine/src/network/d2_protocol.cpp` | Binary packet format for LAN play |
| Tests (Google Test) | `engine/tests/` | 1,216 tests, ~97% pass rate |

### Namespace Conventions

- `namespace d2` -- Most game engine components
- `namespace d2portable` -- Utility components (utils, sprites, core)

## Build Commands

```bash
# Desktop build + tests
cmake -B build && cmake --build build
./build/engine/tests/d2_unit_tests

# Android build (requires Android Studio SDK)
cd android && ./gradlew assembleDebug
```

Required SDK components: NDK 27.0.12077973, CMake 3.22.1+, SDK Platform 33.

## Technical Standards

- C++17, Android Min SDK 26, Target SDK 33
- OpenGL ES 3.0, Oboe (audio), CMake + Gradle
- Performance targets: 60 FPS, <1.5GB RAM, 4+ hours battery

## TDD Rules

1. **RED**: Write ONE failing test. See it fail.
2. **GREEN**: Write MINIMAL code to pass it.
3. **REFACTOR**: Clean up, keep tests green.
4. **COMMIT** after each green cycle.

Tests are contracts -- change implementation, not tests. No implementation without tests.

```bash
cmake --build build && ./build/engine/tests/d2_unit_tests
```

## Key Files

- `engine/include/rendering/render_backend.h` -- IRenderBackend interface
- `engine/include/rendering/render_context.h` -- Static backend accessor
- `engine/src/rendering/mock_render_backend.cpp` -- Test mock (replaces old mock_opengl.cpp)
- `engine/src/rendering/gles3_render_backend.cpp` -- Real Android GL backend
- `engine/src/android/jni_bridge.cpp` -- JNI bridge (creates real GameEngine)
- `engine/src/network/d2_protocol.cpp` -- Binary network protocol
- `engine/src/game/game_engine.cpp` -- Main engine with DI support

## Historical Context

Phase tracking and detailed status moved to `CLAUDE_ARCHIVE.md`.
