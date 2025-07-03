#include <gtest/gtest.h>
#include "tools/extraction_monitor.h"
#include "tools/asset_extractor.h"
#include <filesystem>
#include <vector>
#include <chrono>
#include <fstream>

namespace fs = std::filesystem;
using namespace d2;

class ExtractionMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        testD2Path = fs::temp_directory_path() / "test_d2_extraction_monitor";
        outputPath = fs::temp_directory_path() / "test_extracted_monitor";
        
        fs::create_directories(testD2Path);
        fs::create_directories(outputPath);
        
        // Create mock MPQ files
        createMockMPQFile(testD2Path / "d2data.mpq");
        createMockMPQFile(testD2Path / "d2char.mpq");
    }
    
    void TearDown() override {
        fs::remove_all(testD2Path);
        fs::remove_all(outputPath);
    }
    
    void createMockMPQFile(const fs::path& path) {
        std::ofstream file(path, std::ios::binary);
        const char signature[] = "MPQ\x1A";
        file.write(signature, 4);
        file.close();
    }
    
    fs::path testD2Path;
    fs::path outputPath;
};

TEST_F(ExtractionMonitorTest, TrackProgressWithCallback) {
    ExtractionMonitor monitor;
    std::vector<ProgressUpdate> updates;
    
    monitor.setProgressCallback([&](const ProgressUpdate& update) {
        updates.push_back(update);
    });
    
    AssetExtractor extractor;
    extractor.setMonitor(&monitor);
    
    extractor.extractFromD2(testD2Path.string(), outputPath.string());
    
    EXPECT_GT(updates.size(), 10); // Multiple progress updates
    EXPECT_FLOAT_EQ(updates.back().percentage, 1.0f); // Completed
    EXPECT_FALSE(updates.back().currentFile.empty());
}