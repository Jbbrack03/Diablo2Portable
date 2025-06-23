#include "performance/performance_monitor.h"
#include <algorithm>

namespace d2::performance {

PerformanceMonitor::PerformanceMonitor() {
    reset();
}

void PerformanceMonitor::startFrame() {
    frameStartTime_ = Clock::now();
}

void PerformanceMonitor::endFrame() {
    auto now = Clock::now();
    Duration frameTime = now - frameStartTime_;
    lastFrameTime_ = frameTime.count();
    
    // Update history
    frameTimeHistory_.push_back(lastFrameTime_);
    if (frameTimeHistory_.size() > maxHistorySize_) {
        frameTimeHistory_.pop_front();
    }
    
    // Update min/max
    minFrameTime_ = std::min(minFrameTime_, lastFrameTime_);
    maxFrameTime_ = std::max(maxFrameTime_, lastFrameTime_);
    
    lastFrameEndTime_ = now;
}

double PerformanceMonitor::getCurrentFPS() const {
    if (lastFrameTime_ <= 0.0) {
        return 0.0;
    }
    return 1.0 / lastFrameTime_;
}

double PerformanceMonitor::getAverageFPS() const {
    double avgFrameTime = getAverageFrameTime() / 1000.0;  // Convert to seconds
    if (avgFrameTime <= 0.0) {
        return 0.0;
    }
    return 1.0 / avgFrameTime;
}

double PerformanceMonitor::getFrameTime() const {
    return lastFrameTime_ * 1000.0;  // Convert to milliseconds
}

double PerformanceMonitor::getAverageFrameTime() const {
    if (frameTimeHistory_.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(frameTimeHistory_.begin(), frameTimeHistory_.end(), 0.0);
    return (sum / frameTimeHistory_.size()) * 1000.0;  // Convert to milliseconds
}

double PerformanceMonitor::getMinFPS() const {
    if (maxFrameTime_ <= 0.0) {
        return 0.0;
    }
    return 1.0 / maxFrameTime_;
}

double PerformanceMonitor::getMaxFPS() const {
    if (minFrameTime_ <= 0.0) {
        return 0.0;
    }
    return 1.0 / minFrameTime_;
}

void PerformanceMonitor::reset() {
    frameTimeHistory_.clear();
    minFrameTime_ = std::numeric_limits<double>::max();
    maxFrameTime_ = 0.0;
    lastFrameTime_ = 0.0;
}

void PerformanceMonitor::setFrameHistorySize(size_t size) {
    maxHistorySize_ = size;
    while (frameTimeHistory_.size() > maxHistorySize_) {
        frameTimeHistory_.pop_front();
    }
}

} // namespace d2::performance