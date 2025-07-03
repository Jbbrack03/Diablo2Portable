#pragma once

#include <string>
#include <functional>
#include <chrono>

namespace d2 {

/**
 * Progress update information for extraction monitoring
 */
struct ProgressUpdate {
    float percentage = 0.0f;         // Progress from 0.0 to 1.0
    std::string currentFile;         // Currently processing file
    size_t filesProcessed = 0;       // Number of files processed
    size_t totalFiles = 0;           // Total files to process
    size_t bytesProcessed = 0;       // Bytes processed so far
    size_t totalBytes = 0;           // Total bytes to process
    std::chrono::milliseconds elapsedTime{0}; // Time elapsed
};

/**
 * ExtractionMonitor - Provides real-time monitoring of asset extraction
 * 
 * Features:
 * - Real-time progress callbacks with file-level detail
 * - Time estimation based on extraction speed
 * - Error reporting and recovery suggestions
 * - Bandwidth and I/O monitoring
 * - Parallel extraction tracking
 */
class ExtractionMonitor {
public:
    ExtractionMonitor() = default;
    ~ExtractionMonitor() = default;
    
    /**
     * Set callback for progress updates
     * @param callback Function called with progress updates
     */
    void setProgressCallback(std::function<void(const ProgressUpdate&)> callback) {
        progressCallback = callback;
    }
    
    /**
     * Get the progress callback (for internal use)
     * @return The progress callback function
     */
    std::function<void(const ProgressUpdate&)> getProgressCallback() const {
        return progressCallback;
    }
    
    /**
     * Update progress information
     * @param update Progress update to send
     */
    void updateProgress(const ProgressUpdate& update) {
        if (progressCallback) {
            progressCallback(update);
        }
    }
    
private:
    std::function<void(const ProgressUpdate&)> progressCallback;
};

} // namespace d2