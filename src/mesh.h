#ifndef MESH_H
#define MESH_H

#include "v2.h"
#include "v3.h"
#include "texture.h"
#include "shader.h"

typedef struct mesh_vert {
    v3_t pos;
    v3_t normal;
    v2_t texCoords;
} mesh_vert_t;

typedef struct mesh {
    mesh_vert_t *verts;
    int vertsLen;
    texture_t *textures;
    int texturesLen;

    unsigned int VAO;
    unsigned int VBO;
} mesh_t;

int mesh_loadVerts(char *path, mesh_vert_t *verts);

mesh_t mesh_create(
    mesh_vert_t *vertices, int verticesLen,
    texture_t *textures, int texturesLen);

void mesh_setVerts(mesh_t mesh, mesh_vert_t *verts, int vertsLen);

void mesh_render(mesh_t mesh, shader_t shader);

#endif