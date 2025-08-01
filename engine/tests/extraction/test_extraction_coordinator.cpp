#include <gtest/gtest.h>
#include "extraction/extraction_coordinator.h"
#include <filesystem>
#include <string>
#include <fstream>

namespace d2 {

class ExtractionCoordinatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directories for testing
        tempDir = std::filesystem::temp_directory_path() / "test_extraction_coordinator";
        std::filesystem::create_directories(tempDir);
        
        outputDir = tempDir / "output";
        std::filesystem::create_directories(outputDir);
    }
    
    void TearDown() override {
        // Clean up temp directories
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    std::filesystem::path tempDir;
    std::filesystem::path outputDir;
};

// Test that we can create an ExtractionCoordinator instance
TEST_F(ExtractionCoordinatorTest, CanCreateExtractionCoordinator) {
    ExtractionCoordinator coordinator;
    
    // If we can create it without errors, the test passes
    EXPECT_TRUE(true);
}

// Test that ExtractionCoordinator can extract from a source path to output directory
TEST_F(ExtractionCoordinatorTest, CanExtractFromSourceToOutput) {
    ExtractionCoordinator coordinator;
    
    // Create a mock source file to test with
    std::filesystem::path sourcePath = tempDir / "test_source.iso";
    std::ofstream sourceFile(sourcePath);
    sourceFile << "dummy iso content";
    sourceFile.close();
    
    // The coordinator should be able to extract from any source to output
    bool result = coordinator.extractFrom(sourcePath.string(), outputDir.string());
    
    // This should succeed with proper implementation
    EXPECT_TRUE(result);
}

// Test that ExtractionCoordinator can detect different source types
TEST_F(ExtractionCoordinatorTest, CanDetectSourceType) {
    ExtractionCoordinator coordinator;
    
    // Test ISO file detection
    std::filesystem::path isoPath = tempDir / "test.iso";
    std::ofstream isoFile(isoPath);
    isoFile << "dummy iso content";
    isoFile.close();
    
    std::string sourceType = coordinator.detectSourceType(isoPath.string());
    EXPECT_EQ(sourceType, "ISO");
    
    // Test MPQ file detection
    std::filesystem::path mpqPath = tempDir / "test.mpq";
    std::ofstream mpqFile(mpqPath);
    mpqFile << "dummy mpq content";
    mpqFile.close();
    
    sourceType = coordinator.detectSourceType(mpqPath.string());
    EXPECT_EQ(sourceType, "MPQ");
}

// Test that ExtractionCoordinator can provide progress updates during extraction
TEST_F(ExtractionCoordinatorTest, CanProvideProgressUpdates) {
    ExtractionCoordinator coordinator;
    
    // Set up progress tracking variables
    bool progressCallbackCalled = false;
    float lastProgress = -1.0f;
    std::string lastFile;
    
    // Set up progress callback
    coordinator.setProgressCallback([&](float progress, const std::string& currentFile) {
        progressCallbackCalled = true;
        lastProgress = progress;
        lastFile = currentFile;
    });
    
    // Create a mock source file
    std::filesystem::path sourcePath = tempDir / "test_source.mpq";
    std::ofstream sourceFile(sourcePath);
    sourceFile << "dummy mpq content";
    sourceFile.close();
    
    // Extract and verify progress was reported
    bool result = coordinator.extractFrom(sourcePath.string(), outputDir.string());
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(progressCallbackCalled);
    EXPECT_GE(lastProgress, 0.0f);
    EXPECT_LE(lastProgress, 1.0f);
}

// Test that ExtractionCoordinator has access to existing extraction systems
TEST_F(ExtractionCoordinatorTest, CanAccessExistingExtractionSystems) {
    ExtractionCoordinator coordinator;
    
    // The coordinator should be able to indicate if it has the required extractors
    bool hasISOExtractor = coordinator.hasISOExtractor();
    bool hasPatchSystem = coordinator.hasPatchSystem();
    bool hasAssetExtractor = coordinator.hasAssetExtractor();
    
    // These should all be true for a properly integrated coordinator
    EXPECT_TRUE(hasISOExtractor);
    EXPECT_TRUE(hasPatchSystem);
    EXPECT_TRUE(hasAssetExtractor);
}

} // namespace d2