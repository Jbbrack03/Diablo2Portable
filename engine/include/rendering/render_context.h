#pragma once

namespace d2::rendering {

class IRenderBackend;

// Static accessor for the active render backend.
// Lifecycle is managed by the caller (test harness or GameEngine).
class RenderContext {
public:
    static void setBackend(IRenderBackend* backend) { backend_ = backend; }
    static IRenderBackend* getBackend() { return backend_; }

private:
    static inline IRenderBackend* backend_ = nullptr;
};

} // namespace d2::rendering
