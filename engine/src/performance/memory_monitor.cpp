#include "performance/memory_monitor.h"
#include <unordered_map>

namespace d2 {

class MemoryMonitor::Impl {
public:
    Impl() : total_memory_usage(0) {}
    
    size_t getCurrentMemoryUsage() const {
        return total_memory_usage;
    }
    
    void recordAllocation(const std::string& identifier, size_t size) {
        allocations[identifier] += size;
        total_memory_usage += size;
    }
    
private:
    std::unordered_map<std::string, size_t> allocations;
    size_t total_memory_usage;
};

MemoryMonitor::MemoryMonitor() : pImpl(std::make_unique<Impl>()) {
}

MemoryMonitor::~MemoryMonitor() = default;

size_t MemoryMonitor::getCurrentMemoryUsage() const {
    return pImpl->getCurrentMemoryUsage();
}

void MemoryMonitor::recordAllocation(const std::string& identifier, size_t size) {
    pImpl->recordAllocation(identifier, size);
}

} // namespace d2