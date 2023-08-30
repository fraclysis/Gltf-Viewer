#include "pch.h"

#ifndef VIEWER_RENDERER_H_
#define VIEWER_RENDERER_H_

namespace viewer {

    void bind_texture(int t_unit, GLuint t_texture, GLuint t_uniform_location);

    void bind_texture_sampler(int t_unit, GLuint t_texture, GLuint t_uniform_location, GLuint t_sampler);

    void adjust_viewport(int width, int height);

    /// Adjusts the viewport to target aspect ratio to set the border color use
    /// glClearColor
    void adjust_viewport_to(float t_width, float t_height, float t_aspect_ratio);

    void log_renderer_api_info();

    void APIENTRY myOpenGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                   const GLchar* message, const GLvoid* userParam);

} // namespace viewer

#endif // !VIEWER_RENDERER_H_
