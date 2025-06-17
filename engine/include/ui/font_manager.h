#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "ui/font.h"

namespace d2 {

class FontManager {
public:
    FontManager() = default;
    virtual ~FontManager() = default;
    
    size_t getFontCount() const { return fonts_.size(); }
    
    std::shared_ptr<Font> getFont(const std::string& name) const {
        auto it = fonts_.find(name);
        return (it != fonts_.end()) ? it->second : nullptr;
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<Font>> fonts_;
};

} // namespace d2