#include "performance/memory_monitor.h"
#include <unordered_map>

namespace d2 {

class MemoryMonitor::Impl {
public:
    Impl() : total_memory_usage(0), memory_budget(1536 * 1024 * 1024) {} // Default 1.5GB
    
    size_t getCurrentMemoryUsage() const {
        return total_memory_usage;
    }
    
    void recordAllocation(const std::string& identifier, size_t size) {
        allocations[identifier] += size;
        total_memory_usage += size;
    }
    
    void recordDeallocation(const std::string& identifier, size_t size) {
        auto it = allocations.find(identifier);
        if (it != allocations.end()) {
            if (it->second >= size) {
                it->second -= size;
                total_memory_usage -= size;
                
                // Remove the entry if it reaches zero
                if (it->second == 0) {
                    allocations.erase(it);
                }
            }
        }
    }
    
    void setMemoryBudget(size_t budget) {
        memory_budget = budget;
    }
    
    size_t getMemoryBudget() const {
        return memory_budget;
    }
    
    bool isWithinBudget() const {
        return total_memory_usage <= memory_budget;
    }
    
    bool tryRecordAllocation(const std::string& identifier, size_t size) {
        if (total_memory_usage + size > memory_budget) {
            return false; // Would exceed budget
        }
        recordAllocation(identifier, size);
        return true;
    }
    
private:
    std::unordered_map<std::string, size_t> allocations;
    size_t total_memory_usage;
    size_t memory_budget;
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

void MemoryMonitor::recordDeallocation(const std::string& identifier, size_t size) {
    pImpl->recordDeallocation(identifier, size);
}

void MemoryMonitor::setMemoryBudget(size_t budget) {
    pImpl->setMemoryBudget(budget);
}

size_t MemoryMonitor::getMemoryBudget() const {
    return pImpl->getMemoryBudget();
}

bool MemoryMonitor::isWithinBudget() const {
    return pImpl->isWithinBudget();
}

bool MemoryMonitor::tryRecordAllocation(const std::string& identifier, size_t size) {
    return pImpl->tryRecordAllocation(identifier, size);
}

} // namespace d2