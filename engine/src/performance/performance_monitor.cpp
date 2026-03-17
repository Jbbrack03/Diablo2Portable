#include "performance/performance_monitor.h"
#include <algorithm>
#include <cstdlib>
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
    double avgFrameTime = getAverageFrameTime() / 1000.0;
    if (avgFrameTime <= 0.0) {
        return 0.0;
    }
    return 1.0 / avgFrameTime;
}

double PerformanceMonitor::getFrameTime() const {
    return lastFrameTime_ * 1000.0;
}

double PerformanceMonitor::getAverageFrameTime() const {
    if (frameTimeHistory_.empty()) {
        return 0.0;
    }

    double sum = std::accumulate(frameTimeHistory_.begin(), frameTimeHistory_.end(), 0.0);
    return (sum / frameTimeHistory_.size()) * 1000.0;
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
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.find("VmRSS:") == 0) {
            size_t pos = line.find_first_of("0123456789");
            if (pos != std::string::npos) {
                return std::stoull(line.substr(pos)) * 1024;
            }
        }
    }
    return 0;
#else
    return 0;
#endif
}

void PerformanceMonitor::recordDrawCall() {
    // No-op until real GPU timer queries are available (Phase 51)
}

void PerformanceMonitor::processInputEvent() {
    // No-op until real input latency tracking is available
}

void PerformanceMonitor::swapBuffers() {
    // No-op until real vsync monitoring is available
}

void PerformanceMonitor::recordTextureStateChange() {
    // Previously simulated with sleep_for(100us).
    // Now a no-op -- real cost is measured by GPU timer queries on Android.
}

void PerformanceMonitor::recordShaderSwitch(int shaderId) {
    (void)shaderId;
    // Previously simulated with sleep_for(200us).
    // Now a no-op -- real cost is measured by GPU timer queries on Android.
}

void PerformanceMonitor::recordVertexBufferUpload(size_t dataSize) {
    (void)dataSize;
    // Previously simulated with sleep_for based on data size.
    // Now a no-op -- real cost is measured by GPU timer queries on Android.
}

void PerformanceMonitor::recordFullScreenQuad(int width, int height) {
    (void)width;
    (void)height;
    // Previously simulated fill rate with sleep_for.
    // Now a no-op -- real fill rate is measured on Android device.
}

} // namespace d2::performance
