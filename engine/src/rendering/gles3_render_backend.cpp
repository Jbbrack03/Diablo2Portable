#ifdef __ANDROID__

#include "rendering/gles3_render_backend.h"
#include <GLES3/gl3.h>

namespace d2::rendering {

void GLES3RenderBackend::genBuffers(GLsizei n, GLuint* buffers) { ::glGenBuffers(n, buffers); }
void GLES3RenderBackend::bindBuffer(GLenum target, GLuint buffer) { ::glBindBuffer(target, buffer); }
void GLES3RenderBackend::bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) { ::glBufferData(target, size, data, usage); }
void GLES3RenderBackend::bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) { ::glBufferSubData(target, offset, size, data); }
void GLES3RenderBackend::deleteBuffers(GLsizei n, const GLuint* buffers) { ::glDeleteBuffers(n, buffers); }

void GLES3RenderBackend::genVertexArrays(GLsizei n, GLuint* arrays) { ::glGenVertexArrays(n, arrays); }
void GLES3RenderBackend::bindVertexArray(GLuint array) { ::glBindVertexArray(array); }
void GLES3RenderBackend::deleteVertexArrays(GLsizei n, const GLuint* arrays) { ::glDeleteVertexArrays(n, arrays); }

void GLES3RenderBackend::genTextures(GLsizei n, GLuint* textures) { ::glGenTextures(n, textures); }
void GLES3RenderBackend::bindTexture(GLenum target, GLuint texture) { ::glBindTexture(target, texture); }
void GLES3RenderBackend::texImage2D(GLenum target, GLint level, GLint internalformat,
                                     GLsizei width, GLsizei height, GLint border,
                                     GLenum format, GLenum type, const void* pixels) {
    ::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}
void GLES3RenderBackend::texParameteri(GLenum target, GLenum pname, GLint param) { ::glTexParameteri(target, pname, param); }
void GLES3RenderBackend::deleteTextures(GLsizei n, const GLuint* textures) { ::glDeleteTextures(n, textures); }

GLuint GLES3RenderBackend::createShader(GLenum shaderType) { return ::glCreateShader(shaderType); }
void GLES3RenderBackend::shaderSource(GLuint shader, GLsizei count, const char* const* string, const GLint* length) { ::glShaderSource(shader, count, string, length); }
void GLES3RenderBackend::compileShader(GLuint shader) { ::glCompileShader(shader); }
void GLES3RenderBackend::getShaderiv(GLuint shader, GLenum pname, GLint* params) { ::glGetShaderiv(shader, pname, params); }
void GLES3RenderBackend::deleteShader(GLuint shader) { ::glDeleteShader(shader); }

GLuint GLES3RenderBackend::createProgram() { return ::glCreateProgram(); }
void GLES3RenderBackend::attachShader(GLuint program, GLuint shader) { ::glAttachShader(program, shader); }
void GLES3RenderBackend::linkProgram(GLuint program) { ::glLinkProgram(program); }
void GLES3RenderBackend::getProgramiv(GLuint program, GLenum pname, GLint* params) { ::glGetProgramiv(program, pname, params); }
void GLES3RenderBackend::deleteProgram(GLuint program) { ::glDeleteProgram(program); }
void GLES3RenderBackend::useProgram(GLuint program) { ::glUseProgram(program); }

void GLES3RenderBackend::enableVertexAttribArray(GLuint index) { ::glEnableVertexAttribArray(index); }
void GLES3RenderBackend::vertexAttribPointer(GLuint index, GLint size, GLenum type,
                                              GLboolean normalized, GLsizei stride, const void* pointer) {
    ::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void GLES3RenderBackend::drawArrays(GLenum mode, GLint first, GLsizei count) { ::glDrawArrays(mode, first, count); }
void GLES3RenderBackend::drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) { ::glDrawElements(mode, count, type, indices); }

void GLES3RenderBackend::enable(GLenum cap) { ::glEnable(cap); }
void GLES3RenderBackend::disable(GLenum cap) { ::glDisable(cap); }
void GLES3RenderBackend::blendFunc(GLenum sfactor, GLenum dfactor) { ::glBlendFunc(sfactor, dfactor); }
void GLES3RenderBackend::depthFunc(GLenum func) { ::glDepthFunc(func); }
void GLES3RenderBackend::depthMask(GLboolean flag) { ::glDepthMask(flag); }

GLenum GLES3RenderBackend::getError() { return ::glGetError(); }

} // namespace d2::rendering

#endif // __ANDROID__
