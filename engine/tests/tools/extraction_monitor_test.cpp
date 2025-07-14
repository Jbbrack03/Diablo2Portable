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
    
    // With mock MPQ files, we get fewer progress updates
    EXPECT_GE(updates.size(), 2u); // At least start and end updates
    EXPECT_FLOAT_EQ(updates.back().percentage, 1.0f); // Completed
    
    // Check that we got some meaningful progress updates
    bool hasStartUpdate = false;
    bool hasEndUpdate = false;
    for (const auto& update : updates) {
        if (update.percentage == 0.0f || update.percentage < 0.1f) {
            hasStartUpdate = true;
        }
        if (update.percentage == 1.0f) {
            hasEndUpdate = true;
        }
    }
    EXPECT_TRUE(hasStartUpdate) << "Should have a start progress update";
    EXPECT_TRUE(hasEndUpdate) << "Should have an end progress update";
}

TEST_F(ExtractionMonitorTest, EstimateTimeRemaining) {
    ExtractionMonitor monitor;
    
    // Simulate 50% completion after 30 seconds
    monitor.updateProgress(0.5f, "file.dc6", 30000);
    
    auto estimate = monitor.getTimeRemaining();
    
    EXPECT_NEAR(estimate.totalSeconds, 30.0, 5.0); // ~30 seconds remaining
    EXPECT_TRUE(estimate.isReliable);
}

TEST_F(ExtractionMonitorTest, HandleExtractionErrors) {
    ExtractionMonitor monitor;
    std::vector<ExtractionError> errors;
    
    monitor.setErrorCallback([&](const ExtractionError& error) {
        errors.push_back(error);
    });
    
    // Simulate extraction with corrupted file
    AssetExtractor extractor;
    extractor.setMonitor(&monitor);
    
    // Create a corrupted MPQ file for testing
    fs::path corruptedMPQ = testD2Path / "corrupted.mpq";
    std::ofstream file(corruptedMPQ, std::ios::binary);
    file << "CORRUPT_DATA";
    file.close();
    
    extractor.extractFromD2(testD2Path.string(), outputPath.string());
    
    EXPECT_GT(errors.size(), 0);
    EXPECT_EQ(errors[0].type, ErrorType::CORRUPTED_MPQ);
    EXPECT_FALSE(errors[0].isRecoverable);
}