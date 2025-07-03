#include <gtest/gtest.h>
#include "tools/differential_extractor.h"
#include "tools/asset_manifest.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;
using namespace d2;

class DifferentialExtractorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        testD2Path = fs::temp_directory_path() / "test_d2_differential";
        outputPath = fs::temp_directory_path() / "test_extracted_differential";
        
        fs::create_directories(testD2Path);
        fs::create_directories(outputPath);
        
        // Create test MPQ files
        createTestMPQFile(testD2Path / "d2data.mpq", "v1");
        createTestMPQFile(testD2Path / "d2char.mpq", "v1");
    }
    
    void TearDown() override {
        fs::remove_all(testD2Path);
        fs::remove_all(outputPath);
    }
    
    void createTestMPQFile(const fs::path& path, const std::string& version) {
        std::ofstream file(path, std::ios::binary);
        const char signature[] = "MPQ\x1A";
        file.write(signature, 4);
        file << version; // Add version marker
        file.close();
    }
    
    void modifyTestFile(const fs::path& mpqPath) {
        // Append data to simulate file modification
        std::ofstream file(mpqPath, std::ios::binary | std::ios::app);
        file << "_modified";
        file.close();
    }
    
    fs::path testD2Path;
    fs::path outputPath;
};

TEST_F(DifferentialExtractorTest, DetectChangedFiles) {
    DifferentialExtractor extractor;
    
    // Initial extraction
    extractor.fullExtraction(testD2Path.string(), outputPath.string() + "_v1");
    auto manifest1 = extractor.generateManifest(outputPath.string() + "_v1");
    
    // Simulate file changes
    modifyTestFile(testD2Path / "d2data.mpq");
    
    // Differential extraction
    auto changes = extractor.detectChanges(testD2Path.string(), *manifest1);
    
    EXPECT_GT(changes.modifiedFiles.size(), 0);
    EXPECT_TRUE(changes.hasFile("d2data.mpq"));
    EXPECT_EQ(changes.changeType("d2data.mpq"), ChangeType::MODIFIED);
}

TEST_F(DifferentialExtractorTest, IncrementalUpdate) {
    DifferentialExtractor extractor;
    
    // Create base extraction with some test files
    fs::create_directories(outputPath / "sprites");
    fs::create_directories(outputPath / "sounds");
    
    // Create initial files
    std::ofstream(outputPath / "sprites" / "test1.dc6") << "original_content_1";
    std::ofstream(outputPath / "sprites" / "test2.dc6") << "original_content_2";
    std::ofstream(outputPath / "sounds" / "test.wav") << "original_sound";
    
    // Generate base manifest
    auto baseManifest = extractor.generateManifest(outputPath.string());
    
    // Modify one file
    std::ofstream(outputPath / "sprites" / "test1.dc6") << "modified_content_1";
    
    // Add a new file
    std::ofstream(outputPath / "sprites" / "test3.dc6") << "new_content";
    
    // Perform incremental update
    auto result = extractor.incrementalUpdate(
        outputPath.string(),  // Source path (where modified files are)
        outputPath.string() + "_updated",  // Destination path
        *baseManifest
    );
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.filesUpdated, 2); // 1 modified + 1 new
    EXPECT_GT(result.extractionTime, 0.0); // Should have taken some time
    EXPECT_GT(result.estimatedFullExtractionTime, result.extractionTime); // Full extraction should take longer
    
    // Verify only changed files were processed
    fs::path updatedPath = outputPath.string() + "_updated";
    EXPECT_TRUE(fs::exists(updatedPath / "sprites" / "test1.dc6"));
    EXPECT_TRUE(fs::exists(updatedPath / "sprites" / "test3.dc6"));
    
    // Unchanged file should also exist (copied from base)
    EXPECT_TRUE(fs::exists(updatedPath / "sprites" / "test2.dc6"));
    EXPECT_TRUE(fs::exists(updatedPath / "sounds" / "test.wav"));
}