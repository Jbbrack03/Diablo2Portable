#pragma once

namespace d2 {

class ExtractionHelpDialog {
public:
    ExtractionHelpDialog() = default;
    ~ExtractionHelpDialog() = default;
    
    bool isVisible() const { return visible; }

private:
    bool visible = false;
};

} // namespace d2