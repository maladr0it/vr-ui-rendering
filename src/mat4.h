#ifndef MAT4_H
#define MAT4_H

#include "v3.h"

typedef struct mat4
{
    float m[4][4];
} mat4_t;

mat4_t mat4_mul(mat4_t a, mat4_t b);

mat4_t mat4_createIdentity(void);

mat4_t mat4_createScale(v3_t s);

mat4_t mat4_createRotX(float theta);

mat4_t mat4_createRotY(float theta);

mat4_t mat4_createRotZ(float theta);

mat4_t mat4_createTranslate(v3_t t);

mat4_t mat4_createProj(float aspectRatio, float fov, float zNear, float zFar);

mat4_t mat4_createLookAt(v3_t pos, v3_t target, v3_t up);

#endif