#include "pch.h"

#include "program.h"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>

GLuint CompileShader(GLenum shaderType, const char* shaderSource, int shaderSourceSize, std::string& error_message) {
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &shaderSource, &shaderSourceSize);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        // Provide the Infolog in whatever manor you deem best.
        // Exit with failure.
        if (shaderType == GL_VERTEX_SHADER)
            std::cout << "GL_VERTEX_SHADER" << std::endl;

        if (shaderType == GL_FRAGMENT_SHADER)
            std::cout << "GL_FRAGMENT_SHADER" << std::endl;

        std::cout << errorLog.data() << std::endl;

        glDeleteShader(shader); // Don't leak the shader.
        if (errorLog.size()) {
            error_message = std::string(errorLog.data());
        } else {
            error_message = "Failed to compile shader.";
        }
        return (GLuint)0;
    }

    return shader;
}

GLuint createProgram(std::vector<char>& vertexSource, std::vector<char>& fragmentSource) {
    std::string error_message;
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource.data(), (int)vertexSource.size(), error_message);
    if (error_message.size()) {
        throw error_message;
    }

    // error_message.clear();

    GLuint fragmentShader =
        CompileShader(GL_FRAGMENT_SHADER, fragmentSource.data(), (int)fragmentSource.size(), error_message);
    if (error_message.size()) {
        glDeleteShader(vertexShader);
        throw error_message;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::string(infoLog.data());
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
};

#include <system_error>

std::vector<char> read_to_vector(const WCHAR* path) {
    HANDLE file = CreateFileW(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        throw std::system_category().message(GetLastError());
    }

    LARGE_INTEGER size;
    if (GetFileSizeEx(file, &size) == 0) {
        CloseHandle(file);
        throw std::system_category().message(GetLastError());
    }

    std::vector<char> contents;
    contents.resize((size.QuadPart));
    DWORD to_read = (DWORD)size.QuadPart;
    DWORD read = 0;

    if (ReadFile(file, contents.data(), to_read, &read, NULL) == 0) {
        CloseHandle(file);
        throw std::system_category().message(GetLastError());
    }

    if (read != to_read) {
        CloseHandle(file);
        throw std::system_category().message(GetLastError());
    }

    if (CloseHandle(file) == 0) {
        log_error("Close Handle Error: %s", std::system_category().message(GetLastError()).c_str());
    }

    return contents;
}

GLuint load_program_from_path(const WCHAR* vertexPath, const WCHAR* fragmentPath) {
    auto vertexSource = read_to_vector(vertexPath);
    auto fragmentSource = read_to_vector(fragmentPath);

    GLuint res = createProgram(vertexSource, fragmentSource);

    return res;
}

void program::load(const WCHAR* t_vertex_source, const WCHAR* t_fragment_source) {
    m_program = load_program_from_path(t_vertex_source, t_fragment_source);

    GL(UNIFORM_LOAD(model));
    GL(UNIFORM_LOAD(viewProjection));
    GL(UNIFORM_LOAD(cameraPosition));
    GL(UNIFORM_LOAD(lightColors));
    GL(UNIFORM_LOAD(lightPositions));
    GL(UNIFORM_LOAD(directionalLight));
    GL(UNIFORM_LOAD(shadowMap));

    GL(UNIFORM_LOAD(emissiveFactor));
    GL(UNIFORM_LOAD(normalTexture));
    GL(UNIFORM_LOAD(normalTextureScale));
    GL(UNIFORM_LOAD(occlusionTexture));
    GL(UNIFORM_LOAD(occlusionTextureStrength));
    GL(UNIFORM_LOAD(emissiveTexture));

    GL(UNIFORM_LOAD(baseColorFactor));
    GL(UNIFORM_LOAD(metallicFactor));
    GL(UNIFORM_LOAD(roughnessFactor));
    GL(UNIFORM_LOAD(baseColorTexture));
    GL(UNIFORM_LOAD(metallicRoughnessTexture));
    GL(UNIFORM_LOAD(jointMat));
    GL(UNIFORM_LOAD(jointMatSize));
    GL(UNIFORM_LOAD(alphaCutoff));
    GL(UNIFORM_LOAD(alphaCutoffEnabled));

    GL(glUseProgram(m_program));
}

void program::free() {
    GL(glUseProgram(0));
    GL(glDeleteProgram(m_program));
}
