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

#include "bspline.h"
#include "drawutil.h"
#include "mesh.h"
#include "shader.h"
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

static glm::mat4 rotation_for(glm::vec3 dp, glm::vec3 d2p) {
    if (d2p.x == 0 && d2p.y == 0 && d2p.z == 0) {
        d2p = {4576.678534856f, 4255646.753627535f, 3642.4563756476f};
    }
    glm::vec3 w = glm::normalize(dp);
    glm::vec3 u = glm::normalize(glm::cross(dp, d2p));
    glm::vec3 v = glm::normalize(glm::cross(w, u));
    float m[] = {
        w.x, u.x, v.x, 0,
        w.y, u.y, v.y, 0,
        w.z, u.z, v.z, 0,
        0, 0, 0, 1
    };
    return glm::transpose(glm::make_mat4(m));
}

glm::vec3 STARTING_ORIENTATION = {0, 0, 1};

int main(int argc, char *argv[]) {

    if (argc < 4) {
        std::cout << "Arguments missing." << std::endl;
        std::cout << "Usage: ./main <object> <bspline> <eye_lookat>" << std::endl;
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
    // glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glCall(glEnable(GL_DEPTH_TEST));
    XYZDrawer xyz {1.0f};

    Mesh m = Mesh::from_file(std::string {argv[1]});
    BSpline bspline = BSpline {parse_vectors(std::string {argv[2]})};
    glm::vec3 eye = parse_vectors(std::string {argv[3]})[0];
    glm::vec3 look_at = parse_vectors(std::string {argv[3]})[1];
    Shader shader {VERTEX_SHADER, FRAGMENT_SHADER};
    glm::vec3 light_source {1.0f, 1.0f, 1.0f};
    glm::vec3 light_source_position {80, -10, -10};
    glm::vec3 ambient_light {0.3f, 0.3f, 0.3f};
    glm::mat4 view = glm::lookAt(eye, look_at, glm::vec3 {0, 0, 1});
    glm::mat4 projection = glm::perspective((float) M_PI / 4, 1.0f, 0.1f, 100.0f);
    long long start_time_us = current_time_us();
    const float f = 0.25f;
    while (!glfwWindowShouldClose(window)) {

        glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        xyz.draw(view, projection);
        bspline.draw(view, projection);
        
        float t_s = (current_time_us() - start_time_us) / 1E6f;

        glm::vec3 p = bspline.value_for(t_s, (int) t_s);
        glm::vec3 dp = bspline.tangent_for(t_s, (int) t_s);
        glm::vec3 d2p = bspline.d2_for(t_s, (int) t_s);
        draw_line(p, p + 3.0f*glm::normalize(dp), view, projection);
        // draw_line(p, p + glm::normalize(d2p), view, projection);
        // p = {0, 0, 0};

        shader.use();
        shader.setUniform("light_source", light_source);
        shader.setUniform("light_source_position", light_source_position);
        shader.setUniform("ambient_light", ambient_light);
        
        // shader.setUniform("model", glm::scale(glm::rotate(
        //     glm::translate(glm::mat4 {1}, p), 
        //     acosf(glm::dot(glm::normalize(STARTING_ORIENTATION), glm::normalize(p))), 
        //     glm::cross(STARTING_ORIENTATION, dp)),
        //     glm::vec3 {5, 5, 5}));

        shader.setUniform("model", glm::translate(glm::mat4 {1}, p) * 
            glm::scale(
                rotation_for(dp, d2p), glm::vec3 {5, 5, 5}) * 
                glm::rotate(glm::mat4 {1}, (float) -M_PI_2, {1, 0, 0}) * 
                glm::rotate(glm::mat4 {1}, (float) M_PI_2, {0, 1, 0}));
        
        shader.setUniform("view", view);
        shader.setUniform("projection", projection);

        m.draw(shader);

        glCall(glfwSwapBuffers(window));
        glCall(glfwPollEvents());
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
