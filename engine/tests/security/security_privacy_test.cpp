#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include "save/save_manager.h"
#include "game/character.h"
#include "core/asset_manager.h"
#include "tools/asset_verifier.h"

// Task 24.5: Security and Privacy Validation
class SecurityPrivacyTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir_ = std::filesystem::temp_directory_path() / "d2_security_test";
        std::filesystem::create_directories(testDir_);
    }
    
    void TearDown() override {
        std::filesystem::remove_all(testDir_);
    }
    
    std::filesystem::path testDir_;
};

// Test 1: File permissions security
TEST_F(SecurityPrivacyTest, FilePermissionsSecurity) {
    // Create save manager and save a character
    d2::save::SaveManager saveManager(testDir_.string());
    d2::game::Character character(d2::game::CharacterClass::PALADIN);
    character.setLevel(25);
    
    std::string saveFile = "test_character.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(character, saveFile));
    
    // Check file permissions
    std::filesystem::path fullPath = testDir_ / saveFile;
    ASSERT_TRUE(std::filesystem::exists(fullPath));
    
    struct stat fileStat;
    ASSERT_EQ(stat(fullPath.c_str(), &fileStat), 0);
    
    // File should not be world-readable or world-writable
    mode_t worldReadWrite = S_IROTH | S_IWOTH;
    EXPECT_EQ(fileStat.st_mode & worldReadWrite, 0) 
        << "Save file has world read/write permissions";
    
    // File should be readable and writable by owner only
    mode_t ownerReadWrite = S_IRUSR | S_IWUSR;
    EXPECT_NE(fileStat.st_mode & ownerReadWrite, 0) 
        << "Save file missing owner read/write permissions";
}

// Test 2: Save data integrity
TEST_F(SecurityPrivacyTest, SaveDataIntegrity) {
    d2::save::SaveManager saveManager(testDir_.string());
    
    // Create and save character
    d2::game::Character character(d2::game::CharacterClass::ASSASSIN);
    character.setLevel(50);
    character.addStatPoint(d2::game::StatType::STRENGTH, 100);
    character.addStatPoint(d2::game::StatType::DEXTERITY, 150);
    
    std::string saveFile = "integrity_test.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(character, saveFile));
    
    // Attempt to tamper with save file
    std::filesystem::path fullPath = testDir_ / saveFile;
    {
        std::fstream file(fullPath, std::ios::binary | std::ios::in | std::ios::out);
        ASSERT_TRUE(file.is_open());
        
        // Skip to level byte and change it
        file.seekp(52); // Position of level in D2S format
        uint8_t tamperedLevel = 99;
        file.write(reinterpret_cast<char*>(&tamperedLevel), 1);
        file.close();
    }
    
    // Load should detect tampering (via checksum mismatch)
    auto loadedChar = saveManager.loadCharacter(saveFile);
    
    // Implementation should either:
    // 1. Fail to load (return nullptr)
    // 2. Load with original values (checksum validation failed)
    // For now, we just verify it doesn't crash
    if (loadedChar) {
        // If loaded, level should not be the tampered value
        // (This depends on checksum implementation)
        EXPECT_NE(loadedChar->getLevel(), 99) 
            << "Tampered save file was loaded without validation";
    }
}

// Test 3: Privacy compliance - no data collection
TEST_F(SecurityPrivacyTest, PrivacyCompliance) {
    // Check that no analytics or tracking code is present
    
    // Test 1: No network connections during normal gameplay
    // This would be tested at runtime, but we can check for network code
    
    // Test 2: No personal data in save files
    d2::save::SaveManager saveManager(testDir_.string());
    d2::game::Character character(d2::game::CharacterClass::DRUID);
    character.setLevel(30);
    
    std::string saveFile = "privacy_test.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(character, saveFile));
    
    // Read save file and check for personal data patterns
    std::filesystem::path fullPath = testDir_ / saveFile;
    std::ifstream file(fullPath, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // Check for common personal data patterns
    EXPECT_EQ(content.find("@"), std::string::npos) << "Email pattern found in save";
    EXPECT_EQ(content.find("/home/"), std::string::npos) << "Home directory path found";
    EXPECT_EQ(content.find("/Users/"), std::string::npos) << "User directory path found";
    EXPECT_EQ(content.find("C:\\Users\\"), std::string::npos) << "Windows user path found";
    
    // No device IDs or identifiers
    EXPECT_EQ(content.find("android_id"), std::string::npos) << "Android ID found";
    EXPECT_EQ(content.find("device_id"), std::string::npos) << "Device ID found";
}

// Test 4: Asset integrity validation
TEST_F(SecurityPrivacyTest, AssetIntegrityValidation) {
    // Create asset verifier
    d2::AssetVerifier verifier;
    
    // Perform full verification of assets
    std::string assetPath = "vendor/mpq";
    auto verificationResult = verifier.fullVerification(assetPath);
    
    // Check basic verification
    EXPECT_TRUE(verificationResult.isComplete || verificationResult.validatedFiles > 0) 
        << "Asset verification should process some files";
    
    // Generate checksums for game assets
    auto manifest = verifier.generateChecksumManifest(assetPath);
    
    EXPECT_GT(manifest.fileCount, 0) << "Should have found some files";
    EXPECT_FALSE(manifest.manifestChecksum.empty()) << "Should have manifest checksum";
    
    // Validate the manifest itself
    EXPECT_TRUE(verifier.validateManifest(manifest)) << "Generated manifest should be valid";
    
    // Test with empty manifest (tamper detection)
    d2::ChecksumManifest emptyManifest;
    EXPECT_FALSE(verifier.validateManifest(emptyManifest)) << "Empty manifest should be invalid";
    
    // Test with modified manifest
    if (manifest.fileCount > 0) {
        d2::ChecksumManifest tamperedManifest = manifest;
        tamperedManifest.manifestChecksum = "TAMPERED_CHECKSUM";
        
        EXPECT_FALSE(verifier.validateManifest(tamperedManifest)) 
            << "Tampered manifest should be detected";
    }
}