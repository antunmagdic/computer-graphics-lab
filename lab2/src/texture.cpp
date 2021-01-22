#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "util.h"

Texture::Texture(unsigned char* data, int width, int height) {
    glCall(glGenTextures(1, &texture_id));
    glCall(glBindTexture(GL_TEXTURE_2D, texture_id));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, 
        GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    glCall(glGenerateMipmap(GL_TEXTURE_2D));
}

void Texture::use(Shader shader) {
    glCall(glActiveTexture(GL_TEXTURE0));
    glCall(glBindTexture(GL_TEXTURE_2D, texture_id));
    shader.setUniform("texture0", 0);
}

Texture Texture::from_file(const std::string& filename) {
    int width;
    int height;
    int n_channels;
    unsigned char* data = stbi_load(
        filename.c_str(), &width, &height, &n_channels, 0);
    Texture texture {data, width, height};
    stbi_image_free(data);
    return texture;
}
