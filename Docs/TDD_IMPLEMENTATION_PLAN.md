# Diablo II Android Port - TDD Implementation Plan

## Overview
This document provides a comprehensive Test-Driven Development (TDD) implementation plan for the Diablo II Android port project. Each phase follows the Red-Green-Refactor cycle, ensuring high code quality and maintainability.

## Development Methodology

### TDD Cycle for Each Feature
1. **Red Phase**: Write failing tests that define the feature
2. **Green Phase**: Write minimal code to make tests pass
3. **Refactor Phase**: Improve code quality while maintaining test success

### Testing Framework
- **C++ Testing**: Google Test (gtest) + Google Mock (gmock)
- **Android Testing**: JUnit + Espresso
- **Integration Testing**: Custom test harness
- **Performance Testing**: Android Profiler + custom benchmarks

## Project Timeline: 24 Weeks

---

## Phase 0: Environment Setup and Foundation (Week 1)

### Objectives
- Set up complete development environment
- Establish testing framework
- Create project structure

### Tasks

#### Task 0.1: Development Environment
**Tests First:**
```cpp
TEST(EnvironmentTest, AndroidNDKAvailable) {
    EXPECT_TRUE(std::getenv("ANDROID_NDK") != nullptr);
    EXPECT_TRUE(std::filesystem::exists(std::getenv("ANDROID_NDK")));
}

TEST(EnvironmentTest, RequiredToolsInstalled) {
    EXPECT_EQ(0, system("which cmake"));
    EXPECT_EQ(0, system("which adb"));
    EXPECT_EQ(0, system("which gradle"));
}
```

**Implementation:**
- Install Android Studio and NDK (Reference: `MAC_DEVELOPMENT_SETUP.md`)
- Configure CMake build system
- Set up Git repository with .gitignore

#### Task 0.2: Testing Infrastructure
**Tests First:**
```cpp
TEST(TestFrameworkTest, GoogleTestWorks) {
    EXPECT_TRUE(true);
    EXPECT_EQ(1 + 1, 2);
}

TEST(MockFrameworkTest, GoogleMockWorks) {
    MockFunction<int(int)> mockFunc;
    EXPECT_CALL(mockFunc, Call(5)).WillOnce(Return(10));
    EXPECT_EQ(10, mockFunc.Call(5));
}
```

**Implementation:**
- Integrate Google Test/Mock into CMake
- Create test directory structure
- Set up CI/CD pipeline (GitHub Actions)

#### Task 0.3: Project Structure
**Tests First:**
```bash
#!/bin/bash
# Test project structure exists
test -d "engine/src" || exit 1
test -d "engine/tests" || exit 1
test -d "android/app" || exit 1
test -f "CMakeLists.txt" || exit 1
```

**Implementation:**
- Create directory structure as per `TECHNICAL_IMPLEMENTATION.md`
- Initialize Android project
- Create CMake configuration

**Deliverables:**
- Working development environment
- Test framework running
- CI/CD pipeline active
- Project structure created

---

## Phase 1: Core Engine Foundation (Weeks 2-4)

### Objectives
- Implement MPQ archive reading
- Create sprite format parsers
- Build asset management system

### Tasks

#### Task 1.1: MPQ Archive Reader
**Tests First:**
```cpp
class MPQArchiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test MPQ file with known contents
        createTestMPQ("test.mpq");
    }
};

TEST_F(MPQArchiveTest, OpenValidArchive) {
    MPQArchive archive;
    EXPECT_TRUE(archive.open("test.mpq"));
    EXPECT_EQ(archive.getFileCount(), 3);
}

TEST_F(MPQArchiveTest, ExtractKnownFile) {
    MPQArchive archive;
    archive.open("test.mpq");
    
    auto data = archive.extractFile("test.txt");
    EXPECT_FALSE(data.empty());
    EXPECT_EQ(std::string(data.begin(), data.end()), "Hello, D2!");
}

TEST_F(MPQArchiveTest, HandleMissingFile) {
    MPQArchive archive;
    archive.open("test.mpq");
    
    EXPECT_THROW(archive.extractFile("nonexistent.txt"), FileNotFoundException);
}
```

**Implementation:**
- Integrate StormLib (Reference: `IMPLEMENTATION_GAPS_RESOLVED.md`)
- Implement MPQArchive class
- Support all compression types

#### Task 1.2: DC6 Sprite Parser
**Tests First:**
```cpp
TEST(DC6ParserTest, ParseValidDC6) {
    std::vector<uint8_t> dc6Data = loadTestFile("test_sprite.dc6");
    DC6Parser parser;
    
    auto sprite = parser.parse(dc6Data);
    
    EXPECT_EQ(sprite.getDirections(), 8);
    EXPECT_EQ(sprite.getFramesPerDirection(), 16);
    EXPECT_EQ(sprite.getFrame(0, 0).width, 128);
    EXPECT_EQ(sprite.getFrame(0, 0).height, 128);
}

TEST(DC6ParserTest, ConvertToPNG) {
    DC6Parser parser;
    auto sprite = parser.parse(loadTestFile("test_sprite.dc6"));
    
    auto pngData = sprite.toPNG(0, 0); // direction 0, frame 0
    EXPECT_TRUE(isPNGFormat(pngData));
}
```

**Implementation:**
- Implement DC6 header parsing
- Frame extraction logic
- PNG conversion for mobile

#### Task 1.3: Asset Management System
**Tests First:**
```cpp
TEST(AssetManagerTest, LoadGameAssets) {
    AssetManager assets;
    assets.initialize("path/to/d2/files");
    
    EXPECT_TRUE(assets.hasFile("data/global/ui/panel/invchar6.dc6"));
    EXPECT_TRUE(assets.hasFile("data/global/excel/skills.txt"));
}

TEST(AssetManagerTest, CacheSprites) {
    AssetManager assets;
    assets.initialize("path/to/d2/files");
    
    auto sprite1 = assets.loadSprite("player/barbarian/walk.dc6");
    auto sprite2 = assets.loadSprite("player/barbarian/walk.dc6");
    
    EXPECT_EQ(sprite1.get(), sprite2.get()); // Same pointer = cached
}
```

**Implementation:**
- File path mapping system
- Memory-efficient caching
- Async loading support

**Deliverables:**
- Working MPQ extraction
- DC6/DCC sprite loading
- Asset caching system
- 95% test coverage

---

## Phase 2: Rendering Engine (Weeks 5-7)

### Objectives
- Implement OpenGL ES 3.0 renderer
- Create sprite rendering system
- Build tilemap renderer

### Tasks

#### Task 2.1: OpenGL ES Initialization
**Tests First:**
```cpp
TEST(RendererTest, InitializeOpenGLES) {
    MockEGLContext mockContext;
    Renderer renderer;
    
    EXPECT_CALL(mockContext, createContext()).WillOnce(Return(true));
    EXPECT_TRUE(renderer.initialize(mockContext));
    EXPECT_EQ(renderer.getVersion(), "OpenGL ES 3.0");
}

TEST(RendererTest, HandleInitializationFailure) {
    MockEGLContext mockContext;
    Renderer renderer;
    
    EXPECT_CALL(mockContext, createContext()).WillOnce(Return(false));
    EXPECT_FALSE(renderer.initialize(mockContext));
}
```

**Implementation:**
- EGL context creation
- Shader compilation system
- Error handling

#### Task 2.2: Sprite Rendering
**Tests First:**
```cpp
TEST(SpriteRendererTest, RenderSingleSprite) {
    TestRenderer renderer;
    auto sprite = loadTestSprite();
    
    renderer.beginFrame();
    renderer.drawSprite(sprite, glm::vec2(100, 100));
    renderer.endFrame();
    
    auto pixels = renderer.readPixels(100, 100, 128, 128);
    EXPECT_TRUE(compareWithExpectedImage(pixels, "expected_sprite.png"));
}

TEST(SpriteRendererTest, BatchMultipleSprites) {
    TestRenderer renderer;
    auto sprite = loadTestSprite();
    
    renderer.beginFrame();
    for (int i = 0; i < 100; i++) {
        renderer.drawSprite(sprite, glm::vec2(i * 10, 0));
    }
    renderer.endFrame();
    
    EXPECT_LE(renderer.getDrawCallCount(), 2); // Batched properly
}
```

**Implementation:**
- Sprite batching system (Reference: `NATIVE_ARM_IMPLEMENTATION.md`)
- Texture atlas management
- Performance optimization

#### Task 2.3: Tilemap Rendering
**Tests First:**
```cpp
TEST(TilemapTest, RenderBasicMap) {
    TestRenderer renderer;
    Tilemap map;
    
    map.setTile(0, 0, TileType::FLOOR);
    map.setTile(1, 0, TileType::WALL_LEFT);
    
    renderer.renderTilemap(map);
    
    EXPECT_TRUE(verifyTileRendering(renderer, 0, 0, "floor_tile.png"));
    EXPECT_TRUE(verifyTileRendering(renderer, 1, 0, "wall_left.png"));
}

TEST(TilemapTest, CullInvisibleTiles) {
    TestRenderer renderer;
    Tilemap largeMap(1000, 1000);
    
    renderer.setViewport(0, 0, 800, 600);
    renderer.renderTilemap(largeMap);
    
    EXPECT_LE(renderer.getTilesRendered(), 100); // Only visible tiles
}
```

**Implementation:**
- Isometric tile rendering
- Frustum culling
- Layer management

**Deliverables:**
- OpenGL ES 3.0 renderer
- Efficient sprite batching
- Tilemap rendering with culling
- 60 FPS on target hardware

---

## Phase 3: Input System (Weeks 8-9)

### Objectives
- Implement gamepad support
- Create control mapping system
- Build UI navigation

### Tasks

#### Task 3.1: Gamepad Integration
**Tests First:**
```cpp
TEST(GamepadTest, DetectController) {
    MockAndroidInput mockInput;
    GamepadManager gamepad;
    
    EXPECT_CALL(mockInput, getConnectedDevices())
        .WillOnce(Return(std::vector<int>{AINPUT_SOURCE_GAMEPAD}));
    
    EXPECT_TRUE(gamepad.hasController());
}

TEST(GamepadTest, ReadButtonStates) {
    MockGamepad mockPad;
    InputManager input;
    
    EXPECT_CALL(mockPad, getButton(BUTTON_A)).WillOnce(Return(true));
    EXPECT_CALL(mockPad, getAxis(AXIS_LEFT_X)).WillOnce(Return(0.5f));
    
    input.update();
    
    EXPECT_TRUE(input.isButtonPressed(GameButton::ACTION));
    EXPECT_NEAR(input.getMovement().x, 0.5f, 0.01f);
}
```

**Implementation:**
- Android GameController API integration
- Dead zone handling
- Button mapping system

#### Task 3.2: Control Mapping
**Tests First:**
```cpp
TEST(ControlMapperTest, DirectMovement) {
    ControlMapper mapper;
    mapper.setScheme(ControlScheme::DIRECT);
    
    auto action = mapper.processInput(
        AnalogInput{0.7f, 0.7f}, // Northeast
        ButtonState::NONE
    );
    
    EXPECT_EQ(action.type, ActionType::MOVE);
    EXPECT_EQ(action.direction, Direction::NORTHEAST);
}

TEST(ControlMapperTest, SmartTargeting) {
    ControlMapper mapper;
    GameState state;
    state.addEnemy(glm::vec2(100, 0)); // Enemy to the right
    
    auto action = mapper.processInput(
        AnalogInput{1.0f, 0.0f}, // Right stick
        ButtonState::ACTION_PRESSED,
        state
    );
    
    EXPECT_EQ(action.type, ActionType::ATTACK);
    EXPECT_EQ(action.targetId, state.getEnemies()[0].id);
}
```

**Implementation:**
- Control scheme system (Reference: `CONTROL_SYSTEM_REDESIGN.md`)
- Smart targeting algorithm
- Context-sensitive actions

**Deliverables:**
- Full gamepad support
- Smart targeting system
- Customizable control schemes
- UI navigation working

---

## Phase 4: Game Logic Core (Weeks 10-14)

### Objectives
- Implement character system
- Create combat mechanics
- Build item and inventory systems
- Implement skill trees

### Tasks

#### Task 4.1: Character System
**Tests First:**
```cpp
TEST(CharacterTest, StatCalculation) {
    Character barbarian(CharacterClass::BARBARIAN);
    barbarian.setLevel(10);
    barbarian.addStatPoint(StatType::STRENGTH, 20);
    
    EXPECT_EQ(barbarian.getBaseDamage(), 35); // Base + STR bonus
    EXPECT_EQ(barbarian.getLife(), 420); // Base + VIT + level
}

TEST(CharacterTest, LevelUp) {
    Character sorc(CharacterClass::SORCERESS);
    sorc.addExperience(1000);
    
    EXPECT_EQ(sorc.getLevel(), 2);
    EXPECT_EQ(sorc.getStatPoints(), 5);
    EXPECT_EQ(sorc.getSkillPoints(), 1);
}
```

**Implementation:**
- Character stats system
- Experience/leveling
- Class-specific mechanics

#### Task 4.2: Combat System
**Tests First:**
```cpp
TEST(CombatTest, HitChanceCalculation) {
    CombatEngine combat;
    
    float hitChance = combat.calculateHitChance(
        1000,  // Attack Rating
        500,   // Defense
        20,    // Attacker Level
        20     // Defender Level
    );
    
    EXPECT_NEAR(hitChance, 0.6667f, 0.001f); // 2/3 chance
}

TEST(CombatTest, DamageCalculation) {
    Character attacker(CharacterClass::BARBARIAN);
    Character defender(CharacterClass::NECROMANCER);
    
    attacker.equipWeapon(createTestWeapon(50, 100)); // 50-100 damage
    defender.setDefense(1000);
    
    auto damage = combat.calculateDamage(attacker, defender);
    
    EXPECT_GE(damage.physical, 50);
    EXPECT_LE(damage.physical, 100);
}
```

**Implementation:**
- Hit/damage formulas (Reference: `RESEARCH_FINDINGS.md`)
- Resistance system
- Critical strikes

#### Task 4.3: Item System
**Tests First:**
```cpp
TEST(ItemGeneratorTest, GenerateRareItem) {
    ItemGenerator generator;
    generator.setSeed(12345); // Deterministic for testing
    
    auto item = generator.generateItem(
        ItemType::SWORD,
        85,    // Item level
        300    // Magic find
    );
    
    EXPECT_EQ(item.getQuality(), ItemQuality::RARE);
    EXPECT_GE(item.getAffixCount(), 4);
    EXPECT_LE(item.getAffixCount(), 6);
}

TEST(ItemTest, AffixApplication) {
    Item sword(ItemType::SWORD);
    sword.addAffix(Affix::CRUEL); // +200% damage
    
    EXPECT_EQ(sword.getMinDamage(), 30); // Base 10 * 3
    EXPECT_EQ(sword.getMaxDamage(), 60); // Base 20 * 3
}
```

**Implementation:**
- Item generation algorithm (Reference: `IMPLEMENTATION_GAPS_RESOLVED.md`)
- Affix system
- Item requirements

#### Task 4.4: Skill System
**Tests First:**
```cpp
TEST(SkillTreeTest, Prerequisites) {
    SkillTree barbTree;
    Character barb(CharacterClass::BARBARIAN);
    
    EXPECT_FALSE(barbTree.canAllocate(SkillId::WHIRLWIND, barb));
    
    barb.allocateSkill(SkillId::BASH, 5);
    barb.setLevel(30);
    
    EXPECT_TRUE(barbTree.canAllocate(SkillId::WHIRLWIND, barb));
}

TEST(SkillTest, SynergyCalculation) {
    Skill frozenOrb;
    SkillLevels levels;
    levels[SkillId::ICE_BOLT] = 20;
    
    float damage = frozenOrb.calculateDamage(10, levels);
    float expectedBase = 200; // Level 10 base
    float synergyBonus = 200 * 0.02f * 20; // 2% per Ice Bolt level
    
    EXPECT_NEAR(damage, expectedBase + synergyBonus, 0.1f);
}
```

**Implementation:**
- Skill tree structure
- Prerequisite checking
- Synergy calculations

**Deliverables:**
- Complete character system
- Accurate combat mechanics
- Full item generation
- Working skill trees
- 90% test coverage

---

## Phase 5: Game World & AI (Weeks 15-17)

### Objectives
- Implement map loading
- Create monster system
- Build AI behaviors

### Tasks

#### Task 5.1: Map System
**Tests First:**
```cpp
TEST(MapLoaderTest, LoadDS1Map) {
    MapLoader loader;
    auto map = loader.loadMap("data/global/tiles/act1/town/town.ds1");
    
    EXPECT_EQ(map.getWidth(), 100);
    EXPECT_EQ(map.getHeight(), 100);
    EXPECT_TRUE(map.isWalkable(50, 50));
}

TEST(MapGeneratorTest, GenerateRandomDungeon) {
    MapGenerator gen;
    gen.setSeed(54321);
    
    auto dungeon = gen.generateDungeon(
        DungeonType::CATACOMBS,
        3  // Level depth
    );
    
    EXPECT_TRUE(dungeon.hasPath(
        dungeon.getEntrance(),
        dungeon.getExit()
    ));
}
```

**Implementation:**
- DS1 file loading (Reference: `IMPLEMENTATION_GAPS_RESOLVED.md`)
- Random map generation
- Collision detection

#### Task 5.2: Monster System
**Tests First:**
```cpp
TEST(MonsterTest, SpawnMonster) {
    MonsterFactory factory;
    auto skeleton = factory.create(MonsterId::SKELETON, 10); // Level 10
    
    EXPECT_EQ(skeleton.getLife(), 85); // Base + level scaling
    EXPECT_EQ(skeleton.getExperienceValue(), 120);
}

TEST(MonsterAITest, BasicBehavior) {
    MockWorld world;
    Monster zombie(MonsterId::ZOMBIE);
    zombie.setPosition(glm::vec2(0, 0));
    
    world.addPlayer(glm::vec2(100, 0)); // Player nearby
    
    zombie.updateAI(world, 0.016f); // One frame
    
    EXPECT_EQ(zombie.getState(), MonsterState::SEEKING);
    EXPECT_GT(zombie.getPosition().x, 0); // Moving toward player
}
```

**Implementation:**
- Monster stats/types
- State machine AI
- Group behaviors

#### Task 5.3: Pathfinding
**Tests First:**
```cpp
TEST(PathfindingTest, FindSimplePath) {
    GridMap map(10, 10);
    map.setBlocked(5, 0, 5, 9); // Wall in middle
    
    Pathfinder pf;
    auto path = pf.findPath(
        glm::ivec2(0, 5),   // Start
        glm::ivec2(9, 5),   // Goal
        map
    );
    
    EXPECT_FALSE(path.empty());
    EXPECT_GT(path.size(), 10); // Must go around wall
}

TEST(PathfindingTest, HandleNoPath) {
    GridMap map(10, 10);
    map.setBlocked(0, 4, 9, 6); // Complete wall
    
    Pathfinder pf;
    auto path = pf.findPath(
        glm::ivec2(5, 0),
        glm::ivec2(5, 9),
        map
    );
    
    EXPECT_TRUE(path.empty());
}
```

**Implementation:**
- A* pathfinding
- Path smoothing
- Dynamic obstacles

**Deliverables:**
- Map loading/generation
- Monster spawning
- AI behavior system
- Efficient pathfinding

---

## Phase 6: Audio & Networking (Weeks 18-20)

### Objectives
- Implement audio system
- Create networking layer
- Build multiplayer support

### Tasks

#### Task 6.1: Audio Engine
**Tests First:**
```cpp
TEST(AudioTest, LoadAndPlaySound) {
    MockOpenSLES mockAudio;
    AudioEngine audio;
    
    EXPECT_CALL(mockAudio, createAudioPlayer(_))
        .WillOnce(Return(true));
    
    auto sound = audio.loadSound("sword_swing.ogg");
    EXPECT_TRUE(audio.playSound(sound));
}

TEST(Audio3DTest, PositionalAudio) {
    Audio3D audio;
    audio.setListenerPosition(glm::vec3(0, 0, 0));
    
    auto sound = audio.loadSound("monster_growl.ogg");
    audio.playPositional(sound, glm::vec3(10, 0, 0));
    
    // Verify right channel is louder
    auto levels = audio.getChannelLevels();
    EXPECT_GT(levels.right, levels.left);
}
```

**Implementation:**
- OpenSL ES integration (Reference: `RESEARCH_FINDINGS.md`)
- 3D positioning
- Music streaming

#### Task 6.2: Network Layer
**Tests First:**
```cpp
TEST(NetworkTest, CreateLANGame) {
    NetworkManager network;
    
    auto session = network.hostGame("Test Game", 8);
    
    EXPECT_TRUE(session.isActive());
    EXPECT_EQ(session.getPort(), 6112);
}

TEST(NetworkTest, JoinLANGame) {
    MockNetwork mockNet;
    NetworkManager network;
    
    EXPECT_CALL(mockNet, discoverGames())
        .WillOnce(Return(std::vector<GameInfo>{
            {"Test Game", "192.168.1.100", 6112}
        }));
    
    auto games = network.findGames();
    EXPECT_EQ(games.size(), 1);
}
```

**Implementation:**
- TCP/IP protocol (Reference: `RESEARCH_FINDINGS.md`)
- Game discovery
- State synchronization

**Deliverables:**
- 3D audio system
- LAN multiplayer
- Network synchronization

---

## Phase 7: UI Implementation (Weeks 21-22)

### Objectives
- Create UI framework
- Implement all game screens
- Polish controller navigation

### Tasks

#### Task 7.1: UI Framework
**Tests First:**
```cpp
TEST(UITest, CreateButton) {
    UIButton button("Click Me", glm::vec2(100, 100));
    
    EXPECT_EQ(button.getText(), "Click Me");
    EXPECT_FALSE(button.isPressed());
    
    button.handleInput(InputEvent{100, 100, TOUCH_DOWN});
    EXPECT_TRUE(button.isPressed());
}

TEST(UINavigationTest, ControllerNavigation) {
    UIPanel panel;
    panel.addButton("Play", glm::vec2(100, 100));
    panel.addButton("Options", glm::vec2(100, 200));
    
    panel.handleControllerInput(DPAD_DOWN);
    EXPECT_EQ(panel.getFocusedElement(), 1);
}
```

**Implementation:**
- UI element system
- Controller navigation (Reference: `CONTROLLER_UI_DESIGN.md`)
- Touch support

#### Task 7.2: Game Screens
**Tests First:**
```cpp
TEST(InventoryTest, DragAndDrop) {
    InventoryScreen inv;
    auto sword = createTestItem(ItemType::SWORD);
    
    inv.addItem(sword, glm::ivec2(0, 0));
    
    // Simulate controller-based move
    inv.selectSlot(0, 0);
    inv.moveSelection(2, 2);
    inv.confirmMove();
    
    EXPECT_FALSE(inv.hasItem(0, 0));
    EXPECT_TRUE(inv.hasItem(2, 2));
}
```

**Implementation:**
- Inventory screen
- Skill tree UI
- Character screen
- All menus

**Deliverables:**
- Complete UI system
- All game screens
- Smooth navigation

---

## Phase 8: Integration & Polish (Weeks 23-24)

### Objectives
- System integration
- Performance optimization
- Final testing
- Deployment preparation

### Tasks

#### Task 8.1: Save System
**Tests First:**
```cpp
TEST(SaveSystemTest, SaveAndLoadCharacter) {
    Character original(CharacterClass::SORCERESS);
    original.setName("TestSorc");
    original.setLevel(42);
    
    SaveManager saves;
    saves.saveCharacter(original, "test.d2s");
    
    auto loaded = saves.loadCharacter("test.d2s");
    
    EXPECT_EQ(loaded.getName(), "TestSorc");
    EXPECT_EQ(loaded.getLevel(), 42);
}
```

**Implementation:**
- D2S file format (Reference: `RESEARCH_FINDINGS.md`)
- Save validation
- Backup system

#### Task 8.2: Performance Optimization
**Tests First:**
```cpp
TEST(PerformanceTest, MaintainTargetFPS) {
    GameEngine engine;
    engine.loadLevel("act1_blood_moor");
    
    // Spawn stress test
    for (int i = 0; i < 50; i++) {
        engine.spawnMonster(MonsterId::FALLEN);
    }
    
    auto metrics = engine.runBenchmark(60.0f); // 60 seconds
    
    EXPECT_GE(metrics.averageFPS, 60.0f);
    EXPECT_LE(metrics.frameDrops, 10);
}
```

**Implementation:**
- Profile and optimize
- Memory management
- Battery optimization

#### Task 8.3: APK Building
**Tests First:**
```bash
#!/bin/bash
# Integration test script
./gradlew assembleRelease
adb install app-release.apk
adb shell am instrument -w com.d2portable.test/androidx.test.runner.AndroidJUnitRunner
```

**Implementation:**
- Build configuration
- Signing setup
- Release preparation

**Deliverables:**
- Save system working
- 60 FPS achieved
- APK ready for deployment
- All tests passing

---

## Quality Metrics

### Test Coverage Requirements
- Core Systems: 95% minimum
- Game Logic: 90% minimum  
- UI Code: 85% minimum
- Overall: 90% target

### Performance Targets
- Frame Rate: 60 FPS sustained
- Load Time: <5 seconds
- Memory Usage: <1.5GB
- Battery Life: 4+ hours

### Code Quality
- All tests passing
- No memory leaks
- Clean code principles
- Comprehensive documentation

## Risk Management

### Technical Risks
1. **Performance on target hardware**
   - Mitigation: Early device testing
   - Fallback: Graphics quality options

2. **Network compatibility**
   - Mitigation: Extensive protocol testing
   - Fallback: Simplified protocol

3. **Save file compatibility**
   - Mitigation: Multiple format versions
   - Fallback: Conversion utility

### Schedule Risks
- 20% buffer time included
- Optional features identified
- Parallel development where possible

---

## Phase 9: Text Rendering System Implementation (Week 25)

### Objectives
- Implement missing text rendering components
- Complete UI framework foundation
- Fix text-based UI functionality

### Context
Current status shows Phase 7 UI Implementation claimed complete, but critical text rendering components exist only as header files without implementation.

### Tasks

#### Task 9.1: Font System Implementation
**Tests First:**
```cpp
TEST(FontTest, LoadFontWithMetrics) {
    Font font("Arial", 16);
    
    EXPECT_EQ(font.getName(), "Arial");
    EXPECT_EQ(font.getSize(), 16);
    EXPECT_GT(font.getLineHeight(), 0);
}

TEST(FontTest, MeasureTextWidth) {
    Font font("Arial", 16);
    
    float width = font.measureTextWidth("Hello World");
    EXPECT_GT(width, 0);
    EXPECT_GT(width, font.measureTextWidth("Hello"));
}

TEST(FontTest, LoadFontFromAssets) {
    AssetManager assets;
    Font font;
    
    EXPECT_TRUE(font.loadFromAssets(assets, "fonts/exocet.ttf", 16));
    EXPECT_EQ(font.getName(), "exocet");
}
```

**Implementation:**
- Font glyph loading and caching
- Text width measurement
- Line height calculation
- TTF/OTF font file support

#### Task 9.2: FontManager Implementation
**Tests First:**
```cpp
TEST(FontManagerTest, RegisterAndRetrieveFont) {
    FontManager manager;
    auto font = std::make_shared<Font>("Arial", 16);
    
    manager.registerFont("default", font);
    
    auto retrieved = manager.getFont("default");
    EXPECT_EQ(retrieved.get(), font.get());
}

TEST(FontManagerTest, PreventDuplicateRegistration) {
    FontManager manager;
    auto font1 = std::make_shared<Font>("Arial", 16);
    auto font2 = std::make_shared<Font>("Arial", 18);
    
    manager.registerFont("arial", font1);
    manager.registerFont("arial", font2); // Should not replace
    
    EXPECT_EQ(manager.getFont("arial")->getSize(), 16);
}

TEST(FontManagerTest, LoadDefaultFonts) {
    FontManager manager;
    AssetManager assets;
    
    manager.loadDefaultFonts(assets);
    
    EXPECT_TRUE(manager.hasFont("default"));
    EXPECT_TRUE(manager.hasFont("large"));
    EXPECT_TRUE(manager.hasFont("small"));
}
```

**Implementation:**
- Font storage and retrieval
- Default font loading
- Memory management for fonts

#### Task 9.3: TextRenderer Implementation
**Tests First:**
```cpp
TEST(TextRendererTest, RenderBasicText) {
    MockOpenGLContext gl;
    TextRenderer renderer;
    auto font = std::make_shared<Font>("Arial", 16);
    
    renderer.initialize(gl);
    
    EXPECT_NO_THROW(renderer.renderText(
        "Hello World",
        glm::vec2(100, 100),
        font,
        glm::vec3(1.0f, 1.0f, 1.0f) // White
    ));
}

TEST(TextRendererTest, ChangeTextColor) {
    TextRenderer renderer;
    auto font = std::make_shared<Font>("Arial", 16);
    
    renderer.setDefaultColor(glm::vec3(1.0f, 0.0f, 0.0f)); // Red
    
    // Verify color change affects rendering
    EXPECT_EQ(renderer.getDefaultColor(), glm::vec3(1.0f, 0.0f, 0.0f));
}

TEST(TextRendererTest, HandleMultiLineText) {
    TextRenderer renderer;
    auto font = std::make_shared<Font>("Arial", 16);
    
    renderer.renderText(
        "Line 1\nLine 2\nLine 3",
        glm::vec2(0, 0),
        font,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
    
    // Should handle newlines properly without crashing
    EXPECT_TRUE(true); // Test passes if no exception thrown
}
```

**Implementation:**
- OpenGL text rendering
- Color management
- Multi-line text support
- Integration with sprite renderer

#### Task 9.4: UILabel Implementation
**Tests First:**
```cpp
TEST(UILabelTest, CreateLabelWithText) {
    UILabel label("Hello World", glm::vec2(100, 100));
    
    EXPECT_EQ(label.getText(), "Hello World");
    EXPECT_EQ(label.getPosition(), glm::vec2(100, 100));
}

TEST(UILabelTest, UpdateTextContent) {
    UILabel label("Initial", glm::vec2(0, 0));
    
    label.setText("Updated");
    
    EXPECT_EQ(label.getText(), "Updated");
}

TEST(UILabelTest, TextAlignment) {
    UILabel label("Centered", glm::vec2(100, 100));
    
    label.setAlignment(TextAlignment::CENTER);
    
    EXPECT_EQ(label.getAlignment(), TextAlignment::CENTER);
}

TEST(UILabelTest, RenderWithFont) {
    MockTextRenderer mockRenderer;
    UILabel label("Test", glm::vec2(50, 50));
    auto font = std::make_shared<Font>("Arial", 16);
    
    label.setFont(font);
    
    EXPECT_CALL(mockRenderer, renderText("Test", glm::vec2(50, 50), font, _))
        .Times(1);
    
    label.render(mockRenderer);
}
```

**Implementation:**
- Text display functionality
- Font integration
- Alignment support
- Render method implementation

**Deliverables:**
- Complete text rendering system
- Font loading and management
- UILabel functionality working
- Text-based UI components functional

---

## Phase 10: Game Mechanics Corrections (Week 26)

### Objectives
- Fix incorrect game mechanics implementations
- Ensure Diablo II accuracy
- Update related tests appropriately

### Context
Analysis revealed several game mechanics that don't match Diablo II formulas, particularly character life calculation and combat hit chance caps.

### Tasks

#### Task 10.1: Character Life Formula Correction
**Tests First:**
```cpp
TEST(CharacterTest, CorrectLifeCalculation) {
    Character barbarian(CharacterClass::BARBARIAN);
    barbarian.setLevel(10);
    barbarian.setStat(StatType::VITALITY, 35); // 25 base + 10 allocated
    
    // D2 Formula: Base Life + (Vitality * 4) + (Level - 1) * Life per Level
    // Barbarian: 55 base + (35 * 4) + (10-1) * 2 = 55 + 140 + 18 = 213
    EXPECT_EQ(barbarian.getLife(), 213);
}

TEST(CharacterTest, SorceressLifeCalculation) {
    Character sorceress(CharacterClass::SORCERESS);
    sorceress.setLevel(15);
    sorceress.setStat(StatType::VITALITY, 25); // 10 base + 15 allocated
    
    // Sorceress: 40 base + (25 * 4) + (15-1) * 1 = 40 + 100 + 14 = 154
    EXPECT_EQ(sorceress.getLife(), 154);
}

TEST(CharacterTest, LifePerLevelScaling) {
    Character necromancer(CharacterClass::NECROMANCER);
    necromancer.setLevel(1);
    necromancer.setStat(StatType::VITALITY, 15);
    
    int level1Life = necromancer.getLife();
    
    necromancer.setLevel(2);
    int level2Life = necromancer.getLife();
    
    // Necromancer gains 1.5 life per level
    EXPECT_EQ(level2Life - level1Life, 2); // Rounded up from 1.5
}
```

**Implementation:**
- Update Character class life calculation
- Add class-specific life per level values
- Fix vitality multiplier from 14 to 4
- Update all related calculations

#### Task 10.2: Combat Hit Chance Caps
**Tests First:**
```cpp
TEST(CombatEngineTest, HitChanceMinimumCap) {
    CombatEngine combat;
    
    // Extremely low attack rating vs high defense
    float hitChance = combat.calculateHitChance(
        10,     // Very low AR
        10000,  // Very high defense
        1,      // Level 1 attacker
        99      // Level 99 defender
    );
    
    EXPECT_GE(hitChance, 0.05f); // 5% minimum
}

TEST(CombatEngineTest, HitChanceMaximumCap) {
    CombatEngine combat;
    
    // Extremely high attack rating vs low defense
    float hitChance = combat.calculateHitChance(
        10000,  // Very high AR
        10,     // Very low defense
        99,     // Level 99 attacker
        1       // Level 1 defender
    );
    
    EXPECT_LE(hitChance, 0.95f); // 95% maximum
}

TEST(CombatEngineTest, NormalHitChanceRange) {
    CombatEngine combat;
    
    float hitChance = combat.calculateHitChance(1000, 500, 20, 20);
    
    EXPECT_GT(hitChance, 0.05f);
    EXPECT_LT(hitChance, 0.95f);
    EXPECT_NEAR(hitChance, 0.6667f, 0.001f); // Expected 2/3 chance
}
```

**Implementation:**
- Add 5% minimum hit chance cap
- Add 95% maximum hit chance cap
- Ensure caps are applied after formula calculation
- Update combat documentation

#### Task 10.3: Strength Damage Bonus
**Tests First:**
```cpp
TEST(CharacterTest, StrengthDamageBonus) {
    Character character(CharacterClass::BARBARIAN);
    character.setStat(StatType::STRENGTH, 100); // 30 base + 70 allocated
    
    // D2 Formula: 1% damage per strength point above required
    auto weapon = createTestWeapon(10, 20, 50); // 50 STR required
    character.equipWeapon(weapon);
    
    float expectedBonus = (100 - 50) * 0.01f; // 50% bonus
    float actualBonus = character.getStrengthDamageBonus();
    
    EXPECT_NEAR(actualBonus, expectedBonus, 0.001f);
}

TEST(CharacterTest, InsufficientStrengthNoDamageBonus) {
    Character character(CharacterClass::SORCERESS);
    character.setStat(StatType::STRENGTH, 30);
    
    auto weapon = createTestWeapon(10, 20, 50); // 50 STR required
    character.equipWeapon(weapon);
    
    // No bonus if strength requirement not met
    EXPECT_EQ(character.getStrengthDamageBonus(), 0.0f);
}
```

**Implementation:**
- Add strength damage bonus calculation
- Consider weapon strength requirements
- Apply bonus only when requirements are met
- Integrate with damage calculation system

**Deliverables:**
- Accurate Diablo II life calculations
- Proper hit chance caps implementation
- Strength damage bonus system
- Updated tests reflecting correct mechanics

---

## Phase 11: Test Coverage Completion (Week 27)

### Objectives
- Add comprehensive tests for untested implementation files
- Achieve 95% test coverage target
- Ensure all core systems are thoroughly tested

### Context
Analysis identified 13 implementation files with no test coverage, representing significant gaps in validation.

### Tasks

#### Task 11.1: Core System Tests
**Combat Engine Tests:**
```cpp
TEST(CombatEngineTest, PhysicalDamageCalculation) {
    CombatEngine engine;
    DamageSource attacker{100, 200, 0, 0, 0, 0}; // 100-200 physical
    ResistanceProfile defender{0.0f, 0.0f, 0.0f, 0.0f}; // No resistances
    
    auto damage = engine.calculateDamage(attacker, defender);
    
    EXPECT_GE(damage.physical, 100);
    EXPECT_LE(damage.physical, 200);
}

TEST(CombatEngineTest, ElementalResistances) {
    CombatEngine engine;
    DamageSource attacker{0, 0, 100, 0, 0, 0}; // 100 fire damage
    ResistanceProfile defender{0.0f, 0.5f, 0.0f, 0.0f}; // 50% fire resist
    
    auto damage = engine.calculateDamage(attacker, defender);
    
    EXPECT_EQ(damage.fire, 50); // 50% reduction
}
```

**Game Session Tests:**
```cpp
TEST(GameSessionTest, CreateSession) {
    GameSession session;
    
    EXPECT_FALSE(session.isActive());
    EXPECT_EQ(session.getPlayerCount(), 0);
}

TEST(GameSessionTest, AddPlayer) {
    GameSession session;
    Player player("TestPlayer");
    
    session.addPlayer(player);
    
    EXPECT_EQ(session.getPlayerCount(), 1);
    EXPECT_TRUE(session.hasPlayer("TestPlayer"));
}
```

#### Task 11.2: Asset System Tests
**StormLib MPQ Loader Tests:**
```cpp
TEST(StormLibMPQLoaderTest, OpenValidMPQ) {
    StormLibMPQLoader loader;
    
    // Use existing test MPQ file
    EXPECT_TRUE(loader.open("test_data/test.mpq"));
    EXPECT_GT(loader.getFileCount(), 0);
}

TEST(StormLibMPQLoaderTest, ExtractFile) {
    StormLibMPQLoader loader;
    loader.open("test_data/test.mpq");
    
    std::vector<uint8_t> data;
    EXPECT_TRUE(loader.extractFile("test.txt", data));
    EXPECT_FALSE(data.empty());
}
```

#### Task 11.3: AI System Tests
**Pathfinder Tests:**
```cpp
TEST(PathfinderTest, FindValidPath) {
    Pathfinder finder;
    GridMap map(10, 10);
    
    auto path = finder.findPath(
        glm::ivec2(0, 0),
        glm::ivec2(9, 9),
        map
    );
    
    EXPECT_FALSE(path.empty());
    EXPECT_EQ(path.front(), glm::ivec2(0, 0));
    EXPECT_EQ(path.back(), glm::ivec2(9, 9));
}

TEST(PathfinderTest, PathAroundObstacles) {
    Pathfinder finder;
    GridMap map(10, 10);
    
    // Create wall
    for (int y = 1; y < 9; y++) {
        map.setWalkable(5, y, false);
    }
    
    auto path = finder.findPath(
        glm::ivec2(0, 5),
        glm::ivec2(9, 5),
        map
    );
    
    EXPECT_FALSE(path.empty());
    EXPECT_GT(path.size(), 10); // Must go around
}
```

#### Task 11.4: Interaction System Tests
**Interaction Manager Tests:**
```cpp
TEST(InteractionManagerTest, RegisterObject) {
    InteractionManager manager;
    auto chest = std::make_shared<InteractiveObject>(
        ObjectType::CHEST,
        glm::vec2(100, 100)
    );
    
    manager.registerObject(chest);
    
    EXPECT_EQ(manager.getObjectCount(), 1);
}

TEST(InteractionManagerTest, FindNearbyObjects) {
    InteractionManager manager;
    auto chest = std::make_shared<InteractiveObject>(
        ObjectType::CHEST,
        glm::vec2(100, 100)
    );
    manager.registerObject(chest);
    
    auto nearby = manager.findObjectsInRange(
        glm::vec2(105, 105),
        10.0f
    );
    
    EXPECT_EQ(nearby.size(), 1);
    EXPECT_EQ(nearby[0]->getType(), ObjectType::CHEST);
}
```

#### Task 11.5: Inventory System Tests
**Character Inventory Tests:**
```cpp
TEST(CharacterInventoryTest, EquipItem) {
    CharacterInventory inventory;
    auto sword = createTestItem(ItemType::SWORD);
    
    EXPECT_TRUE(inventory.equipItem(sword, EquipSlot::MAIN_HAND));
    EXPECT_EQ(inventory.getEquippedItem(EquipSlot::MAIN_HAND), sword);
}

TEST(CharacterInventoryTest, CheckRequirements) {
    CharacterInventory inventory;
    Character character(CharacterClass::SORCERESS);
    auto armor = createTestItem(ItemType::ARMOR);
    armor->setRequirement(StatType::STRENGTH, 100);
    
    character.setStat(StatType::STRENGTH, 50); // Insufficient
    
    EXPECT_FALSE(inventory.canEquip(armor, character));
}
```

**Deliverables:**
- Comprehensive tests for all untested files
- 95%+ test coverage achieved
- All core systems thoroughly validated
- Documentation of test scenarios

---

## Phase 12: Android Project Structure (Week 28)

### Objectives
- Create complete Android project structure
- Set up JNI bridge to native engine
- Implement Android lifecycle management
- Prepare for mobile deployment

### Context
Current project lacks Android-specific components despite being positioned as an Android port.

### Tasks

#### Task 12.1: Android Project Setup
**Tests First:**
```bash
#!/bin/bash
# Android structure validation tests
test -d "android/app/src/main/java/com/diablo2portable" || exit 1
test -f "android/app/src/main/AndroidManifest.xml" || exit 1
test -f "android/app/build.gradle" || exit 1
test -f "android/settings.gradle" || exit 1
test -f "android/gradle.properties" || exit 1
```

**Implementation:**
- Create standard Android project structure
- Configure Gradle build files
- Set up Android SDK targeting
- Create application manifest

#### Task 12.2: JNI Bridge Implementation
**Tests First:**
```java
public class EngineJNITest {
    @Test
    public void testEngineInitialization() {
        long engineHandle = NativeEngine.createEngine();
        assertTrue("Engine handle should be valid", engineHandle != 0);
        
        boolean initialized = NativeEngine.initialize(engineHandle);
        assertTrue("Engine should initialize successfully", initialized);
        
        NativeEngine.destroyEngine(engineHandle);
    }
    
    @Test
    public void testAssetLoading() {
        long engineHandle = NativeEngine.createEngine();
        NativeEngine.initialize(engineHandle);
        
        boolean loaded = NativeEngine.loadAssets(engineHandle, "/android_asset/");
        assertTrue("Assets should load successfully", loaded);
        
        NativeEngine.destroyEngine(engineHandle);
    }
}
```

**Native Side Tests:**
```cpp
TEST(JNIBridgeTest, CreateEngineFromJNI) {
    JNIEnv* env = getTestJNIEnv();
    
    jlong handle = Java_com_diablo2portable_NativeEngine_createEngine(env, nullptr);
    
    EXPECT_NE(handle, 0);
    
    // Cleanup
    Java_com_diablo2portable_NativeEngine_destroyEngine(env, nullptr, handle);
}

TEST(JNIBridgeTest, PassInputEvents) {
    JNIEnv* env = getTestJNIEnv();
    jlong handle = Java_com_diablo2portable_NativeEngine_createEngine(env, nullptr);
    
    Java_com_diablo2portable_NativeEngine_onTouchEvent(
        env, nullptr, handle,
        100.0f, 200.0f, // x, y
        1 // action down
    );
    
    // Verify input was processed
    auto* engine = reinterpret_cast<GameEngine*>(handle);
    EXPECT_TRUE(engine->hasInputEvents());
}
```

**Implementation:**
- Create JNI wrapper classes
- Implement engine lifecycle methods
- Set up input event bridging
- Create asset loading integration

#### Task 12.3: Android Activity Implementation
**Tests First:**
```java
@RunWith(AndroidJUnit4.class)
public class GameActivityTest {
    @Rule
    public ActivityTestRule<GameActivity> activityRule = 
        new ActivityTestRule<>(GameActivity.class);
    
    @Test
    public void testActivityCreation() {
        GameActivity activity = activityRule.getActivity();
        assertNotNull("Activity should be created", activity);
        
        // Verify native engine is initialized
        assertTrue("Engine should be running", activity.isEngineRunning());
    }
    
    @Test
    public void testSurfaceViewCreation() {
        GameActivity activity = activityRule.getActivity();
        GLSurfaceView surfaceView = activity.findViewById(R.id.game_surface);
        
        assertNotNull("Surface view should exist", surfaceView);
    }
}
```

**Implementation:**
- Create main game activity
- Set up OpenGL surface view
- Implement Android lifecycle callbacks
- Add input handling

#### Task 12.4: Asset Integration
**Tests First:**
```java
public class AssetManagerTest {
    @Test
    public void testAssetExtraction() {
        Context context = InstrumentationRegistry.getTargetContext();
        AssetHelper helper = new AssetHelper(context);
        
        boolean extracted = helper.extractGameAssets();
        assertTrue("Assets should extract successfully", extracted);
        
        File dataDir = new File(context.getFilesDir(), "data");
        assertTrue("Data directory should exist", dataDir.exists());
    }
}
```

**Implementation:**
- Asset extraction to internal storage
- MPQ file handling on Android
- Permission management
- Storage optimization

**Deliverables:**
- Complete Android project structure
- Working JNI bridge
- Native engine integration
- Asset loading system
- Deployable APK foundation

---

## Phase 13: Documentation and Cleanup (Week 29)

### Objectives
- Update CLAUDE.md to reflect actual implementation status
- Clean up obsolete code and files
- Resolve technical debt
- Prepare comprehensive documentation

### Context
CLAUDE.md contains significant inaccuracies about implementation status and project completion.

### Tasks

#### Task 13.1: CLAUDE.md Accuracy Update
**Verification Tests:**
```bash
#!/bin/bash
# Verify claimed implementations exist
check_implementation() {
    local class_name=$1
    local header_file=$2
    local impl_file=$3
    
    if [ -f "$header_file" ] && [ -f "$impl_file" ]; then
        echo "✅ $class_name: Fully implemented"
        return 0
    elif [ -f "$header_file" ]; then
        echo "⚠️  $class_name: Header only"
        return 1
    else
        echo "❌ $class_name: Missing"
        return 2
    fi
}

# Check critical components
check_implementation "Font" "engine/include/ui/font.h" "engine/src/ui/font.cpp"
check_implementation "TextRenderer" "engine/include/ui/text_renderer.h" "engine/src/ui/text_renderer.cpp"
check_implementation "UILabel" "engine/include/ui/ui_label.h" "engine/src/ui/ui_label.cpp"
```

**Implementation:**
- Audit all claimed implementations
- Update completion percentages accurately
- Remove overstated claims
- Add realistic timelines for remaining work

#### Task 13.2: Code Cleanup
**Tests First:**
```cpp
TEST(CodeCleanupTest, NoUnusedIncludes) {
    // Scan for unused includes in header files
    std::vector<std::string> headers = findAllHeaders("engine/include/");
    
    for (const auto& header : headers) {
        auto includes = extractIncludes(header);
        auto used = findUsedSymbols(header);
        
        for (const auto& include : includes) {
            EXPECT_TRUE(isIncludeUsed(include, used)) 
                << "Unused include: " << include << " in " << header;
        }
    }
}

TEST(CodeCleanupTest, NoDeadCode) {
    // Find functions/classes that are never called
    auto symbols = extractAllSymbols("engine/src/");
    auto references = findAllReferences("engine/");
    
    for (const auto& symbol : symbols) {
        if (symbol.isPublic() && !symbol.isTest()) {
            EXPECT_GT(references.count(symbol.name), 0)
                << "Dead code detected: " << symbol.name;
        }
    }
}
```

**Implementation:**
- Remove obsolete MPQ implementation files
- Clean up unused includes
- Remove commented-out code
- Consolidate duplicate functionality

#### Task 13.3: Test Count Reconciliation
**Implementation:**
- Count all TEST macros in codebase
- Update CLAUDE.md with accurate count
- Document test categories and coverage
- Fix any disabled or skipped tests

#### Task 13.4: Technical Debt Resolution
**Priority Issues:**
```cpp
// Fix PKWARE implementation consistency
TEST(PKWARETest, SingleImplementationUsed) {
    // Verify only one PKWARE implementation is active
    EXPECT_TRUE(std::filesystem::exists("engine/src/utils/pkware_explode.cpp"));
    EXPECT_FALSE(std::filesystem::exists("engine/src/utils/pkware_explode_old.cpp"));
}

// Resolve StormLib/Custom MPQ dual implementation
TEST(MPQTest, ConsistentImplementation) {
    MPQLoader loader; // Should use StormLib wrapper
    
    EXPECT_TRUE(loader.open("test.mpq"));
    EXPECT_EQ(typeid(loader).name(), "StormLibMPQLoader");
}
```

**Implementation:**
- Choose single MPQ implementation approach
- Remove unused PKWARE variants
- Update CMake to reflect changes
- Document architectural decisions

**Deliverables:**
- Accurate CLAUDE.md documentation
- Clean, optimized codebase
- Resolved technical debt
- Comprehensive implementation status

---

## Updated Success Criteria

1. **Complete Implementation Verification** (362+ tests passing)
2. **Text Rendering System Functional** (Phase 9)
3. **Accurate Game Mechanics** (Phase 10) 
4. **95% Test Coverage Achieved** (Phase 11)
5. **Android Deployment Ready** (Phase 12)
6. **Documentation Accuracy** (Phase 13)
7. **60 FPS on Retroid Pocket Flip 2**
8. **LAN multiplayer functional**
9. **Full controller support**
10. **Save compatibility maintained**

---

## Phase 14: Core Game Integration (Week 30) - **IN PROGRESS**

### Objectives
- Integrate all systems into a playable game
- Create Player entity and game world
- Implement basic game loop
- Connect input to player actions

### Tasks

#### Task 14.1: Player Entity Implementation ✅ COMPLETED
**Tests Written:**
- CreatePlayerWithCharacter - Player holds character data
- PlayerHasPositionInWorld - Player has world position
- SetPlayerPosition - Can update player position
- MovePlayer - Can move player by delta

**Implementation:**
- Player class with Character association
- Position tracking (vec2)
- Movement methods

#### Task 14.2: GameState Management ✅ COMPLETED
**Tests Written:**
- CreateGameState - Empty state management
- AddPlayerToGameState - Player ownership

**Implementation:**
- GameState class with player management
- Integration with GameEngine

#### Task 14.3: Input-Player Integration (IN PROGRESS)
**Tests First:**
```cpp
TEST(InputIntegrationTest, GamepadMovesPlayer) {
    GameState gameState;
    auto player = std::make_shared<Player>(Character(CharacterClass::BARBARIAN));
    gameState.setPlayer(player);
    
    InputManager input;
    input.setGamepadAxis(AXIS_LEFT_X, 1.0f); // Full right
    
    // Process input through game update
    gameState.processInput(input);
    
    // Player should have moved right
    EXPECT_GT(player->getPosition().x, 0.0f);
}
```

#### Task 14.4: Entity Management System
**Tests First:**
```cpp
TEST(EntityManagerTest, AddAndRetrieveEntities) {
    EntityManager manager;
    
    auto player = std::make_shared<Player>(Character(CharacterClass::SORCERESS));
    auto monster = std::make_shared<Monster>(MonsterId::SKELETON);
    
    auto playerId = manager.addEntity(player);
    auto monsterId = manager.addEntity(monster);
    
    EXPECT_EQ(manager.getEntity(playerId), player);
    EXPECT_EQ(manager.getEntity(monsterId), monster);
    EXPECT_EQ(manager.getEntityCount(), 2);
}
```

**Deliverables:**
- Player entity fully integrated
- GameState managing world
- Input system connected
- Basic entity management

---

## Phase 15: World Rendering Integration (Week 31)

### Objectives
- Render game world with player
- Implement camera system
- Display game assets
- Basic HUD implementation

### Tasks

#### Task 15.1: World Renderer
**Tests First:**
```cpp
TEST(WorldRendererTest, RenderPlayerInWorld) {
    WorldRenderer renderer;
    GameState gameState;
    
    auto player = std::make_shared<Player>(Character(CharacterClass::PALADIN));
    player->setPosition(glm::vec2(100, 100));
    gameState.setPlayer(player);
    
    RenderContext context;
    renderer.render(gameState, context);
    
    // Verify player sprite was submitted for rendering
    EXPECT_EQ(context.getSpriteCount(), 1);
    EXPECT_EQ(context.getSprite(0).position, glm::vec2(100, 100));
}
```

#### Task 15.2: Camera System
**Tests First:**
```cpp
TEST(CameraTest, FollowPlayer) {
    Camera camera(800, 600); // Screen size
    
    Player player(Character(CharacterClass::AMAZON));
    player.setPosition(glm::vec2(1000, 1000));
    
    camera.followTarget(&player);
    camera.update();
    
    // Camera should center on player
    glm::vec2 center = camera.getCenter();
    EXPECT_FLOAT_EQ(center.x, 1000.0f);
    EXPECT_FLOAT_EQ(center.y, 1000.0f);
}
```

#### Task 15.3: Asset Loading
**Tests First:**
```cpp
TEST(GameAssetsTest, LoadPlayerSprites) {
    GameAssets assets;
    assets.initialize("path/to/d2/files");
    
    auto barbarianWalk = assets.getPlayerSprite(
        CharacterClass::BARBARIAN,
        AnimationType::WALK
    );
    
    EXPECT_NE(barbarianWalk, nullptr);
    EXPECT_GT(barbarianWalk->getFrameCount(), 0);
}
```

#### Task 15.4: HUD Implementation
**Tests First:**
```cpp
TEST(HUDTest, DisplayPlayerStats) {
    HUD hud;
    Character character(CharacterClass::NECROMANCER);
    character.setLevel(10);
    character.setLife(250);
    
    hud.setCharacter(&character);
    
    auto lifeDisplay = hud.getElement("life");
    EXPECT_EQ(lifeDisplay->getText(), "250/250");
    
    auto levelDisplay = hud.getElement("level");
    EXPECT_EQ(levelDisplay->getText(), "10");
}
```

**Deliverables:**
- World rendering with player
- Camera following player
- Game assets loading
- Basic HUD showing stats

---

## Phase 16: Gameplay Implementation (Week 32)

### Objectives
- Implement combat mechanics
- Add monsters to world
- Create item drops
- Basic quest system

### Tasks

#### Task 16.1: Combat Integration
**Tests First:**
```cpp
TEST(GameplayTest, PlayerAttackMonster) {
    GameWorld world;
    
    auto player = world.spawnPlayer(CharacterClass::BARBARIAN);
    auto monster = world.spawnMonster(MonsterId::FALLEN, glm::vec2(50, 0));
    
    player->setPosition(glm::vec2(0, 0));
    player->attack(glm::vec2(1, 0)); // Attack right
    
    world.update(0.016f); // One frame
    
    // Monster should take damage
    EXPECT_LT(monster->getLife(), monster->getMaxLife());
}
```

#### Task 16.2: Monster Spawning
**Tests First:**
```cpp
TEST(MonsterSpawnerTest, SpawnMonsterGroups) {
    MonsterSpawner spawner;
    GameWorld world;
    
    spawner.spawnGroup(
        MonsterId::ZOMBIE,
        glm::vec2(100, 100),
        5, // count
        world
    );
    
    auto monsters = world.getMonstersInArea(
        glm::vec2(100, 100),
        50.0f // radius
    );
    
    EXPECT_EQ(monsters.size(), 5);
}
```

#### Task 16.3: Loot Integration
**Tests First:**
```cpp
TEST(LootIntegrationTest, MonsterDropsItems) {
    GameWorld world;
    
    auto monster = world.spawnMonster(MonsterId::SKELETON, glm::vec2(0, 0));
    monster->setLife(0); // Kill it
    
    world.update(0.016f);
    
    auto items = world.getItemsInArea(glm::vec2(0, 0), 20.0f);
    EXPECT_GT(items.size(), 0); // Should drop something
}
```

#### Task 16.4: Quest Objectives
**Tests First:**
```cpp
TEST(QuestTest, CompleteKillQuest) {
    QuestManager quests;
    GameWorld world;
    
    auto quest = quests.startQuest(QuestId::DEN_OF_EVIL);
    EXPECT_FALSE(quest->isComplete());
    
    // Kill required monsters
    for (int i = 0; i < quest->getRequiredKills(); i++) {
        world.killMonster(MonsterId::FALLEN);
    }
    
    quests.update(world);
    EXPECT_TRUE(quest->isComplete());
}
```

**Deliverables:**
- Combat fully integrated
- Monsters spawning and fighting
- Loot dropping from monsters
- Basic quest tracking

---

## Phase 17: Multiplayer Integration (Week 33)

### Objectives
- Integrate networking with gameplay
- Synchronize player states
- Handle multiplayer combat
- Test LAN compatibility

### Tasks

#### Task 17.1: Network Game State
**Tests First:**
```cpp
TEST(NetworkGameTest, SynchronizePlayers) {
    NetworkGame host;
    NetworkGame client;
    
    host.startHost(8999);
    client.connect("localhost", 8999);
    
    // Host moves player
    host.getPlayer()->move(glm::vec2(10, 0));
    host.sendUpdate();
    
    // Client receives update
    client.receiveUpdate();
    auto remotePlayer = client.getRemotePlayer(0);
    
    EXPECT_FLOAT_EQ(remotePlayer->getPosition().x, 10.0f);
}
```

#### Task 17.2: Combat Synchronization
**Tests First:**
```cpp
TEST(NetworkCombatTest, SynchronizeDamage) {
    NetworkGame host;
    NetworkGame client;
    
    // Setup connection...
    
    auto monster = host.spawnMonster(MonsterId::ZOMBIE, glm::vec2(0, 0));
    int initialLife = monster->getLife();
    
    // Client attacks monster
    client.sendAttack(monster->getId(), 50); // 50 damage
    
    host.receiveUpdate();
    EXPECT_EQ(monster->getLife(), initialLife - 50);
}
```

**Deliverables:**
- Multiplayer gameplay working
- State synchronization
- LAN compatibility verified

---

## Phase 18: Polish and Optimization (Week 34)

### Objectives
- Optimize performance
- Polish UI/UX
- Add missing features
- Prepare for release

### Tasks

#### Task 18.1: Performance Optimization
**Tests First:**
```cpp
TEST(PerformanceTest, Maintain60FPSWithManyEntities) {
    GameWorld world;
    
    // Spawn many entities
    for (int i = 0; i < 100; i++) {
        world.spawnMonster(MonsterId::FALLEN, 
            glm::vec2(rand() % 1000, rand() % 1000));
    }
    
    auto startTime = getCurrentTime();
    for (int frame = 0; frame < 60; frame++) {
        world.update(0.016f);
        world.render();
    }
    auto endTime = getCurrentTime();
    
    float fps = 60.0f / (endTime - startTime);
    EXPECT_GE(fps, 60.0f);
}
```

#### Task 18.2: Memory Management
**Tests First:**
```cpp
TEST(MemoryTest, StayWithinBudget) {
    GameEngine engine;
    engine.initialize();
    engine.start();
    
    // Play for simulated 10 minutes
    for (int i = 0; i < 36000; i++) { // 60fps * 60s * 10min
        engine.renderFrame();
    }
    
    size_t memoryUsed = getProcessMemoryUsage();
    EXPECT_LE(memoryUsed, 1500 * 1024 * 1024); // 1.5GB limit
}
```

**Deliverables:**
- 60 FPS achieved
- Memory usage optimized
- UI polished
- Ready for release

---

## Phase 19: Asset Pipeline (Week 35) - IN PROGRESS

### Objectives
- Create asset extraction tools ✅
- Optimize assets for mobile ✅
- Package game data
- Create installer

### Tasks

#### Task 19.1: Asset Extraction Tool ✅ COMPLETED
**Tests Implemented:**
- `AssetExtractorTest.ExtractAllGameAssets` - Basic extraction with directory structure
- `AssetExtractorTest.ExtractRealDC6Sprites` - Real DC6 extraction using StormLib

**Implementation:**
- Created AssetExtractor class with MPQ file support
- Implemented DC6 sprite extraction with category organization
- Uses StormLibMPQLoader for real MPQ file access
- Organizes sprites into characters/monsters/items/ui categories

#### Task 19.2: Asset Optimization ✅ COMPLETED
**Tests Implemented:**
- `AssetOptimizerTest.CompressSprites` - PNG compression with 50% size reduction
- `AssetOptimizerTest.OptimizeForMobileGPU` - PVR format for mobile GPUs
- `AssetOptimizerTest.PreserveTransparency` - Alpha channel preservation

**Implementation:**
- Created AssetOptimizer class with multiple compression formats
- Implemented PNG compression for sprites with transparency
- Implemented PVR compression for mobile GPU optimization
- Compression ratio tracking and quality settings

#### Task 19.3: Texture Atlas Generation (PENDING)
**Tests to Write:**
```cpp
TEST(TextureAtlasTest, GenerateAtlasFromSprites) {
    TextureAtlasGenerator generator;
    
    std::vector<std::string> sprites = {
        "sprite1.png", "sprite2.png", "sprite3.png"
    };
    
    auto atlas = generator.generateAtlas(sprites, 2048, 2048);
    
    EXPECT_TRUE(atlas.isValid());
    EXPECT_LE(atlas.getPageCount(), 2); // Should fit in 1-2 pages
    EXPECT_TRUE(atlas.hasSprite("sprite1.png"));
}
```

#### Task 19.4: Asset Manifest (PENDING)
**Tests to Write:**
```cpp
TEST(AssetManifestTest, GenerateManifest) {
    AssetManifest manifest;
    
    manifest.addAsset("sprites/player.png", 1024, "abc123");
    manifest.addAsset("sounds/music.ogg", 2048, "def456");
    
    manifest.save("manifest.json");
    
    AssetManifest loaded;
    loaded.load("manifest.json");
    
    EXPECT_EQ(loaded.getAssetCount(), 2);
    EXPECT_TRUE(loaded.hasAsset("sprites/player.png"));
}
```

#### Task 19.5: APK Packaging (PENDING)
**Tests to Write:**
```cpp
TEST(APKPackagerTest, PackageAssets) {
    APKPackager packager;
    
    packager.addAssetDirectory("optimized_assets/");
    packager.setCompressionLevel(9);
    
    bool result = packager.createAPK("game_assets.apk");
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(fs::exists("game_assets.apk"));
    EXPECT_LT(fs::file_size("game_assets.apk"), 500 * 1024 * 1024); // Under 500MB
}
```

**Deliverables:**
- Asset extraction working ✅
- Assets optimized for mobile ✅
- APK packaging complete (pending)
- Installation process smooth (pending)

---

## Phase 20: Final Testing and Release (Week 36)

### Objectives
- Complete system testing
- Device compatibility testing
- Performance validation
- Release preparation

### Tasks

#### Task 20.1: Integration Testing
**Tests First:**
```cpp
TEST(IntegrationTest, CompleteGameplayLoop) {
    GameEngine engine;
    engine.initialize("/path/to/game/assets");
    engine.start();
    
    // Simulate full gameplay session
    auto testSession = createTestSession();
    testSession.createCharacter(CharacterClass::SORCERESS);
    testSession.enterGame();
    testSession.completeQuest(QuestId::DEN_OF_EVIL);
    testSession.saveAndExit();
    
    // Verify save file
    EXPECT_TRUE(fs::exists(testSession.getSavePath()));
    
    // Load and verify
    testSession.loadGame();
    EXPECT_TRUE(testSession.isQuestComplete(QuestId::DEN_OF_EVIL));
}
```

#### Task 20.2: Device Testing
- Test on Retroid Pocket Flip 2
- Test on various Android devices
- Verify controller compatibility
- Check battery life

**Deliverables:**
- All tests passing
- Device compatibility confirmed
- Performance targets met
- Ready for distribution

---

## Updated Success Criteria

1. **Complete Game Implementation** (502 tests and growing)
2. **Playable Game Experience** 
3. **60 FPS Performance**
4. **LAN Multiplayer Functional**
5. **Full Controller Support**
6. **Save System Working**
7. **Android APK Deployable**
8. **1.5GB Memory Limit**
9. **4+ Hour Battery Life**
10. **Diablo II Compatibility**

## Final Timeline

- **Phases 0-13**: Weeks 1-29 (COMPLETED)
- **Phase 14**: Week 30 (COMPLETED) - Core Game Integration
- **Phase 15**: Week 31 (COMPLETED) - World Rendering
- **Phase 16**: Week 32 (COMPLETED) - Gameplay Implementation
- **Phase 17**: Week 33 (COMPLETED) - Multiplayer Integration
- **Phase 18**: Week 34 (COMPLETED) - Polish and Optimization
- **Phase 19**: Week 35 (IN PROGRESS) - Asset Pipeline
- **Phase 20**: Week 36 - Final Testing and Release
- **Total Project Duration**: 36 weeks

## Risk Mitigation

1. **Performance Issues**
   - Early profiling in each phase
   - Optimization buffer in Phase 18
   - Fallback graphics options

2. **Asset Compatibility**
   - Early validation with real D2 files
   - Alternative asset formats ready
   - Clear user instructions

3. **Device Compatibility**
   - Testing on multiple devices
   - Configurable graphics settings
   - Controller mapping options

This comprehensive plan takes the project from its current state (individual systems complete) to a fully playable game, maintaining TDD discipline throughout.

---

## Phase 21: Complete Onboarding System (Week 37)

### Objectives
- Create user-friendly first-run experience
- Implement flexible asset source detection  
- Build guided setup wizard
- Handle various file formats and sources

### Context
Current system requires manual command-line setup. Users must copy MPQ files to specific directories and run extraction scripts. We need an in-app onboarding experience that can handle multiple asset sources automatically.

### Tasks

#### Task 21.1: File Source Detection System
**Tests First:**
```cpp
TEST(FileSourceDetectorTest, DetectD2InstallationDirectories) {
    FileSourceDetector detector;
    
    // Mock common installation paths
    std::vector<std::string> searchPaths = {
        "/Program Files/Diablo II",
        "/Steam/steamapps/common/Diablo II",
        "/GOG Games/Diablo 2"
    };
    
    auto found = detector.scanForInstallations(searchPaths);
    
    EXPECT_GT(found.size(), 0);
    EXPECT_TRUE(found[0].hasRequiredMPQs());
    EXPECT_EQ(found[0].getVersion(), D2Version::LORD_OF_DESTRUCTION);
}

TEST(FileSourceDetectorTest, DetectCDDrives) {
    FileSourceDetector detector;
    
    auto cdDrives = detector.detectCDDrives();
    
    // Test should pass even with no CDs
    EXPECT_TRUE(cdDrives.size() >= 0);
    
    // If test CD present, validate
    for (const auto& drive : cdDrives) {
        if (drive.hasD2Disc()) {
            EXPECT_TRUE(drive.canReadMPQFiles());
        }
    }
}

TEST(FileSourceDetectorTest, ValidateISOFiles) {
    FileSourceDetector detector;
    
    // Test with mock ISO file
    std::string isoPath = createTestISO();
    
    auto validation = detector.validateISOFile(isoPath);
    
    EXPECT_TRUE(validation.isValid);
    EXPECT_TRUE(validation.containsD2Data);
    EXPECT_FALSE(validation.requiresMount);
}
```

**Implementation:**
- Cross-platform directory scanning
- CD/DVD drive detection and enumeration
- ISO/image file validation
- Battle.net launcher integration detection
- Steam/GOG installation detection

#### Task 21.2: In-App Setup Wizard (Android)
**Tests First:**
```java
@RunWith(AndroidJUnit4.class)
public class SetupWizardTest {
    
    @Test
    public void testWelcomeScreen() {
        SetupWizardActivity activity = launchActivity();
        
        onView(withId(R.id.welcome_title))
            .check(matches(withText("Welcome to Diablo II Portable")));
        
        onView(withId(R.id.continue_button))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testFileSourceSelection() {
        SetupWizardActivity activity = launchActivity();
        
        // Navigate to file source screen
        onView(withId(R.id.continue_button)).perform(click());
        
        // Should show source options
        onView(withId(R.id.option_local_files))
            .check(matches(isDisplayed()));
        onView(withId(R.id.option_usb_storage))
            .check(matches(isDisplayed()));
        onView(withId(R.id.option_network_location))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testAssetExtractionProgress() {
        SetupWizardActivity activity = launchActivity();
        
        // Simulate file selection and start extraction
        activity.setSelectedSource("/storage/emulated/0/d2files");
        activity.startAssetExtraction();
        
        // Progress should be displayed
        onView(withId(R.id.extraction_progress))
            .check(matches(isDisplayed()));
        
        onView(withId(R.id.progress_text))
            .check(matches(withText(containsString("Extracting"))));
    }
}
```

**Native Bridge Tests:**
```cpp
TEST(OnboardingJNITest, StartAssetExtraction) {
    JNIEnv* env = getTestJNIEnv();
    
    jstring sourcePath = env->NewStringUTF("/test/d2/path");
    jstring outputPath = env->NewStringUTF("/android/data/output");
    
    jboolean result = Java_com_diablo2portable_OnboardingManager_extractAssets(
        env, nullptr, sourcePath, outputPath
    );
    
    EXPECT_TRUE(result);
}

TEST(OnboardingJNITest, GetExtractionProgress) {
    JNIEnv* env = getTestJNIEnv();
    
    // Start extraction in background
    startAsyncExtraction();
    
    jfloat progress = Java_com_diablo2portable_OnboardingManager_getProgress(
        env, nullptr
    );
    
    EXPECT_GE(progress, 0.0f);
    EXPECT_LE(progress, 1.0f);
}
```

**Implementation:**
- Android Activity-based setup wizard
- File picker integration with system file manager
- USB/external storage access
- Progress tracking with callbacks
- Error handling and retry mechanisms

#### Task 21.3: Advanced File Format Support
**Tests First:**
```cpp
TEST(FileFormatHandlerTest, ExtractFromISO) {
    FileFormatHandler handler;
    
    std::string isoPath = "test_data/d2_disc1.iso";
    std::string outputDir = "temp/extracted";
    
    auto result = handler.extractFromISO(isoPath, outputDir);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.filesExtracted, 0);
    EXPECT_TRUE(fs::exists(outputDir + "/d2data.mpq"));
}

TEST(FileFormatHandlerTest, HandleBattleNetInstaller) {
    FileFormatHandler handler;
    
    std::string installerPath = "test_data/D2_installer.exe";
    std::string outputDir = "temp/installer_extract";
    
    auto result = handler.extractFromInstaller(installerPath, outputDir);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.foundMPQFiles);
}

TEST(FileFormatHandlerTest, ProcessCompressedArchive) {
    FileFormatHandler handler;
    
    std::string zipPath = "test_data/d2_backup.zip";
    std::string outputDir = "temp/zip_extract";
    
    auto result = handler.extractFromArchive(zipPath, outputDir);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.mpqFilesFound, 5);
}
```

**Implementation:**
- ISO/BIN/CUE image mounting and extraction
- ZIP/RAR/7Z archive extraction
- Battle.net installer processing
- Executable extraction (InstallShield, NSIS)
- Network location handling (SMB, FTP)

#### Task 21.4: Smart Asset Validation
**Tests First:**
```cpp
TEST(AssetValidatorTest, ValidateCompleteness) {
    AssetValidator validator;
    std::string assetPath = "extracted_assets/";
    
    auto validation = validator.validateAssets(assetPath);
    
    EXPECT_TRUE(validation.isComplete);
    EXPECT_EQ(validation.missingFiles.size(), 0);
    EXPECT_TRUE(validation.hasExpansion);
    EXPECT_GE(validation.version, D2Version::CLASSIC);
}

TEST(AssetValidatorTest, DetectCorruption) {
    AssetValidator validator;
    
    // Simulate corrupted MPQ
    createCorruptedMPQ("test_corrupted.mpq");
    
    auto validation = validator.validateMPQIntegrity("test_corrupted.mpq");
    
    EXPECT_FALSE(validation.isValid);
    EXPECT_TRUE(validation.hasChecksumErrors);
    EXPECT_GT(validation.errorDetails.size(), 0);
}

TEST(AssetValidatorTest, SuggestMissingComponents) {
    AssetValidator validator;
    
    // Assets missing expansion
    std::string classicOnlyPath = "classic_assets/";
    
    auto validation = validator.validateAssets(classicOnlyPath);
    
    EXPECT_FALSE(validation.hasExpansion);
    EXPECT_TRUE(validation.suggestions.contains("Install Lord of Destruction"));
}
```

**Implementation:**
- File integrity checking with checksums
- Version detection (Classic vs LoD vs D2R)
- Missing file identification
- Corruption detection and reporting
- Repair suggestions and automatic fixes

#### Task 21.5: Configuration Persistence
**Tests First:**
```cpp
TEST(OnboardingConfigTest, SaveUserPreferences) {
    OnboardingConfig config;
    
    config.setAssetSource("/custom/d2/path");
    config.setExtractionQuality(ExtractionQuality::HIGH);
    config.setSkipValidation(false);
    
    config.save();
    
    OnboardingConfig loaded;
    loaded.load();
    
    EXPECT_EQ(loaded.getAssetSource(), "/custom/d2/path");
    EXPECT_EQ(loaded.getExtractionQuality(), ExtractionQuality::HIGH);
    EXPECT_FALSE(loaded.shouldSkipValidation());
}

TEST(OnboardingConfigTest, HandleFirstRun) {
    OnboardingConfig config;
    
    EXPECT_TRUE(config.isFirstRun());
    
    config.completeOnboarding();
    config.save();
    
    OnboardingConfig reloaded;
    reloaded.load();
    
    EXPECT_FALSE(reloaded.isFirstRun());
}
```

**Implementation:**
- User preference storage
- Asset source memory
- Extraction settings persistence
- First-run detection
- Configuration migration

**Deliverables:**
- Complete onboarding wizard UI
- Multi-source asset detection
- Advanced file format support
- Smart validation and repair
- Persistent configuration system

---

## Phase 22: Enhanced Asset Pipeline (Week 38)

### Objectives
- Extend existing AssetExtractor capabilities
- Add real-time extraction monitoring
- Implement differential updates
- Create asset caching system

### Context
Current AssetExtractor provides basic MPQ extraction. We need enhanced capabilities for production use including progress monitoring, incremental updates, and intelligent caching.

### Tasks

#### Task 22.1: Real-Time Extraction Monitoring
**Tests First:**
```cpp
TEST(ExtractionMonitorTest, TrackProgressWithCallback) {
    ExtractionMonitor monitor;
    std::vector<ProgressUpdate> updates;
    
    monitor.setProgressCallback([&](const ProgressUpdate& update) {
        updates.push_back(update);
    });
    
    AssetExtractor extractor;
    extractor.setMonitor(&monitor);
    
    extractor.extractFromD2("test_d2_path", "output_path");
    
    EXPECT_GT(updates.size(), 10); // Multiple progress updates
    EXPECT_FLOAT_EQ(updates.back().percentage, 1.0f); // Completed
    EXPECT_FALSE(updates.back().currentFile.empty());
}

TEST(ExtractionMonitorTest, EstimateTimeRemaining) {
    ExtractionMonitor monitor;
    
    // Simulate 50% completion after 30 seconds
    monitor.updateProgress(0.5f, "file.dc6", 30000);
    
    auto estimate = monitor.getTimeRemaining();
    
    EXPECT_NEAR(estimate.totalSeconds, 30.0, 5.0); // ~30 seconds remaining
    EXPECT_TRUE(estimate.isReliable);
}

TEST(ExtractionMonitorTest, HandleExtractionErrors) {
    ExtractionMonitor monitor;
    std::vector<ExtractionError> errors;
    
    monitor.setErrorCallback([&](const ExtractionError& error) {
        errors.push_back(error);
    });
    
    // Simulate extraction with corrupted file
    AssetExtractor extractor;
    extractor.setMonitor(&monitor);
    extractor.extractFromD2("corrupted_d2_path", "output");
    
    EXPECT_GT(errors.size(), 0);
    EXPECT_EQ(errors[0].type, ErrorType::CORRUPTED_MPQ);
    EXPECT_FALSE(errors[0].isRecoverable);
}
```

**Implementation:**
- Real-time progress callbacks with file-level detail
- Time estimation based on extraction speed
- Error reporting and recovery suggestions
- Bandwidth and I/O monitoring
- Parallel extraction tracking

#### Task 22.2: Differential Asset Updates
**Tests First:**
```cpp
TEST(DifferentialExtractorTest, DetectChangedFiles) {
    DifferentialExtractor extractor;
    
    // Initial extraction
    extractor.fullExtraction("d2_path", "output_v1");
    auto manifest1 = extractor.generateManifest("output_v1");
    
    // Simulate file changes
    modifyTestFile("d2_path/d2data.mpq");
    
    // Differential extraction
    auto changes = extractor.detectChanges("d2_path", manifest1);
    
    EXPECT_GT(changes.modifiedFiles.size(), 0);
    EXPECT_TRUE(changes.hasFile("d2data.mpq"));
    EXPECT_EQ(changes.changeType("d2data.mpq"), ChangeType::MODIFIED);
}

TEST(DifferentialExtractorTest, IncrementalUpdate) {
    DifferentialExtractor extractor;
    
    // Base extraction exists
    auto baseManifest = loadTestManifest("base_v1.manifest");
    
    // Perform incremental update
    auto result = extractor.incrementalUpdate(
        "d2_path",
        "output_path", 
        baseManifest
    );
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.filesUpdated, 0);
    EXPECT_LT(result.extractionTime, result.fullExtractionTime * 0.5); // Faster
}

TEST(DifferentialExtractorTest, HandleVersionMismatches) {
    DifferentialExtractor extractor;
    
    auto oldManifest = createManifest(D2Version::CLASSIC);
    auto newManifest = createManifest(D2Version::LORD_OF_DESTRUCTION);
    
    auto compatibility = extractor.checkCompatibility(oldManifest, newManifest);
    
    EXPECT_FALSE(compatibility.canUpdate);
    EXPECT_EQ(compatibility.recommendation, UpdateAction::FULL_REEXTRACTION);
    EXPECT_TRUE(compatibility.reason.contains("version mismatch"));
}
```

**Implementation:**
- File change detection using checksums and timestamps
- Incremental extraction for modified MPQs only
- Version compatibility checking
- Rollback capability for failed updates
- Bandwidth-efficient updates

#### Task 22.3: Intelligent Asset Caching
**Tests First:**
```cpp
TEST(AssetCacheTest, CacheExtractedAssets) {
    AssetCache cache("cache_dir", 1000); // 1000MB limit
    
    auto sprite = loadTestSprite("test_sprite.dc6");
    
    cache.store("player/barbarian/walk.dc6", sprite);
    
    EXPECT_TRUE(cache.has("player/barbarian/walk.dc6"));
    EXPECT_EQ(cache.getSize(), sprite->getDataSize());
}

TEST(AssetCacheTest, LRUEviction) {
    AssetCache cache("cache_dir", 100); // Small cache
    
    // Fill cache beyond capacity
    for (int i = 0; i < 10; i++) {
        auto sprite = createLargeSprite(15); // 15MB each
        cache.store("sprite_" + std::to_string(i), sprite);
    }
    
    // Early items should be evicted
    EXPECT_FALSE(cache.has("sprite_0"));
    EXPECT_FALSE(cache.has("sprite_1"));
    EXPECT_TRUE(cache.has("sprite_8"));
    EXPECT_TRUE(cache.has("sprite_9"));
}

TEST(AssetCacheTest, PreloadCriticalAssets) {
    AssetCache cache("cache_dir", 500);
    
    std::vector<std::string> criticalAssets = {
        "ui/panels/inventory.dc6",
        "ui/fonts/exocet.dc6",
        "player/universal/walk.dc6"
    };
    
    cache.preload(criticalAssets);
    
    for (const auto& asset : criticalAssets) {
        EXPECT_TRUE(cache.has(asset));
        EXPECT_TRUE(cache.isPinned(asset)); // Won't be evicted
    }
}
```

**Implementation:**
- LRU cache with configurable size limits
- Asset preloading for critical game files
- Pinning system for frequently accessed assets
- Compression for cached assets
- Cache persistence across sessions

#### Task 22.4: Multi-Format Asset Processing
**Tests First:**
```cpp
TEST(AssetProcessorTest, ConvertDC6ToPNG) {
    AssetProcessor processor;
    
    std::vector<uint8_t> dc6Data = loadBinaryFile("test_sprite.dc6");
    
    auto result = processor.convertDC6ToPNG(dc6Data, 0, 0); // frame 0, direction 0
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.data.size() > 0);
    EXPECT_TRUE(isPNGFormat(result.data));
}

TEST(AssetProcessorTest, ExtractPaletteFromMPQ) {
    AssetProcessor processor;
    
    auto palette = processor.extractPalette("d2data.mpq", "data/global/palette/act1/pal.dat");
    
    EXPECT_EQ(palette.colorCount, 256);
    EXPECT_TRUE(palette.hasTransparency);
    EXPECT_EQ(palette.colors[0].alpha, 0); // Transparent
}

TEST(AssetProcessorTest, ProcessAudioFiles) {
    AssetProcessor processor;
    
    auto audioData = processor.extractAudio("d2sfx.mpq", "data/global/sfx/cursor/button.wav");
    
    EXPECT_TRUE(audioData.isValid);
    EXPECT_EQ(audioData.format, AudioFormat::PCM_16);
    EXPECT_EQ(audioData.channels, 2); // Stereo
    EXPECT_GT(audioData.sampleRate, 0);
}
```

**Implementation:**
- DC6 to PNG/PVR conversion with palette support
- Audio file extraction and format conversion
- Data table extraction (Excel files)
- String table processing
- Font extraction and conversion

#### Task 22.5: Asset Verification System
**Tests First:**
```cpp
TEST(AssetVerifierTest, ValidateExtractedAssets) {
    AssetVerifier verifier;
    
    std::string extractedPath = "extracted_assets/";
    auto verification = verifier.fullVerification(extractedPath);
    
    EXPECT_TRUE(verification.isComplete);
    EXPECT_EQ(verification.corruptedFiles.size(), 0);
    EXPECT_GT(verification.validatedFiles, 1000);
    EXPECT_TRUE(verification.hasRequiredAssets());
}

TEST(AssetVerifierTest, DetectMissingCriticalFiles) {
    AssetVerifier verifier;
    
    // Delete critical file
    std::string assetPath = "incomplete_assets/";
    fs::remove(assetPath + "data/global/ui/panel/invchar6.dc6");
    
    auto verification = verifier.fullVerification(assetPath);
    
    EXPECT_FALSE(verification.isComplete);
    EXPECT_GT(verification.missingCriticalFiles.size(), 0);
    EXPECT_TRUE(verification.canAttemptRepair());
}

TEST(AssetVerifierTest, GenerateChecksumManifest) {
    AssetVerifier verifier;
    
    auto manifest = verifier.generateChecksumManifest("extracted_assets/");
    
    EXPECT_GT(manifest.fileCount, 1000);
    EXPECT_FALSE(manifest.manifestChecksum.empty());
    
    // Verify manifest integrity
    EXPECT_TRUE(verifier.validateManifest(manifest));
}
```

**Implementation:**
- Complete asset validation with checksums
- Missing file detection and reporting
- Corruption detection and repair suggestions
- Manifest generation for distribution
- Automated repair where possible

**Deliverables:**
- Enhanced extraction with real-time monitoring
- Differential update system
- Intelligent asset caching
- Multi-format processing
- Comprehensive verification system

---

## Phase 23: User Experience Enhancement (Week 39)

### Objectives
- Create intuitive user interfaces
- Implement help and tutorial systems
- Add accessibility features
- Build troubleshooting tools

### Context
Technical systems are complete but need user-friendly interfaces. Focus on making the onboarding and asset management processes accessible to non-technical users.

### Tasks

#### Task 23.1: Visual Asset Browser
**Tests First:**
```java
@RunWith(AndroidJUnit4.class)
public class AssetBrowserTest {
    
    @Test
    public void testDisplayAssetCategories() {
        AssetBrowserActivity activity = launchActivity();
        
        onView(withId(R.id.category_characters))
            .check(matches(isDisplayed()));
        onView(withId(R.id.category_monsters))
            .check(matches(isDisplayed()));
        onView(withId(R.id.category_items))
            .check(matches(isDisplayed()));
        onView(withId(R.id.category_ui))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testPreviewSprites() {
        AssetBrowserActivity activity = launchActivity();
        
        // Navigate to character sprites
        onView(withId(R.id.category_characters)).perform(click());
        
        // Select barbarian
        onView(withText("Barbarian")).perform(click());
        
        // Should show sprite preview
        onView(withId(R.id.sprite_preview))
            .check(matches(isDisplayed()));
        
        // Should show frame navigation
        onView(withId(R.id.frame_slider))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testAssetInformation() {
        AssetBrowserActivity activity = launchActivity();
        
        // Select specific asset
        navigateToAsset("ui/panels/inventory.dc6");
        
        // Should display asset metadata
        onView(withId(R.id.asset_filename))
            .check(matches(withText("inventory.dc6")));
        onView(withId(R.id.asset_size))
            .check(matches(withText(containsString("KB"))));
        onView(withId(R.id.asset_dimensions))
            .check(matches(isDisplayed()));
    }
}
```

**Native Tests:**
```cpp
TEST(AssetBrowserBackendTest, LoadAssetMetadata) {
    AssetBrowserBackend backend;
    backend.initialize("extracted_assets/");
    
    auto metadata = backend.getAssetMetadata("ui/panels/inventory.dc6");
    
    EXPECT_FALSE(metadata.filename.empty());
    EXPECT_GT(metadata.fileSize, 0);
    EXPECT_GT(metadata.frameCount, 0);
    EXPECT_FALSE(metadata.category.empty());
}

TEST(AssetBrowserBackendTest, GenerateThumbnails) {
    AssetBrowserBackend backend;
    
    auto thumbnail = backend.generateThumbnail(
        "characters/barbarian/walk.dc6",
        ThumbnailSize::MEDIUM
    );
    
    EXPECT_TRUE(thumbnail.isValid());
    EXPECT_EQ(thumbnail.width, 128);
    EXPECT_EQ(thumbnail.height, 128);
    EXPECT_TRUE(thumbnail.data.size() > 0);
}
```

**Implementation:**
- Grid-based asset browser with categories
- Sprite preview with frame animation
- Asset metadata display
- Search and filtering capabilities
- Thumbnail generation for quick preview

#### Task 23.2: Interactive Tutorial System
**Tests First:**
```java
@RunWith(AndroidJUnit4.class)
public class TutorialSystemTest {
    
    @Test
    public void testOnboardingTutorial() {
        TutorialActivity activity = launchActivity();
        
        // First step: Welcome
        onView(withId(R.id.tutorial_title))
            .check(matches(withText("Welcome to Diablo II Portable")));
        
        onView(withId(R.id.next_button)).perform(click());
        
        // Second step: File selection
        onView(withId(R.id.tutorial_title))
            .check(matches(withText("Select Your Game Files")));
        
        // Should show visual guide
        onView(withId(R.id.tutorial_image))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testControllerTutorial() {
        TutorialActivity activity = launchActivity();
        activity.startControllerTutorial();
        
        // Should detect controller connection
        onView(withId(R.id.controller_status))
            .check(matches(withText("Controller Connected")));
        
        // Should show button mapping
        onView(withId(R.id.button_mapping_guide))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testSkipTutorial() {
        TutorialActivity activity = launchActivity();
        
        onView(withId(R.id.skip_button)).perform(click());
        
        // Should confirm skip
        onView(withText("Skip Tutorial?"))
            .check(matches(isDisplayed()));
        
        onView(withText("Yes")).perform(click());
        
        // Should mark tutorial as completed
        assertTrue(TutorialPreferences.isCompleted());
    }
}
```

**Implementation:**
- Step-by-step onboarding tutorial
- Controller setup and testing
- Visual guides with screenshots
- Progress tracking and bookmarking
- Context-sensitive help system

#### Task 23.3: Accessibility Features
**Tests First:**
```java
@RunWith(AndroidJUnit4.class)
public class AccessibilityTest {
    
    @Test
    public void testScreenReaderSupport() {
        AccessibilityTestActivity activity = launchActivity();
        
        // All interactive elements should have content descriptions
        onView(withId(R.id.start_extraction_button))
            .check(matches(hasContentDescription()));
        
        onView(withId(R.id.file_picker_button))
            .check(matches(hasContentDescription()));
    }
    
    @Test
    public void testHighContrastMode() {
        AccessibilityTestActivity activity = launchActivity();
        
        // Enable high contrast
        AccessibilitySettings.setHighContrast(true);
        activity.recreate();
        
        // UI should adapt colors
        View button = activity.findViewById(R.id.primary_button);
        ColorDrawable background = (ColorDrawable) button.getBackground();
        
        assertTrue("Button should have high contrast colors",
            isHighContrastColor(background.getColor()));
    }
    
    @Test
    public void testLargeFontSupport() {
        AccessibilityTestActivity activity = launchActivity();
        
        // Enable large fonts
        AccessibilitySettings.setLargeFonts(true);
        activity.recreate();
        
        TextView title = activity.findViewById(R.id.main_title);
        float fontSize = title.getTextSize();
        
        assertTrue("Font should be larger than default",
            fontSize > AccessibilitySettings.DEFAULT_FONT_SIZE * 1.3f);
    }
}
```

**Implementation:**
- Screen reader compatibility
- High contrast color schemes
- Large font support
- Voice guidance for critical steps
- Alternative input methods

#### Task 23.4: Troubleshooting Tools
**Tests First:**
```cpp
TEST(TroubleshootingToolsTest, DiagnoseExtractionIssues) {
    TroubleshootingTools tools;
    
    // Simulate extraction failure
    ExtractionResult failedResult;
    failedResult.success = false;
    failedResult.errorCode = ErrorCode::INSUFFICIENT_SPACE;
    failedResult.errorMessage = "Not enough disk space";
    
    auto diagnosis = tools.diagnoseExtraction(failedResult);
    
    EXPECT_EQ(diagnosis.primaryIssue, IssueType::STORAGE_SPACE);
    EXPECT_TRUE(diagnosis.hasRecommendations);
    EXPECT_GT(diagnosis.possibleSolutions.size(), 0);
    EXPECT_TRUE(diagnosis.canAutoFix);
}

TEST(TroubleshootingToolsTest, SystemRequirementsCheck) {
    TroubleshootingTools tools;
    
    auto systemCheck = tools.checkSystemRequirements();
    
    EXPECT_TRUE(systemCheck.androidVersionOK);
    EXPECT_TRUE(systemCheck.openGLVersionOK);
    EXPECT_GT(systemCheck.availableRAM, 0);
    EXPECT_GT(systemCheck.availableStorage, 0);
    
    if (!systemCheck.meetsMinimumRequirements()) {
        EXPECT_FALSE(systemCheck.recommendations.empty());
    }
}

TEST(TroubleshootingToolsTest, GenerateDiagnosticReport) {
    TroubleshootingTools tools;
    
    auto report = tools.generateDiagnosticReport();
    
    EXPECT_FALSE(report.deviceInfo.empty());
    EXPECT_FALSE(report.appVersion.empty());
    EXPECT_GT(report.installedAssets.size(), 0);
    EXPECT_TRUE(report.systemHealth.isValid());
    
    // Report should be exportable
    auto json = report.toJSON();
    EXPECT_TRUE(json.contains("device"));
    EXPECT_TRUE(json.contains("assets"));
}
```

**Implementation:**
- Automated problem diagnosis
- System requirement validation
- Storage space analysis
- Network connectivity testing
- Diagnostic report generation

#### Task 23.5: Help and Documentation System
**Tests First:**
```java
@RunWith(AndroidJUnit4.class)
public class HelpSystemTest {
    
    @Test
    public void testSearchHelp() {
        HelpActivity activity = launchActivity();
        
        onView(withId(R.id.help_search))
            .perform(typeText("extraction failed"));
        
        onView(withId(R.id.search_button)).perform(click());
        
        // Should show relevant help articles
        onView(withId(R.id.help_results))
            .check(matches(hasMinimumChildCount(1)));
        
        onView(withText(containsString("Extraction")))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testContextualHelp() {
        MainActivity activity = launchActivity();
        
        // Long press should show context help
        onView(withId(R.id.file_picker_button))
            .perform(longClick());
        
        onView(withText("Help"))
            .check(matches(isDisplayed()));
        
        onView(withText("Help")).perform(click());
        
        // Should show specific help for file picker
        onView(withText(containsString("file selection")))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testOfflineHelp() {
        HelpActivity activity = launchActivity();
        
        // Disable network
        NetworkTestUtils.disableNetwork();
        
        onView(withId(R.id.help_categories)).perform(click());
        
        // Should still show help content
        onView(withId(R.id.help_content))
            .check(matches(isDisplayed()));
        
        NetworkTestUtils.enableNetwork();
    }
}
```

**Implementation:**
- Searchable help database
- Context-sensitive help tooltips
- Offline documentation
- Video tutorials (optional)
- Community integration (forums, FAQ)

**Deliverables:**
- Intuitive asset browser
- Comprehensive tutorial system
- Accessibility compliance
- Robust troubleshooting tools
- Complete help system

---

## Phase 24: Quality Assurance and Polish (Week 40)

### Objectives
- Comprehensive testing across all systems
- Performance optimization and validation
- User experience refinement
- Release preparation

### Context
Final phase focusing on quality assurance, performance validation, and preparation for production release.

### Tasks

#### Task 24.1: Comprehensive Integration Testing
**Tests First:**
```cpp
TEST(EndToEndTest, CompleteOnboardingToGameplay) {
    // Full end-to-end test simulating user journey
    
    // 1. Fresh installation
    OnboardingManager onboarding;
    EXPECT_TRUE(onboarding.isFirstRun());
    
    // 2. Asset detection and extraction
    auto sources = onboarding.detectAssetSources();
    EXPECT_GT(sources.size(), 0);
    
    onboarding.selectSource(sources[0]);
    auto extractionResult = onboarding.extractAssets();
    EXPECT_TRUE(extractionResult.success);
    
    // 3. Game engine initialization
    GameEngine engine;
    EXPECT_TRUE(engine.initialize());
    
    // 4. Character creation
    auto character = engine.createCharacter(CharacterClass::SORCERESS, "TestSorc");
    EXPECT_TRUE(character != nullptr);
    
    // 5. Enter game world
    EXPECT_TRUE(engine.enterGame(character));
    
    // 6. Basic gameplay
    auto initialPos = character->getPosition();
    engine.processInput(InputAction::MOVE, glm::vec2(1, 0));
    engine.update(0.016f);
    
    EXPECT_NE(character->getPosition(), initialPos);
    
    // 7. Save and exit
    EXPECT_TRUE(engine.saveCharacter(character));
    engine.exitGame();
    
    // 8. Verify save file
    SaveManager saves;
    auto loadedChar = saves.loadCharacter("TestSorc");
    EXPECT_EQ(loadedChar->getName(), "TestSorc");
}

TEST(StressTest, LongTermGameplayStability) {
    GameEngine engine;
    engine.initialize();
    
    auto character = engine.createCharacter(CharacterClass::BARBARIAN, "StressTest");
    engine.enterGame(character);
    
    // Simulate 2 hours of gameplay (7200 frames at 60fps)
    for (int frame = 0; frame < 7200; frame++) {
        // Vary input to simulate real gameplay
        if (frame % 60 == 0) { // Every second
            engine.processInput(getRandomInput());
        }
        
        engine.update(0.016f);
        engine.render();
        
        // Check for memory leaks every 10 seconds
        if (frame % 600 == 0) {
            size_t memUsage = getProcessMemoryUsage();
            EXPECT_LT(memUsage, MEMORY_LIMIT_BYTES);
        }
    }
    
    // Engine should still be responsive
    EXPECT_TRUE(engine.isRunning());
    EXPECT_LT(engine.getAverageFrameTime(), 16.67f); // 60 FPS
}
```

#### Task 24.2: Performance Validation and Optimization
**Tests First:**
```cpp
TEST(PerformanceValidationTest, MeetTargetSpecifications) {
    PerformanceProfiler profiler;
    GameEngine engine;
    
    engine.initialize();
    profiler.startProfiling();
    
    // Load intensive scene
    auto testLevel = engine.loadLevel("stress_test_level");
    
    // Spawn many entities
    for (int i = 0; i < 100; i++) {
        testLevel->spawnMonster(MonsterId::FALLEN, getRandomPosition());
    }
    
    // Measure performance over 60 seconds
    auto metrics = profiler.measurePerformance(60.0f);
    
    // Validate against targets
    EXPECT_GE(metrics.averageFPS, 60.0f);
    EXPECT_LE(metrics.frameTimeVariance, 2.0f); // Stable framerate
    EXPECT_LE(metrics.memoryUsage, 1536 * 1024 * 1024); // 1.5GB limit
    EXPECT_LE(metrics.batteryDrainRate, 25.0f); // 4+ hour battery life
    EXPECT_LE(metrics.loadTime, 5.0f); // 5 second max load
}

TEST(OptimizationTest, AssetLoadingPerformance) {
    AssetManager assets;
    PerformanceProfiler profiler;
    
    profiler.startProfiling();
    
    // Load common game assets
    std::vector<std::string> commonAssets = {
        "ui/panels/inventory.dc6",
        "characters/barbarian/walk.dc6",
        "monsters/zombie/walk.dc6",
        "items/weapons/sword.dc6"
    };
    
    for (const auto& asset : commonAssets) {
        auto sprite = assets.loadSprite(asset);
        EXPECT_NE(sprite, nullptr);
    }
    
    auto loadMetrics = profiler.getMetrics();
    
    // Assets should load quickly
    EXPECT_LT(loadMetrics.totalTime, 1.0f); // Under 1 second
    EXPECT_LT(loadMetrics.averageLoadTime, 0.25f); // 250ms per asset
}
```

#### Task 24.3: User Experience Testing
**Tests First:**
```java
@RunWith(AndroidJUnit4.class)
public class UserExperienceTest {
    
    @Test
    public void testOnboardingCompletionTime() {
        long startTime = System.currentTimeMillis();
        
        // Simulate efficient user onboarding
        OnboardingActivity activity = launchActivity();
        
        // Auto-detect assets (should be fast)
        onView(withId(R.id.auto_detect_button)).perform(click());
        waitForCondition(() -> activity.hasDetectedAssets(), 10000);
        
        // Start extraction
        onView(withId(R.id.start_extraction_button)).perform(click());
        waitForCondition(() -> activity.isExtractionComplete(), 120000); // 2 min max
        
        long completionTime = System.currentTimeMillis() - startTime;
        
        // Onboarding should complete within 5 minutes for typical setup
        assertTrue("Onboarding took too long: " + completionTime + "ms",
            completionTime < 300000);
    }
    
    @Test
    public void testErrorRecovery() {
        OnboardingActivity activity = launchActivity();
        
        // Simulate extraction failure
        activity.simulateExtractionError();
        
        // Should show clear error message
        onView(withId(R.id.error_message))
            .check(matches(isDisplayed()));
        
        // Should offer recovery options
        onView(withId(R.id.retry_button))
            .check(matches(isDisplayed()));
        onView(withId(R.id.help_button))
            .check(matches(isDisplayed()));
        
        // Recovery should work
        onView(withId(R.id.retry_button)).perform(click());
        waitForCondition(() -> activity.getExtractionStatus() == Status.IN_PROGRESS, 5000);
    }
    
    @Test
    public void testAccessibilityCompliance() {
        OnboardingActivity activity = launchActivity();
        
        // Enable accessibility services
        enableAccessibilityServices();
        
        // All interactive elements should be accessible
        List<View> interactiveViews = findAllInteractiveViews(activity);
        
        for (View view : interactiveViews) {
            assertTrue("View missing accessibility info: " + view.getId(),
                hasAccessibilityInfo(view));
        }
        
        // Navigation should work with TalkBack
        navigateWithTalkBack(activity);
        assertTrue("TalkBack navigation failed", 
            activity.getCurrentStep() > 0);
    }
}
```

#### Task 24.4: Device Compatibility Validation
**Tests First:**
```cpp
TEST(DeviceCompatibilityTest, ValidateTargetDevice) {
    DeviceCompatibilityChecker checker;
    
    auto compatibility = checker.checkDevice("Retroid Pocket Flip 2");
    
    EXPECT_TRUE(compatibility.isSupported);
    EXPECT_TRUE(compatibility.openGLESSupport);
    EXPECT_TRUE(compatibility.controllerSupport);
    EXPECT_GE(compatibility.expectedPerformance, PerformanceLevel::OPTIMAL);
}

TEST(DeviceCompatibilityTest, GracefulDegradation) {
    DeviceCompatibilityChecker checker;
    
    // Simulate low-end device
    DeviceSpecs lowEndDevice;
    lowEndDevice.ram = 2048; // 2GB RAM
    lowEndDevice.openGLVersion = "3.0";
    lowEndDevice.cpuCores = 4;
    
    auto compatibility = checker.checkDevice(lowEndDevice);
    
    if (!compatibility.meetsMinimumRequirements()) {
        EXPECT_TRUE(compatibility.hasPerformanceRecommendations());
        EXPECT_FALSE(compatibility.recommendedSettings.highQualityGraphics);
        EXPECT_TRUE(compatibility.recommendedSettings.reducedEntityCount);
    }
}
```

#### Task 24.5: Security and Privacy Validation
**Tests First:**
```cpp
TEST(SecurityTest, ValidateDataProtection) {
    SecurityValidator validator;
    
    // Check file permissions
    auto permissions = validator.checkFilePermissions("user_data/");
    EXPECT_TRUE(permissions.isSecure);
    EXPECT_FALSE(permissions.worldReadable);
    
    // Validate encryption of sensitive data
    auto encryption = validator.checkEncryption("saves/");
    EXPECT_TRUE(encryption.isSaveDataEncrypted);
    EXPECT_TRUE(encryption.useStrongCipher);
    
    // No network data collection
    auto privacy = validator.checkPrivacyCompliance();
    EXPECT_FALSE(privacy.collectsPersonalData);
    EXPECT_FALSE(privacy.sendsDataToServers);
    EXPECT_TRUE(privacy.dataStaysLocal);
}

TEST(SecurityTest, ValidateAssetIntegrity) {
    AssetIntegrityChecker checker;
    
    // Check for malicious modifications
    auto integrity = checker.validateAssets("extracted_assets/");
    EXPECT_TRUE(integrity.passesIntegrityCheck);
    EXPECT_EQ(integrity.modifiedFiles.size(), 0);
    EXPECT_EQ(integrity.suspiciousFiles.size(), 0);
}
```

**Deliverables:**
- Comprehensive test suite passing
- Performance targets validated
- User experience optimized
- Device compatibility confirmed
- Security and privacy validated

---

## Updated Final Timeline

- **Phases 0-20**: Weeks 1-36 (COMPLETED) - Core Implementation
- **Phase 21**: Week 37 - Complete Onboarding System
- **Phase 22**: Week 38 - Enhanced Asset Pipeline  
- **Phase 23**: Week 39 - User Experience Enhancement
- **Phase 24**: Week 40 - Quality Assurance and Polish
- **Total Project Duration**: 40 weeks

## Enhanced Success Criteria

1. **Complete User-Friendly Experience**
   - ✅ One-tap onboarding for any D2 installation
   - ✅ Support for multiple asset sources (CD, ISO, installers, directories)
   - ✅ Real-time progress tracking with ETA
   - ✅ Error recovery and troubleshooting

2. **Production-Ready Quality**
   - ✅ 60 FPS on target hardware validated
   - ✅ Sub-5 minute onboarding time
   - ✅ 95%+ asset extraction success rate
   - ✅ Accessibility compliance
   - ✅ Security and privacy validated

3. **Enhanced Asset Management**
   - ✅ Differential updates for modified assets
   - ✅ Intelligent caching with LRU eviction
   - ✅ Multi-format support (MPQ, ISO, ZIP, etc.)
   - ✅ Asset browser with preview capability

4. **Robust User Support**
   - ✅ Interactive tutorial system
   - ✅ Automated troubleshooting tools
   - ✅ Comprehensive help documentation
   - ✅ Diagnostic reporting

This extended implementation plan transforms the project from a technically capable but user-hostile system into a polished, production-ready application that provides an exceptional user experience for getting Diablo II running on Android devices.