# Diablo II Android Port - Technical Implementation Guide

## Architecture Overview

### Technology Stack
- **Language**: C++ (core engine) with Java/Kotlin (Android framework)
- **Graphics**: OpenGL ES 3.0
- **Build System**: CMake with Android NDK
- **Asset Management**: Custom MPQ loader
- **Audio**: OpenSL ES or Oboe library
- **Networking**: POSIX sockets for LAN play

## Project Structure

```
Diablo2Portable/
├── android/                    # Android Studio project
│   ├── app/
│   │   ├── src/
│   │   │   ├── main/
│   │   │   │   ├── java/       # Android UI and framework
│   │   │   │   ├── cpp/        # Native game engine
│   │   │   │   └── assets/     # Extracted game assets
│   │   └── build.gradle
├── engine/                     # Core game engine
│   ├── src/
│   │   ├── core/              # Game logic
│   │   ├── rendering/         # OpenGL ES renderer
│   │   ├── input/             # Controller handling
│   │   ├── audio/             # Sound system
│   │   ├── networking/        # LAN multiplayer
│   │   └── utils/             # MPQ loader, etc.
│   └── CMakeLists.txt
├── tools/                      # Development tools
│   ├── mpq_extractor/         # Asset extraction
│   ├── sprite_converter/      # Sprite optimization
│   └── ui_editor/             # UI asset modification
└── docs/                       # Documentation
```

## Core Components Implementation

### 1. MPQ File Handling

```cpp
class MPQArchive {
private:
    std::string filepath;
    std::vector<MPQFileEntry> fileTable;
    
public:
    bool open(const std::string& path);
    std::vector<uint8_t> extractFile(const std::string& filename);
    std::vector<std::string> listFiles();
};

class AssetManager {
private:
    std::unordered_map<std::string, MPQArchive> archives;
    std::string cacheDirectory;
    
public:
    void loadGameFiles(const std::string& gameDir);
    std::shared_ptr<Texture> loadSprite(const std::string& path);
    std::shared_ptr<AudioClip> loadSound(const std::string& path);
};
```

### 2. Rendering System

```cpp
class Renderer {
private:
    GLuint shaderProgram;
    GLuint vertexBuffer;
    glm::mat4 projectionMatrix;
    
public:
    void initialize(int width, int height);
    void beginFrame();
    void drawSprite(const Sprite& sprite, glm::vec2 position);
    void drawUI(const UIElement& element);
    void endFrame();
};

class Sprite {
private:
    GLuint textureId;
    std::vector<Frame> frames;
    
public:
    void load(const std::vector<uint8_t>& data);
    void draw(const glm::vec2& position, int frameIndex);
};
```

### 3. Input System

```cpp
class InputManager {
private:
    std::unordered_map<int, ButtonState> buttonStates;
    glm::vec2 leftStick;
    glm::vec2 rightStick;
    
public:
    void processGamepadInput(const AInputEvent* event);
    bool isButtonPressed(GameButton button);
    glm::vec2 getMovementVector();
    glm::vec2 getAimVector();
};

enum class GameButton {
    ACTION_PRIMARY,      // A button - primary action
    ACTION_SECONDARY,    // B button - secondary action
    INVENTORY,          // X button - open inventory
    SKILLS,             // Y button - open skills
    POTION_HEALTH,      // L1 - use health potion
    POTION_MANA,        // R1 - use mana potion
    SKILL_1,            // L2 + face button combinations
    SKILL_2,            // R2 + face button combinations
    // ... more mappings
};
```

### 4. Game State Management

```cpp
class GameEngine {
private:
    std::unique_ptr<World> world;
    std::unique_ptr<Player> player;
    std::unique_ptr<UIManager> ui;
    GameState currentState;
    
public:
    void initialize();
    void update(float deltaTime);
    void render(Renderer& renderer);
    void handleInput(const InputManager& input);
};

class World {
private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::unique_ptr<Map> currentMap;
    
public:
    void loadMap(const std::string& mapName);
    void spawnMonster(MonsterType type, glm::vec2 position);
    void update(float deltaTime);
};
```

## Controller Mapping Design

### Movement System
- **Left Stick**: Character movement (8-directional)
- **Right Stick**: Cursor control for targeting
- **L3**: Toggle run/walk
- **R3**: Center camera on player

### Combat Controls
- **A Button**: Primary attack/interact
- **B Button**: Secondary skill/cancel
- **X Button**: Pick up items/open containers
- **Y Button**: Toggle skill bar

### Quick Actions
- **L1**: Use health potion
- **R1**: Use mana potion
- **L2 + Face Buttons**: Skills 1-4
- **R2 + Face Buttons**: Skills 5-8

### Menu Navigation
- **D-Pad**: Navigate menus/inventory
- **Start**: Game menu
- **Select**: Map/Quest log

## UI Modifications

### Inventory System
```cpp
class ControllerInventory : public UIPanel {
private:
    int selectedSlotX, selectedSlotY;
    
public:
    void navigateGrid(int dx, int dy);
    void selectItem();
    void equipItem();
    void dropItem();
    void render(Renderer& renderer) override;
};
```

### Skill Tree Navigation
```cpp
class ControllerSkillTree : public UIPanel {
private:
    SkillNode* selectedNode;
    
public:
    void navigateToNextSkill(Direction dir);
    void allocateSkillPoint();
    void assignToQuickSlot(int slot);
};
```

## Asset Pipeline

### Sprite Conversion Process
1. Extract DC6/DCC files from MPQ archives
2. Convert palettized sprites to RGBA
3. Pack into texture atlases for GPU efficiency
4. Generate metadata for animation frames

### UI Asset Modification
1. Extract original UI elements
2. Redesign for widescreen layout
3. Add controller button prompts
4. Create new skill/inventory layouts

## Performance Optimization

### Memory Management
- Asset streaming based on game area
- Texture compression (ETC2/ASTC)
- Object pooling for entities
- Efficient sprite batching

### Rendering Optimization
- Frustum culling
- Sprite instancing
- Reduced overdraw
- Frame rate limiting for battery life

## Networking Implementation

### LAN Discovery
```cpp
class LANGameFinder {
private:
    int broadcastSocket;
    std::vector<GameSession> availableGames;
    
public:
    void startDiscovery();
    void broadcastGame(const GameInfo& info);
    std::vector<GameSession> getAvailableGames();
};
```

### Game Protocol
- TCP for reliable game state updates
- UDP for position updates
- Compatible with original D2 protocol
- Host migration support

## Build Configuration

### Android Studio Setup
```gradle
android {
    compileSdk 33
    
    defaultConfig {
        applicationId "com.personal.diablo2portable"
        minSdk 24
        targetSdk 33
        
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++17"
                arguments "-DANDROID_STL=c++_shared"
            }
        }
    }
    
    externalNativeBuild {
        cmake {
            path "engine/CMakeLists.txt"
        }
    }
}
```

### CMake Configuration
```cmake
cmake_minimum_required(VERSION 3.18)
project(Diablo2Engine)

set(CMAKE_CXX_STANDARD 17)

# Find Android libraries
find_library(OPENGL_LIB GLESv3)
find_library(ANDROID_LIB android)
find_library(LOG_LIB log)

# Add engine sources
add_library(diablo2engine SHARED
    src/core/GameEngine.cpp
    src/rendering/Renderer.cpp
    src/input/InputManager.cpp
    # ... more sources
)

target_link_libraries(diablo2engine
    ${OPENGL_LIB}
    ${ANDROID_LIB}
    ${LOG_LIB}
)
```

## Testing Strategy

### Unit Tests
- Game logic verification
- Save/load integrity
- Network protocol compliance

### Integration Tests
- Controller input mapping
- Asset loading pipeline
- Rendering performance

### Device Testing
- Retroid Pocket Flip 2 compatibility
- Performance benchmarking
- Battery life measurement

## Development Tools

### MPQ Extractor Tool
```bash
./mpq_extractor --input /path/to/d2data.mpq --output ./extracted_assets
```

### Asset Converter
```bash
./sprite_converter --input ./extracted_assets --output ./android/app/src/main/assets
```

### Debug Console
- In-game performance metrics
- Asset loading visualization
- Network traffic monitor

## Security Considerations

- No hardcoded game assets
- User must provide legitimate game files
- No circumvention of copy protection
- Secure local save file storage

This implementation guide provides the technical foundation for building the Diablo II Android port while respecting intellectual property and focusing on technical excellence.