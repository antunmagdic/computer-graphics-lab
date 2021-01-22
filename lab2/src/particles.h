#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "random.h"
#include "shader.h"
#include "texture.h"

struct Particle {
    glm::vec3 r;
    glm::vec3 dr;
    float time;

    Particle(glm::vec3 r, glm::vec3 dr, float size, float time);

    void draw();

private:

    unsigned int vao;
};

extern std::string PARTICLES_VERTEX_SHADER;
extern std::string PARTICLES_FRAGMENT_SHADER;

class Particles {
public:

    Particles(int n_particles, Texture texture, float size, glm::vec3 d2r, glm::vec3 dr0, long long time);

    void update(long long t);

    void draw(glm::vec3 eye, glm::vec3 up, glm::mat4 view, glm::mat4 projection);

private:

    Shader shader;
    Texture texture;
    std::vector<Particle> particles;
    glm::vec3 d2r;
    glm::vec3 dr0;
    Random random;
    long long time;

};

#endif // PARTICLES_H