#include "pch.h"

#include <share.h>

std::mutex g_log_mutex;
thread_local size_t gt_thread_id = -1;
FILE* g_log_out = stdout;

void log_open() {
#ifdef _WIN32
    FILE* file = _fsopen("log.txt", "a", SH_DENYWR);
#else
    FILE* file = fopen("log.txt", "a");
#endif // _WIN32

    if (file != NULL) {
        g_log_out = file;
    }
}

void log_close() {
    if ((g_log_out == NULL) || (g_log_out == stdout)) {
        return;
    }

    int err = fclose(g_log_out);
    g_log_out = stdout;
    if (err == -1) {
        log_error("Failed to close the file handle %p", g_log_out);
    }
}

void log_prefix(const char* t_log_level, const char* t_file_path, int t_line) {
    (void)fprintf_s(g_log_out, "\n[%2zd][%s:%d] %s ", gt_thread_id, t_file_path, t_line, t_log_level);
}

static const char* glGetErrorEnum(GLenum t_enum) {
    switch (t_enum) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";

    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";

    default:
        return "NO_MATCH";
    }
}

void CheckOpenGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        log_error("OpenGL %08x %s, at %s:%i - for %s", err, glGetErrorEnum(err), fname, line, stmt);
        log_flush();
    }
}