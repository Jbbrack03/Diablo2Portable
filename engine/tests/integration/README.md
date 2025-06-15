# MPQ Integration Tests

These tests verify the MPQ loading functionality with real Diablo II game files.

## Running the Tests

1. **Extract MPQ files from your Diablo II installation**
   - You need legally owned Diablo II game files
   - Extract or copy the MPQ files (d2data.mpq, d2exp.mpq, etc.) to a directory

2. **Set environment variables**
   ```bash
   export ENABLE_MPQ_INTEGRATION_TESTS=1
   export D2_DATA_PATH=/path/to/your/d2/files/
   ```

3. **Run the tests**
   ```bash
   ./build/engine/tests/d2_unit_tests --gtest_filter="RealMPQIntegrationTest.*"
   ```

## Expected Files

The tests look for these common files in d2data.mpq:
- `data\global\ui\cursor\cursor.dc6` - Mouse cursor sprites
- `data\global\ui\fonts\font8.dc6` - Small font
- `data\global\ui\fonts\font16.dc6` - Large font  
- `data\global\excel\armor.txt` - Armor data tables
- `data\global\excel\weapons.txt` - Weapon data tables
- `data\global\excel\misc.txt` - Miscellaneous item data

## Test Coverage

The integration tests verify:
1. MPQ file loading
2. File existence checks
3. DC6 sprite extraction and parsing
4. Text file extraction
5. Compression type handling
6. Performance and caching

## Extracting MPQ Files from ISOs

If you have ISO files, you can extract MPQ files using:

### macOS:
```bash
# Mount ISO
hdiutil mount "Diablo II install.iso"

# Copy MPQ files
cp /Volumes/Diablo*/d2data.mpq ./vendor/mpq/

# Unmount
hdiutil unmount /Volumes/Diablo*
```

### Linux:
```bash
# Mount ISO
sudo mount -o loop "Diablo II install.iso" /mnt/cdrom

# Copy MPQ files
cp /mnt/cdrom/d2data.mpq ./vendor/mpq/

# Unmount
sudo umount /mnt/cdrom
```

### Windows:
- Mount the ISO by double-clicking
- Copy MPQ files from the mounted drive to vendor/mpq/