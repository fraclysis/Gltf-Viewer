#include "pch.h"

#include "morph.h"

// Vertex shader
const GLchar* vertexShaderSrc = R"glsl(
    #version 450 core

    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 morphPosition;
    
    layout(location = 2) in vec3 inNormal;
    layout(location = 3) in vec3 morphNormal;

    layout(xfb_buffer = 0) out vec3 outPosition;
    layout(xfb_buffer = 1) out vec3 outNormal;

    uniform float u_weight;

    void main()
    {
        outPosition = inPosition + (u_weight *  morphPosition);
        outNormal = inNormal + (u_weight *  morphNormal);
    }
)glsl";

//    layout(location = 4) in vec3 inTangent;
//    layout(location = 5) in vec3 morphTangent;
//    outTangent = inTangent + (u_weight * morphTangent);z
//    layout(xfb_buffer = 2) out vec3 outTangent;

namespace viewer {

    morph_program g_tfp;

    void morph_program::load() {
        GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
        GL(glShaderSource(v_shader, 1, &vertexShaderSrc, nullptr));
        GL(glCompileShader(v_shader));

        m_program = glCreateProgram();
        GL(glAttachShader(m_program, v_shader));

        const GLchar* feedbackVarying[] = {"outPosition", "outNormal", "outTangent"};
        GL(glTransformFeedbackVaryings(m_program, 2, feedbackVarying, GL_SEPARATE_ATTRIBS));

        GL(glLinkProgram(m_program));
        GL(glUseProgram(m_program));

        GL(glGenVertexArrays(1, &m_vao));
        GL(glBindVertexArray(m_vao));
        GL(glEnableVertexArrayAttrib(m_vao, 0));
        GL(glEnableVertexArrayAttrib(m_vao, 1));
        GL(glEnableVertexArrayAttrib(m_vao, 2));
        GL(glEnableVertexArrayAttrib(m_vao, 3));

        GL(glDeleteShader(v_shader));

        return; // --------------------------------------------
    }

    void morph_program::free() {
        GL(glDeleteVertexArrays(1, &m_vao));
        GL(glDeleteProgram(m_program));
    }

} // namespace viewer