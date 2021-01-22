#include "particles.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static glm::mat4 rotation(glm::vec3 eye, glm::vec3 up, glm::vec3 r) {
    
    glm::vec3 w = glm::normalize(glm::cross(up, eye - r));
    glm::vec3 u = glm::normalize(glm::cross(eye - r, w));
    glm::vec3 v = glm::normalize(eye - r);
    float m[] = {
        w.x, u.x, v.x, 0,
        w.y, u.y, v.y, 0,
        w.z, u.z, v.z, 0,
        0, 0, 0, 1
    };
    return glm::transpose(glm::make_mat4(m));
}

Particle::Particle(glm::vec3 r, glm::vec3 dr, float size, float time) 
:r {r}, dr {dr}, time {time} {

    float vertices[] = {
        size, size, 0.0f, 1.0f, 1.0f,
        -size, size, 0.0f, 0.0f, 1.0f,
        -size, -size, 0.0f, 0.0f, 0.0f,
        -size, -size, 0.0f, 0.0f, 0.0f,
        size, -size, 0.0f, 1.0f, 0.0f,
        size, size, 0.0f, 1.0f, 1.0f
    };

    unsigned int vbo;
    glCall(glGenVertexArrays(1, &vao));
    glCall(glGenBuffers(1, &vbo));
    glCall(glBindVertexArray(vao));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr));
    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3*sizeof(float))));
    glCall(glEnableVertexAttribArray(1));

    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindVertexArray(0));
}

void Particle::draw() {
    glCall(glBindVertexArray(vao));
    glCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

Particles::Particles(int n_particles, Texture texture, float size, glm::vec3 d2r, glm::vec3 dr0, long long time)
:shader {PARTICLES_VERTEX_SHADER, PARTICLES_FRAGMENT_SHADER}, 
texture {texture},
dr0 {dr0}, time {time}, d2r {d2r} {
    float dev = 0.05 * sqrt(dr0.x*dr0.x + dr0.y*dr0.y + dr0.z*dr0.z);
    for (int i = 0; i < n_particles; i++) {
        particles.push_back(
            Particle {
                {0, 0, 0}, 
                dr0 + dev * glm::vec3 {random.next_gaussian(), random.next_gaussian(), random.next_gaussian()},
                0.2f, (float) random.next_double()});
    }
}

void Particles::update(long long t) {
    float dt = (t - time) / 1E6f;
    for (Particle& p : particles) {
        if (p.time >= 0) {
            p.r += dt * p.dr;
            p.dr += dt * d2r;
            p.time -= 0.1 * dt;
        } else {
            float dev = sqrt(dr0.x*dr0.x + dr0.y*dr0.y + dr0.z*dr0.z);
            p.r = {0, 0, 0};
            p.dr = dr0 + dev * glm::vec3 {random.next_gaussian(), random.next_gaussian(), random.next_gaussian()};
            p.time = 1;
        }
    }
    time = t;
}

void Particles::draw(glm::vec3 eye, glm::vec3 up, glm::mat4 view, glm::mat4 projection) {
    shader.use();
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);
    texture.use(shader);
    for (Particle& p : particles) {
        shader.setUniform("life", p.time);
        shader.setUniform("model", 
            glm::translate(glm::mat4 {1}, p.r) * glm::scale(rotation(eye, up, p.r), glm::vec3 {2 - p.time}));
        p.draw();
    }
}