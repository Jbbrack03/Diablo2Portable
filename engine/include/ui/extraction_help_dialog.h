#pragma once

#include <string>
#include <vector>

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
    
    std::vector<std::string> getTroubleshootingTips() const {
        std::vector<std::string> tips;
        switch (context) {
            case ExtractionHelpContext::FILE_SELECTION:
                tips.push_back("Check file permissions if you cannot access the installation directory.");
                tips.push_back("Verify the correct location - look for folders containing .mpq files.");
                break;
            case ExtractionHelpContext::EXTRACTION_OPTIONS:
                tips.push_back("Ensure you have enough disk space for extracted assets.");
                tips.push_back("Try extracting fewer asset types if running out of space.");
                break;
            case ExtractionHelpContext::PROGRESS:
                tips.push_back("If extraction fails, check available disk space.");
                tips.push_back("Verify MPQ files are not corrupted.");
                break;
        }
        return tips;
    }

private:
    bool visible = false;
    ExtractionHelpContext context = ExtractionHelpContext::FILE_SELECTION;
};

} // namespace d2