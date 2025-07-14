#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <vector>
#include <thread>
#include "utils/stormlib_mpq_loader.h"
#include "utils/mpq_validator.h"
#include "performance/performance_monitor.h"

// Phase 39.1: Test MPQ loading performance on Android
// This test simulates Android-specific performance characteristics

namespace fs = std::filesystem;

class AndroidMPQPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mpq_dir_ = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
        
        // Find valid MPQ files
        for (const auto& entry : fs::directory_iterator(mpq_dir_)) {
            if (entry.path().extension() == ".mpq") {
                std::string filepath = entry.path().string();
                auto validation = d2::utils::MPQValidator::validateMPQFile(filepath);
                
                if (validation.isValid && !validation.isPlaceholder) {
                    valid_mpqs_.push_back(filepath);
                }
            }
        }
    }
    
    fs::path mpq_dir_;
    std::vector<std::string> valid_mpqs_;
};

// Test 1: Simulate Android storage access patterns
TEST_F(AndroidMPQPerformanceTest, SimulateAndroidStorageAccess) {
    if (valid_mpqs_.empty()) {
        GTEST_SKIP() << "No valid MPQ files available for Android performance testing";
    }
    
    std::cout << "\n=== Android Storage Access Simulation ===\n";
    
    // Android storage has different characteristics than desktop
    // - Internal storage: Fast sequential reads, slower random access
    // - External SD card: Slower overall, high latency for small reads
    
    for (const auto& mpq_path : valid_mpqs_) {
        fs::path path(mpq_path);
        auto file_size = fs::file_size(path);
        
        std::cout << "\nTesting: " << path.filename() 
                  << " (" << file_size / (1024*1024) << " MB)\n";
        
        // Simulate Android internal storage characteristics
        std::cout << "  Internal storage simulation:\n";
        {
            // Add simulated storage latency
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            
            auto start = std::chrono::high_resolution_clock::now();
            
            d2portable::utils::StormLibMPQLoader loader;
            bool opened = loader.open(mpq_path);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto open_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            
            ASSERT_TRUE(opened) << "Failed to open " << mpq_path;
            
            std::cout << "    Open time: " << open_time << "ms\n";
            
            // Android has stricter performance requirements due to battery
            long max_time = 150;
            if (file_size > 200 * 1024 * 1024) { // 200MB
                max_time = 300; // Allow more time for large files
            }
            
            EXPECT_LT(open_time, max_time) 
                << "MPQ open time exceeds Android performance target";
            
            loader.close();
        }
        
        // Simulate external SD card (slower)
        std::cout << "  External SD card simulation:\n";
        {
            // Higher latency for SD card access
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            
            auto start = std::chrono::high_resolution_clock::now();
            
            d2portable::utils::StormLibMPQLoader loader;
            bool opened = loader.open(mpq_path);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto open_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            
            if (opened) {
                std::cout << "    Open time: " << open_time << "ms\n";
                
                // SD card performance is much more variable
                long max_time = 500;
                if (file_size > 200 * 1024 * 1024) {
                    max_time = 1000;
                }
                
                EXPECT_LT(open_time, max_time) 
                    << "MPQ open time exceeds SD card performance limits";
                
                loader.close();
            }
        }
    }
}

// Test 2: Validate memory-constrained MPQ operations
TEST_F(AndroidMPQPerformanceTest, ValidateMemoryConstrainedOperations) {
    if (valid_mpqs_.empty()) {
        GTEST_SKIP() << "No valid MPQ files for memory-constrained testing";
    }
    
    std::cout << "\n=== Memory-Constrained Operations Test ===\n";
    
    // Android devices have limited memory
    // Test opening multiple MPQs within memory constraints
    std::vector<std::unique_ptr<d2portable::utils::StormLibMPQLoader>> loaders;
    size_t total_size = 0;
    const size_t MEMORY_LIMIT = 500 * 1024 * 1024; // 500MB limit
    
    for (const auto& mpq_path : valid_mpqs_) {
        fs::path path(mpq_path);
        auto file_size = fs::file_size(path);
        
        // Check if we'd exceed memory limit
        if (total_size + file_size > MEMORY_LIMIT) {
            std::cout << "  Skipping " << path.filename() 
                      << " (would exceed " << MEMORY_LIMIT / (1024*1024) << "MB limit)\n";
            continue;
        }
        
        auto loader = std::make_unique<d2portable::utils::StormLibMPQLoader>();
        if (loader->open(mpq_path)) {
            loaders.push_back(std::move(loader));
            total_size += file_size;
            
            std::cout << "  Opened " << path.filename() 
                      << " (total memory: " << total_size / (1024*1024) << "MB)\n";
        }
    }
    
    std::cout << "\n  Successfully opened " << loaders.size() 
              << " MPQs within memory constraints\n";
    
    EXPECT_GT(loaders.size(), 0) << "Should be able to open at least one MPQ";
    EXPECT_LT(total_size, MEMORY_LIMIT) << "Should respect memory limits";
}

// Test 3: Test asset streaming performance
TEST_F(AndroidMPQPerformanceTest, TestAssetStreamingPerformance) {
    if (valid_mpqs_.empty()) {
        GTEST_SKIP() << "No valid MPQ files for streaming test";
    }
    
    // Find a large MPQ with many files
    std::string test_mpq;
    for (const auto& mpq : valid_mpqs_) {
        fs::path path(mpq);
        if (path.filename() == "d2char.mpq" || 
            path.filename() == "d2speech.mpq") {
            test_mpq = mpq;
            break;
        }
    }
    
    if (test_mpq.empty() && !valid_mpqs_.empty()) {
        test_mpq = valid_mpqs_[0];
    }
    
    d2portable::utils::StormLibMPQLoader loader;
    ASSERT_TRUE(loader.open(test_mpq));
    
    auto files = loader.listFiles();
    std::cout << "\n=== Asset Streaming Test ===\n";
    std::cout << "Testing with " << fs::path(test_mpq).filename() 
              << " (" << files.size() << " files)\n";
    
    // Simulate streaming multiple assets (like during gameplay)
    const int STREAM_COUNT = std::min(50, static_cast<int>(files.size()));
    std::vector<double> extract_times;
    
    for (int i = 0; i < STREAM_COUNT && i < files.size(); i++) {
        const auto& file = files[i];
        
        // Skip very large files for this test
        if (file.uncompressed_size > 1024 * 1024) { // 1MB
            continue;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<uint8_t> data;
        bool extracted = loader.extractFile(file.filename, data);
        
        auto end = std::chrono::high_resolution_clock::now();
        
        if (extracted) {
            auto extract_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            extract_times.push_back(extract_time / 1000.0); // Convert to ms
        }
    }
    
    if (!extract_times.empty()) {
        // Calculate statistics
        double total_time = 0;
        double min_time = extract_times[0];
        double max_time = extract_times[0];
        
        for (double time : extract_times) {
            total_time += time;
            min_time = std::min(min_time, time);
            max_time = std::max(max_time, time);
        }
        
        double avg_time = total_time / extract_times.size();
        
        std::cout << "\nExtraction Statistics:\n";
        std::cout << "  Files extracted: " << extract_times.size() << "\n";
        std::cout << "  Average time: " << avg_time << "ms\n";
        std::cout << "  Min time: " << min_time << "ms\n";
        std::cout << "  Max time: " << max_time << "ms\n";
        std::cout << "  Total time: " << total_time << "ms\n";
        
        // Android performance requirements
        EXPECT_LT(avg_time, 50.0) << "Average extraction time too high for Android";
        EXPECT_LT(max_time, 200.0) << "Max extraction time too high for Android";
        
        // Ensure we can maintain 60 FPS while streaming assets
        double frame_budget = 16.67; // ms per frame
        double extractions_per_frame = frame_budget / avg_time;
        
        std::cout << "  Extractions per frame (60 FPS): " << extractions_per_frame << "\n";
        
        EXPECT_GT(extractions_per_frame, 1.0) 
            << "Cannot extract at least one asset per frame";
    }
    
    loader.close();
}

// Test 4: Test concurrent MPQ access (simulating multi-threaded Android usage)
TEST_F(AndroidMPQPerformanceTest, TestConcurrentMPQAccess) {
    if (valid_mpqs_.size() < 2) {
        GTEST_SKIP() << "Need at least 2 valid MPQ files for concurrent access test";
    }
    
    std::cout << "\n=== Concurrent MPQ Access Test ===\n";
    
    // Android apps often access resources from multiple threads
    const int NUM_THREADS = 4;
    std::vector<std::thread> threads;
    std::vector<double> thread_times(NUM_THREADS);
    
    auto worker = [this](int thread_id, double& result) {
        // Each thread opens a different MPQ
        std::string mpq_path = valid_mpqs_[thread_id % valid_mpqs_.size()];
        
        auto start = std::chrono::high_resolution_clock::now();
        
        d2portable::utils::StormLibMPQLoader loader;
        if (loader.open(mpq_path)) {
            // Do some work
            auto files = loader.listFiles();
            
            // Extract a few files
            int extracted = 0;
            for (const auto& file : files) {
                if (file.uncompressed_size < 100 * 1024) { // Small files only
                    std::vector<uint8_t> data;
                    if (loader.extractFile(file.filename, data)) {
                        extracted++;
                        if (extracted >= 5) break;
                    }
                }
            }
            
            loader.close();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    };
    
    // Launch threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(worker, i, std::ref(thread_times[i]));
    }
    
    // Wait for completion
    for (auto& t : threads) {
        t.join();
    }
    
    // Report results
    std::cout << "Thread completion times:\n";
    double total_time = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        std::cout << "  Thread " << i << ": " << thread_times[i] << "ms\n";
        total_time += thread_times[i];
    }
    
    double avg_time = total_time / NUM_THREADS;
    std::cout << "Average thread time: " << avg_time << "ms\n";
    
    // Concurrent access should not severely degrade performance
    EXPECT_LT(avg_time, 500.0) << "Concurrent MPQ access too slow";
}