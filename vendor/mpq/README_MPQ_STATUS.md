# MPQ File Status in Vendor Directory

This document clarifies the status of MPQ files in the `vendor/mpq/` directory.

## ⚠️ Important Notice

Due to copyright restrictions, this repository cannot include authentic Diablo II game files. Some MPQ files in this directory are placeholders to allow the build and test systems to function.

## File Status

### 🚫 Placeholder Files (1MB, filled with 'X' characters)
These files are NOT authentic Diablo II files and cannot be used for actual gameplay:
- `d2data.mpq` - Main game data (placeholder)
- `d2exp.mpq` - Expansion data (placeholder)  
- `d2sfx.mpq` - Sound effects (placeholder)

### ✅ Authentic Files
These files contain actual game data:
- `d2char.mpq` - Character graphics (263 MB)
- `d2music.mpq` - Background music (345 MB)
- `d2speech.mpq` - Character speech (163 MB)
- `d2video.mpq` - Cinematics (441 MB)
- `d2xmusic.mpq` - Expansion music (54 MB)
- `d2xtalk.mpq` - Expansion speech (60 MB)
- `d2xvideo.mpq` - Expansion cinematics (110 MB)
- `playd2.mpq` - Play disc data (659 KB)
- `setup.mpq` - Setup resources (4.5 MB)

## How to Obtain Authentic Files

To use the full functionality of this project, you need to provide your own legally-owned Diablo II game files:

1. **From Original CDs/DVDs**: Copy the MPQ files from your Diablo II installation discs
2. **From Digital Purchase**: Extract from your Battle.net or other legitimate digital purchase
3. **From Existing Installation**: Copy from `C:\Program Files (x86)\Diablo II\` or equivalent

## Integration Test Behavior

The test suite automatically detects placeholder files and will:
- Skip tests that require authentic MPQ data
- Display clear messages explaining why tests were skipped
- Continue running other tests that don't require MPQ files

Example skip message:
```
MPQ file is a placeholder. File is a placeholder (filled with 'X' characters)
```

## MPQ Validation

The project includes an `MPQValidator` utility that can:
- Detect placeholder files (filled with 'X' characters)
- Validate authentic MPQ files (check for proper MPQ header)
- Identify corrupted or invalid files

To validate your MPQ files programmatically:
```cpp
#include "utils/mpq_validator.h"

auto result = MPQValidator::validateMPQFile("path/to/file.mpq");
if (result.isPlaceholder) {
    // Handle placeholder file
} else if (result.isValid) {
    // File is a valid MPQ
}
```

## Asset Extraction Limitations

Without authentic MPQ files, the following features will not work:
- Character sprite loading
- Game data table extraction
- Sound effect playback
- Map file loading
- Item graphics display

The onboarding system (Phase 21-24) was designed to help users extract these files from their legitimate game copies.

## Development Impact

Developers can still:
- Build the project successfully
- Run unit tests that don't require MPQ data
- Test UI components and game logic
- Develop new features

However, integration testing and full gameplay require authentic game files.

## Legal Notice

This project does not and will not distribute copyrighted Diablo II game content. Users must provide their own legally-obtained game files. The placeholder files exist solely to maintain build system compatibility.