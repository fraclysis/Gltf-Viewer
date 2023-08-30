#include "pch.h"

#include "renderer.h"

namespace viewer {

    void bind_texture(int t_unit, GLuint t_texture, GLuint t_uniform_location) {
        GL(glActiveTexture(GL_TEXTURE0 + t_unit));
        GL(glBindTexture(GL_TEXTURE_2D, t_texture));
        GL(glUniform1i(t_uniform_location, t_unit));
    }

    void bind_texture_sampler(int t_unit, GLuint t_texture, GLuint t_uniform_location, GLuint t_sampler) {
        bind_texture(t_unit, t_texture, t_uniform_location);
        GL(glBindSampler(t_unit, t_sampler));
    }

    void APIENTRY myOpenGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                   const GLchar* message, const GLvoid* userParam) {

        int filter = GL_DEBUG_SEVERITY_NOTIFICATION;
        GLenum severitycmp = severity;
        // minor fixup for filtering so notification becomes lowest priority
        if (GL_DEBUG_SEVERITY_NOTIFICATION == filter) {
            filter = GL_DEBUG_SEVERITY_LOW_ARB + 1;
        }
        if (GL_DEBUG_SEVERITY_NOTIFICATION == severitycmp) {
            severitycmp = GL_DEBUG_SEVERITY_LOW_ARB + 1;
        }

        if (!filter || (int)severitycmp <= filter) {

            // static std::map<GLuint, bool> ignoreMap;
            // if(ignoreMap[id] == true)
            //     return;
            const char* strSource = "0";
            const char* strType = strSource;
            switch (source) {
            case GL_DEBUG_SOURCE_API_ARB:
                strSource = "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
                strSource = "WINDOWS";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
                strSource = "SHADER COMP.";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
                strSource = "3RD PARTY";
                break;
            case GL_DEBUG_SOURCE_APPLICATION_ARB:
                strSource = "APP";
                break;
            case GL_DEBUG_SOURCE_OTHER_ARB:
                strSource = "OTHER";
                break;
            }
            switch (type) {
            case GL_DEBUG_TYPE_ERROR_ARB:
                strType = "ERROR";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
                strType = "Deprecated";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
                strType = "Undefined";
                break;
            case GL_DEBUG_TYPE_PORTABILITY_ARB:
                strType = "Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE_ARB:
                strType = "Performance";
                break;
            case GL_DEBUG_TYPE_OTHER_ARB:
                strType = "Other";
                break;
            }
            switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH_ARB:
                log_error("ARB_debug : %s High - %s - %s : %s\n", "WindowName", strSource, strType, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM_ARB:
                log_warn("ARB_debug : %s Medium - %s - %s : %s\n", "WindowName", strSource, strType, message);
                break;
            case GL_DEBUG_SEVERITY_LOW_ARB:
                log_warn("ARB_debug : %s Low - %s - %s : %s\n", "WindowName", strSource, strType, message);
                break;
            default:
                // log_info("ARB_debug : comment - %s - %s : %s\n", strSource,
                // strType, message);
                break;
            }
        }
    }

    void adjust_viewport(int width, int height) { glViewport(0, 0, width, height); }

    void adjust_viewport_to(float t_width, float t_height, float t_aspect_ratio) {
        float windows_aspect_ratio = t_width / t_height;

        float scale_height = windows_aspect_ratio / t_aspect_ratio;

        if (scale_height < 1.0) {
            // TOP BLACK height
            float scaled_height = scale_height * t_height;
            GL(glViewport(0, ((GLsizei)(t_height - scaled_height) / 2), (GLsizei)t_width, (GLsizei)scaled_height));
        } else {
            // SIDE BLACK width
            float scaled_width = (1.0f / scale_height) * t_width;
            GL(glViewport(((GLsizei)(t_width - scaled_width) / 2), 0, (GLsizei)scaled_width, (GLsizei)t_height));
        }
    }

    void log_renderer_api_info() {
        log_info("Version: %s", glGetString(GL_VERSION));
        log_info("Vendor: %s", glGetString(GL_VENDOR));
        log_info("Renderer: %s", glGetString(GL_RENDERER));
        log_info("Shading language version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

} // namespace viewer