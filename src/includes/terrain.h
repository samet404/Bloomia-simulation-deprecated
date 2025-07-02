//
// Created by notfound404 on 7/1/25.
//

#ifndef TERRAIN_H
#define TERRAIN_H
#include "raylib.h"
#define TERRAIN_SIZE 100
#define TERRAIN_SCALE 1.0f

#endif //TERRAIN_H

float getTerrainHeightAt(float x, float z);
Mesh GenTerrainMesh(void);