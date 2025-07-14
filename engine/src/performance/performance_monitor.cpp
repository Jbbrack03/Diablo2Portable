#include "performance/performance_monitor.h"
#include <algorithm>
#include <cstdlib>
#include <thread>
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/task.h>
#endif
#ifdef __linux__
#include <unistd.h>
#include <fstream>
#include <string>
#endif

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

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
#ifdef __APPLE__
    // macOS memory monitoring
    struct mach_task_basic_info info;
    mach_msg_type_number_t size = MACH_TASK_BASIC_INFO_COUNT;
    kern_return_t kerr = task_info(mach_task_self(),
                                   MACH_TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);
    
    if (kerr == KERN_SUCCESS) {
        return info.resident_size;
    }
    return 0;
#elif defined(__linux__)
    // Linux/Android memory monitoring
    // Read from /proc/self/status
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.find("VmRSS:") == 0) {
            size_t pos = line.find_first_of("0123456789");
            if (pos != std::string::npos) {
                return std::stoull(line.substr(pos)) * 1024; // Convert KB to bytes
            }
        }
    }
    return 0;
#else
    // Fallback for other platforms
    return 0;
#endif
}

void PerformanceMonitor::recordDrawCall() {
    // TODO: Implement draw call recording for performance monitoring
}

void PerformanceMonitor::processInputEvent() {
    // TODO: Implement input event timing for latency measurement
}

void PerformanceMonitor::swapBuffers() {
    // TODO: Implement buffer swap timing for vsync monitoring
}

void PerformanceMonitor::recordTextureStateChange() {
    // Simulate texture state change cost (typically 0.1-0.5ms on mobile)
    auto start = Clock::now();
    // Texture binding involves GPU state synchronization
    std::this_thread::sleep_for(std::chrono::microseconds(100)); // 0.1ms
    auto end = Clock::now();
}

void PerformanceMonitor::recordShaderSwitch(int shaderId) {
    // Simulate shader program switch cost
    auto start = Clock::now();
    // Shader switches require pipeline state change
    std::this_thread::sleep_for(std::chrono::microseconds(200)); // 0.2ms
    auto end = Clock::now();
}

void PerformanceMonitor::recordVertexBufferUpload(size_t dataSize) {
    // Simulate vertex buffer upload cost based on data size
    auto start = Clock::now();
    // Upload speed varies by GPU, assume ~1GB/s for mobile
    size_t microseconds = dataSize / 1000; // 1 byte per microsecond
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    auto end = Clock::now();
}

void PerformanceMonitor::recordFullScreenQuad(int width, int height) {
    // Simulate fill rate cost for full screen quad
    // Mobile GPUs typically have ~5-10 GPixels/s fill rate
    // For 1920x1080, that's ~2M pixels
    size_t pixels = static_cast<size_t>(width) * height;
    
    // Check current frame's overdraw count
    static thread_local int currentFrameOverdraw = 0;
    static thread_local Clock::time_point frameStart = Clock::now();
    
    // Reset overdraw counter if new frame
    auto now = Clock::now();
    if (now - frameStart > std::chrono::milliseconds(16)) {
        currentFrameOverdraw = 0;
        frameStart = now;
    }
    
    currentFrameOverdraw++;
    
    // Base cost: pixels / fill rate
    size_t baseMicroseconds = pixels / 5000; // 5 pixels per microsecond
    
    // Apply overdraw penalty beyond fill rate limit (4x)
    if (currentFrameOverdraw > 4) {
        // Exponential penalty for exceeding fill rate
        baseMicroseconds = baseMicroseconds * currentFrameOverdraw / 2;
    }
    
    std::this_thread::sleep_for(std::chrono::microseconds(baseMicroseconds));
}

} // namespace d2::performance