#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include "v2.h"
#include "v3.h"
#include "mat4.h"

typedef struct shader
{
    unsigned int id;
} shader_t;

shader_t shader_create(char *vertexPath, char *fragmentPath);

void shader_use(shader_t program);

void shader_setBool(shader_t program, char *name, bool value);

void shader_setInt(shader_t program, char *name, int value);

void shader_setFloat(shader_t program, char *name, float value);

void shader_setV2(shader_t program, char *name, v2_t v);

void shader_setV3(shader_t program, char *name, v3_t v);

void shader_setMat4(shader_t program, char *name, mat4_t mat);

void shader_getUniformString(char *name, int index, char *property, char *out);

#endif