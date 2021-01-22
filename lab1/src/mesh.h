#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh {
public:
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void set_color(float r, float g, float b);

    void draw(Shader shader);

    static Mesh from_file(const std::string& filepath);

private:

    unsigned int vao; // vertex array object
    unsigned int vbo; // vertex buffer object
    unsigned int ebo; // element buffer object

    float r = 0.59;
    float g = 0.66;
    float b = 0.84;

    void initialize_buffers();

};

#endif // MESH_H