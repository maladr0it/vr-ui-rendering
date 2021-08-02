#ifndef CAMERA_H
#define CAMERA_H

#include "mat4.h"
#include "v3.h"

enum camera_moveDirection
{
    CAMERA_FORWARD  = 0b00000001,
    CAMERA_BACKWARD = 0b00000010,
    CAMERA_LEFT     = 0b00000100,
    CAMERA_RIGHT    = 0b00001000,
};

typedef struct camera
{
    v3_t pos;
    float yaw;
    float pitch;
} camera_t;

camera_t camera_create(v3_t pos, float yaw, float pitch);

v3_t camera_getFront(camera_t camera);

mat4_t camera_getViewTransform(camera_t camera);

void camera_move(camera_t *camera, unsigned char dirs, float dt);

void camera_turn(camera_t *camera, float dYaw, float dPitch);

#endif