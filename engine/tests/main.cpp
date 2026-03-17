#include <gtest/gtest.h>
#include "rendering/render_context.h"
#include "rendering/mock_render_backend.h"

// Global test environment that installs the MockRenderBackend
class RenderBackendEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        backend_ = new d2::rendering::MockRenderBackend();
        d2::rendering::RenderContext::setBackend(backend_);
    }
    void TearDown() override {
        d2::rendering::RenderContext::setBackend(nullptr);
        delete backend_;
        backend_ = nullptr;
    }
private:
    d2::rendering::MockRenderBackend* backend_ = nullptr;
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new RenderBackendEnvironment());
    return RUN_ALL_TESTS();
}
