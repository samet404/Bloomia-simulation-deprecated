#include "raylib.h"
#include "rcamera.h"
#include "skybox.h"
#include <stdio.h>

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


    InitWindow(screenWidth, screenHeight, "Bloomia sim");
    initSkybox();

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    float lastCameraRotationYaw = 0.0f;
    float lastCameraRotationPitch = 0.0f;


    bool wasWindowMinimized = false;

    // We are making window resizeable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    SetExitKey(KEY_NULL);
    SetTargetFPS(fps);                   // Set our game to run at 60 frames-per-second

    //--------------------------------------------------------------------------------------
    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------

        if (IsWindowMinimized()) wasWindowMinimized = true;

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
        DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, (Color){115, 196, 197, 255}); // Draw ground

        EndMode3D();
        EndDrawing();

        if (IsWindowFocused()) wasWindowMinimized = false;
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    deInitSkybox();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}