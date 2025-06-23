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
    
    /**
     * @brief Record a memory deallocation
     * @param identifier Name/identifier for the deallocation
     * @param size Size of the deallocation in bytes
     */
    void recordDeallocation(const std::string& identifier, size_t size);
    
    /**
     * @brief Set the memory budget limit
     * @param budget Maximum memory usage in bytes (default 1.5GB for mobile)
     */
    void setMemoryBudget(size_t budget);
    
    /**
     * @brief Get the current memory budget
     * @return Memory budget in bytes
     */
    size_t getMemoryBudget() const;
    
    /**
     * @brief Check if current usage is within budget
     * @return true if within budget, false otherwise
     */
    bool isWithinBudget() const;
    
    /**
     * @brief Try to record an allocation only if it won't exceed budget
     * @param identifier Name/identifier for the allocation
     * @param size Size of the allocation in bytes
     * @return true if allocation was recorded, false if it would exceed budget
     */
    bool tryRecordAllocation(const std::string& identifier, size_t size);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace d2

#endif // D2_MEMORY_MONITOR_H