#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <glad/glad.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "utils.h"
#include "v3.h"
#include "mat4.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "font.h"

#include "../tmp/softmask_data.h"
#include "../tmp/sdf_data.h"
#include "../tmp/msdf_data.h"

static void handleWindowResize(GLFWwindow *window, int width, int height);
static void handleInput(GLFWwindow *window);
static void handleMouseMove(GLFWwindow *window, double xPos, double yPos);

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const float FOV = M_PI_2;
static const float Z_NEAR = 0.1f;
static const float Z_FAR = 100.0f;
static const float MOUSE_SENSITIVITY = 0.002f;

// global state
static double lastMouseX = (double)WINDOW_WIDTH / 2.0f;
static double lastMouseY = (double)WINDOW_HEIGHT / 2.0f;
static float dt;
static camera_t playerCamera;

int main(int argc, char **argv)
{
    // use path buffers to store paths modified to be absolute
    char pathBuffer1[1024];
    char pathBuffer2[1024];
    //
    // Create window
    //
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "font-rendering", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, handleWindowResize);
    glfwSetCursorPosCallback(window, handleMouseMove);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //
    // textures
    //
    texture_t softmaskAtlas = texture_create(utils_getFullPath("textures/softmask-atlas.png", pathBuffer1));
    texture_t sdfAtlas = texture_create(utils_getFullPath("textures/sdf-atlas.png", pathBuffer1));
    texture_t msdfAtlas = texture_create(utils_getFullPath("textures/msdf-atlas.png", pathBuffer1));

    //
    // create geometry
    //
    font_glyphVert_t *verts = utils_malloc(sizeof(*verts) * 6); // 6 verts per char
    font_createGlyphVerts(SOFTMASK_DATA, 'X', verts);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*verts) * 6, verts, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(font_glyphVert_t),
                          (void *)offsetof(font_glyphVert_t, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(font_glyphVert_t),
                          (void *)offsetof(font_glyphVert_t, texCoords));
    glEnableVertexAttribArray(1);

    //
    // Initialize loop variables
    //
    float lastFrame = 0.0f;
    shader_t rasterShader = shader_create(
        utils_getFullPath("shaders/vertex.vs", pathBuffer1),
        utils_getFullPath("shaders/fragment.fs", pathBuffer2));
    shader_t sdfShader = shader_create(
        utils_getFullPath("shaders/vertex.vs", pathBuffer1),
        utils_getFullPath("shaders/sdf.fs", pathBuffer2));

    playerCamera = camera_create(v3_create(0.0f, 0.0f, 0.0f), -M_PI_2, 0.0f);

    //
    // Main loop
    //
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        handleInput(window);

        mat4_t viewMat = camera_getViewTransform(playerCamera);
        mat4_t projectionMat = mat4_createProj((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, FOV, Z_NEAR, Z_FAR);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //
        // Render rasterized glyph
        //
        glBindTexture(GL_TEXTURE_2D, softmaskAtlas.id);
        // temp - figure out a way to update buffer data quickly
        font_createGlyphVerts(SOFTMASK_DATA, 'W', verts);
        glBufferData(GL_ARRAY_BUFFER, sizeof(*verts) * 6, verts, GL_STREAM_DRAW);
        //
        mat4_t rasterModelMat = mat4_createTranslate(v3_create(-2.0f, 0.0f, -2.0f));
        shader_use(rasterShader);
        shader_setMat4(rasterShader, "model", rasterModelMat);
        shader_setMat4(rasterShader, "view", viewMat);
        shader_setMat4(rasterShader, "projection", projectionMat);
        shader_setInt(rasterShader, "glyphAtlas", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //
        // Render sdf glyph
        //
        glBindTexture(GL_TEXTURE_2D, sdfAtlas.id);
        // temp
        font_createGlyphVerts(SDF_DATA, 'W', verts);
        glBufferData(GL_ARRAY_BUFFER, sizeof(*verts) * 6, verts, GL_STREAM_DRAW);
        //
        mat4_t sdfModelMat = mat4_createTranslate(v3_create(0.0f, 0.0f, -2.0f));
        shader_use(sdfShader);
        shader_setMat4(sdfShader, "model", sdfModelMat);
        shader_setMat4(sdfShader, "view", viewMat);
        shader_setMat4(sdfShader, "projection", projectionMat);
        shader_setInt(sdfShader, "glyphAtlas", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

static void handleWindowResize(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void handleInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    unsigned char moveDir = 0b00000000;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveDir |= CAMERA_FORWARD;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveDir |= CAMERA_BACKWARD;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveDir |= CAMERA_LEFT;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveDir |= CAMERA_RIGHT;
    }
    camera_move(&playerCamera, moveDir, dt);
}

static void handleMouseMove(GLFWwindow *window, double xPos, double yPos)
{
    double dx = xPos - lastMouseX;
    double dy = -(yPos - lastMouseY); // down is +ve for mouse coords
    lastMouseX = xPos;
    lastMouseY = yPos;
    camera_turn(&playerCamera, dx * MOUSE_SENSITIVITY, dy * MOUSE_SENSITIVITY);
}