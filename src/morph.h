#include "pch.h"

#ifndef VIEWER_MORPH_H
#define VIEWER_MORPH_H

namespace viewer {

    struct morph_program {
        GLuint m_tfo;
        GLuint m_program;
        GLuint m_vao;
        void load();
        void free();
    };

} // namespace viewer

#endif // !VIEWER_MORPH_H
