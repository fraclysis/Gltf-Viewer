#pragma once
#include "pch.h"

#include "app_camera.h"
#include "program.h"
#include <glm/glm.hpp>

struct lights {
    GLuint m_program;
    GLuint lightTexture;
    GLuint mvp;

    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_eao;
    GLuint m_texture;

    void load();
    void draw(glm::mat4& t_vp, glm::vec3* t_position);
    void free();
};