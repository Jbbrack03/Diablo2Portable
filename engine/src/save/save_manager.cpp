#include "save/save_manager.h"
#include <filesystem>

namespace d2::save {

SaveManager::SaveManager(const std::string& saveDirectory) 
    : m_saveDirectory(saveDirectory) {
    // Create the save directory if it doesn't exist
    std::filesystem::create_directories(saveDirectory);
}

} // namespace d2::save