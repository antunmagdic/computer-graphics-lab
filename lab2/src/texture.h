#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <GL/glew.h>

#include "shader.h"

class Texture {
public:

    Texture(unsigned char* data, int width, int height);

    void use(Shader shader);

    static Texture from_file(const std::string& filename);

private:

    unsigned int texture_id;

};

#endif // TEXTURE_H
