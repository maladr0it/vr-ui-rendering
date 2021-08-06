#ifndef TEXTURE_H
#define TEXTURE_H

enum texture_type
{
    DIFFUSE,
    SPECULAR,
};

typedef struct texture
{
    unsigned int id;
    int width;
    int height;
    enum texture_type type;
} texture_t;

texture_t texture_create(char *path, enum texture_type type);

#endif