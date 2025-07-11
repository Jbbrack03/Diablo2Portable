# Diablo 2 Android Asset Pipeline Analysis

## Overview
This document analyzes the complete pipeline from MPQ extraction to in-game rendering in the Diablo 2 Android port project.

## 1. Asset Extraction Phase

### Extraction Location
- **Android App**: `getFilesDir()/assets/` (e.g., `/data/data/com.diablo2portable/files/assets/`)
- **Desktop/Testing**: User-specified output directory

### Directory Structure Created by AssetExtractor
```
outputPath/
├── sprites/
│   ├── characters/
│   ├── monsters/
│   ├── items/
│   └── ui/
├── sounds/
│   ├── music/
│   ├── effects/
│   └── speech/
└── data/
    ├── excel/      (.txt files like armor.txt)
    ├── string_tables/ (.tbl files)
    └── binary/     (.bin files)
```

### Extraction Process
1. **User Initiates**: OnboardingActivity → Extract button
2. **Extraction Coordinator**: Orchestrates the extraction from ISO/MPQ sources
3. **AssetExtractor**: 
   - Extracts DC6 sprites → categorized into subdirectories
   - Extracts WAV files → categorized by type (music/effects/speech)
   - Extracts data files → categorized by format (.txt/.tbl/.bin)
4. **Output**: Files written to `getFilesDir()/assets/` with preserved directory structure

## 2. Asset Loading Phase

### AssetManager Initialization
1. **Android Path**: NativeEngine loads with path `/android_asset/` (APK assets)
2. **Extracted Assets Path**: Should be `getFilesDir()/assets/` but **GAP IDENTIFIED**

### Current AssetManager Modes
- `initialize(data_path)`: Points to a directory containing game files
- `initializeWithMPQ(mpq_path)`: Loads directly from MPQ files
- `initializeWithMPQs(mpq_directory)`: Loads from directory of MPQ files

### File Resolution
AssetManager looks for files using:
1. **MPQ Mode**: Searches inside loaded MPQ archives
2. **Fallback Path**: Secondary directory for extracted files
3. **Filesystem Mode**: Direct file access from data_path

## 3. Rendering Pipeline

### Asset to Texture Flow
1. **AssetManager::loadSprite(relative_path)**
   - Example: `"data/global/ui/panel/invchar6.dc6"`
   - Returns: `shared_ptr<DC6Sprite>`

2. **TextureManager::uploadSprite(sprite, direction, frame)**
   - Converts DC6 sprite frame to OpenGL texture
   - Returns: OpenGL texture ID

3. **SpriteRenderer::drawSprite(texture_id, position, size)**
   - Uses texture ID to render sprite with OpenGL

## 4. Identified Gaps and Issues

### GAP 1: Asset Path Mismatch
- **Extraction Output**: `getFilesDir()/assets/sprites/characters/...`
- **AssetManager Load Path**: Hardcoded to `/android_asset/` (APK assets)
- **Problem**: Extracted assets are never loaded because paths don't match

### GAP 2: Missing Path Configuration
- NativeEngine always loads from `/android_asset/`
- No mechanism to switch to extracted assets directory
- OnboardingHelper saves asset path but it's never used

### GAP 3: Asset Path Format
- AssetManager expects paths like `"data/global/ui/panel/invchar6.dc6"`
- But extraction creates `"sprites/ui/invchar6.dc6"`
- Path structure mismatch between extraction and loading

### GAP 4: Initialization Sequence
- MainActivity doesn't check for extracted assets
- NativeEngine doesn't receive the extracted assets path
- No connection between OnboardingHelper's saved path and engine initialization

## 5. Required Fixes

### Fix 1: Update MainActivity
```java
// Check for extracted assets
OnboardingHelper helper = new OnboardingHelper(this);
String assetPath = helper.getAssetPath();
if (assetPath != null && new File(assetPath).exists()) {
    nativeEngine.loadExtractedAssets(assetPath);
} else {
    nativeEngine.initialize(this);
}
```

### Fix 2: Add Native Method
```java
// In NativeEngine.java
public native boolean loadExtractedAssets(long handle, String extractedPath);
```

### Fix 3: Update AssetManager
- Add support for "extracted assets" mode
- Map game paths to extracted structure:
  - `"data/global/ui/X.dc6"` → `"sprites/ui/X.dc6"`
  - `"data/global/sfx/X.wav"` → `"sounds/effects/X.wav"`

### Fix 4: Path Mapping
Create a path resolver that maps game asset paths to extracted paths:
- Strip `"data/global/"` prefix
- Map file types to correct directories
- Handle both MPQ internal paths and extracted paths

## 6. Complete Flow (After Fixes)

1. **First Run**: OnboardingActivity extracts to `getFilesDir()/assets/`
2. **Subsequent Runs**: MainActivity checks for extracted assets
3. **Engine Init**: Pass extracted path to GameEngine
4. **AssetManager**: Initialize with extracted assets directory
5. **Loading**: AssetManager resolves game paths to extracted paths
6. **Rendering**: Load sprites → create textures → render

## Conclusion

The asset pipeline has all the necessary components but lacks the connection between extraction and usage. The main issues are:
1. Path mismatch between extraction output and loading expectations
2. Missing configuration to use extracted assets instead of APK assets
3. No path translation between game paths and extracted structure
4. Initialization sequence doesn't check for or use extracted assets

These gaps explain why the game might not find assets even after successful extraction.