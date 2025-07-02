//
// Created by notfound404 on 7/1/25.
//

// ========================================================
// This file is responsible for managing shaders in the game.
// ========================================================

#include "raylib.h"
#include "raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

float fogDensity = 0.05f;
int fogDensityLoc = 0;
int ambientLoc = 0;

void initShaders(Shader *shader)
{
    // Load shader and set up some uniforms
    *shader = LoadShader(TextFormat("../resources/shaders/glsl%i/lighting.vs", GLSL_VERSION),
                               TextFormat("../resources/shaders/glsl%i/fog.fs", GLSL_VERSION));
    shader->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(*shader, "matModel");
    shader->locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(*shader, "viewPos");

    // Ambient light level
    ambientLoc = GetShaderLocation(*shader, "ambient");
    SetShaderValue(*shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    fogDensityLoc = GetShaderLocation(*shader, "fogDensity");
    SetShaderValue(*shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // Using just 1 point lights
    CreateLight(LIGHT_POINT, (Vector3){ 0, 2, 6 }, Vector3Zero(), (Color){246, 209, 157, 100}, *shader);
}

void updateShaders(const Camera3D *camera, const Shader *shader)
{
    SetShaderValue(*shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(*shader, shader->locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);
}
