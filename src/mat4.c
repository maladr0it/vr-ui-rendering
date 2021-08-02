#include <stdlib.h>
#include <math.h>
#include "mat4.h"

mat4_t mat4_mul(mat4_t a, mat4_t b)
{
    mat4_t result;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            result.m[row][col] = a.m[row][0] * b.m[0][col] + a.m[row][1] * b.m[1][col] + a.m[row][2] * b.m[2][col] + a.m[row][3] * b.m[3][col];
        }
    }

    return result;
}

mat4_t mat4_createIdentity()
{
    mat4_t result;

    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4_t mat4_createScale(v3_t s)
{
    mat4_t result;

    result.m[0][0] = s.x;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = s.y;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = s.z;
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4_t mat4_createRotX(float theta)
{
    mat4_t result;

    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = cosf(theta);
    result.m[1][2] = -sinf(theta);
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = sinf(theta);
    result.m[2][2] = cosf(theta);
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4_t mat4_createRotY(float theta)
{
    mat4_t result;

    result.m[0][0] = cosf(theta);
    result.m[0][1] = 0;
    result.m[0][2] = sinf(theta);
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = -sinf(theta);
    result.m[2][1] = 0;
    result.m[2][2] = cosf(theta);
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4_t mat4_createRotZ(float theta)
{
    mat4_t result;

    result.m[0][0] = cosf(theta);
    result.m[0][1] = -sinf(theta);
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = sinf(theta);
    result.m[1][1] = cosf(theta);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4_t mat4_createTranslate(v3_t t)
{
    mat4_t result;

    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = t.x;

    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[1][3] = t.y;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
    result.m[2][3] = t.z;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4_t mat4_createProj(float aspectRatio, float fov, float zNear, float zFar)
{
    float right = zNear * tanf(fov / 2.0f);
    float top = right / aspectRatio;

    mat4_t result;

    result.m[0][0] = zNear / right;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = zNear / top;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = -(zFar + zNear) / (zFar - zNear);
    result.m[2][3] = (-2 * zFar * zNear) / (zFar - zNear);

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = -1;
    result.m[3][3] = 0;

    return result;
}

mat4_t mat4_createLookAt(v3_t pos, v3_t target, v3_t worldUp)
{
    v3_t direction = v3_normalize(v3_sub(pos, target));
    v3_t right = v3_normalize(v3_cross(worldUp, direction));
    v3_t up = v3_cross(direction, right);

    mat4_t result;

    result.m[0][0] = right.x;
    result.m[0][1] = right.y;
    result.m[0][2] = right.z;
    result.m[0][3] = right.x * -pos.x + right.y * -pos.y + right.z * -pos.z;

    result.m[1][0] = up.x;
    result.m[1][1] = up.y;
    result.m[1][2] = up.z;
    result.m[1][3] = up.x * -pos.x + up.y * -pos.y + up.z * -pos.z;

    result.m[2][0] = direction.x;
    result.m[2][1] = direction.y;
    result.m[2][2] = direction.z;
    result.m[2][3] = direction.x * -pos.x + direction.y * -pos.y + direction.z * -pos.z;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}