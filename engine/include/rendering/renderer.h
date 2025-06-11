#pragma once

#include <string>

namespace d2::rendering {

class EGLContext;

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    bool initialize(const EGLContext& context);
    bool isInitialized() const;
    std::string getOpenGLVersion() const;

private:
    bool initialized_ = false;
};

} // namespace d2::rendering