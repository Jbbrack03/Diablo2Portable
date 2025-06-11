# Technical Standards - Diablo II Android Port

## Overview
This document establishes the standardized technical specifications for the Diablo II Android port project, resolving inconsistencies across documentation.

## Standardized Technology Stack

### Programming Languages
- **Core Engine**: C++17
- **Android Framework**: Kotlin (preferred) with Java interop
- **Build Scripts**: Gradle (Kotlin DSL preferred)
- **Tools/Scripts**: Python 3.8+

### Android Specifications
- **Minimum SDK**: API 26 (Android 8.0 Oreo)
- **Target SDK**: API 33 (Android 13)
- **Compile SDK**: API 33
- **NDK Version**: r25c (25.2.9519653)
- **CMake Version**: 3.22.1
- **Gradle Version**: 7.5.1
- **Android Gradle Plugin**: 7.4.2

### Graphics & Audio
- **Graphics API**: OpenGL ES 3.0 (not 3.2)
- **Audio API**: Oboe library (preferred over OpenSL ES)
  - Provides lower latency
  - Better device compatibility
  - Automatic fallback to OpenSL ES

### Libraries & Dependencies
```gradle
dependencies {
    // Kotlin
    implementation "org.jetbrains.kotlin:kotlin-stdlib:1.8.20"
    
    // Android
    implementation 'androidx.core:core-ktx:1.10.1'
    implementation 'androidx.appcompat:appcompat:1.6.1'
    
    // Audio
    implementation 'com.google.oboe:oboe:1.7.0'
    
    // Testing
    testImplementation 'junit:junit:4.13.2'
    testImplementation 'com.google.truth:truth:1.1.3'
    androidTestImplementation 'androidx.test.ext:junit:1.1.5'
    androidTestImplementation 'androidx.test.espresso:espresso-core:3.5.1'
}
```

### C++ Standards
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
```

## Project Timeline

### Official Timeline: 24 Weeks
- **Phase 0**: Environment Setup (Week 1)
- **Phase 1**: Core Engine (Weeks 2-4)
- **Phase 2**: Rendering (Weeks 5-7)
- **Phase 3**: Input System (Weeks 8-9)
- **Phase 4**: Game Logic (Weeks 10-14)
- **Phase 5**: World & AI (Weeks 15-17)
- **Phase 6**: Audio & Network (Weeks 18-20)
- **Phase 7**: UI (Weeks 21-22)
- **Phase 8**: Polish (Weeks 23-24)

## File Organization

### Standard Project Structure
```
Diablo2Portable/
├── android/                 # Android Studio project
│   ├── app/
│   │   ├── src/
│   │   │   ├── main/
│   │   │   │   ├── kotlin/  # Kotlin sources
│   │   │   │   ├── java/    # Java sources (if needed)
│   │   │   │   ├── cpp/     # JNI bridge code
│   │   │   │   ├── res/     # Android resources
│   │   │   │   └── assets/  # Game assets (extracted)
│   │   │   └── test/        # Unit tests
│   │   └── build.gradle.kts
│   └── settings.gradle.kts
├── engine/                  # Core C++ engine
│   ├── include/            # Public headers
│   ├── src/                # Implementation
│   ├── tests/              # Engine tests
│   └── CMakeLists.txt
├── tools/                  # Development tools
│   ├── asset_extractor/
│   ├── sprite_converter/
│   └── build_scripts/
├── vendor/                 # Original game files
│   └── (CD images, installers)
├── docs/                   # Documentation
└── README.md
```

## Performance Targets

### Consolidated Requirements
- **Frame Rate**: 60 FPS sustained (minimum 55 FPS)
- **Resolution**: 1080p native rendering
- **Load Time**: < 5 seconds from launch to menu
- **Memory Usage**: < 1.5GB total (1.2GB typical)
- **Battery Life**: 4+ hours continuous play
- **Audio Latency**: < 30ms
- **Network Latency**: < 100ms LAN

## Version Control Strategy

### Git Workflow
```bash
# Branch naming
main                    # Stable releases only
develop                 # Integration branch
feature/feature-name    # New features
bugfix/issue-number    # Bug fixes
release/version        # Release preparation

# Commit format
type(scope): subject

# Types: feat, fix, docs, style, refactor, test, chore
# Example: feat(combat): add critical strike calculation
```

### Version Numbering
```
MAJOR.MINOR.PATCH-STAGE

1.0.0-alpha   # Feature incomplete
1.0.0-beta    # Feature complete, testing
1.0.0-rc.1    # Release candidate
1.0.0         # Stable release
```

## Code Style Guidelines

### C++ Style
```cpp
// File naming: snake_case.cpp, snake_case.h
// Class naming: PascalCase
// Function naming: camelCase
// Constants: UPPER_SNAKE_CASE
// Member variables: m_camelCase

class CharacterStats {
public:
    static constexpr int MAX_LEVEL = 99;
    
    void updateHealth(int amount);
    int getMaxHealth() const { return m_maxHealth; }
    
private:
    int m_currentHealth;
    int m_maxHealth;
};
```

### Kotlin Style
```kotlin
// Follow official Kotlin style guide
class InventoryManager {
    companion object {
        const val MAX_ITEMS = 40
    }
    
    fun addItem(item: Item): Boolean {
        return inventory.add(item)
    }
    
    private val inventory = mutableListOf<Item>()
}
```

## Testing Standards

### Coverage Requirements
- **Core Systems**: 95% minimum
- **Game Logic**: 90% minimum
- **UI Code**: 85% minimum
- **Overall Target**: 90%

### Test Naming Convention
```cpp
TEST(ClassName, MethodName_StateUnderTest_ExpectedBehavior) {
    // Example:
    TEST(Character, AddExperience_EnoughForLevelUp_IncreasesLevel)
}
```

## Security Guidelines

### Data Protection
- No network features beyond LAN
- No telemetry or analytics
- No cloud saves
- Local saves encrypted with device-specific key

### Code Security
```cpp
// Always validate inputs
bool loadSaveFile(const std::string& path) {
    if (!isValidPath(path)) return false;
    if (!fileExists(path)) return false;
    if (getFileSize(path) > MAX_SAVE_SIZE) return false;
    
    // Proceed with loading
}
```

## Documentation Standards

### File Headers
```cpp
/**
 * @file character_system.cpp
 * @brief Character stats and progression system
 * @date 2024-01-15
 * @author Diablo2Portable Team
 */
```

### Update Tracking
Each document should include:
```markdown
---
title: Document Title
version: 1.0.0
last_updated: 2024-01-15
---
```

## Cross-Reference Matrix

| Topic | Primary Doc | Related Docs |
|-------|------------|--------------|
| Architecture | [TECHNICAL_IMPLEMENTATION.md](TECHNICAL_IMPLEMENTATION.md) | [NATIVE_ARM_IMPLEMENTATION.md](NATIVE_ARM_IMPLEMENTATION.md) |
| Controls | [CONTROLLER_DESIGN_UNIFIED.md](CONTROLLER_DESIGN_UNIFIED.md) | [TDD_IMPLEMENTATION_PLAN.md](TDD_IMPLEMENTATION_PLAN.md) |
| Assets | [ASSET_EXTRACTION_GUIDE.md](ASSET_EXTRACTION_GUIDE.md) | [IMPLEMENTATION_GAPS_RESOLVED.md](IMPLEMENTATION_GAPS_RESOLVED.md) |
| Testing | [TESTING_PROCEDURES.md](TESTING_PROCEDURES.md) | [TDD_IMPLEMENTATION_PLAN.md](TDD_IMPLEMENTATION_PLAN.md) |
| Deployment | [DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md) | [MAC_DEVELOPMENT_SETUP.md](MAC_DEVELOPMENT_SETUP.md) |
| Troubleshooting | [TROUBLESHOOTING_GUIDE.md](TROUBLESHOOTING_GUIDE.md) | All docs |

## Migration Notes

### From Old Standards
- OpenSL ES → Oboe library
- API 24 → API 26 minimum
- Java → Kotlin preferred
- 28-week timeline → 24-week timeline

### Deprecated Documents
The following documents have been consolidated:
- `CONTROLLER_UI_DESIGN.md` → `CONTROLLER_DESIGN_UNIFIED.md`
- `CONTROL_SYSTEM_REDESIGN.md` → `CONTROLLER_DESIGN_UNIFIED.md`
- `AI_DEVELOPMENT_CAPABILITIES.md` → `DEVELOPMENT_CAPABILITIES.md`
- `NATIVE_IMPLEMENTATION_CAPABILITIES.md` → `DEVELOPMENT_CAPABILITIES.md`

## Compliance Checklist

Before committing code, ensure:
- [ ] Uses standardized technology versions
- [ ] Follows code style guidelines
- [ ] Includes appropriate tests
- [ ] Updates relevant documentation
- [ ] Cross-references related docs
- [ ] Version numbers updated

This technical standards document supersedes any conflicting information in other documentation.