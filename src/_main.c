#include "raylib.h"
#include "rcamera.h"
#include "shaders.h"
#include "skybox.h"
#include <stdio.h>
#include "terrain.h"

#include "raymath.h"
#define CAMERA_SMOOTH_SPEED 0.05f
#define HEIGHT_THRESHOLD 0.01f

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
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Enable Multi Sampling Anti Aliasing 4x (if available)

    InitWindow(screenWidth, screenHeight, "Bloomia sim");

    initSkybox();
    bool wasWindowMinimized = false;


    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = {0};
    camera.position = (Vector3){0.0f, 7.0f, 4.0f}; // Camera position
    camera.target = (Vector3){0.0f, 2.0f, 0.0f}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
    camera.fovy = 60.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type
    float lastCameraRotationYaw = 0.0f;
    float lastCameraRotationPitch = 0.0f;
    float moveCameraPosTo = 0.0f;
    float currentTerrainHeightAtCameraPos = 0.0f;

    SetExitKey(KEY_NULL);
    SetTargetFPS(fps); // Set our game to run at 60 frames-per-second

    Shader shader;
    initShaders(&shader);

    // Create a plane mesh and model
    Mesh terrainMesh = GenTerrainMesh();
    Model terrainModel = LoadModelFromMesh(terrainMesh);

    terrainModel.materials[0].shader = shader;
    // Set the material color
    terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){133, 216, 217, 255}; // Red color
    // Or you could try setting the base color directly


    //--------------------------------------------------------------------------------------
    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------

        updateShaders(&camera, &shader);

        // Camera system ----------------
        // We are rotating camera based on mouse sensitivity
        Vector2 mouseDelta = GetMouseDelta();
        if (wasWindowMinimized) mouseDelta = (Vector2){0.0f, 0.0f};

        lastCameraRotationYaw = mouseDelta.x * mouseSensitivity;
        lastCameraRotationPitch = mouseDelta.y * mouseSensitivity;

        UpdateCameraPro(&camera,
                        (Vector3){
                            (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) * 0.1f - // Move forward-backward
                            (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) * 0.1f,
                            (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) * 0.1f - // Move right-left
                            (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) * 0.1f,
                            moveCameraPosTo // Move up-down
                        },
                        (Vector3){
                            lastCameraRotationYaw, // Rotation: yaw
                            lastCameraRotationPitch, // Rotation: pitch
                            0.0f // Rotation: roll
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

        DrawModel(terrainModel, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, (Color){115, 196, 197, 255}); // Draw ground

        EndMode3D();

        DrawText(TextFormat("camera pos: %f %f %f", camera.position.x, camera.position.y, camera.position.z), 10, 10,
                 20, BLACK);
        DrawText(TextFormat("getTerrainHeight %f", getTerrainHeightAt(camera.position.x, camera.position.z)), 10, 70,
                 20, BLACK);
        DrawText(TextFormat("prevTerrainHeight %f", currentTerrainHeightAtCameraPos), 10, 100, 20, BLACK);
        DrawText(TextFormat("movecameraposto %f", moveCameraPosTo), 10, 50, 20, BLACK);

        EndDrawing();

        //--------------------------------------------------------------------------------------
        // End operations
        //--------------------------------------------------------------------------------------

        if (IsWindowFocused()) wasWindowMinimized = false;
        if (IsWindowMinimized()) wasWindowMinimized = true;


        // Replace the height adjustment code at the bottom of the main loop with:
        float terrainX = (camera.position.x / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0f;
        float terrainZ = (camera.position.z / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0f;

        float newCurrentTerrainHeightAtCameraPos = getTerrainHeightAt(terrainX, terrainZ) + 2;
        float heightDifference = newCurrentTerrainHeightAtCameraPos - camera.position.y;

        if (fabs(heightDifference) > HEIGHT_THRESHOLD)
        {
            moveCameraPosTo = heightDifference * CAMERA_SMOOTH_SPEED;
        }
        else
        {
            moveCameraPosTo = 0.0f;
        }

        currentTerrainHeightAtCameraPos = newCurrentTerrainHeightAtCameraPos;
    }

    //--------------------------------------------------------------------------------------
    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadShader(shader);
    deInitSkybox();
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
