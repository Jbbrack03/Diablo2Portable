#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

using namespace std::chrono;

class PerformanceMetricsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for performance tests
    }
    
    template<typename Func>
    double measureExecutionTime(Func func, int iterations = 1000) {
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        return static_cast<double>(duration.count()) / iterations;
    }
};

// Test 1: Vector allocation performance
TEST_F(PerformanceMetricsTest, VectorAllocationPerformance) {
    double avg_time = measureExecutionTime([&]() {
        std::vector<uint8_t> data;
        data.reserve(10240); // 10KB reservation
        for (int i = 0; i < 10240; ++i) {
            data.push_back(i & 0xFF);
        }
    }, 1000);
    
    std::cout << "10KB vector allocation average time: " << avg_time << " μs" << std::endl;
    
    // Performance target: < 250 microseconds for 10KB allocation with push_back
    EXPECT_LT(avg_time, 250.0);
}

// Test 2: Memory allocation patterns
TEST_F(PerformanceMetricsTest, MemoryAllocationPerformance) {
    // Test frequent allocation/deallocation patterns
    double avg_time = measureExecutionTime([&]() {
        std::vector<std::unique_ptr<int[]>> allocations;
        
        // Simulate game object allocations
        for (int i = 0; i < 10; ++i) {
            allocations.push_back(std::make_unique<int[]>(1024));
        }
        
        // Clear to deallocate
        allocations.clear();
    });
    
    std::cout << "Memory allocation pattern average time: " << avg_time << " μs" << std::endl;
    
    // Performance target: < 100 microseconds for small allocations
    EXPECT_LT(avg_time, 100.0);
}

// Test 3: String operations performance
TEST_F(PerformanceMetricsTest, StringOperationsPerformance) {
    std::vector<std::string> strings;
    for (int i = 0; i < 100; ++i) {
        strings.push_back("test_string_" + std::to_string(i));
    }
    
    double avg_time = measureExecutionTime([&]() {
        std::string result;
        for (const auto& str : strings) {
            result += str;
        }
    });
    
    std::cout << "String concatenation (100 strings) average time: " << avg_time << " μs" << std::endl;
    
    // Performance target: < 50 microseconds for 100 string concatenations
    EXPECT_LT(avg_time, 50.0);
}

// Test 4: Map lookup performance
TEST_F(PerformanceMetricsTest, MapLookupPerformance) {
    std::unordered_map<int, std::string> map;
    for (int i = 0; i < 10000; ++i) {
        map[i] = "value_" + std::to_string(i);
    }
    
    double avg_time = measureExecutionTime([&]() {
        // Random lookups
        int key = rand() % 10000;
        auto it = map.find(key);
        if (it != map.end()) {
            volatile auto& value = it->second; // Prevent optimization
            (void)value;
        }
    }, 10000);
    
    std::cout << "Hash map lookup average time: " << avg_time << " μs" << std::endl;
    
    // Performance target: < 1 microsecond for hash map lookup
    EXPECT_LT(avg_time, 1.0);
}

// Test 5: Batch data processing
TEST_F(PerformanceMetricsTest, BatchProcessingPerformance) {
    struct GameObject {
        float x, y;
        float vx, vy;
        int health;
        bool active;
    };
    
    std::vector<GameObject> objects(1000);
    for (int i = 0; i < 1000; ++i) {
        objects[i] = {
            static_cast<float>(i % 100),
            static_cast<float>(i / 100),
            1.0f, 1.0f,
            100,
            true
        };
    }
    
    double avg_time = measureExecutionTime([&]() {
        // Simulate game update loop
        for (auto& obj : objects) {
            if (obj.active) {
                obj.x += obj.vx * 0.016f; // 60 FPS timestep
                obj.y += obj.vy * 0.016f;
                
                // Simple bounds check
                if (obj.x < 0 || obj.x > 1000 || obj.y < 0 || obj.y > 1000) {
                    obj.active = false;
                }
            }
        }
    });
    
    std::cout << "Batch update (1000 objects) average time: " << avg_time << " μs" << std::endl;
    
    // Performance target: < 100 microseconds for 1000 objects
    EXPECT_LT(avg_time, 100.0);
}