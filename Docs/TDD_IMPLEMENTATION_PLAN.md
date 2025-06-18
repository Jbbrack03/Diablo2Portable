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

## Revised Timeline

- **Original Plan**: 24 weeks (Phases 0-8)
- **Gap Analysis**: Week 25 (Current session)
- **Corrective Phases**: Weeks 25-29 (Phases 9-13)
- **Total Revised Plan**: 29 weeks

## Next Steps

1. **Prioritize Phase 9** (Text Rendering) - Critical for UI functionality
2. **Execute Phase 10** (Game Mechanics) - Ensure D2 accuracy
3. **Complete Phase 11** (Test Coverage) - Achieve quality targets
4. **Evaluate Phase 12** (Android) - Based on deployment timeline
5. **Finalize Phase 13** (Documentation) - Accurate project status

This expanded plan addresses all identified gaps while maintaining the rigorous TDD methodology and ensuring project completion with accurate documentation and full functionality.