#pragma once
#include <vector>

namespace d2::input {

class AndroidInput {
public:
    virtual ~AndroidInput() = default;
    
    virtual std::vector<int> getConnectedDevices() const = 0;
    virtual bool isGamepad(int deviceId) const = 0;
};

} // namespace d2::input