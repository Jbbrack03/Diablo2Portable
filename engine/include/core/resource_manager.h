#pragma once

#include <memory>
#include <string>
#include <chrono>

namespace d2::core {

/**
 * @brief Base class for resource management
 * 
 * Provides RAII-style resource management with automatic cleanup
 * and resource validation.
 */
class ResourceManager {
public:
    virtual ~ResourceManager() { 
        if (isResourceValid()) {
            cleanup(); 
        }
    }
    
    /**
     * @brief Check if the resource is valid
     * @return true if resource is valid, false otherwise
     */
    bool isResourceValid() const { 
        return resource_valid_; 
    }
    
    /**
     * @brief Get the time when the resource was created
     * @return Creation time
     */
    std::chrono::steady_clock::time_point getCreationTime() const {
        return creation_time_;
    }
    
    /**
     * @brief Get the time when the resource was last accessed
     * @return Last access time
     */
    std::chrono::steady_clock::time_point getLastAccessTime() const {
        return last_access_time_;
    }
    
    /**
     * @brief Update the last access time to now
     */
    void updateAccessTime() {
        last_access_time_ = std::chrono::steady_clock::now();
    }
    
    /**
     * @brief Get the resource identifier
     * @return Resource identifier string
     */
    std::string getResourceId() const {
        return resource_id_;
    }

protected:
    /**
     * @brief Constructor for resource manager
     * @param resource_id Identifier for the resource
     */
    explicit ResourceManager(const std::string& resource_id = "")
        : resource_valid_(false)
        , resource_id_(resource_id)
        , creation_time_(std::chrono::steady_clock::now())
        , last_access_time_(creation_time_) {}
    
    /**
     * @brief Set the resource validity state
     * @param valid true if resource is valid, false otherwise
     */
    void setResourceValid(bool valid) { 
        resource_valid_ = valid; 
    }
    
    /**
     * @brief Set the resource identifier
     * @param resource_id Resource identifier string
     */
    void setResourceId(const std::string& resource_id) {
        resource_id_ = resource_id;
    }
    
    /**
     * @brief Perform resource cleanup
     * Must be implemented by derived classes to handle specific resource cleanup.
     */
    virtual void cleanup() = 0;

private:
    bool resource_valid_;
    std::string resource_id_;
    std::chrono::steady_clock::time_point creation_time_;
    std::chrono::steady_clock::time_point last_access_time_;
};

/**
 * @brief RAII wrapper for OpenGL resources
 * 
 * Automatically handles OpenGL resource cleanup when the object is destroyed.
 */
template<typename ResourceType>
class OpenGLResourceManager : public ResourceManager {
public:
    /**
     * @brief Constructor for OpenGL resource
     * @param resource The OpenGL resource handle
     * @param resource_id Identifier for the resource
     * @param cleanup_func Function to call for cleanup
     */
    OpenGLResourceManager(ResourceType resource, 
                         const std::string& resource_id,
                         std::function<void(ResourceType)> cleanup_func)
        : ResourceManager(resource_id)
        , resource_(resource)
        , cleanup_func_(cleanup_func) {
        setResourceValid(resource != 0);
    }
    
    /**
     * @brief Get the OpenGL resource handle
     * @return OpenGL resource handle
     */
    ResourceType getResource() const {
        updateAccessTime();
        return resource_;
    }
    
    /**
     * @brief Release the resource (mark as invalid without cleanup)
     * Use this when the resource has been transferred to another owner.
     */
    void release() {
        setResourceValid(false);
    }

protected:
    void cleanup() override {
        if (cleanup_func_ && resource_ != 0) {
            cleanup_func_(resource_);
            resource_ = 0;
        }
    }

private:
    ResourceType resource_;
    std::function<void(ResourceType)> cleanup_func_;
};

/**
 * @brief RAII wrapper for file handles
 * 
 * Automatically handles file handle cleanup when the object is destroyed.
 */
class FileResourceManager : public ResourceManager {
public:
    /**
     * @brief Constructor for file resource
     * @param file_path Path to the file
     */
    explicit FileResourceManager(const std::string& file_path);
    
    /**
     * @brief Destructor automatically closes the file
     */
    ~FileResourceManager() override = default;
    
    /**
     * @brief Get the file handle for reading
     * @return File input stream
     */
    std::ifstream& getInputStream();
    
    /**
     * @brief Get the file handle for writing
     * @return File output stream
     */
    std::ofstream& getOutputStream();
    
    /**
     * @brief Check if the file is open
     * @return true if file is open, false otherwise
     */
    bool isOpen() const;

protected:
    void cleanup() override;

private:
    std::string file_path_;
    std::unique_ptr<std::ifstream> input_stream_;
    std::unique_ptr<std::ofstream> output_stream_;
};

/**
 * @brief Resource pool for managing multiple resources of the same type
 * 
 * Provides automatic cleanup and resource lifecycle management.
 */
template<typename ResourceType>
class ResourcePool {
public:
    /**
     * @brief Add a resource to the pool
     * @param resource Resource to add
     */
    void addResource(std::unique_ptr<ResourceType> resource) {
        resources_.push_back(std::move(resource));
    }
    
    /**
     * @brief Get a resource by index
     * @param index Index of the resource
     * @return Pointer to the resource, or nullptr if index is invalid
     */
    ResourceType* getResource(size_t index) {
        if (index < resources_.size()) {
            return resources_[index].get();
        }
        return nullptr;
    }
    
    /**
     * @brief Get the number of resources in the pool
     * @return Number of resources
     */
    size_t getResourceCount() const {
        return resources_.size();
    }
    
    /**
     * @brief Clear all resources from the pool
     */
    void clear() {
        resources_.clear();
    }
    
    /**
     * @brief Remove invalid resources from the pool
     */
    void removeInvalidResources() {
        resources_.erase(
            std::remove_if(resources_.begin(), resources_.end(),
                [](const std::unique_ptr<ResourceType>& resource) {
                    return !resource || !resource->isResourceValid();
                }),
            resources_.end());
    }

private:
    std::vector<std::unique_ptr<ResourceType>> resources_;
};

/**
 * @brief Utility functions for resource management
 */
namespace resource_utils {

/**
 * @brief Create an OpenGL buffer resource manager
 * @param buffer_id OpenGL buffer ID
 * @param resource_id Resource identifier
 * @return Unique pointer to the resource manager
 */
std::unique_ptr<OpenGLResourceManager<unsigned int>> createBufferManager(
    unsigned int buffer_id, 
    const std::string& resource_id);

/**
 * @brief Create an OpenGL texture resource manager
 * @param texture_id OpenGL texture ID
 * @param resource_id Resource identifier
 * @return Unique pointer to the resource manager
 */
std::unique_ptr<OpenGLResourceManager<unsigned int>> createTextureManager(
    unsigned int texture_id, 
    const std::string& resource_id);

/**
 * @brief Create an OpenGL shader resource manager
 * @param shader_id OpenGL shader ID
 * @param resource_id Resource identifier
 * @return Unique pointer to the resource manager
 */
std::unique_ptr<OpenGLResourceManager<unsigned int>> createShaderManager(
    unsigned int shader_id, 
    const std::string& resource_id);

} // namespace resource_utils

} // namespace d2::core