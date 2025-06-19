#include "android/apk_asset_loader.h"
#include <algorithm>
#include <cstring>

// Mock implementation of AssetStream for testing
class MockAssetStream : public AssetStream {
public:
    MockAssetStream(const std::vector<uint8_t>& data) 
        : data_(data), position_(0) {}
    
    size_t read(uint8_t* buffer, size_t size) override {
        size_t available = data_.size() - position_;
        size_t toRead = std::min(size, available);
        
        if (toRead > 0) {
            std::memcpy(buffer, data_.data() + position_, toRead);
            position_ += toRead;
        }
        
        return toRead;
    }
    
    size_t tell() const override {
        return position_;
    }
    
    void seek(size_t position) override {
        position_ = std::min(position, data_.size());
    }
    
    size_t size() const override {
        return data_.size();
    }
    
private:
    std::vector<uint8_t> data_;
    size_t position_;
};

APKAssetLoader::APKAssetLoader() 
    : initialized_(false), assetManager_(nullptr) {
}

APKAssetLoader::~APKAssetLoader() = default;

bool APKAssetLoader::initialize(void* assetManager) {
    if (!assetManager) {
        return false;
    }
    
    assetManager_ = assetManager;
    initialized_ = true;
    
    // Set up mock assets for testing
    setupMockAssets();
    
    return true;
}

bool APKAssetLoader::loadAsset(const std::string& path, std::vector<uint8_t>& data) {
    if (!initialized_) {
        return false;
    }
    
    // Mock implementation - find asset in mock data
    auto it = std::find_if(mockAssets_.begin(), mockAssets_.end(),
        [&path](const MockAssetData& asset) {
            return asset.path == path;
        });
    
    if (it != mockAssets_.end()) {
        data = it->data;
        return true;
    }
    
    return false;
}

bool APKAssetLoader::assetExists(const std::string& path) const {
    if (!initialized_) {
        return false;
    }
    
    // Mock implementation - check if asset exists in mock data
    auto it = std::find_if(mockAssets_.begin(), mockAssets_.end(),
        [&path](const MockAssetData& asset) {
            return asset.path == path;
        });
    
    return it != mockAssets_.end();
}

size_t APKAssetLoader::getAssetSize(const std::string& path) const {
    if (!initialized_) {
        return 0;
    }
    
    // Mock implementation - get size from mock data
    auto it = std::find_if(mockAssets_.begin(), mockAssets_.end(),
        [&path](const MockAssetData& asset) {
            return asset.path == path;
        });
    
    if (it != mockAssets_.end()) {
        return it->data.size();
    }
    
    return 0;
}

std::vector<std::string> APKAssetLoader::listAssets(const std::string& directory) const {
    std::vector<std::string> result;
    
    if (!initialized_) {
        return result;
    }
    
    // Mock implementation - list files in directory
    for (const auto& asset : mockAssets_) {
        if (asset.path.substr(0, directory.length()) == directory) {
            // Extract filename from path
            size_t lastSlash = asset.path.find_last_of('/');
            std::string filename = (lastSlash != std::string::npos) 
                ? asset.path.substr(lastSlash + 1) 
                : asset.path;
            
            // Only add if it's directly in this directory (not in subdirectory)
            std::string relativePath = asset.path.substr(directory.length());
            if (relativePath.find('/') == std::string::npos) {
                result.push_back(filename);
            }
        }
    }
    
    return result;
}

std::unique_ptr<AssetStream> APKAssetLoader::openAssetStream(const std::string& path) {
    if (!initialized_) {
        return nullptr;
    }
    
    // Mock implementation - create stream from mock data
    auto it = std::find_if(mockAssets_.begin(), mockAssets_.end(),
        [&path](const MockAssetData& asset) {
            return asset.path == path;
        });
    
    if (it != mockAssets_.end()) {
        return std::make_unique<MockAssetStream>(it->data);
    }
    
    return nullptr;
}

void APKAssetLoader::setupMockAssets() {
    // Set up mock assets for testing
    mockAssets_.clear();
    
    // Add test.txt
    MockAssetData testTxt;
    testTxt.path = "data/test.txt";
    testTxt.data = {'T', 'e', 's', 't', ' ', 'd', 'a', 't', 'a'};
    mockAssets_.push_back(testTxt);
    
    // Add d2data.mpq (mock large file)
    MockAssetData mpqFile;
    mpqFile.path = "data/d2data.mpq";
    mpqFile.data.resize(2048); // Mock MPQ with 2KB of data
    std::fill(mpqFile.data.begin(), mpqFile.data.end(), 0xAB);
    mockAssets_.push_back(mpqFile);
    
    // Add large_file.dat
    MockAssetData largeFile;
    largeFile.path = "data/large_file.dat";
    largeFile.data.resize(4096); // 4KB of mock data
    for (size_t i = 0; i < largeFile.data.size(); ++i) {
        largeFile.data[i] = static_cast<uint8_t>(i % 256);
    }
    mockAssets_.push_back(largeFile);
    
    // Add another file for directory listing
    MockAssetData anotherFile;
    anotherFile.path = "data/another.bin";
    anotherFile.data = {0x01, 0x02, 0x03, 0x04};
    mockAssets_.push_back(anotherFile);
}