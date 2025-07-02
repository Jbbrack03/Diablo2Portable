#pragma once

#include <string>

namespace d2::release {

// Release builder for Phase 20 Task 3
class ReleaseBuilder {
public:
    ReleaseBuilder() = default;
    ~ReleaseBuilder() = default;
    
    // Configuration setters
    void setProjectName(const std::string& name) { projectName_ = name; }
    void setVersion(const std::string& version) { version_ = version; }
    void setTargetPlatform(const std::string& platform) { targetPlatform_ = platform; }
    void setMinSDKVersion(int sdk) { minSDKVersion_ = sdk; }
    
    // Configuration getters
    const std::string& getProjectName() const { return projectName_; }
    const std::string& getVersion() const { return version_; }
    const std::string& getTargetPlatform() const { return targetPlatform_; }
    int getMinSDKVersion() const { return minSDKVersion_; }
    
    // Generate build script
    std::string generateBuildScript() const {
        std::string script;
        script += "#!/bin/bash\n";
        script += "# Build script for " + projectName_ + " v" + version_ + "\n\n";
        script += "# Configure CMake\n";
        script += "cmake -B build -DCMAKE_BUILD_TYPE=Release\n\n";
        script += "# Build native code\n";
        script += "cmake --build build --config Release\n\n";
        script += "# Build Android APK\n";
        script += "./gradlew assembleRelease\n";
        return script;
    }
    
    // Generate installation guide
    std::string generateInstallationGuide() const {
        std::string guide;
        guide += "# " + projectName_ + " Installation Guide\n\n";
        guide += "## Requirements\n\n";
        guide += "- Android 8.0 (API " + std::to_string(minSDKVersion_) + ") or higher\n";
        guide += "- 2GB RAM minimum\n";
        guide += "- 500MB storage space\n";
        guide += "- Xbox-compatible gamepad\n";
        guide += "- Legally owned Diablo II game files\n\n";
        guide += "## Installation Steps\n\n";
        guide += "1. Download " + projectName_ + "-" + version_ + ".apk\n";
        guide += "2. Enable 'Install from Unknown Sources' in Android settings\n";
        guide += "3. Install the APK\n";
        guide += "4. Copy Diablo II MPQ files to /sdcard/Diablo2Portable/\n";
        guide += "5. Launch the game\n";
        return guide;
    }
    
private:
    std::string projectName_;
    std::string version_;
    std::string targetPlatform_;
    int minSDKVersion_ = 26;
};

} // namespace d2::release