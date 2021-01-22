#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "util.h"

void Mesh::initialize_buffers() {
    glCall(glGenVertexArrays(1, &vao));
    glCall(glGenBuffers(1, &vbo));
    glCall(glGenBuffers(1, &ebo));

    glCall(glBindVertexArray(vao));
    
    glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW));
    
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
    
    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
        sizeof(Vertex), (void*) offsetof(Vertex, position)));
    glCall(glEnableVertexAttribArray(1));
    glCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*) offsetof(Vertex, normal)));

    glCall(glBindVertexArray(0));
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->indices = indices;
    this->initialize_buffers();
}

void Mesh::set_color(float r, float g, float b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

void Mesh::draw(Shader shader) {
    shader.setUniform("object_color", r, g, b, 1.0f);
    glCall(glBindVertexArray(vao));
    glCall(glDrawElements(
        GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));
    glCall(glBindVertexArray(0));
}

static glm::vec3 as_glm_vec3(aiVector3D v) {
    return glm::vec3 {v.x, v.y, v.z};
}

Mesh Mesh::from_file(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, 
        aiProcess_Triangulate | aiProcess_GenNormals);
    aiMesh* mesh = scene->mMeshes[0];
    std::vector<Vertex> vertices;
    for (int i = 0; i < mesh->mNumVertices; i++) {
        auto x = mesh->mVertices[i];
        vertices.push_back(Vertex 
            {as_glm_vec3(mesh->mVertices[i]), as_glm_vec3(mesh->mNormals[i])});
    }
    std::vector<unsigned int> indices;
    for (int i = 0; i < mesh->mNumFaces; i++) {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }
    return Mesh {vertices, indices};
}
