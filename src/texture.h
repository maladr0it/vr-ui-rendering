#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct texture
{
    unsigned int id;
} texture_t;

texture_t texture_create(char *path);

#endif