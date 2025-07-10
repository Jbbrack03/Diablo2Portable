#pragma once

namespace d2 {

class ExtractionHelpDialog {
public:
    ExtractionHelpDialog() = default;
    ~ExtractionHelpDialog() = default;
    
    bool isVisible() const { return visible; }
    void show() { visible = true; }
    void hide() { visible = false; }

private:
    bool visible = false;
};

} // namespace d2