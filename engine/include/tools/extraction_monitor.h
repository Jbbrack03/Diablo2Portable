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
 * Time estimate information
 */
struct TimeEstimate {
    double totalSeconds = 0.0;       // Estimated seconds remaining
    bool isReliable = false;         // Whether estimate is reliable
};

/**
 * Error types that can occur during extraction
 */
enum class ErrorType {
    CORRUPTED_MPQ,
    FILE_NOT_FOUND,
    INSUFFICIENT_SPACE,
    PERMISSION_DENIED,
    UNSUPPORTED_FORMAT,
    NETWORK_ERROR
};

/**
 * Extraction error information
 */
struct ExtractionError {
    ErrorType type = ErrorType::CORRUPTED_MPQ;
    std::string filename;
    std::string message;
    bool isRecoverable = false;
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
    
    /**
     * Update progress with percentage and current file
     * @param percentage Progress from 0.0 to 1.0
     * @param currentFile Current file being processed
     * @param elapsedMs Elapsed time in milliseconds
     */
    void updateProgress(float percentage, const std::string& currentFile, int64_t elapsedMs) {
        ProgressUpdate update;
        update.percentage = percentage;
        update.currentFile = currentFile;
        update.elapsedTime = std::chrono::milliseconds(elapsedMs);
        
        // Track start time if this is first update
        if (startTime == std::chrono::steady_clock::time_point{}) {
            startTime = std::chrono::steady_clock::now();
        }
        
        lastUpdate = update;
        updateProgress(update);
    }
    
    /**
     * Get estimated time remaining
     * @return Time estimate information
     */
    TimeEstimate getTimeRemaining() const {
        TimeEstimate estimate;
        
        if (lastUpdate.percentage > 0.0f && lastUpdate.percentage < 1.0f) {
            // Calculate time per percentage point
            double secondsElapsed = lastUpdate.elapsedTime.count() / 1000.0;
            double percentageComplete = lastUpdate.percentage;
            double percentageRemaining = 1.0f - percentageComplete;
            
            // Estimate time remaining
            double timePerPercent = secondsElapsed / percentageComplete;
            estimate.totalSeconds = timePerPercent * percentageRemaining;
            estimate.isReliable = percentageComplete > 0.1f; // Need at least 10% for reliable estimate
        }
        
        return estimate;
    }
    
    /**
     * Set callback for error notifications
     * @param callback Function called when errors occur
     */
    void setErrorCallback(std::function<void(const ExtractionError&)> callback) {
        errorCallback = callback;
    }
    
    /**
     * Report an extraction error
     * @param error Error information
     */
    void reportError(const ExtractionError& error) {
        if (errorCallback) {
            errorCallback(error);
        }
    }
    
private:
    std::function<void(const ProgressUpdate&)> progressCallback;
    std::function<void(const ExtractionError&)> errorCallback;
    std::chrono::steady_clock::time_point startTime;
    ProgressUpdate lastUpdate;
};

} // namespace d2