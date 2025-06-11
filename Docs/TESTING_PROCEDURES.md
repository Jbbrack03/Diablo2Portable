# Testing Procedures Guide - Diablo II Android Port

## Overview
This document provides comprehensive testing procedures for the Diablo II Android port, ensuring quality and compatibility at every development stage.

## Testing Framework Setup

### Required Tools
- **Google Test**: C++ unit testing
- **Google Mock**: Mock object framework
- **Espresso**: Android UI testing
- **Android Test Orchestrator**: Test isolation
- **Firebase Test Lab**: Device farm testing (optional)

### Installation
```bash
# Add to CMakeLists.txt
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.0.zip
)
FetchContent_MakeAvailable(googletest)

# Add to app/build.gradle
dependencies {
    testImplementation 'junit:junit:4.13.2'
    androidTestImplementation 'androidx.test.ext:junit:1.1.5'
    androidTestImplementation 'androidx.test.espresso:espresso-core:3.5.1'
}
```

## Test Categories

### 1. Unit Tests

#### Structure
```
engine/tests/
├── core/
│   ├── GameEngineTest.cpp
│   ├── CharacterTest.cpp
│   └── CombatTest.cpp
├── rendering/
│   ├── RendererTest.cpp
│   └── SpriteTest.cpp
└── CMakeLists.txt
```

#### Example Test
```cpp
// CharacterTest.cpp
#include <gtest/gtest.h>
#include "Character.h"

class CharacterTest : public ::testing::Test {
protected:
    void SetUp() override {
        barbarian = std::make_unique<Character>(CharacterClass::BARBARIAN);
    }
    
    std::unique_ptr<Character> barbarian;
};

TEST_F(CharacterTest, InitialStats) {
    EXPECT_EQ(barbarian->getLevel(), 1);
    EXPECT_EQ(barbarian->getStrength(), 30);
    EXPECT_EQ(barbarian->getDexterity(), 20);
    EXPECT_EQ(barbarian->getVitality(), 25);
    EXPECT_EQ(barbarian->getEnergy(), 10);
}

TEST_F(CharacterTest, LevelUpStatGain) {
    barbarian->addExperience(500); // Enough for level 2
    
    EXPECT_EQ(barbarian->getLevel(), 2);
    EXPECT_EQ(barbarian->getStatPoints(), 5);
    EXPECT_EQ(barbarian->getSkillPoints(), 1);
}
```

#### Running Unit Tests
```bash
# Build and run all tests
cd build
cmake .. -DBUILD_TESTS=ON
make
./engine/tests/d2_unit_tests

# Run specific test suite
./engine/tests/d2_unit_tests --gtest_filter=CharacterTest.*

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

### 2. Integration Tests

#### Test Scenarios
```cpp
// GameIntegrationTest.cpp
TEST(IntegrationTest, CompleteGameLoop) {
    GameEngine engine;
    engine.initialize();
    
    // Load a map
    ASSERT_TRUE(engine.loadMap("act1_town"));
    
    // Create character
    auto hero = engine.createCharacter(CharacterClass::SORCERESS);
    ASSERT_NE(hero, nullptr);
    
    // Spawn some monsters
    engine.spawnMonster(MonsterId::FALLEN, glm::vec2(100, 100));
    
    // Simulate game frames
    for (int i = 0; i < 60; i++) {
        engine.update(0.016f); // 60 FPS
        engine.render();
    }
    
    // Verify state
    EXPECT_GT(engine.getFrameCount(), 0);
    EXPECT_EQ(engine.getMonsterCount(), 1);
}
```

### 3. Performance Tests

#### Benchmark Framework
```cpp
// PerformanceBenchmark.cpp
class RenderingBenchmark : public ::testing::Test {
protected:
    void SetUp() override {
        renderer = std::make_unique<D2Renderer>();
        renderer->initialize();
        
        // Load test assets
        for (int i = 0; i < 100; i++) {
            sprites.push_back(loadTestSprite());
        }
    }
    
    std::unique_ptr<D2Renderer> renderer;
    std::vector<Sprite> sprites;
};

TEST_F(RenderingBenchmark, SpriteBatching) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int frame = 0; frame < 1000; frame++) {
        renderer->beginFrame();
        
        for (const auto& sprite : sprites) {
            renderer->drawSprite(sprite, randomPosition());
        }
        
        renderer->endFrame();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should render 1000 frames in less than 16.67 seconds (60 FPS)
    EXPECT_LT(duration.count(), 16670);
    
    // Log performance metrics
    float avgFrameTime = duration.count() / 1000.0f;
    std::cout << "Average frame time: " << avgFrameTime << "ms\n";
    std::cout << "FPS: " << (1000.0f / avgFrameTime) << "\n";
}
```

### 4. Android UI Tests

#### Espresso Tests
```kotlin
// InventoryScreenTest.kt
@RunWith(AndroidJUnit4::class)
class InventoryScreenTest {
    
    @get:Rule
    val activityRule = ActivityScenarioRule(GameActivity::class.java)
    
    @Test
    fun testItemPickupAndEquip() {
        // Navigate to game
        onView(withId(R.id.play_button)).perform(click())
        
        // Open inventory
        onView(withId(R.id.inventory_button)).perform(click())
        
        // Verify inventory opened
        onView(withId(R.id.inventory_grid))
            .check(matches(isDisplayed()))
        
        // Drag item to equipment slot
        onView(withId(R.id.item_sword))
            .perform(dragTo(withId(R.id.weapon_slot)))
        
        // Verify equipped
        onView(withId(R.id.weapon_slot))
            .check(matches(hasDescendant(withId(R.id.item_sword))))
    }
}
```

### 5. Device Compatibility Tests

#### Test Matrix
```yaml
# test-matrix.yaml
devices:
  - model: "retroid_pocket_flip_2"
    version: "11"
    orientation: "landscape"
  - model: "pixel_5"
    version: "12"
    orientation: "both"
  - model: "galaxy_s21"
    version: "13"
    orientation: "landscape"

test_targets:
  - performance
  - compatibility
  - controller_input
  - save_system
```

## Test Execution Procedures

### 1. Pre-Commit Tests
```bash
#!/bin/bash
# pre-commit-tests.sh
echo "Running pre-commit tests..."

# Unit tests
./run_unit_tests.sh || exit 1

# Quick integration tests
./run_integration_tests.sh --quick || exit 1

# Code style checks
./check_code_style.sh || exit 1

echo "All pre-commit tests passed!"
```

### 2. Continuous Integration Tests
```yaml
# .github/workflows/ci.yml
name: CI Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up NDK
        uses: nttld/setup-ndk@v1
        with:
          ndk-version: r25c
          
      - name: Run Unit Tests
        run: |
          cmake -B build -DBUILD_TESTS=ON
          cmake --build build
          cd build && ctest --output-on-failure
          
      - name: Run Android Tests
        uses: reactivecircus/android-emulator-runner@v2
        with:
          api-level: 29
          script: ./gradlew connectedAndroidTest
```

### 3. Release Testing

#### Checklist
```markdown
## Release Test Checklist

### Functional Tests
- [ ] New game creation
- [ ] Character progression (level 1-10)
- [ ] All character classes playable
- [ ] Save/load functionality
- [ ] Inventory management
- [ ] Skill usage and progression
- [ ] Monster combat
- [ ] Item generation
- [ ] Town interactions
- [ ] Waypoint system

### Performance Tests
- [ ] Stable 60 FPS in town
- [ ] Stable 60 FPS in combat (10+ monsters)
- [ ] Load time < 5 seconds
- [ ] Memory usage < 1.5GB
- [ ] Battery life > 4 hours

### Compatibility Tests
- [ ] Retroid Pocket Flip 2
- [ ] Generic Android phones
- [ ] Android tablets
- [ ] Different Android versions (8+)

### Multiplayer Tests
- [ ] LAN game creation
- [ ] LAN game joining
- [ ] 2-player stability
- [ ] 8-player stress test
- [ ] Network disconnect handling

### Controller Tests
- [ ] All buttons mapped correctly
- [ ] Dead zones functioning
- [ ] Analog stick precision
- [ ] Button combos working
- [ ] UI navigation smooth
```

## Test Data Management

### Test Assets
```
test_data/
├── mpq/
│   ├── test_archive.mpq
│   └── test_sounds.mpq
├── sprites/
│   ├── test_character.dc6
│   └── test_monster.dcc
├── saves/
│   ├── valid_save.d2s
│   ├── corrupt_save.d2s
│   └── max_level.d2s
└── maps/
    ├── simple_map.ds1
    └── complex_map.ds1
```

### Mock Data Factory
```cpp
class TestDataFactory {
public:
    static Character createTestCharacter(int level = 1) {
        Character hero(CharacterClass::PALADIN);
        hero.setLevel(level);
        hero.setName("TestHero");
        return hero;
    }
    
    static Item createTestWeapon() {
        Item sword(ItemType::SWORD);
        sword.setQuality(ItemQuality::RARE);
        sword.addAffix(Affix::CRUEL);
        sword.addAffix(Affix::OF_THE_TIGER);
        return sword;
    }
    
    static std::vector<uint8_t> createTestMPQ() {
        // Generate minimal valid MPQ
        MPQBuilder builder;
        builder.addFile("test.txt", "Hello D2!");
        return builder.build();
    }
};
```

## Performance Monitoring

### Metrics Collection
```cpp
class PerformanceMonitor {
private:
    struct FrameMetrics {
        float frameTime;
        int drawCalls;
        size_t memoryUsed;
        float cpuUsage;
        float gpuUsage;
    };
    
public:
    void beginFrame() {
        frameStart = std::chrono::high_resolution_clock::now();
        startMemory = getCurrentMemoryUsage();
    }
    
    void endFrame() {
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto duration = frameEnd - frameStart;
        
        FrameMetrics metrics{
            .frameTime = std::chrono::duration<float, std::milli>(duration).count(),
            .drawCalls = renderer->getDrawCallCount(),
            .memoryUsed = getCurrentMemoryUsage(),
            .cpuUsage = getCPUUsage(),
            .gpuUsage = getGPUUsage()
        };
        
        frameHistory.push_back(metrics);
        
        if (frameHistory.size() % 60 == 0) {
            logPerformanceReport();
        }
    }
};
```

### Automated Performance Tests
```bash
#!/bin/bash
# performance_test.sh

# Start performance monitoring
adb shell am start -n com.d2portable/.PerfTestActivity

# Wait for tests to complete
sleep 300

# Pull performance data
adb pull /sdcard/d2portable/perf_report.json ./

# Analyze results
python3 analyze_performance.py perf_report.json

# Check against thresholds
if [ $? -ne 0 ]; then
    echo "Performance regression detected!"
    exit 1
fi
```

## Debugging Procedures

### Crash Analysis
```cpp
// CrashHandler.cpp
void setupCrashHandler() {
    std::set_terminate([]() {
        auto eptr = std::current_exception();
        try {
            if (eptr) {
                std::rethrow_exception(eptr);
            }
        } catch(const std::exception& e) {
            logCrash("Uncaught exception: %s", e.what());
            generateCrashDump();
        }
        
        std::abort();
    });
}
```

### Memory Leak Detection
```bash
# Run with AddressSanitizer
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -g" ..
make
./d2_tests

# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./d2_tests
```

## Test Reporting

### Test Results Format
```json
{
    "test_run": {
        "date": "2024-01-15T10:30:00Z",
        "version": "0.1.0-alpha",
        "device": "Retroid Pocket Flip 2",
        "results": {
            "unit_tests": {
                "total": 1523,
                "passed": 1520,
                "failed": 3,
                "skipped": 0,
                "duration": "45.3s"
            },
            "integration_tests": {
                "total": 87,
                "passed": 87,
                "failed": 0,
                "skipped": 0,
                "duration": "3m 12s"
            },
            "performance": {
                "avg_fps": 62.3,
                "min_fps": 58.1,
                "max_frame_time": "17.2ms",
                "memory_peak": "1.2GB"
            }
        }
    }
}
```

## Best Practices

### 1. Test Naming
```cpp
// Good test names
TEST(ItemGenerator, RareItemHasFourToSixAffixes)
TEST(Character, BarbarianStartsWithThirtyStrength)
TEST(Combat, CriticalStrikeDoublesPhysicalDamage)

// Bad test names
TEST(ItemTest, Test1)
TEST(Character, TestStats)
TEST(Combat, ItWorks)
```

### 2. Test Independence
- Each test must be independent
- No shared state between tests
- Use SetUp/TearDown for initialization

### 3. Test Coverage Goals
- Core Systems: 95% minimum
- Game Logic: 90% minimum
- UI Code: 85% minimum
- Overall: 90% target

### 4. Continuous Testing
- Run tests on every commit
- Monitor test execution time
- Fix flaky tests immediately
- Keep tests maintainable

This comprehensive testing approach ensures the Diablo II Android port maintains high quality throughout development.