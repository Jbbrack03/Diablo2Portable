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