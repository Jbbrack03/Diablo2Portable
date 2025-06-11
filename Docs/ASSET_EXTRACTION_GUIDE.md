# Diablo II Asset Extraction Guide

## Overview
This guide explains how to extract game assets from the Diablo II files in the vendor folder for use in the Android port.

## Available Source Files

### CD Images
- `Diablo II install.iso` - Base game installation files
- `Diablo II play.iso` - Game data files
- `Diablo II Cinematics disc-.iso` - Video cutscenes
- `Diablo 2 Lords of Destruction.iso` - Expansion pack files

### Installers and Patches
- `Downloader_Diablo2_enUS.exe` - Digital installer for base game
- `Downloader_Diablo2_Lord_of_Destruction_enUS.exe` - Digital installer for expansion
- `LODPatch_114d.exe` - Latest official patch (1.14d)

## MPQ Archives Overview

Diablo II stores its game data in MPQ (Mo'PaQ) archives. These are Blizzard's proprietary archive format containing:
- Sprites and animations (DC6/DCC formats)
- Audio files (WAV format)
- Game data tables (TXT/BIN formats)
- Maps and levels (DS1 format)
- Videos (SMK/BIK formats)

### Core MPQ Files
After installation, the main MPQ files are:
- `d2data.mpq` - Base game data
- `d2exp.mpq` - Expansion data
- `d2sfx.mpq` - Sound effects
- `d2speech.mpq` - Voice acting
- `d2video.mpq` - Cinematics
- `patch_d2.mpq` - Latest patch data (overrides base files)

## Extraction Process

### Step 1: Install the Game
First, we need to install the game to access the MPQ files:

```bash
# Create a temporary Windows environment or use Wine
# Mount the ISO files
mkdir -p /tmp/d2_mount
sudo mount -o loop "Diablo II install.iso" /tmp/d2_mount

# Run the installer (requires Wine on Linux/Mac)
wine /tmp/d2_mount/setup.exe

# Install to a known directory
# Default: C:\Program Files\Diablo II
```

### Step 2: Apply Patches
```bash
# Run the patch installer
wine LODPatch_114d.exe

# This updates the game to version 1.14d
# Creates/updates patch_d2.mpq
```

### Step 3: Extract MPQ Contents
Use an MPQ extraction tool:

#### Option A: StormLib (Recommended)
```bash
# Clone and build StormLib
git clone https://github.com/ladislav-zezula/StormLib.git
cd StormLib
cmake .
make

# Use the extraction tool
./storm_extract /path/to/d2data.mpq /output/directory
```

#### Option B: MPQEditor
- Download MPQEditor for Windows
- Open each MPQ file
- Extract contents to organized directories

### Step 4: Organize Extracted Assets

Recommended directory structure:
```
extracted_assets/
├── sprites/
│   ├── characters/
│   ├── monsters/
│   ├── items/
│   └── ui/
├── audio/
│   ├── sfx/
│   ├── music/
│   └── speech/
├── data/
│   ├── global/
│   │   ├── excel/     # Game data tables
│   │   └── ui/        # UI layouts
│   └── local/         # Localized text
├── maps/
└── video/
```

## Asset Types and Locations

### Sprites and Graphics
- **Location**: `data\global\`
- **Formats**: 
  - DC6 - Uncompressed sprites
  - DCC - Compressed sprites
  - COF - Component Object Files (animation data)

### User Interface
- **Location**: `data\global\ui\`
- **Key files**:
  - Panel backgrounds
  - Button graphics
  - Cursor sprites
  - Font files

### Audio Files
- **Location**: `data\global\sfx\`, `data\global\music\`
- **Format**: WAV (22kHz, 16-bit)

### Game Data
- **Location**: `data\global\excel\`
- **Format**: Tab-delimited text files
- **Key files**:
  - `ItemTypes.txt` - Item definitions
  - `Skills.txt` - Skill data
  - `MonStats.txt` - Monster statistics

## Important Files for Android Port

### Essential Graphics
```
data\global\ui\panel\*.dc6          # UI panels
data\global\ui\cursor\*.dc6         # Cursor graphics
data\global\items\*.dc6             # Item sprites
data\global\chars\*.dcc             # Character animations
data\global\monsters\*.dcc          # Monster sprites
```

### Essential Data
```
data\global\excel\*.txt             # All game data tables
data\local\lng\eng\*.tbl            # English text strings
```

### Essential Audio
```
data\global\sfx\cursor\*.wav        # UI sounds
data\global\sfx\item\*.wav          # Item sounds
data\global\music\*.wav             # Background music
```

## Conversion Requirements

### Sprite Conversion
1. DC6/DCC → PNG sprite sheets
2. Palette application (PAL files)
3. Animation frame metadata extraction

### Audio Conversion
1. WAV → OGG/MP3 for size optimization
2. Maintain original sample rates
3. Implement streaming for music

### Data Conversion
1. Parse TXT files into efficient binary format
2. Create indexed lookups for performance
3. Validate data integrity

## Legal Considerations

### User Responsibilities
- Users must own legitimate copies of Diablo II
- Do not distribute extracted assets
- Assets remain property of Blizzard Entertainment

### Technical Implementation
- Implement asset verification system
- Check for required files before running
- Provide clear error messages for missing assets

## Extraction Script Template

```python
#!/usr/bin/env python3
import os
import subprocess

def extract_mpq(mpq_path, output_dir):
    """Extract MPQ archive contents"""
    os.makedirs(output_dir, exist_ok=True)
    
    # Use StormLib or similar tool
    cmd = ['storm_extract', mpq_path, output_dir]
    subprocess.run(cmd, check=True)
    
def organize_assets(raw_dir, organized_dir):
    """Organize extracted files by type"""
    # Implementation for organizing files
    pass

def verify_extraction(asset_dir):
    """Verify all required files are present"""
    required_files = [
        'data/global/ui/panel/invchar6.dc6',
        'data/global/excel/skills.txt',
        # ... more required files
    ]
    
    missing = []
    for file in required_files:
        if not os.path.exists(os.path.join(asset_dir, file)):
            missing.append(file)
    
    return missing

if __name__ == '__main__':
    # Extract each MPQ
    mpqs = ['d2data.mpq', 'd2exp.mpq', 'patch_d2.mpq']
    for mpq in mpqs:
        extract_mpq(mpq, f'raw_extract/{mpq}')
    
    # Organize and verify
    organize_assets('raw_extract', 'game_assets')
    missing = verify_extraction('game_assets')
    
    if missing:
        print(f"Missing files: {missing}")
    else:
        print("Extraction complete!")
```

## Next Steps

1. Create automated extraction tool
2. Implement asset conversion pipeline
3. Design asset caching system for Android
4. Create asset validation system

This extraction process provides all necessary game assets while respecting intellectual property rights and ensuring users provide their own game files.