# Diablo II Android Port - Project Memory

## Current Status (January 2025)
- Project: 96% complete (Phase 20 Task 1 done, Tasks 2-3 pending)
- Total Tests: 536 actual (517 C++ unit tests + 19 Android tests)
  - Documentation claims 532 but actual count is 536
  - 515 C++ tests passing, 2 failing
  - Android tests: 19 Espresso tests (not run in current session)
- Location: /Users/jbbrack03/Diablo2Portable
- GitHub: Fully synced with https://github.com/Jbbrack03/Diablo2Portable

## Recent Updates (January 2025)
- Pulled latest changes from GitHub including SaveManager simplification
- Identified test count discrepancy (documentation vs actual)
- Found MPQ file issues: d2char.mpq and d2music.mpq are 2.9KB macOS resource files, not valid MPQs
- Confirmed character graphics available in d2exp.mpq (239MB)
- Confirmed music available in d2xmusic.mpq (52MB)
- MPQ files located in vendor/mpq/ (not vendor/extracted_mpq/)

## Test Suite Status
- Main test executable: ./build/engine/tests/d2_unit_tests
- Integration tests: 11 additional test executables in build/tools/
- Test failures:
  1. MPQFileEncryptionTest.DetectEncryptedListfile
  2. AssetManagerMPQFixTest.InitializeWithUppercaseMPQExtensions
- Segmentation fault in AndroidGamepadTest when MPQ integration enabled

## MPQ Assets Status
- Total MPQ files: ~1.3GB in vendor/mpq/
- Valid MPQs: d2data.mpq (255MB), d2exp.mpq (239MB), d2sfx.mpq (47MB), etc.
- Invalid MPQs: d2char.mpq, d2music.mpq (2.9KB macOS files from 2000)
- Character graphics: Found in d2exp.mpq under data\global\CHARS\
- Music files: Found in d2xmusic.mpq under data\global\music\

## Recent TDD Implementation
- Corrected TDD violations in SaveManager implementation
- Properly followed RED-GREEN cycle with one test at a time:
  1. CreateSaveManager - empty class
  2. SaveCharacterCreatesFile - added saveCharacter
  3. SavedFileHasD2SSignature - wrote 0xAA55AA55
  4. SaveCharacterLevel - D2S header structure
  5. LoadCharacterFromFile - added loadCharacter

## Key Project Components
- Engine: C++17 with OpenGL ES 3.0
- Platform: Android (min SDK 26)
- Tests: Google Test framework
- Build: CMake + Gradle
- Features: 50+ completed systems including SaveManager
- MPQ Support: StormLib integration for asset extraction

## Phase 20 Status
- Task 1: Integration Testing ✅ COMPLETE
- Task 2: Device Testing 🔲 PENDING
- Task 3: Release Preparation 🔲 PENDING

## Important Files
- CLAUDE.md: Project guidance and status
- TDD_IMPLEMENTATION_PLAN.md: Development roadmap
- engine/: Core C++ implementation
- android/: Android app structure
- vendor/mpq/: Real D2 game files (not tracked in git)
- run_all_tests.sh: Updated to use vendor/mpq/ paths