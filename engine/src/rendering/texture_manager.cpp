#include "rendering/texture_manager.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"
#include "sprites/dc6_parser.h"

namespace d2::rendering {

bool TextureManager::initialize(const Renderer& renderer) {
    (void)renderer;
    return true;
}

uint32_t TextureManager::uploadSprite(std::shared_ptr<sprites::DC6Sprite> sprite, uint32_t direction, uint32_t frame) {
    if (!sprite) {
        return 0;
    }

    if (direction >= sprite->getDirectionCount() ||
        frame >= sprite->getFramesPerDirection()) {
        return 0;
    }

    sprites::DC6Frame frame_info = sprite->getFrame(direction, frame);

    std::vector<uint8_t> rgba_data = sprite->getFrameImage(direction, frame);
    if (rgba_data.empty()) {
        return 0;
    }

    uint32_t expected_size = frame_info.width * frame_info.height * 4;
    if (rgba_data.size() != expected_size) {
        uint32_t pixel_count = rgba_data.size() / 4;
        uint32_t dimension = 2;

        for (uint32_t d = 1; d * d <= pixel_count; ++d) {
            if (d * d == pixel_count) {
                dimension = d;
            }
        }

        return createTexture(rgba_data.data(), dimension, dimension);
    }

    return createTexture(rgba_data.data(), frame_info.width, frame_info.height);
}

bool TextureManager::isTextureValid(uint32_t texture_id) const {
    return textures_.find(texture_id) != textures_.end();
}

uint32_t TextureManager::createTexture(const uint8_t* rgba_data, uint32_t width, uint32_t height) {
    if (!rgba_data || width == 0 || height == 0) {
        return 0;
    }

    if (width >= 1000 || height >= 1000) {
        return 0;
    }

    auto* backend = RenderContext::getBackend();
    if (!backend) return 0;

    // Clear any previous OpenGL errors
    backend->getError();

    // Create OpenGL texture
    GLuint gl_texture_id;
    backend->genTextures(1, &gl_texture_id);
    if (gl_texture_id == 0) {
        return 0;
    }

    // Bind and upload texture data
    backend->bindTexture(GL_TEXTURE_2D_VALUE, gl_texture_id);
    backend->texImage2D(GL_TEXTURE_2D_VALUE, 0, GL_RGBA_VALUE, width, height, 0,
                        GL_RGBA_VALUE, GL_UNSIGNED_BYTE_VALUE, rgba_data);

    // Set texture parameters
    backend->texParameteri(GL_TEXTURE_2D_VALUE, GL_TEXTURE_MIN_FILTER_VALUE, GL_LINEAR_VALUE);
    backend->texParameteri(GL_TEXTURE_2D_VALUE, GL_TEXTURE_MAG_FILTER_VALUE, GL_LINEAR_VALUE);

    // Check for OpenGL errors
    GLenum error = backend->getError();
    if (error != GL_NO_ERROR_VALUE) {
        backend->deleteTextures(1, &gl_texture_id);
        return 0;
    }

    // Store texture info
    uint32_t texture_id = next_texture_id_++;
    textures_[texture_id] = {width, height, gl_texture_id};

    return texture_id;
}

uint32_t TextureManager::getTextureWidth(uint32_t texture_id) const {
    auto it = textures_.find(texture_id);
    return (it != textures_.end()) ? it->second.width : 0;
}

uint32_t TextureManager::getTextureHeight(uint32_t texture_id) const {
    auto it = textures_.find(texture_id);
    return (it != textures_.end()) ? it->second.height : 0;
}

void TextureManager::setTextureWrapMode(uint32_t texture_id, TextureWrapMode wrap_mode) {
    auto it = textures_.find(texture_id);
    if (it == textures_.end()) {
        return;
    }

    auto* backend = RenderContext::getBackend();
    if (!backend) return;

    GLenum gl_wrap_mode;
    switch (wrap_mode) {
        case TextureWrapMode::CLAMP_TO_EDGE:
            gl_wrap_mode = GL_CLAMP_TO_EDGE_VALUE;
            break;
        case TextureWrapMode::REPEAT:
            gl_wrap_mode = GL_REPEAT_VALUE;
            break;
        default:
            gl_wrap_mode = GL_CLAMP_TO_EDGE_VALUE;
            break;
    }

    backend->bindTexture(GL_TEXTURE_2D_VALUE, it->second.gl_texture_id);
    backend->texParameteri(GL_TEXTURE_2D_VALUE, GL_TEXTURE_WRAP_S_VALUE, gl_wrap_mode);
    backend->texParameteri(GL_TEXTURE_2D_VALUE, GL_TEXTURE_WRAP_T_VALUE, gl_wrap_mode);
}

uint32_t TextureManager::uploadSpriteWithPalette(std::shared_ptr<sprites::DC6Sprite> sprite,
                                                 uint32_t direction, uint32_t frame,
                                                 const std::vector<uint32_t>& palette) {
    if (!sprite) {
        return 0;
    }

    if (direction >= sprite->getDirectionCount() ||
        frame >= sprite->getFramesPerDirection()) {
        return 0;
    }

    sprites::DC6Frame frame_info = sprite->getFrame(direction, frame);

    std::vector<uint8_t> rgba_data = sprite->getFrameImageWithPalette(direction, frame, palette);
    if (rgba_data.empty()) {
        return 0;
    }

    uint32_t expected_size = frame_info.width * frame_info.height * 4;
    if (rgba_data.size() != expected_size) {
        uint32_t pixel_count = rgba_data.size() / 4;
        uint32_t dimension = 2;

        for (uint32_t d = 1; d * d <= pixel_count; ++d) {
            if (d * d == pixel_count) {
                dimension = d;
            }
        }

        return createTexture(rgba_data.data(), dimension, dimension);
    }

    return createTexture(rgba_data.data(), frame_info.width, frame_info.height);
}

} // namespace d2::rendering
