#ifndef XYZDRAWER_H
#define XYZDRAWER_H

#include <string>

#include "util.h"

extern std::string LINESTRIP_VERTEX_SHADER;
extern std::string LINESTRIP_FRAGMENT_SHADER;

class XYZDrawer {
public:
    
    XYZDrawer(float size)
    :shader {LINESTRIP_VERTEX_SHADER, LINESTRIP_FRAGMENT_SHADER} {
        float x_vertices[] = {0.0f, 0.0f, 0.0f, size, 0.0f, 0.0f};
        float y_vertices[] = {0.0f, 0.0f, 0.0f, 0.0f, size, 0.0f};
        float z_vertices[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, size};

        glCall(glGenVertexArrays(3, vao));
        glCall(glGenBuffers(3, vbo));

        glCall(glBindVertexArray(vao[0]));
        glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo[0]));
        glCall(glBufferData(
            GL_ARRAY_BUFFER, 6 * sizeof(float), x_vertices, GL_STATIC_DRAW));
        glCall(glEnableVertexAttribArray(0));
        glCall(glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0));

        glCall(glBindVertexArray(vao[1]));
        glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo[1]));
        glCall(glBufferData(
            GL_ARRAY_BUFFER, 6 * sizeof(float), y_vertices, GL_STATIC_DRAW));
        glCall(glEnableVertexAttribArray(0));
        glCall(glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0));

        glCall(glBindVertexArray(vao[2]));
        glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo[2]));
        glCall(glBufferData(
            GL_ARRAY_BUFFER, 6 * sizeof(float), z_vertices, GL_STATIC_DRAW));
        glCall(glEnableVertexAttribArray(0));
        glCall(glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0));

        glCall(glBindVertexArray(0));
    }

    void draw(glm::mat4 view, glm::mat4 projection) {
        shader.use();
        shader.setUniform("view", view);
        shader.setUniform("projection", projection);

        shader.setUniform("color", 1.0f, 0.0f, 0.0f, 1.0f);
        glCall(glBindVertexArray(vao[0]));
        glCall(glDrawArrays(GL_LINE_STRIP, 0, 2));
        shader.setUniform("color", 0.0f, 1.0f, 0.0f, 1.0f);
        glCall(glBindVertexArray(vao[1]));
        glCall(glDrawArrays(GL_LINE_STRIP, 0, 2));
        shader.setUniform("color", 0.0f, 0.0f, 1.0f, 1.0f);
        glCall(glBindVertexArray(vao[2]));
        glCall(glDrawArrays(GL_LINE_STRIP, 0, 2));
    }

private:
    Shader shader;
    unsigned int vao[3];
    unsigned int vbo[3];
};

#endif // XYZDRAWER_H
