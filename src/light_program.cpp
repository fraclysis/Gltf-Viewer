#include "pch.h"

#include "light_program.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"

const float texture_vertex_data[] = {0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.0f,  1.0f, 1.0f,
                                     -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, -0.5f, 0.5f,  0.0f,  0.0f, 0.0f,

                                     0.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  -0.5f, 0.5f,  1.0f, 1.0f,
                                     0.0f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.5f,  -0.5f, 0.0f, 0.0f};

const unsigned char texture_index_data[] = {0, 1, 3, 1, 2, 3,

                                            4, 5, 7, 5, 6, 7};

void lights::load() {
    m_program = load_program_from_path(L"shaders/light_texture.vert", L"shaders/light_texture.frag");
    GL(UNIFORM_LOAD(mvp));
    GL(UNIFORM_LOAD(lightTexture));

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    GLuint buffers[2];
    GL(glGenBuffers(2, buffers));
    m_vbo = buffers[0];
    m_eao = buffers[1];
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertex_data), texture_vertex_data, GL_STATIC_DRAW));

    {
        int vao_index = 0;
        GL(glEnableVertexArrayAttrib(m_vao, vao_index));
        int count = 3;
        GL(glVertexAttribPointer(vao_index, count, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0));
    }

    {
        int vao_index = 1;
        GL(glEnableVertexArrayAttrib(m_vao, vao_index));
        int count = 2;
        GL(glVertexAttribPointer(vao_index, count, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
    }

    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eao));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texture_index_data), texture_index_data, GL_STATIC_DRAW));

    int x, y, channels;
    unsigned char* data = stbi_load("lamp.png", &x, &y, &channels, 4);

    if (data) {
        GL(glGenTextures(1, &m_texture));
        GL(glBindTexture(GL_TEXTURE_2D, m_texture));
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        GL(glGenerateMipmap(GL_TEXTURE_2D));
    }

    stbi_image_free(data);
}

void lights::draw(glm::mat4& t_vp, glm::vec3* t_light_position) {
    GL(glDisable(GL_CULL_FACE));
    GL(glDisable(GL_DEPTH_TEST));

    GL(glEnable(GL_BLEND));
    GL(glUseProgram(m_program));

    viewer::bind_texture(0, m_texture, lightTexture);

    glm::mat4 matrices[4] = {glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)};
    for (int i = 0; i < 4; i++) {
        matrices[i] = t_vp * glm::translate(matrices[i], t_light_position[i]);
    }

    GL(glUniformMatrix4fv(mvp, 4, GL_FALSE, glm::value_ptr(matrices[0])));

    GL(glBindVertexArray(m_vao));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eao));
    GL(glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, 0, 4));

    GL(glEnable(GL_DEPTH_TEST));
}

void lights::free() {
    GL(glDeleteVertexArrays(1, &m_vao));
    GLuint buffers[2] = {m_vbo, m_eao};
    GL(glDeleteBuffers(2, buffers));
    GL(glDeleteTextures(1, &m_texture));
    GL(glDeleteProgram(m_program));
}
