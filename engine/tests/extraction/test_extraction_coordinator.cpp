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

} // namespace d2