#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "android/apk_asset_loader.h"
#include <vector>
#include <cstring>

using namespace testing;

class APKAssetLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        loader = std::make_unique<APKAssetLoader>();
    }

    std::unique_ptr<APKAssetLoader> loader;
};

TEST_F(APKAssetLoaderTest, InitializeWithAssetManager) {
    // Test initializing the loader with Android AssetManager
    void* mockAssetManager = reinterpret_cast<void*>(0x12345678); // Mock pointer
    
    bool result = loader->initialize(mockAssetManager);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(loader->isInitialized());
}

TEST_F(APKAssetLoaderTest, LoadAssetFromAPK) {
    // Test loading an asset file from APK
    void* mockAssetManager = reinterpret_cast<void*>(0x12345678);
    loader->initialize(mockAssetManager);
    
    std::vector<uint8_t> data;
    bool result = loader->loadAsset("data/test.txt", data);
    
    EXPECT_TRUE(result);
    EXPECT_GT(data.size(), 0);
}

TEST_F(APKAssetLoaderTest, CheckAssetExists) {
    // Test checking if an asset exists in APK
    void* mockAssetManager = reinterpret_cast<void*>(0x12345678);
    loader->initialize(mockAssetManager);
    
    EXPECT_TRUE(loader->assetExists("data/test.txt"));
    EXPECT_FALSE(loader->assetExists("data/nonexistent.txt"));
}

TEST_F(APKAssetLoaderTest, GetAssetSize) {
    // Test getting the size of an asset
    void* mockAssetManager = reinterpret_cast<void*>(0x12345678);
    loader->initialize(mockAssetManager);
    
    size_t size = loader->getAssetSize("data/test.txt");
    
    EXPECT_GT(size, 0);
}

TEST_F(APKAssetLoaderTest, LoadMPQFromAPK) {
    // Test loading an MPQ file from APK assets
    void* mockAssetManager = reinterpret_cast<void*>(0x12345678);
    loader->initialize(mockAssetManager);
    
    std::vector<uint8_t> mpqData;
    bool result = loader->loadAsset("data/d2data.mpq", mpqData);
    
    EXPECT_TRUE(result);
    EXPECT_GT(mpqData.size(), 1024); // MPQ files are large
}

TEST_F(APKAssetLoaderTest, ListAssetsInDirectory) {
    // Test listing assets in a directory
    void* mockAssetManager = reinterpret_cast<void*>(0x12345678);
    loader->initialize(mockAssetManager);
    
    auto files = loader->listAssets("data/");
    
    EXPECT_GT(files.size(), 0);
    EXPECT_NE(std::find(files.begin(), files.end(), "test.txt"), files.end());
}

TEST_F(APKAssetLoaderTest, LoadWithoutInitialization) {
    // Test that loading fails without initialization
    std::vector<uint8_t> data;
    bool result = loader->loadAsset("data/test.txt", data);
    
    EXPECT_FALSE(result);
    EXPECT_EQ(data.size(), 0);
}

TEST_F(APKAssetLoaderTest, StreamLargeAsset) {
    // Test streaming a large asset in chunks
    void* mockAssetManager = reinterpret_cast<void*>(0x12345678);
    loader->initialize(mockAssetManager);
    
    auto stream = loader->openAssetStream("data/large_file.dat");
    EXPECT_NE(stream, nullptr);
    
    uint8_t buffer[1024];
    size_t bytesRead = stream->read(buffer, sizeof(buffer));
    
    EXPECT_GT(bytesRead, 0);
    EXPECT_LE(bytesRead, sizeof(buffer));
}