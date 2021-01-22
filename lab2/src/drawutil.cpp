#include "drawutil.h"

#include <string>

#include "shader.h"
#include "util.h"

extern std::string LINESTRIP_VERTEX_SHADER;
extern std::string LINESTRIP_FRAGMENT_SHADER;

static bool first_draw_line = true;
static unsigned int vao;
static unsigned int vbo;
static Shader* shaderp;

static void initialize_draw_line() {
    glCall(glGenVertexArrays(1, &vao));
    glCall(glGenBuffers(1, &vbo));
    glCall(glBindVertexArray(vao));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0));
    glCall(glBindVertexArray(0));
    shaderp = new Shader {LINESTRIP_VERTEX_SHADER, LINESTRIP_FRAGMENT_SHADER};
}

void draw_line
(glm::vec3 from, glm::vec3 to, glm::mat4 view, glm::mat4 projection) {
    if (first_draw_line) {
        initialize_draw_line();
        first_draw_line = false;
    }
    shaderp->use();
    shaderp->setUniform("view", view);
    shaderp->setUniform("projection", projection);
    shaderp->setUniform("color", glm::vec4 {1.0f, 1.0f, 1.0f, 1.0f});
    
    float buffer_data[] = {from.x, from.y, from.z, to.x, to.y, to.z};
    glCall(glBindVertexArray(vao));
    glCall(glBufferData(
        GL_ARRAY_BUFFER, 6 * sizeof(float), buffer_data, GL_DYNAMIC_DRAW));
    
    glCall(glDrawArrays(GL_LINE_STRIP, 0, 2));
}
