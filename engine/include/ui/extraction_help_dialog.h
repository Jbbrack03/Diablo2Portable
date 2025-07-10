#pragma once

namespace d2 {

enum class ExtractionHelpContext {
    FILE_SELECTION,
    EXTRACTION_OPTIONS,
    PROGRESS
};

class ExtractionHelpDialog {
public:
    ExtractionHelpDialog() = default;
    ~ExtractionHelpDialog() = default;
    
    bool isVisible() const { return visible; }
    void show() { visible = true; }
    void hide() { visible = false; }
    
    void setContext(ExtractionHelpContext ctx) { context = ctx; }
    ExtractionHelpContext getCurrentContext() const { return context; }

private:
    bool visible = false;
    ExtractionHelpContext context = ExtractionHelpContext::FILE_SELECTION;
};

} // namespace d2