# Diablo II Portable Tools

This directory contains command-line tools for working with Diablo II game assets.

## Building the Tools

```bash
cmake --build build
```

The tools will be built in `build/tools/`.

## Available Tools

### mpq_extractor

Extract files from MPQ archives.

**Usage:**
```bash
./mpq_extractor <command> [options]

Commands:
  list <mpq_file>                    List all files in the MPQ
  extract <mpq_file> <file> [output] Extract a single file
  extractall <mpq_file> <output_dir> Extract all files
  info <mpq_file>                    Show MPQ information
  search <mpq_file> <pattern>        Search for files matching pattern
```

**Examples:**
```bash
# List all files in d2data.mpq
./mpq_extractor list d2data.mpq

# Extract a specific file
./mpq_extractor extract d2data.mpq "data\global\ui\cursor\cursor.dc6"

# Extract all files to a directory
./mpq_extractor extractall d2data.mpq ./extracted/

# Search for all DC6 files
./mpq_extractor search d2data.mpq "*.dc6"

# Get MPQ statistics
./mpq_extractor info d2data.mpq
```

### asset_cache_builder

Pre-process and cache game assets for faster loading.

**Usage:**
```bash
./asset_cache_builder [options] <mpq_files...> <output_dir>

Options:
  --no-optimize      Don't optimize sprites
  --generate-mipmaps Generate mipmaps for textures
  --max-size <size>  Maximum texture size (default: 2048)
```

**Examples:**
```bash
# Build cache from MPQ files
./asset_cache_builder d2data.mpq d2exp.mpq ./cache

# Build without optimization
./asset_cache_builder --no-optimize d2data.mpq ./cache
```

The cache builder:
- Extracts and processes important sprites
- Caches game data files
- Creates an optimized directory structure
- Generates a manifest file for quick lookups

### Other Development Tools

- `test_mpq_real` - Test MPQ loading with real files
- `test_compression` - Test compression algorithms
- `analyze_mpq` - Analyze MPQ file structure
- `find_extractable` - Find extractable files in MPQs
- `check_compression` - Check compression types in MPQs
- `test_dc6_extraction` - Test DC6 sprite extraction

## Working with Game Assets

### Legal Notice

These tools require legally owned Diablo II game files. Do not distribute copyrighted game assets.

### Extracting from ISOs

If you have Diablo II ISO files:

1. Mount the ISO
2. Copy MPQ files from the mounted drive
3. Use the tools on the extracted MPQ files

### Asset Types

Common file types in Diablo II:
- `.dc6` - Sprite files (characters, items, UI)
- `.dcc` - Animation files
- `.cof` - Component object files
- `.txt` - Data tables (items, monsters, skills)
- `.tbl` - String tables
- `.ds1` - Map files
- `.dt1` - Tile files

### Performance Tips

1. Use the cache builder for production builds
2. Extract only needed files rather than full archives
3. Keep extracted assets organized by type
4. Use the MPQ integration in AssetManager for dynamic loading