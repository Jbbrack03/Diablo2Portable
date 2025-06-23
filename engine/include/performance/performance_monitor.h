#pragma once

#include <chrono>
#include <deque>
#include <numeric>

namespace d2::performance {

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    
    // Frame timing
    void startFrame();
    void endFrame();
    
    // Get performance metrics
    double getCurrentFPS() const;
    double getAverageFPS() const;
    double getFrameTime() const;  // In milliseconds
    double getAverageFrameTime() const;  // In milliseconds
    double getMinFPS() const;
    double getMaxFPS() const;
    
    // Reset statistics
    void reset();
    
    // Set the number of frames to keep for averaging
    void setFrameHistorySize(size_t size);
    
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<double>;
    
    TimePoint frameStartTime_;
    TimePoint lastFrameEndTime_;
    double lastFrameTime_ = 0.0;  // In seconds
    
    std::deque<double> frameTimeHistory_;  // In seconds
    size_t maxHistorySize_ = 60;  // Default to 1 second at 60 FPS
    
    double minFrameTime_ = std::numeric_limits<double>::max();
    double maxFrameTime_ = 0.0;
};

} // namespace d2::performance