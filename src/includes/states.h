
#ifndef GAME_MAIN_STATE_H
#define GAME_MAIN_STATE_H

#include "raylib.h"
#include <stdbool.h>

//----------------------------------------------------------------------------------
// Window state
//----------------------------------------------------------------------------------
typedef struct
{
    int screenWidth;
    int screenHeight;
} WindowState;

//----------------------------------------------------------------------------------
// Enum that represents the current screen
//----------------------------------------------------------------------------------
typedef enum
{
    MAIN_MENU = 0,
    GAME
} Screen;

#endif