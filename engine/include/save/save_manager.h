#pragma once

#include <string>

namespace d2::save {

class SaveManager {
public:
    explicit SaveManager(const std::string& saveDirectory);
    
    const std::string& getSaveDirectory() const { return m_saveDirectory; }
    
private:
    std::string m_saveDirectory;
};

} // namespace d2::save