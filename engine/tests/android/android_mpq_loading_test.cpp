#include <gtest/gtest.h>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>
#include <numeric>
#include "utils/stormlib_mpq_loader.h"
#include "core/asset_manager.h"
#include "performance/memory_monitor.h"

namespace fs = std::filesystem;

// Phase 39.1: Test MPQ loading performance on Android
class AndroidMPQLoadingTest : public ::testing::Test {
protected:
    void SetUp() override {
        mpqDir = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
        
        // Check if we have real MPQ files
        if (fs::exists(mpqDir / "d2data.mpq") && fs::file_size(mpqDir / "d2data.mpq") > 10 * 1024 * 1024) {
            hasRealMPQs = true;
        }
    }
    
    fs::path mpqDir;
    bool hasRealMPQs = false;
};

// Test sequential MPQ loading performance
TEST_F(AndroidMPQLoadingTest, SequentialMPQLoadingPerformance) {
    if (!hasRealMPQs) {
        GTEST_SKIP() << "Skipping MPQ performance test - real MPQ files not available";
    }
    
    d2portable::utils::StormLibMPQLoader loader;
    std::vector<std::string> mpqFiles = {"d2data.mpq", "d2char.mpq", "d2music.mpq"};
    
    std::cout << "\n=== Android MPQ Sequential Loading Performance ===\n";
    
    // Simulate Android memory constraints
    const size_t ANDROID_MEMORY_LIMIT = 1536 * 1024 * 1024; // 1.5GB
    d2::MemoryMonitor memMonitor;
    
    for (const auto& mpqName : mpqFiles) {
        fs::path mpqPath = mpqDir / mpqName;
        if (!fs::exists(mpqPath)) continue;
        
        // Measure memory before loading
        size_t memBefore = memMonitor.getCurrentMemoryUsage();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool loaded = loader.open(mpqPath.string());
        
        auto openTime = std::chrono::high_resolution_clock::now();
        auto openDuration = std::chrono::duration_cast<std::chrono::milliseconds>(openTime - startTime);
        
        if (loaded) {
            // Get file list to simulate Android asset enumeration
            auto fileList = loader.listFiles();
            auto listTime = std::chrono::high_resolution_clock::now();
            auto listDuration = std::chrono::duration_cast<std::chrono::milliseconds>(listTime - openTime);
            
            // Simulate extracting a few files (Android would need this for assets)
            int extractCount = 0;
            auto extractStart = std::chrono::high_resolution_clock::now();
            
            for (const auto& fileInfo : fileList) {
                if (extractCount >= 5) break;
                
                std::vector<uint8_t> data;
                if (loader.extractFile(fileInfo.filename, data)) {
                    extractCount++;
                }
            }
            
            auto extractEnd = std::chrono::high_resolution_clock::now();
            auto extractDuration = std::chrono::duration_cast<std::chrono::milliseconds>(extractEnd - extractStart);
            
            // Measure memory after loading
            size_t memAfter = memMonitor.getCurrentMemoryUsage();
            size_t memDelta = memAfter - memBefore;
            
            std::cout << mpqName << ":\n";
            std::cout << "  Open time: " << openDuration.count() << "ms\n";
            std::cout << "  List files time: " << listDuration.count() << "ms\n";
            std::cout << "  Extract 5 files: " << extractDuration.count() << "ms\n";
            std::cout << "  File count: " << fileList.size() << "\n";
            std::cout << "  Memory delta: " << (memDelta / (1024 * 1024)) << "MB\n";
            
            // Android performance expectations
            EXPECT_LT(openDuration.count(), 2000) << mpqName << " takes too long to open on Android";
            EXPECT_LT(listDuration.count(), 1000) << mpqName << " file listing too slow for Android";
            EXPECT_LT(memDelta, 200 * 1024 * 1024) << mpqName << " uses too much memory";
            
            loader.close();
            
            // Small delay to simulate Android activity lifecycle
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

// Test concurrent MPQ access (simulating Android multi-threaded asset loading)
TEST_F(AndroidMPQLoadingTest, ConcurrentMPQAccessPerformance) {
    if (!hasRealMPQs) {
        GTEST_SKIP() << "Skipping concurrent MPQ test - real MPQ files not available";
    }
    
    fs::path d2dataPath = mpqDir / "d2data.mpq";
    if (!fs::exists(d2dataPath)) {
        GTEST_SKIP() << "d2data.mpq not found";
    }
    
    std::cout << "\n=== Android Concurrent MPQ Access Performance ===\n";
    
    const int NUM_THREADS = 4; // Typical Android core count
    const int REQUESTS_PER_THREAD = 10;
    
    // Open MPQ once (shared across threads)
    d2portable::utils::StormLibMPQLoader sharedLoader;
    ASSERT_TRUE(sharedLoader.open(d2dataPath.string()));
    
    auto fileList = sharedLoader.listFiles();
    ASSERT_GT(fileList.size(), 100) << "Need enough files for concurrent testing";
    
    // Create worker threads
    std::vector<std::thread> workers;
    std::vector<double> threadTimes(NUM_THREADS);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int t = 0; t < NUM_THREADS; t++) {
        workers.emplace_back([&, t]() {
            auto threadStart = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < REQUESTS_PER_THREAD; i++) {
                // Each thread extracts different files
                int fileIndex = (t * REQUESTS_PER_THREAD + i) % fileList.size();
                std::vector<uint8_t> data;
                sharedLoader.extractFile(fileList[fileIndex].filename, data);
            }
            
            auto threadEnd = std::chrono::high_resolution_clock::now();
            threadTimes[t] = std::chrono::duration_cast<std::chrono::milliseconds>(threadEnd - threadStart).count();
        });
    }
    
    // Wait for all threads
    for (auto& worker : workers) {
        worker.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Calculate average thread time
    double avgThreadTime = std::accumulate(threadTimes.begin(), threadTimes.end(), 0.0) / NUM_THREADS;
    
    std::cout << "Total time: " << totalDuration.count() << "ms\n";
    std::cout << "Average thread time: " << avgThreadTime << "ms\n";
    std::cout << "Speedup vs sequential: " << (avgThreadTime * NUM_THREADS) / totalDuration.count() << "x\n";
    
    // Concurrent access should provide good speedup
    EXPECT_LT(totalDuration.count(), avgThreadTime * NUM_THREADS * 0.7) 
        << "Concurrent access should provide at least 30% speedup";
    
    sharedLoader.close();
}

// Test MPQ memory mapping efficiency on Android
TEST_F(AndroidMPQLoadingTest, MemoryMappingEfficiency) {
    if (!hasRealMPQs) {
        GTEST_SKIP() << "Skipping memory mapping test - real MPQ files not available";
    }
    
    fs::path d2charPath = mpqDir / "d2char.mpq";
    if (!fs::exists(d2charPath)) {
        GTEST_SKIP() << "d2char.mpq not found";
    }
    
    std::cout << "\n=== Android Memory Mapping Efficiency ===\n";
    
    d2portable::utils::StormLibMPQLoader loader;
    d2::MemoryMonitor memMonitor;
    
    // Baseline memory
    size_t baselineMemory = memMonitor.getCurrentMemoryUsage();
    
    // Open MPQ
    ASSERT_TRUE(loader.open(d2charPath.string()));
    size_t afterOpenMemory = memMonitor.getCurrentMemoryUsage();
    
    // List all files
    auto fileList = loader.listFiles();
    size_t afterListMemory = memMonitor.getCurrentMemoryUsage();
    
    // Extract multiple DC6 sprites
    int spritesExtracted = 0;
    size_t totalDataSize = 0;
    
    for (const auto& fileInfo : fileList) {
        if (fileInfo.filename.find(".dc6") != std::string::npos || 
            fileInfo.filename.find(".DC6") != std::string::npos) {
            
            std::vector<uint8_t> data;
            if (loader.extractFile(fileInfo.filename, data)) {
                spritesExtracted++;
                totalDataSize += data.size();
                
                if (spritesExtracted >= 20) break;
            }
        }
    }
    
    size_t afterExtractMemory = memMonitor.getCurrentMemoryUsage();
    
    // Calculate memory deltas
    size_t openDelta = afterOpenMemory - baselineMemory;
    size_t listDelta = afterListMemory - afterOpenMemory;
    size_t extractDelta = afterExtractMemory - afterListMemory;
    
    std::cout << "Memory usage:\n";
    std::cout << "  MPQ open overhead: " << (openDelta / (1024 * 1024)) << "MB\n";
    std::cout << "  File list overhead: " << (listDelta / (1024 * 1024)) << "MB\n";
    std::cout << "  Extract overhead: " << (extractDelta / (1024 * 1024)) << "MB\n";
    std::cout << "  Total data extracted: " << (totalDataSize / (1024 * 1024)) << "MB\n";
    std::cout << "  Memory efficiency: " << ((double)totalDataSize / extractDelta) << "x\n";
    
    // Memory efficiency expectations for Android
    EXPECT_LT(openDelta, 50 * 1024 * 1024) << "MPQ open uses too much memory";
    EXPECT_LT(listDelta, 10 * 1024 * 1024) << "File listing uses too much memory";
    EXPECT_GT((double)totalDataSize / extractDelta, 0.8) << "Poor memory efficiency during extraction";
    
    loader.close();
}

// Test AssetManager integration for Android
TEST_F(AndroidMPQLoadingTest, AssetManagerAndroidIntegration) {
    d2portable::core::AssetManager assetManager;
    
    std::cout << "\n=== Android AssetManager Integration ===\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool initialized = assetManager.initialize(mpqDir.string());
    
    auto initTime = std::chrono::high_resolution_clock::now();
    auto initDuration = std::chrono::duration_cast<std::chrono::milliseconds>(initTime - startTime);
    
    std::cout << "AssetManager initialization time: " << initDuration.count() << "ms\n";
    
    if (hasRealMPQs) {
        EXPECT_TRUE(initialized) << "AssetManager should initialize with real MPQs";
        EXPECT_LT(initDuration.count(), 5000) << "AssetManager init too slow for Android";
        
        // Test asset lookup performance
        std::vector<std::string> testAssets = {
            "data/global/ui/panel/invchar6.dc6",
            "data/global/excel/armor.txt",
            "data/global/sfx/cursor/button.wav"
        };
        
        for (const auto& assetPath : testAssets) {
            auto lookupStart = std::chrono::high_resolution_clock::now();
            
            bool hasAsset = assetManager.hasFile(assetPath);
            
            auto lookupEnd = std::chrono::high_resolution_clock::now();
            auto lookupTime = std::chrono::duration_cast<std::chrono::microseconds>(lookupEnd - lookupStart).count();
            
            std::cout << "Asset lookup '" << assetPath << "': " 
                      << (hasAsset ? "found" : "not found") 
                      << " in " << lookupTime << "μs\n";
            
            // Asset lookups should be very fast on Android
            EXPECT_LT(lookupTime, 1000) << "Asset lookup too slow for Android UI thread";
        }
    } else {
        // Should still initialize in fallback mode
        EXPECT_TRUE(initialized) << "AssetManager should initialize even without MPQs";
        std::cout << "AssetManager initialized in fallback mode (no real MPQs)\n";
    }
}