#ifndef BSPLINE_H
#define BSPLINE_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "shader.h"

const float DT = 0.01;

class BSpline {
public:

    BSpline(const std::vector<glm::vec3>& points);

    void draw(glm::mat4 view, glm::mat4 projection);

    glm::vec3 value_for(float t, int i);

    glm::vec3 tangent_for(float t, int i);

    glm::vec3 d2_for(float t, int i);

    static BSpline from_file(const std::string& filepath);

private:

    std::vector<glm::vec3> points;
    unsigned int vao;
    unsigned int vbo;
    Shader shader;
    int linestrip_size;

    void initialize_buffer();
    std::vector<float> generate_linestrip();

};


#endif // BSPLINE_H
