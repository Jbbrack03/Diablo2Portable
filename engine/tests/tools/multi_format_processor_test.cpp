#include <gtest/gtest.h>
#include "tools/multi_format_processor.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;
using namespace d2;

class MultiFormatProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        testPath = fs::temp_directory_path() / "test_multi_format";
        outputPath = fs::temp_directory_path() / "test_multi_format_output";
        
        fs::create_directories(testPath);
        fs::create_directories(outputPath);
        
        // Create a simple DC6 file for testing
        createTestDC6File();
    }
    
    void TearDown() override {
        fs::remove_all(testPath);
        fs::remove_all(outputPath);
    }
    
    void createTestDC6File() {
        fs::path dc6Path = testPath / "test_sprite.dc6";
        std::ofstream file(dc6Path, std::ios::binary);
        
        // DC6 header
        uint32_t version = 6;
        uint32_t unknown1 = 1;
        uint32_t unknown2 = 0;
        uint32_t termination = 0xEEEEEEEE;
        uint32_t directions = 1;
        uint32_t frames = 1;
        
        file.write(reinterpret_cast<const char*>(&version), 4);
        file.write(reinterpret_cast<const char*>(&unknown1), 4);
        file.write(reinterpret_cast<const char*>(&unknown2), 4);
        file.write(reinterpret_cast<const char*>(&termination), 4);
        file.write(reinterpret_cast<const char*>(&directions), 4);
        file.write(reinterpret_cast<const char*>(&frames), 4);
        
        // Frame pointer
        uint32_t framePointer = 24 + 4; // After header + pointer
        file.write(reinterpret_cast<const char*>(&framePointer), 4);
        
        // Frame header
        uint32_t flip = 0;
        uint32_t width = 10;
        uint32_t height = 10;
        int32_t offsetX = 0;
        int32_t offsetY = 0;
        uint32_t unknown = 0;
        uint32_t nextBlock = 0;
        uint32_t length = 100; // 10x10 pixels
        
        file.write(reinterpret_cast<const char*>(&flip), 4);
        file.write(reinterpret_cast<const char*>(&width), 4);
        file.write(reinterpret_cast<const char*>(&height), 4);
        file.write(reinterpret_cast<const char*>(&offsetX), 4);
        file.write(reinterpret_cast<const char*>(&offsetY), 4);
        file.write(reinterpret_cast<const char*>(&unknown), 4);
        file.write(reinterpret_cast<const char*>(&nextBlock), 4);
        file.write(reinterpret_cast<const char*>(&length), 4);
        
        // Frame data - simple gradient
        for (uint32_t i = 0; i < 100; ++i) {
            uint8_t pixel = static_cast<uint8_t>(i * 2);
            file.write(reinterpret_cast<const char*>(&pixel), 1);
        }
    }
    
    void createTestPaletteFile(const fs::path& path) {
        std::ofstream file(path, std::ios::binary);
        
        // D2 palette format: 256 colors * 3 bytes (RGB)
        for (int i = 0; i < 256; ++i) {
            uint8_t r = static_cast<uint8_t>(i);
            uint8_t g = static_cast<uint8_t>(i);
            uint8_t b = static_cast<uint8_t>(i);
            
            file.write(reinterpret_cast<const char*>(&r), 1);
            file.write(reinterpret_cast<const char*>(&g), 1);
            file.write(reinterpret_cast<const char*>(&b), 1);
        }
    }
    
    void createTestWAVFile(const fs::path& path) {
        std::ofstream file(path, std::ios::binary);
        
        // RIFF header
        file.write("RIFF", 4);
        uint32_t fileSize = 36 + 100; // Header + small data
        file.write(reinterpret_cast<const char*>(&fileSize), 4);
        file.write("WAVE", 4);
        
        // fmt chunk
        file.write("fmt ", 4);
        uint32_t fmtSize = 16;
        uint16_t audioFormat = 1; // PCM
        uint16_t channels = 2; // Stereo
        uint32_t sampleRate = 44100;
        uint32_t byteRate = sampleRate * channels * 2; // 16-bit
        uint16_t blockAlign = channels * 2;
        uint16_t bitsPerSample = 16;
        
        file.write(reinterpret_cast<const char*>(&fmtSize), 4);
        file.write(reinterpret_cast<const char*>(&audioFormat), 2);
        file.write(reinterpret_cast<const char*>(&channels), 2);
        file.write(reinterpret_cast<const char*>(&sampleRate), 4);
        file.write(reinterpret_cast<const char*>(&byteRate), 4);
        file.write(reinterpret_cast<const char*>(&blockAlign), 2);
        file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
        
        // data chunk
        file.write("data", 4);
        uint32_t dataSize = 100;
        file.write(reinterpret_cast<const char*>(&dataSize), 4);
        
        // Write some sample data
        for (int i = 0; i < 50; ++i) {
            int16_t sample = static_cast<int16_t>(i * 100);
            file.write(reinterpret_cast<const char*>(&sample), 2);
        }
    }
    
    fs::path testPath;
    fs::path outputPath;
};

TEST_F(MultiFormatProcessorTest, ConvertDC6ToPNG) {
    MultiFormatProcessor processor;
    
    fs::path dc6File = testPath / "test_sprite.dc6";
    fs::path pngFile = outputPath / "test_sprite.png";
    
    ConversionResult result = processor.convertDC6ToPNG(dc6File.string(), pngFile.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(fs::exists(pngFile));
    EXPECT_GT(fs::file_size(pngFile), 0);
    EXPECT_EQ(result.format, "PNG");
    EXPECT_GT(result.compressionRatio, 0.0f);
}

TEST_F(MultiFormatProcessorTest, ExtractPaletteFromMPQ) {
    MultiFormatProcessor processor;
    
    // Create a test palette file
    fs::path palettePath = testPath / "test_palette.dat";
    createTestPaletteFile(palettePath);
    
    auto palette = processor.extractPalette(palettePath.string());
    
    EXPECT_EQ(palette.colorCount, 256);
    EXPECT_TRUE(palette.hasTransparency);
    EXPECT_EQ(palette.colors[0].alpha, 0); // First color is transparent
}

TEST_F(MultiFormatProcessorTest, ProcessAudioFiles) {
    MultiFormatProcessor processor;
    
    // Create a test WAV file
    fs::path audioPath = testPath / "test_audio.wav";
    createTestWAVFile(audioPath);
    
    auto audioData = processor.extractAudio(audioPath.string());
    
    EXPECT_TRUE(audioData.isValid);
    EXPECT_EQ(audioData.format, AudioFormat::PCM_16);
    EXPECT_EQ(audioData.channels, 2); // Stereo
    EXPECT_EQ(audioData.sampleRate, 44100);
}

TEST_F(MultiFormatProcessorTest, ConvertDC6ToPVR) {
    MultiFormatProcessor processor;
    
    fs::path dc6File = testPath / "test_sprite.dc6";
    fs::path pvrFile = outputPath / "test_sprite.pvr";
    
    ConversionResult result = processor.convertDC6ToPVR(dc6File.string(), pvrFile.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(fs::exists(pvrFile));
    EXPECT_GT(fs::file_size(pvrFile), 0);
    EXPECT_EQ(result.format, "PVR");
    EXPECT_GT(result.compressionRatio, 0.0f);
}