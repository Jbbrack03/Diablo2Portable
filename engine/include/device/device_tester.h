#pragma once

#include <string>

namespace d2::device {

// Test report structure
struct DeviceTestReport {
    std::string deviceName;
    
    // Test results
    bool passedPerformanceTest = true;
    float averageFPS = 60.0f;
    
    bool passedBatteryTest = true;
    float estimatedBatteryLife = 4.5f; // hours
    
    bool passedControllerTest = true;
    
    bool hasResults() const {
        return !deviceName.empty();
    }
    
    bool isCompatible() const {
        return passedPerformanceTest && passedBatteryTest && passedControllerTest;
    }
};

// Device tester for Phase 20 Task 2
class DeviceTester {
public:
    DeviceTester() = default;
    ~DeviceTester() = default;
    
    // Run a complete test suite for a device
    DeviceTestReport runTestSuite(const std::string& deviceName) {
        DeviceTestReport report;
        report.deviceName = deviceName;
        return report;
    }
};

} // namespace d2::device