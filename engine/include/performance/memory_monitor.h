#ifndef D2_MEMORY_MONITOR_H
#define D2_MEMORY_MONITOR_H

#include <string>
#include <cstddef>
#include <memory>

namespace d2 {

/**
 * @brief Memory monitoring and tracking system for the game engine
 * 
 * Tracks memory allocations, deallocations, and current usage to ensure
 * the game stays within the 1.5GB memory budget for mobile devices.
 */
class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();
    
    /**
     * @brief Get the current total memory usage tracked by the monitor
     * @return Current memory usage in bytes
     */
    size_t getCurrentMemoryUsage() const;
    
    /**
     * @brief Record a memory allocation
     * @param identifier Name/identifier for the allocation
     * @param size Size of the allocation in bytes
     */
    void recordAllocation(const std::string& identifier, size_t size);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace d2

#endif // D2_MEMORY_MONITOR_H