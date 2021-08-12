#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <glad/glad.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "utils.h"
#include "v2.h"
#include "v3.h"
#include "mat4.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "font.h"

#include "../tmp/softmask_data.h"
#include "../tmp/sdf_data.h"
#include "../tmp/msdf_data.h"

static void createQuadVerts(float x, float y, float w, float h, float density, texture_t texture, mesh_vert_t *verts);

static void handleWindowResize(GLFWwindow *window, int width, int height);
static void handleKey(GLFWwindow *window, int key, int scancode, int action, int mods);
static void handleChar(GLFWwindow *window, unsigned int codepoint);
static void handleInput(GLFWwindow *window);
static void handleMouseMove(GLFWwindow *window, double xPos, double yPos);

static const int WINDOW_WIDTH = 1350;
static const int WINDOW_HEIGHT = 900;
static const float FOV = M_PI_2;
static const float Z_NEAR = 0.1f;
static const float Z_FAR = 100.0f;
static const float MOUSE_SENSITIVITY = 0.002f;

// global state
static bool editMode = false;
static bool directionalLightEnabled = false;
static bool pointLightsEnabled = false;
static char buttonText[64];
static float cornerRadius = 0.0f;
static float strokeWidth = 0.1f;

static double lastMouseX = (double)WINDOW_WIDTH / 2.0f;
static double lastMouseY = (double)WINDOW_HEIGHT / 2.0f;
static float dt;
static camera_t playerCamera;

int main(int argc, char **argv)
{
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
    glfwSetKeyCallback(window, handleKey);
    glfwSetCharCallback(window, handleChar);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //
    // Create meshes
    //
    texture_t scratchesTex = texture_create("textures/scratches.jpg", DIFFUSE);
    texture_t scratchesSpecularTex = texture_create("textures/scratches.jpg", SPECULAR);
    texture_t msdfAtlas = texture_create("textures/msdf-atlas.png", DIFFUSE);

    mesh_vert_t quadVerts[6];
    texture_t buttonTextures[2] = {scratchesTex, scratchesSpecularTex};
    mesh_t buttonMesh = mesh_create(quadVerts, 6, buttonTextures, 2);

    mesh_vert_t cubeVerts[64];
    int numCubeVerts = mesh_loadVerts("meshes/cube.obj", cubeVerts);
    mesh_t cubeMesh = mesh_create(cubeVerts, numCubeVerts, NULL, 0);

    mesh_vert_t *textVerts = utils_malloc(sizeof(*textVerts) * 1024);
    mesh_t textMesh = mesh_create(textVerts, 1024, &msdfAtlas, 1);

    shader_t buttonShader = shader_create("shaders/vertex.vs", "shaders/shape.fs");
    shader_t lightShader = shader_create("shaders/vertex.vs", "shaders/solid.fs");
    shader_t rasterTextShader = shader_create("shaders/vertex.vs", "shaders/fragment.fs");
    shader_t textShader = shader_create("shaders/vertex.vs", "shaders/msdf.fs");

    //
    // Initialize loop variables
    //
    float lastFrame = 0.0f;
    playerCamera = camera_create(v3_create(0.0f, 0.0f, 0.0f), -M_PI_2, 0.0f);

    //
    // Main loop
    //
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        v3_t directionalLightDir = v3_create(0.0f, -0.25f, -1.0f);
        v3_t directionalLightColor = directionalLightEnabled ? v3_create(0.8f, 0.8f, 1.0f) : v3_create(0.0f, 0.0f, 0.0f);
        v3_t pointLightColor = v3_create(1.0f, 1.0f, 1.0f);

        int numPointLights = pointLightsEnabled ? 4 : 0;

        v3_t pointLightPositions[] = {
            v3_create(2.0 * sinf(currentFrame), 2.0 * cosf(currentFrame), 0.0f),
            v3_create(2.0 * sinf(currentFrame + M_PI_2), 2.0 * cosf(currentFrame + M_PI_2), 0.0f),
            v3_create(2.0 * sinf(currentFrame + M_PI), 2.0 * cosf(currentFrame + M_PI), 0.0f),
            v3_create(2.0 * sinf(currentFrame + M_PI + M_PI_2), 2.0 * cosf(currentFrame + M_PI + M_PI_2), 0.0f),
        };

        // input
        if (!editMode)
        {
            handleInput(window);
        }

        mat4_t viewMat = camera_getViewTransform(playerCamera);
        mat4_t projectionMat = mat4_createProj((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, FOV, Z_NEAR, Z_FAR);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //
        // Render Point Lights
        //
        for (int i = 0; i < numPointLights; ++i)
        {
            mat4_t lightModelMat = mat4_createIdentity();
            lightModelMat = mat4_mul(lightModelMat, mat4_createTranslate(pointLightPositions[i]));
            lightModelMat = mat4_mul(lightModelMat, mat4_createScale(v3_create(0.01, 0.01, 0.01)));

            shader_use(lightShader);
            shader_setMat4(lightShader, "model", lightModelMat);
            shader_setMat4(lightShader, "view", viewMat);
            shader_setMat4(lightShader, "projection", projectionMat);
            shader_setV3(lightShader, "color", pointLightColor);

            mesh_render(cubeMesh, lightShader);
        }

        //
        // Render Button
        //
        float textWidth = font_getTextWidth(MSDF_DATA, buttonText);
        float buttonTexDensity = 256.0f;
        float buttonWidth = textWidth + 1.0f;
        float buttonHeight = 1.6f;
        v3_t buttonPos = v3_create(0.5f - buttonWidth / 2.0f, 0.5f - buttonHeight / 2.0f, -2.1f);

        mat4_t buttonModelMat = mat4_createTranslate(buttonPos);

        shader_use(buttonShader);
        shader_setMat4(buttonShader, "model", buttonModelMat);
        shader_setMat4(buttonShader, "view", viewMat);
        shader_setMat4(buttonShader, "projection", projectionMat);
        shader_setV3(buttonShader, "viewPos", playerCamera.pos);

        shader_setV3(buttonShader, "directionalLight.dir", directionalLightDir);
        shader_setV3(buttonShader, "directionalLight.ambient", v3_mul(directionalLightColor, 0.1f));
        shader_setV3(buttonShader, "directionalLight.diffuse", v3_mul(directionalLightColor, 0.6f));
        shader_setV3(buttonShader, "directionalLight.specular", v3_mul(directionalLightColor, 1.0f));

        shader_setInt(buttonShader, "numPointLights", numPointLights);
        char uniformName[64];
        for (int i = 0; i < numPointLights; ++i)
        {
            shader_use(buttonShader);
            shader_getUniformString("pointLights", i, "pos", uniformName);
            shader_setV3(buttonShader, uniformName, pointLightPositions[i]);
            shader_getUniformString("pointLights", i, "ambient", uniformName);
            shader_setV3(buttonShader, uniformName, v3_mul(pointLightColor, 0.01f));
            shader_getUniformString("pointLights", i, "diffuse", uniformName);
            shader_setV3(buttonShader, uniformName, v3_mul(pointLightColor, 0.1f));
            shader_getUniformString("pointLights", i, "specular", uniformName);
            shader_setV3(buttonShader, uniformName, v3_mul(pointLightColor, 0.4f));
            shader_getUniformString("pointLights", i, "constant", uniformName);
            shader_setFloat(buttonShader, uniformName, 1.0f);
            shader_getUniformString("pointLights", i, "linear", uniformName);
            shader_setFloat(buttonShader, uniformName, 0.09);
            shader_getUniformString("pointLights", i, "quadratic", uniformName);
            shader_setFloat(buttonShader, uniformName, 0.032);
        }

        shader_setFloat(buttonShader, "material.shininess", 32.0f);

        shader_setV2(buttonShader, "dimensions", v2_create(buttonWidth, buttonHeight));
        shader_setV2(buttonShader, "texDimensions", v2_create(scratchesTex.width, scratchesTex.height));
        shader_setFloat(buttonShader, "texDensity", buttonTexDensity);
        shader_setFloat(buttonShader, "cornerRadius", cornerRadius);
        createQuadVerts(-0.5f, -0.4f, buttonWidth, buttonHeight, buttonTexDensity, scratchesTex, quadVerts);
        mesh_setVerts(buttonMesh, quadVerts, 6);
        mesh_render(buttonMesh, buttonShader);

        //
        // Render MSDF Text
        //
        v3_t textPos = v3_create(-textWidth / 2.0f, -0.3f, -2.0f);
        mat4_t msdfModelMat = mat4_createTranslate(textPos);
        shader_use(textShader);
        shader_setMat4(textShader, "model", msdfModelMat);
        shader_setMat4(textShader, "view", viewMat);
        shader_setMat4(textShader, "projection", projectionMat);
        shader_setFloat(textShader, "strokeWidth", strokeWidth);
        // TODO: createGlyphVerts API doesn't gel well with setVerts
        textMesh.vertsLen = font_createGlyphVerts(MSDF_DATA, buttonText, textMesh.verts);
        mesh_setVerts(textMesh, textMesh.verts, textMesh.vertsLen);
        mesh_render(textMesh, textShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

static void createQuadVerts(float x, float y, float w, float h, float density, texture_t texture, mesh_vert_t *verts)
{
    float texRight = density * w / texture.width;
    float texTop = density * h / texture.height;

    mesh_vert_t bottomLeft = {
        .pos = v3_create(x, y, 0.0f),
        .normal = v3_create(0.0f, 0.0f, 1.0f),
        .texCoords = v2_create(0.0f, 0.0f),
    };

    mesh_vert_t bottomRight = {
        .pos = v3_create(x + w, y, 0.0f),
        .normal = v3_create(0.0f, 0.0f, 1.0f),
        .texCoords = v2_create(texRight, 0.0f),

    };

    mesh_vert_t topRight = {
        .pos = v3_create(x + w, y + h, 0.0f),
        .normal = v3_create(0.0f, 0.0f, 1.0f),
        .texCoords = v2_create(texRight, texTop),
    };

    mesh_vert_t topLeft = {
        .pos = v3_create(x, y + h, 0.0f),
        .normal = v3_create(0.0f, 0.0f, 1.0f),
        .texCoords = v2_create(0.0f, texTop),
    };

    verts[0] = bottomLeft;
    verts[1] = topRight;
    verts[2] = topLeft;
    verts[3] = bottomLeft;
    verts[4] = bottomRight;
    verts[5] = topRight;
}

static void handleWindowResize(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void handleKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, true);
            return;
        }
        if (key == GLFW_KEY_ENTER)
        {
            editMode = !editMode;
            return;
        }
        if (key == GLFW_KEY_BACKSPACE)
        {
            if (editMode)
            {
                buttonText[strlen(buttonText) - 1] = '\0';
            }
        }
        if (key == GLFW_KEY_UP)
        {
            cornerRadius += 0.1f;
            return;
        }
        if (key == GLFW_KEY_DOWN)
        {
            cornerRadius -= 0.1f;
            return;
        }
        if (key == GLFW_KEY_LEFT)
        {
            strokeWidth -= 0.1f;
        }
        if (key == GLFW_KEY_RIGHT)
        {
            strokeWidth += 0.1f;
        }
        if (key == GLFW_KEY_LEFT_BRACKET)
        {
            if (!editMode)
            {
                directionalLightEnabled = !directionalLightEnabled;
                return;
            }
        }
        if (key == GLFW_KEY_RIGHT_BRACKET)
        {
            if (!editMode)
            {
                pointLightsEnabled = !pointLightsEnabled;
                return;
            }
        }
    }
}

static void handleChar(GLFWwindow *window, unsigned int codepoint)
{
    if (editMode)
    {
        char c = (char)codepoint;
        strncat(buttonText, &c, 1);
    }
}

static void handleInput(GLFWwindow *window)
{
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