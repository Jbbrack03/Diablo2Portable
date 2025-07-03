#include <gtest/gtest.h>
#include "tools/asset_verifier.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;
using namespace d2;

class AssetVerifierTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        extractedPath = fs::temp_directory_path() / "test_extracted_assets";
        incompletePath = fs::temp_directory_path() / "test_incomplete_assets";
        
        fs::create_directories(extractedPath);
        fs::create_directories(incompletePath);
        
        // Create test asset structure
        createTestAssets();
    }
    
    void TearDown() override {
        fs::remove_all(extractedPath);
        fs::remove_all(incompletePath);
    }
    
    void createTestAssets() {
        // Create a complete asset structure
        std::vector<std::string> criticalFiles = {
            "data/global/ui/panel/invchar6.dc6",
            "data/global/chars/ba/cof/bacof.d2",
            "data/global/sfx/cursor/button.wav"
        };
        
        for (const auto& file : criticalFiles) {
            fs::path fullPath = extractedPath / file;
            fs::create_directories(fullPath.parent_path());
            
            std::ofstream out(fullPath, std::ios::binary);
            // Write some dummy data
            for (int i = 0; i < 100; ++i) {
                out.put(static_cast<char>(i));
            }
        }
        
        // Create incomplete asset structure (missing first file)
        for (size_t i = 1; i < criticalFiles.size(); ++i) {
            fs::path fullPath = incompletePath / criticalFiles[i];
            fs::create_directories(fullPath.parent_path());
            
            std::ofstream out(fullPath, std::ios::binary);
            for (int j = 0; j < 100; ++j) {
                out.put(static_cast<char>(j));
            }
        }
    }
    
    fs::path extractedPath;
    fs::path incompletePath;
};

TEST_F(AssetVerifierTest, ValidateExtractedAssets) {
    AssetVerifier verifier;
    
    auto verification = verifier.fullVerification(extractedPath.string());
    
    EXPECT_TRUE(verification.isComplete);
    EXPECT_EQ(verification.corruptedFiles.size(), 0);
    EXPECT_GT(verification.validatedFiles, 0);
    EXPECT_TRUE(verification.hasRequiredAssets());
}

TEST_F(AssetVerifierTest, DetectMissingCriticalFiles) {
    AssetVerifier verifier;
    
    // Use the incomplete assets directory which is missing critical files
    auto verification = verifier.fullVerification(incompletePath.string());
    
    EXPECT_FALSE(verification.isComplete);
    EXPECT_GT(verification.missingCriticalFiles.size(), 0);
    EXPECT_TRUE(verification.canAttemptRepair());
}

TEST_F(AssetVerifierTest, GenerateChecksumManifest) {
    AssetVerifier verifier;
    
    auto manifest = verifier.generateChecksumManifest(extractedPath.string());
    
    EXPECT_GT(manifest.fileCount, 0);
    EXPECT_FALSE(manifest.manifestChecksum.empty());
    
    // Verify manifest integrity
    EXPECT_TRUE(verifier.validateManifest(manifest));
}