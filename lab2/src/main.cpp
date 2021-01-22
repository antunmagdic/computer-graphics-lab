#define GL_SILENCE_DEPRECATION

#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "drawutil.h"
#include "mesh.h"
#include "particles.h"
#include "shader.h"
#include "texture.h"
#include "util.h"
#include "xyzdrawer.h"

extern std::string VERTEX_SHADER;
extern std::string FRAGMENT_SHADER;

std::vector<glm::vec3> parse_vectors(const std::string& filepath) {
    std::ifstream ifs {filepath};
    std::string s;
    std::vector<glm::vec3> vectors;
    while (std::getline(ifs, s)) {
        if (s.empty()) continue;
        auto n = s.find(" ");
        auto xs = s.substr(0, n);
        s = s.substr(n + 1, s.length() - n);
        n = s.find(" ");
        auto ys = s.substr(0, n);
        s = s.substr(n + 1, s.length() - n);
        auto zs = s;
        auto x = std::stof(xs);
        auto y = std::stof(ys);
        auto z = std::stof(zs);
        vectors.push_back(glm::vec3 {x, y, z});
    }
    return vectors;
}

class glfw_error : public std::logic_error {
public:
    explicit glfw_error(const std::string& str) :std::logic_error {str} { }
    explicit glfw_error(const char* str) :std::logic_error {str} { }
};

static GLFWwindow* createWindow(int width, int height, const char* title) {
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(
        width, height, title, nullptr, nullptr);
    if (!window) {
        throw glfw_error("glfwCreateWindow failed.");
    }
    return window;
}

static void printOpenGLVersion() {
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        std::cout << "usage: ./main <object> <particle> <view>" << std::endl;
        exit(1);
    }

    if (!glfwInit()) {
        std::cout << "glfwInit failed." << std::endl;
        exit(1);
    }

    GLFWwindow* window;
    try {
        window = createWindow(720, 720, "My Window");
    } catch (glfw_error err) {
        glfwTerminate();
        std::cout << err.what() << std::endl;
        exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit failed." << std::endl;
        exit(1);
    }

    printOpenGLVersion();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glCall(glEnable(GL_BLEND));

    XYZDrawer xyz {1.0f};
    
    Texture particle_texture = Texture::from_file(argv[2]);

    Mesh m = Mesh::from_file(std::string {argv[1]});
    Shader object_shader {VERTEX_SHADER, FRAGMENT_SHADER};

    glm::vec3 eye = parse_vectors(std::string {argv[3]})[0];
    glm::vec3 look_at = parse_vectors(std::string {argv[3]})[1];
    glm::vec3 up {0, 0, 1};
    glm::vec3 light_source {1.0f, 1.0f, 1.0f};
    glm::vec3 light_source_position {80, 10, 10};
    glm::vec3 ambient_light {0.3f, 0.3f, 0.3f};
    glm::mat4 view = glm::lookAt(eye, look_at, up);
    glm::mat4 projection = glm::perspective((float) M_PI / 4, 1.0f, 0.1f, 100.0f);

    long long start_time_us = current_time_us();
    Particles particles {100, particle_texture, 1.0f, 
        {0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 0.1f}, start_time_us};
    const float f = 1.0f;
    while (!glfwWindowShouldClose(window)) {

        glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        long long t_us = current_time_us();
        float t_s = (t_us - start_time_us) / 1E6f;

        particles.update(t_us);

        xyz.draw(view, projection);
        
        particles.draw(eye, up, view, projection);

        object_shader.use();
        object_shader.setUniform("light_source", light_source);
        object_shader.setUniform("light_source_position", light_source_position);
        object_shader.setUniform("ambient_light", ambient_light);
        
        object_shader.setUniform("model", glm::rotate(
            glm::rotate(
                glm::scale(glm::mat4 {1}, glm::vec3 {3, 3, 3}),
            f * t_s, glm::vec3 {0, 0, 1}), 
        (float) M_PI_2, glm::vec3 {1, 0, 0}));
        
        object_shader.setUniform("view", view);
        object_shader.setUniform("projection", projection);

        m.draw(object_shader);

        glCall(glfwSwapBuffers(window));
        glCall(glfwPollEvents());
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
