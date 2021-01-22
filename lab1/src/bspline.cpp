#include "bspline.h"

#include <cmath>

#include "util.h"

extern std::string LINESTRIP_VERTEX_SHADER;
extern std::string LINESTRIP_FRAGMENT_SHADER;

std::vector<float> BSpline::generate_linestrip() {
    float t = 0;
    std::vector<float> linestrip;
    while (t < points.size() - 3) {
        glm::vec3 p = value_for(t, (int) t);
        linestrip.push_back(p.x);
        linestrip.push_back(p.y);
        linestrip.push_back(p.z);
        t += DT;
    }
    return linestrip;
}

void BSpline::initialize_buffer() {
    std::vector<float> linestrip = generate_linestrip();
    linestrip_size = linestrip.size() / 3;
    glCall(glGenVertexArrays(1, &vao));
    glCall(glGenBuffers(1, &vbo));
    glCall(glBindVertexArray(vao));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, linestrip.size() * sizeof(float),
        linestrip.data(), GL_STATIC_DRAW));
    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0));
    glCall(glBindVertexArray(0));
}

BSpline::BSpline(const std::vector<glm::vec3>& points)
:shader {LINESTRIP_VERTEX_SHADER, LINESTRIP_FRAGMENT_SHADER} {
    this->points = std::vector<glm::vec3> {points};
    initialize_buffer();
}

void BSpline::draw(glm::mat4 view, glm::mat4 projection) {
    shader.use();
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);
    shader.setUniform("color", 1.0f, 0.6f, 0.6f, 1.0f);
    glCall(glBindVertexArray(vao));
    glCall(glDrawArrays(GL_LINE_STRIP, 0, linestrip_size));
}

static std::ostream& operator<<(std::ostream& os, glm::vec3 v) {
    return os << "[" << v.x << " " << v.y << " " << v.z << "]";
}

glm::vec3 BSpline::value_for(float t, int i) {
    float _;
    t = std::modff(t, &_);
    float t2 = t * t;
    float t3 = t * t2;
    i = i % (points.size() - 3);

    glm::vec3 r0 = points[i];
    glm::vec3 r1 = points[i + 1];
    glm::vec3 r2 = points[i + 2];
    glm::vec3 r3 = points[i + 3];

    float c0 = -t3 + 3*t2 -3*t + 1;
    float c1 = 3*t3 - 6*t2 + 4;
    float c2 = -3*t3 + 3*t2 + 3*t + 1;
    float c3 = t3;

    return 1/6.0f * (c0*r0 + c1*r1 + c2*r2 + c3*r3);
}

glm::vec3 BSpline::tangent_for(float t, int i) {
    float _;
    t = std::modff(t, &_);
    float t2 = t * t;
    i = i % (points.size() - 3);

    glm::vec3 r0 = points[i];
    glm::vec3 r1 = points[i + 1];
    glm::vec3 r2 = points[i + 2];
    glm::vec3 r3 = points[i + 3];

    float c0 = -t2 + 2*t - 1;
    float c1 = 3*t2 -4*t;
    float c2 = -3*t2 + 2*t + 1;
    float c3 = t2;

    return 1/2.0f * (c0*r0 + c1*r1 + c2*r2 + c3*r3);
}

glm::vec3 BSpline::d2_for(float t, int i) {
    float _;
    t = std::modff(t, &_);
    i = i % (points.size() - 3);

    glm::vec3 r0 = points[i];
    glm::vec3 r1 = points[i + 1];
    glm::vec3 r2 = points[i + 2];
    glm::vec3 r3 = points[i + 3];

    float c0 = -t + 1;
    float c1 = 3*t - 2;
    float c2 = -3*t + 1;
    float c3 = t;

    return c0*r0 + c1*r1 + c2*r2 + c3*r3;
}
