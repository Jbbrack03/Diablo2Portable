#pragma once

#include <string>

namespace d2::core {

/**
 * @brief Base interface for initializable components
 * 
 * This interface provides a standard pattern for components that require
 * initialization and cleanup, ensuring consistent behavior across the codebase.
 */
class Initializable {
public:
    virtual ~Initializable() = default;
    
    /**
     * @brief Initialize the component
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Check if the component is initialized
     * @return true if initialized, false otherwise
     */
    virtual bool isInitialized() const = 0;
    
    /**
     * @brief Shutdown the component and clean up resources
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Get the last error message
     * @return Error message string
     */
    virtual std::string getLastError() const = 0;

protected:
    /**
     * @brief Set the last error message
     * @param error Error message to set
     */
    virtual void setLastError(const std::string& error) = 0;
};

/**
 * @brief Context-based initialization interface
 * 
 * For components that require initialization parameters.
 */
template<typename ContextType>
class InitializableWithContext {
public:
    virtual ~InitializableWithContext() = default;
    
    /**
     * @brief Initialize the component with context
     * @param context Initialization context
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize(const ContextType& context) = 0;
    
    /**
     * @brief Check if the component is initialized
     * @return true if initialized, false otherwise
     */
    virtual bool isInitialized() const = 0;
    
    /**
     * @brief Shutdown the component and clean up resources
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Get the last error message
     * @return Error message string
     */
    virtual std::string getLastError() const = 0;

protected:
    /**
     * @brief Set the last error message
     * @param error Error message to set
     */
    virtual void setLastError(const std::string& error) = 0;
};

/**
 * @brief Base implementation for initializable components
 * 
 * Provides common initialization state management.
 */
class BaseInitializable : public Initializable {
public:
    BaseInitializable() : initialized_(false) {}
    
    bool isInitialized() const override {
        return initialized_;
    }
    
    void shutdown() override {
        if (initialized_) {
            doShutdown();
            initialized_ = false;
            last_error_.clear();
        }
    }
    
    std::string getLastError() const override {
        return last_error_;
    }

protected:
    void setLastError(const std::string& error) override {
        last_error_ = error;
    }
    
    void setInitialized(bool initialized) {
        initialized_ = initialized;
    }
    
    /**
     * @brief Perform component-specific shutdown
     * Called by shutdown() when the component is initialized.
     */
    virtual void doShutdown() = 0;

private:
    bool initialized_;
    std::string last_error_;
};

/**
 * @brief Base implementation for context-based initializable components
 */
template<typename ContextType>
class BaseInitializableWithContext : public InitializableWithContext<ContextType> {
public:
    BaseInitializableWithContext() : initialized_(false) {}
    
    bool isInitialized() const override {
        return initialized_;
    }
    
    void shutdown() override {
        if (initialized_) {
            doShutdown();
            initialized_ = false;
            last_error_.clear();
        }
    }
    
    std::string getLastError() const override {
        return last_error_;
    }

protected:
    void setLastError(const std::string& error) override {
        last_error_ = error;
    }
    
    void setInitialized(bool initialized) {
        initialized_ = initialized;
    }
    
    /**
     * @brief Perform component-specific shutdown
     * Called by shutdown() when the component is initialized.
     */
    virtual void doShutdown() = 0;

private:
    bool initialized_;
    std::string last_error_;
};

} // namespace d2::core