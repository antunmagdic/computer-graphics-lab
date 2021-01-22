#include "shader.h"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

static void handle_compilation_errors
(unsigned int shader, const std::string& name) {
    int success;
    glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        int length;
        glCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
        char* message = new char[length];
        glCall(glGetShaderInfoLog(shader, length, &length, message));
        std::cout << name << " failed to compile: " << message << std::endl;
        delete[] message;
    }
}

static void handle_linkage_errors
(unsigned int program, const std::string& name) {
    int success;
    glCall(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success) {
        int length;
        glCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        char* message = new char[length];
        glCall(glGetProgramInfoLog(program, length, &length, message));
        std::cout << name << " failed to link: " << message << std::endl;
        delete[] message;
    }
}

Shader::Shader(const std::string& vs, const std::string& fs) {
    const char* vsc = vs.c_str();
    const char* fsc = fs.c_str();

    unsigned int vertex_shader;
    unsigned int fragment_shader;

    glCall(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
    glCall(glShaderSource(vertex_shader, 1, &vsc, nullptr));
    glCall(glCompileShader(vertex_shader));
    handle_compilation_errors(vertex_shader, "Vertex shader");
    glCall(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
    glCall(glShaderSource(fragment_shader, 1, &fsc, nullptr));
    glCall(glCompileShader(fragment_shader));
    handle_compilation_errors(fragment_shader, "Fragment shader");

    glCall(shader_program = glCreateProgram());
    glCall(glAttachShader(shader_program, vertex_shader));
    glCall(glAttachShader(shader_program, fragment_shader));
    glCall(glLinkProgram(shader_program));
    handle_linkage_errors(shader_program, "Shader program");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::use() {
    glCall(glUseProgram(shader_program));
}

void Shader::setUniform(const std::string& name, int x) {
    glUniform1i(glGetUniformLocation(shader_program, name.c_str()), x);
}

void Shader::setUniform(const std::string& name, float x) {
    glUniform1f(glGetUniformLocation(shader_program, name.c_str()), x);
}

void Shader::setUniform(const std::string& name, float x, float y) {
    glUniform2f(glGetUniformLocation(shader_program, name.c_str()), x, y);
}

void Shader::setUniform(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shader_program, name.c_str()), x, y, z);
}

void Shader::setUniform
(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader_program, name.c_str()), x, y, z, w);
}

void Shader::setUniform(const std::string& name, const glm::vec3& vector) {
    glUniform3f(glGetUniformLocation(shader_program, name.c_str()),
        vector.x, vector.y, vector.z);
}

void Shader::setUniform(const std::string& name, const glm::vec4& vector) {
    glUniform4f(glGetUniformLocation(shader_program, name.c_str()),
        vector.x, vector.y, vector.z, vector.w);
}

void Shader::setUniform
(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(glGetUniformLocation(shader_program, name.c_str()),
        1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::use_none() {
    glCall(glUseProgram(0));
}
