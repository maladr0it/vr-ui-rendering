#include <stdbool.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
// #define STBI_ONLY_PNG
#include <stb/stb_image.h>
#include "utils.h"
#include "texture.h"

texture_t texture_create(char *path, enum texture_type type)
{
    stbi_set_flip_vertically_on_load(true); // TODO: this should be set globally

    char fullPath[512];
    utils_getFullPath(path, fullPath);

    int width, height, numComponents;
    unsigned char *data = stbi_load(fullPath, &width, &height, &numComponents, 0);

    if (data == NULL)
    {
        printf("Failed to load image at path: %s\n", fullPath);
        exit(EXIT_FAILURE);
    }

    GLenum format = 0;
    if (numComponents == 1)
    {
        format = GL_RED;
    }
    else if (numComponents == 3)
    {
        format = GL_RGB;
    }
    else if (numComponents == 4)
    {
        format = GL_RGBA;
    }

    texture_t texture;
    glGenTextures(1, &texture.id);
    texture.type = type;
    texture.width = width;
    texture.height = height;

    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Seems this is optional
    // We want nearest interpolation, not linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return texture;
}