#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>

#define glCall(x) glClearError();\
    x;\
    glPrintErrors(__LINE__, __FILE__)

static void glClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static void glPrintErrors(int line, const std::string& file) {
    auto err = glGetError();
    while (err != GL_NO_ERROR) {
        std::cout << "[OpenGL error] in file " << file 
            << ", line " << line << ": " << err << std::endl;
        err = glGetError();
    }
    return;
}

long long inline current_time_us() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}

double inline current_time_s() {
    return current_time_us() / 1E6;
}

#endif // UTIL_H
