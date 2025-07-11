#include "core/resource_manager.h"
#include "utils/file_utils.h"
#include <fstream>
#include <iostream>

// Include OpenGL headers for cleanup functions
#ifdef ANDROID
#include <GLES3/gl3.h>
#else
// Mock OpenGL for desktop testing
extern "C" {
void glDeleteBuffers(int n, const unsigned int* buffers);
void glDeleteTextures(int n, const unsigned int* textures);
void glDeleteShader(unsigned int shader);
}
#endif

namespace d2::core {

// FileResourceManager implementation
FileResourceManager::FileResourceManager(const std::string& file_path)
    : ResourceManager(file_path), file_path_(file_path) {
    setResourceValid(d2::utils::FileUtils::validateFileExists(file_path));
}

std::ifstream& FileResourceManager::getInputStream() {
    if (!input_stream_) {
        input_stream_ = std::make_unique<std::ifstream>();
        if (!d2::utils::FileUtils::safeOpenBinaryFileForReading(file_path_, *input_stream_)) {
            setResourceValid(false);
        }
    }
    updateAccessTime();
    return *input_stream_;
}

std::ofstream& FileResourceManager::getOutputStream() {
    if (!output_stream_) {
        output_stream_ = std::make_unique<std::ofstream>();
        if (!d2::utils::FileUtils::safeOpenBinaryFileForWriting(file_path_, *output_stream_)) {
            setResourceValid(false);
        }
    }
    updateAccessTime();
    return *output_stream_;
}

bool FileResourceManager::isOpen() const {
    bool input_open = input_stream_ && input_stream_->is_open();
    bool output_open = output_stream_ && output_stream_->is_open();
    return input_open || output_open;
}

void FileResourceManager::cleanup() {
    if (input_stream_) {
        d2::utils::FileUtils::safeCloseFile(*input_stream_, file_path_);
        input_stream_.reset();
    }
    
    if (output_stream_) {
        d2::utils::FileUtils::safeCloseFile(*output_stream_, file_path_);
        output_stream_.reset();
    }
}

// Resource utility functions
namespace resource_utils {

std::unique_ptr<OpenGLResourceManager<unsigned int>> createBufferManager(
    unsigned int buffer_id, 
    const std::string& resource_id) {
    
    auto cleanup_func = [](unsigned int id) {
        glDeleteBuffers(1, &id);
    };
    
    return std::make_unique<OpenGLResourceManager<unsigned int>>(
        buffer_id, resource_id, cleanup_func);
}

std::unique_ptr<OpenGLResourceManager<unsigned int>> createTextureManager(
    unsigned int texture_id, 
    const std::string& resource_id) {
    
    auto cleanup_func = [](unsigned int id) {
        glDeleteTextures(1, &id);
    };
    
    return std::make_unique<OpenGLResourceManager<unsigned int>>(
        texture_id, resource_id, cleanup_func);
}

std::unique_ptr<OpenGLResourceManager<unsigned int>> createShaderManager(
    unsigned int shader_id, 
    const std::string& resource_id) {
    
    auto cleanup_func = [](unsigned int id) {
        glDeleteShader(id);
    };
    
    return std::make_unique<OpenGLResourceManager<unsigned int>>(
        shader_id, resource_id, cleanup_func);
}

} // namespace resource_utils

} // namespace d2::core