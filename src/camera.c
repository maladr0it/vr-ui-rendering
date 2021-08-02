#include <math.h>
#include "camera.h"
#include "utils.h"

static const float MIN_YAW = -M_PI_2 + 0.01f;
static const float MAX_YAW = M_PI_2 - 0.01f;
static const float SPEED = 5.0f;
static const v3_t WORLD_UP = {.x = 0.0f, .y = 1.0f, .z = 0.0f};

camera_t camera_create(v3_t pos, float yaw, float pitch)
{
    camera_t result;
    result.pos = pos;
    result.yaw = yaw;
    result.pitch = pitch;
    return result;
}

// TODO: consider caching this
v3_t camera_getFront(camera_t camera)
{
    v3_t result;
    result.x = cosf(camera.yaw) * cosf(camera.pitch);
    result.y = sinf(camera.pitch);
    result.z = sinf(camera.yaw) * cosf(camera.pitch);
    return v3_normalize(result);
}

mat4_t camera_getViewTransform(camera_t camera)
{
    return mat4_createLookAt(camera.pos, v3_add(camera.pos, camera_getFront(camera)), WORLD_UP);
}

void camera_move(camera_t *camera, unsigned char dirs, float dt)
{
    v3_t front = camera_getFront(*camera);
    // remove y component from movement (FPS controls)
    v3_t forward = v3_normalize(v3_create(front.x, 0.0f, front.z));
    v3_t right = v3_cross(forward, WORLD_UP);

    if (dirs & CAMERA_FORWARD)
    {
        camera->pos = v3_add(camera->pos, v3_mul(forward, dt * SPEED));
    }
    if (dirs & CAMERA_BACKWARD)
    {
        camera->pos = v3_sub(camera->pos, v3_mul(forward, dt * SPEED));
    }
    if (dirs & CAMERA_LEFT)
    {
        camera->pos = v3_sub(camera->pos, v3_mul(right, dt * SPEED));
    }
    if (dirs & CAMERA_RIGHT)
    {
        camera->pos = v3_add(camera->pos, v3_mul(right, dt * SPEED));
    }
}

void camera_turn(camera_t *camera, float dYaw, float dPitch)
{
    camera->yaw += dYaw;
    camera->pitch = utils_clampf(camera->pitch + dPitch, MIN_YAW, MAX_YAW);
}