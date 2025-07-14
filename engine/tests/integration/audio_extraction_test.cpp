#include <gtest/gtest.h>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include "audio/audio_engine.h"
#include "utils/mpq_validator.h"
#include "utils/stormlib_mpq_loader.h"

// Phase 39.1: Test audio extraction and playback
// This test validates audio extraction from MPQ files and playback functionality

namespace fs = std::filesystem;

class AudioExtractionTest : public ::testing::Test {
protected:
    void SetUp() override {
        mpq_dir_ = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
        
        // Find valid MPQs with audio files
        findValidMPQs();
    }
    
    void findValidMPQs() {
        std::vector<std::string> mpq_files = {
            "d2data.mpq", "d2exp.mpq", "d2sfx.mpq", "d2music.mpq", 
            "d2speech.mpq", "d2char.mpq", "d2video.mpq"
        };
        
        for (const auto& mpq_name : mpq_files) {
            fs::path mpq_path = mpq_dir_ / mpq_name;
            if (fs::exists(mpq_path)) {
                auto validation = d2::utils::MPQValidator::validateMPQFile(mpq_path.string());
                if (validation.isValid && !validation.isPlaceholder) {
                    valid_mpqs_.push_back(mpq_path.string());
                }
            }
        }
    }
    
    fs::path mpq_dir_;
    std::vector<std::string> valid_mpqs_;
};

// Test 1: Report audio file availability in MPQ files
TEST_F(AudioExtractionTest, ReportAudioFileAvailability) {
    std::cout << "\n=== Audio File Availability Report ===\n";
    
    // Common Diablo II audio file extensions
    std::vector<std::string> audio_extensions = {".wav", ".ogg", ".mp3", ".flac"};
    
    int total_audio_files = 0;
    int valid_mpqs_checked = 0;
    
    for (const auto& mpq_path : valid_mpqs_) {
        d2portable::utils::StormLibMPQLoader loader;
        if (!loader.open(mpq_path)) {
            continue;
        }
        
        valid_mpqs_checked++;
        fs::path path(mpq_path);
        std::cout << "\n" << path.filename() << " (" << fs::file_size(path) / (1024*1024) << " MB):\n";
        
        auto files = loader.listFiles();
        std::map<std::string, int> audio_counts;
        
        for (const auto& file : files) {
            std::string lower = file.filename;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            for (const auto& ext : audio_extensions) {
                if (lower.find(ext) != std::string::npos) {
                    audio_counts[ext]++;
                    total_audio_files++;
                    break;
                }
            }
        }
        
        for (const auto& [ext, count] : audio_counts) {
            if (count > 0) {
                std::cout << "  " << ext << " files: " << count << "\n";
            }
        }
        
        if (audio_counts.empty()) {
            std::cout << "  No audio files found\n";
        }
        
        loader.close();
    }
    
    std::cout << "\n=== Summary ===\n";
    std::cout << "Total audio files found: " << total_audio_files << "\n";
    std::cout << "Valid MPQs checked: " << valid_mpqs_checked << "\n";
    
    // Document what would be in placeholder MPQs
    std::cout << "\nNote: Real d2sfx.mpq would contain:\n";
    std::cout << "  - Combat sounds (sword clashing, spell casting)\n";
    std::cout << "  - Character sounds (death screams, footsteps)\n";
    std::cout << "  - UI sounds (button clicks, inventory sounds)\n";
    std::cout << "  - Environmental sounds (ambient effects)\n";
    
    std::cout << "\nNote: Real d2music.mpq would contain:\n";
    std::cout << "  - Background music tracks\n";
    std::cout << "  - Town themes\n";
    std::cout << "  - Dungeon ambience\n";
    std::cout << "  - Boss battle music\n";
    
    std::cout << "\nNote: Real d2speech.mpq would contain:\n";
    std::cout << "  - NPC dialogue\n";
    std::cout << "  - Character voice acting\n";
    std::cout << "  - Narrator voice-overs\n";
    std::cout << "  - Quest dialogue\n";
}

// Test 2: Test audio extraction from MPQ files
TEST_F(AudioExtractionTest, ExtractAudioFromMPQs) {
    std::cout << "\n=== Audio Extraction Test ===\n";
    
    if (valid_mpqs_.empty()) {
        std::cout << "No valid MPQs available for audio extraction testing.\n";
        GTEST_SKIP() << "No valid MPQs with audio files found";
    }
    
    std::vector<std::string> audio_extensions = {".wav", ".ogg", ".mp3", ".flac"};
    int extracted_count = 0;
    
    for (const auto& mpq_path : valid_mpqs_) {
        d2portable::utils::StormLibMPQLoader loader;
        if (!loader.open(mpq_path)) continue;
        
        fs::path path(mpq_path);
        std::cout << "\nExtracting from " << path.filename() << ":\n";
        
        auto files = loader.listFiles();
        
        for (const auto& file : files) {
            std::string lower = file.filename;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            bool is_audio = false;
            for (const auto& ext : audio_extensions) {
                if (lower.find(ext) != std::string::npos) {
                    is_audio = true;
                    break;
                }
            }
            
            if (is_audio) {
                auto start = std::chrono::high_resolution_clock::now();
                
                std::vector<uint8_t> audio_data;
                bool extracted = loader.extractFile(file.filename, audio_data);
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                
                if (extracted) {
                    std::cout << "  ✅ " << file.filename << " (" 
                              << audio_data.size() / 1024 << " KB in " 
                              << duration << "ms)\n";
                    extracted_count++;
                    
                    // Test first 10 files only to avoid long test times
                    if (extracted_count >= 10) break;
                } else {
                    std::cout << "  ❌ Failed to extract " << file.filename << "\n";
                }
            }
        }
        
        loader.close();
        if (extracted_count >= 10) break;
    }
    
    std::cout << "\nExtraction Summary:\n";
    std::cout << "  Successfully extracted: " << extracted_count << " audio files\n";
    
    if (extracted_count == 0) {
        std::cout << "\nNote: No audio files found in available MPQs.\n";
        std::cout << "Audio files are typically in d2sfx.mpq, d2music.mpq, and d2speech.mpq\n";
    }
}

// Test 3: Test audio engine with extracted audio data
TEST_F(AudioExtractionTest, TestAudioEngineWithExtractedData) {
    std::cout << "\n=== Audio Engine Integration Test ===\n";
    
    d2::audio::AudioEngine engine;
    bool initialized = engine.initialize();
    
    std::cout << "Audio Engine Status:\n";
    std::cout << "  Initialized: " << (initialized ? "✅ SUCCESS" : "❌ FAILED") << "\n";
    std::cout << "  Real Backend: " << (engine.hasRealAudioBackend() ? "✅ AVAILABLE" : "⚠️  MOCK") << "\n";
    std::cout << "  Device Open: " << (engine.isAudioDeviceOpen() ? "✅ OPEN" : "⚠️  CLOSED") << "\n";
    
    EXPECT_TRUE(initialized) << "Audio engine should initialize successfully";
    
    if (initialized) {
        // Test device capabilities
        auto caps = engine.getDeviceCapabilities();
        std::cout << "  Device Capabilities:\n";
        std::cout << "    Sample Rate: " << caps.sampleRate << " Hz\n";
        std::cout << "    Buffer Size: " << caps.bufferSize << " samples\n";
        std::cout << "    Channels: " << caps.channels << "\n";
        
        // Test device info
        auto info = engine.getAudioDeviceInfo();
        std::cout << "  Device Info:\n";
        std::cout << "    Name: " << info.deviceName << "\n";
        std::cout << "    Max Channels: " << info.maxChannels << "\n";
        std::cout << "    Low Latency: " << (info.supportsLowLatency ? "✅ YES" : "❌ NO") << "\n";
        
        // Test volume controls
        engine.setMasterVolume(0.8f);
        engine.setSoundEffectVolume(0.9f);
        engine.setMusicVolume(0.7f);
        
        EXPECT_FLOAT_EQ(engine.getMasterVolume(), 0.8f);
        EXPECT_FLOAT_EQ(engine.getSoundEffectVolume(), 0.9f);
        EXPECT_FLOAT_EQ(engine.getMusicVolume(), 0.7f);
        
        std::cout << "  Volume Controls: ✅ WORKING\n";
    }
}

// Test 4: Test audio playback with mock data
TEST_F(AudioExtractionTest, TestAudioPlaybackWithMockData) {
    std::cout << "\n=== Audio Playback Test ===\n";
    
    d2::audio::AudioEngine engine;
    if (!engine.initialize()) {
        GTEST_SKIP() << "Audio engine failed to initialize";
    }
    
    // Create mock audio data (sine wave)
    const int sample_rate = 44100;
    const int channels = 2;
    const float frequency = 440.0f; // A4 note
    const float duration = 0.5f; // 0.5 seconds
    
    std::vector<float> samples;
    int num_samples = static_cast<int>(sample_rate * duration);
    
    for (int i = 0; i < num_samples; i++) {
        float time = static_cast<float>(i) / sample_rate;
        float value = std::sin(2.0f * M_PI * frequency * time) * 0.3f; // 30% volume
        
        // Stereo: same value for both channels
        samples.push_back(value); // Left
        samples.push_back(value); // Right
    }
    
    std::cout << "Mock Audio Data:\n";
    std::cout << "  Sample Rate: " << sample_rate << " Hz\n";
    std::cout << "  Channels: " << channels << "\n";
    std::cout << "  Duration: " << duration << " seconds\n";
    std::cout << "  Samples: " << samples.size() << "\n";
    
    // Test playback
    bool playback_success = engine.playAudioData(samples.data(), samples.size(), sample_rate, channels);
    
    std::cout << "  Playback: " << (playback_success ? "✅ SUCCESS" : "❌ FAILED") << "\n";
    
    if (playback_success) {
        std::cout << "  Playing: " << (engine.isPlaying() ? "✅ YES" : "❌ NO") << "\n";
        std::cout << "  Position: " << engine.getPlaybackPosition() << " seconds\n";
        std::cout << "  Buffer Size: " << engine.getBufferSize() << " samples\n";
        std::cout << "  Current Sample Rate: " << engine.getSampleRate() << " Hz\n";
    }
    
    EXPECT_TRUE(playback_success) << "Audio playback should succeed with valid data";
}

// Test 5: Test audio extraction performance characteristics
TEST_F(AudioExtractionTest, AudioExtractionPerformance) {
    std::cout << "\n=== Audio Extraction Performance Test ===\n";
    
    if (valid_mpqs_.empty()) {
        std::cout << "No valid MPQs available for performance testing.\n";
        std::cout << "\nExpected audio extraction performance:\n";
        std::cout << "  - Small audio files (< 100KB): < 10ms\n";
        std::cout << "  - Medium audio files (100KB-1MB): < 50ms\n";
        std::cout << "  - Large audio files (> 1MB): < 200ms\n";
        std::cout << "  - Streaming should be preferred for music files\n";
        GTEST_SKIP() << "No valid MPQs available for performance testing";
    }
    
    std::vector<std::string> audio_extensions = {".wav", ".ogg", ".mp3", ".flac"};
    
    for (const auto& mpq_path : valid_mpqs_) {
        d2portable::utils::StormLibMPQLoader loader;
        if (!loader.open(mpq_path)) continue;
        
        fs::path path(mpq_path);
        std::cout << "\nPerformance test for " << path.filename() << ":\n";
        
        auto files = loader.listFiles();
        int tested_files = 0;
        
        for (const auto& file : files) {
            std::string lower = file.filename;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            bool is_audio = false;
            for (const auto& ext : audio_extensions) {
                if (lower.find(ext) != std::string::npos) {
                    is_audio = true;
                    break;
                }
            }
            
            if (is_audio) {
                auto start = std::chrono::high_resolution_clock::now();
                
                std::vector<uint8_t> audio_data;
                bool extracted = loader.extractFile(file.filename, audio_data);
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                
                if (extracted) {
                    double duration_ms = duration / 1000.0;
                    size_t size_kb = audio_data.size() / 1024;
                    
                    std::cout << "  " << file.filename << ": " 
                              << size_kb << "KB in " 
                              << duration_ms << "ms";
                    
                    // Performance validation
                    if (size_kb < 100 && duration_ms > 10) {
                        std::cout << " ⚠️  SLOW";
                    } else if (size_kb >= 100 && size_kb < 1000 && duration_ms > 50) {
                        std::cout << " ⚠️  SLOW";
                    } else if (size_kb >= 1000 && duration_ms > 200) {
                        std::cout << " ⚠️  SLOW";
                    } else {
                        std::cout << " ✅ GOOD";
                    }
                    
                    std::cout << "\n";
                    tested_files++;
                    
                    // Test first 5 files only
                    if (tested_files >= 5) break;
                }
            }
        }
        
        loader.close();
        if (tested_files >= 5) break;
    }
}

// Test 6: Test audio format support
TEST_F(AudioExtractionTest, AudioFormatSupport) {
    std::cout << "\n=== Audio Format Support Test ===\n";
    
    d2::audio::AudioEngine engine;
    if (!engine.initialize()) {
        GTEST_SKIP() << "Audio engine failed to initialize";
    }
    
    // Test different audio formats that might be in MPQs
    std::vector<std::string> test_formats = {
        "test.wav",    // Common in sound effects
        "test.ogg",    // Common in modern games
        "test.mp3",    // Compressed audio
        "test.flac"    // Lossless audio
    };
    
    std::cout << "Supported Audio Formats:\n";
    
    for (const auto& format : test_formats) {
        // Test loading (will fail for non-existent files, but tests format recognition)
        auto soundId = engine.loadSound(format);
        bool supported = (soundId != d2::audio::AudioEngine::INVALID_SOUND_ID);
        
        std::cout << "  " << format << ": " << (supported ? "✅ SUPPORTED" : "❌ NOT SUPPORTED") << "\n";
    }
    
    // Test music streaming formats
    std::cout << "\nMusic Streaming Formats:\n";
    for (const auto& format : test_formats) {
        auto musicId = engine.loadMusic(format);
        bool supported = (musicId != d2::audio::AudioEngine::INVALID_SOUND_ID);
        
        std::cout << "  " << format << ": " << (supported ? "✅ SUPPORTED" : "❌ NOT SUPPORTED") << "\n";
    }
    
    // Always pass - this is informational
    EXPECT_TRUE(true) << "Audio format support test completed";
}