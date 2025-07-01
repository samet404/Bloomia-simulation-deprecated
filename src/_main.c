#include "raylib.h"
#include "rcamera.h"
#include "shaders.h"
#include "skybox.h"
#include <stdio.h>
#include "terrain.h"

#include "raymath.h"

#define MAX_COLUMNS 20

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

int main(void)
{
    //--------------------------------------------------------------------------------------
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const float mouseSensitivity = 0.05f;
    const int fps = 60;



    // We are making window resizeable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)

    InitWindow(screenWidth, screenHeight, "Bloomia sim");

    initSkybox();
    bool wasWindowMinimized = false;


    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    float lastCameraRotationYaw = 0.0f;
    float lastCameraRotationPitch = 0.0f;

    SetExitKey(KEY_NULL);
    SetTargetFPS(fps);                   // Set our game to run at 60 frames-per-second

    Shader shader;
    initShaders(&shader);

    // TODO: Clear these models here
    Model modelA;
    Model modelB;
    Model modelC;
    Texture texture;

    // Load models and texture
    modelA = LoadModelFromMesh(GenMeshTorus(0.4f, 1.0f, 16, 32));
    modelB = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
    modelC = LoadModelFromMesh(GenMeshSphere(0.5f, 32, 32));
    texture = LoadTexture("../resources/img/texel_checker.png");

    // Assign texture to default model material
    modelA.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    modelB.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    modelC.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    // NOTE: All models share the same shader
    modelA.materials[0].shader = shader;
    modelB.materials[0].shader = shader;
    modelC.materials[0].shader = shader;

    // Create a plane mesh and model
    Mesh terrainMesh = GenTerrainMesh();
    Model terrainModel = LoadModelFromMesh(terrainMesh);
    terrainModel.materials[0].shader = shader;
    terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;


    //--------------------------------------------------------------------------------------
    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------

        updateShaders(&camera, &shader);

        // Camera system ----------------
        // We are rotating camera based on mouse sensitivity
        Vector2 mouseDelta = GetMouseDelta();
        if (wasWindowMinimized) mouseDelta = (Vector2){0.0f, 0.0f};

        lastCameraRotationYaw = mouseDelta.x*mouseSensitivity;
        lastCameraRotationPitch = mouseDelta.y*mouseSensitivity;

        UpdateCameraPro(&camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.1f -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
                0.0f                                                // Move up-down
            },
            (Vector3){
                lastCameraRotationYaw,         // Rotation: yaw
                lastCameraRotationPitch,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            }, 0); // Move to target (zoom)


        // We are disabling cursor to prevent it from leaving the window
        DisableCursor();

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        drawSkybox();

        // Draw the three models TODO: clear these models later
        DrawModel(modelA, (Vector3){ 10.6f, 40, 0 }, 1.0f, WHITE);
        DrawModel(modelB, (Vector3){ -2.6f, 40, 0 }, 1.0f, WHITE);
        DrawModel(modelC, (Vector3){ 2.6f, 40, 0 }, 1.0f, WHITE);

        for (int i = -20; i < 20; i += 2) DrawModel(modelA,(Vector3){ (float)i, 0, 2 }, 1.0f, WHITE);

        DrawModel(terrainModel, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, (Color){115, 196, 197, 255}); // Draw ground

        EndMode3D();
        EndDrawing();

        if (IsWindowFocused()) wasWindowMinimized = false;
        if (IsWindowMinimized()) wasWindowMinimized = true;
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadModel(modelA);        // Unload model A
    UnloadModel(modelB);        // Unload model B
    UnloadModel(modelC);        // Unload model C
    UnloadTexture(texture);     // Unload texture
    UnloadShader(shader);

    deInitSkybox();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}