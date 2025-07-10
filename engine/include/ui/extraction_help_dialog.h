#pragma once

#include <string>

namespace d2 {

enum class ExtractionHelpContext {
    FILE_SELECTION,
    EXTRACTION_OPTIONS,
    PROGRESS
};

struct HelpContent {
    std::string title;
    std::string content;
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
    
    HelpContent getHelpContent() const {
        HelpContent help;
        switch (context) {
            case ExtractionHelpContext::FILE_SELECTION:
                help.title = "File Selection Help";
                help.content = "Select the location of your Diablo II installation or MPQ files.";
                break;
            case ExtractionHelpContext::EXTRACTION_OPTIONS:
                help.title = "Extraction Options Help";
                help.content = "Choose which assets to extract and configure extraction settings.";
                break;
            case ExtractionHelpContext::PROGRESS:
                help.title = "Extraction Progress Help";
                help.content = "Monitor the extraction progress and handle any errors.";
                break;
        }
        return help;
    }

private:
    bool visible = false;
    ExtractionHelpContext context = ExtractionHelpContext::FILE_SELECTION;
};

} // namespace d2