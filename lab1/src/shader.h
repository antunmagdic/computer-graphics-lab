#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "util.h"

class Shader {
public:

    Shader(const std::string& vs, const std::string& fs);

    void use();

    void setUniform
    (const std::string& name, float x);
    void setUniform
    (const std::string& name, float x, float y);
    void setUniform
    (const std::string& name, float x, float y, float z);
    void setUniform
    (const std::string& name, float x, float y, float z, float w);
    void setUniform
    (const std::string& name, const glm::vec3& vector);
    void setUniform
    (const std::string& name, const glm::vec4& vector);
    void setUniform
    (const std::string& name, const glm::mat4& matrix);

    static void use_none();

private:

    unsigned int shader_program;

};

#endif // SHADER_H