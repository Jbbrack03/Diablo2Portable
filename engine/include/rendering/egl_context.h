#pragma once

#include <string>

namespace d2::rendering {

class EGLContext {
public:
    EGLContext() = default;
    ~EGLContext() = default;

    bool initialize();
    bool isValid() const;
    std::string getVersion() const;

private:
    bool valid_ = false;
};

} // namespace d2::rendering