#include "pch.h"

#ifndef VIEWER_PROGRAM_H
#define VIEWER_PROGRAM_H

#define UNIFORM_LOAD(uniform) uniform = glGetUniformLocation(m_program, #uniform)

GLuint load_program_from_path(const WCHAR* vertexPath, const WCHAR* fragmentPath);

class program {
  public:
    GLuint model;
    GLuint viewProjection;
    GLuint cameraPosition;
    GLuint lightColors;
    GLuint lightPositions;
    GLuint directionalLight;
    GLuint shadowMap;

    GLuint emissiveFactor;
    GLuint normalTexture;
    GLuint normalTextureScale;
    GLuint occlusionTexture;
    GLuint occlusionTextureStrength;
    GLuint emissiveTexture;
    GLuint alphaCutoff;
    GLuint alphaCutoffEnabled;

    // Metallic
    GLuint baseColorFactor;
    GLuint metallicFactor;
    GLuint roughnessFactor;
    GLuint baseColorTexture;
    GLuint metallicRoughnessTexture;

    GLuint jointMat;
    GLuint jointMatSize;

    GLuint m_program;
    void load(const WCHAR* t_vertex_source, const WCHAR* t_fragment_source);
    void free();
};

#endif // !VIEWER_PROGRAM_H
