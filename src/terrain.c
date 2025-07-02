//
// Created by notfound404 on 7/1/25.
//
#include "terrain.h"

#include <math.h>
#include "raylib.h"


// Function to generate height at any point
// Also can be used to replace objects on top of terrain like player's camera, models
float getTerrainHeightAt(float x, float z)
{
    float amplitude = 1.0f;
    float frequency = 0.1f;
    float height = 0.0f;
    float maxValue = 0.0f;
    
    // We'll use 4 octaves of noise
    for(int i = 0; i < 4; i++)
    {
        height += amplitude * (sinf(x * frequency) * cosf(z * frequency));
        maxValue += amplitude;
        amplitude *= 0.5f;    // Reduce amplitude for each octave
        frequency *= 2.0f;    // Double the frequency for each octave
    }
    
    // Normalize the height
    height /= maxValue;
    
    // Scale the final height to desired range
    return height * 1.4f;
}

Mesh GenTerrainMesh(void)
{
    Mesh mesh = { 0 };

    // Calculate total number of vertices and triangles needed
    // Vertices: grid of (TERRAIN_SIZE+1) x (TERRAIN_SIZE+1) points
    // Triangles: each grid cells needs 2 triangles (6 indices)
    int vertices = (TERRAIN_SIZE + 1) * (TERRAIN_SIZE + 1);
    int triangles = TERRAIN_SIZE * TERRAIN_SIZE * 6;

    // Allocate memory for mesh data:
    // - vertices: x, y, z coordinates (3 floats per vertex)
    // - texcoords: u, v coordinates (2 floats per vertex)
    // - normals: x, y, z components (3 floats per vertex)
    // - indices: 6 indices per grid cell (2 triangles)
    mesh.vertices = (float *)MemAlloc(vertices * 3 * sizeof(float));
    mesh.texcoords = (float *)MemAlloc(vertices * 2 * sizeof(float));
    mesh.normals = (float *)MemAlloc(vertices * 3 * sizeof(float));
    mesh.indices =  (unsigned short *)MemAlloc(triangles * sizeof(unsigned short));

    int vCounter = 0;
    int tCounter = 0;
    int nCounter = 0;
    int iCounter = 0;

    // Generate vertex data by iterating over the grid points
    for (int z = 0; z <= TERRAIN_SIZE; z++)
    {
        for (int x = 0; x <= TERRAIN_SIZE; x++)
        {
            // Calculate vertex position:
            // - Center the terrain around origin (subtract TERRAIN_SIZE/2)
            // - Scale the coordinates by TERRAIN_SCALE
            // - Y coordinate comes from height function
            mesh.vertices[vCounter] = (x - TERRAIN_SIZE / 2.0f) * TERRAIN_SCALE;
            mesh.vertices[vCounter + 1] = getTerrainHeightAt(x, z);
            mesh.vertices[vCounter + 2] = (z - TERRAIN_SIZE/2.0f) * TERRAIN_SCALE;
            vCounter += 3;

            // Calculate texture coordinates (0 to 1 range)
            mesh.texcoords[tCounter] = (float)x / TERRAIN_SIZE;
            mesh.texcoords[tCounter + 1] = (float)z/TERRAIN_SIZE;
            tCounter += 2;

            // Set normal vectors (simplified to point straight up)
            mesh.normals[nCounter] = 0.0f;
            mesh.normals[nCounter + 1] = 1.0f;
            mesh.normals[nCounter + 2] = 0.0f;
            nCounter += 3;
        }
    }

    // Generate indices for triangle faces
    for (int z = 0; z < TERRAIN_SIZE; z++)
    {
        for (int x = 0; x < TERRAIN_SIZE; x++)
        {
            // Calculate vertex indices for current grid cell
            int topLeft = z * (TERRAIN_SIZE + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (TERRAIN_SIZE + 1) + x;
            int bottomRight = bottomLeft + 1;

            // Create two triangles for the grid cell:
            // Triangle 1: topLeft -> bottomLeft -> topRight
            // Triangle 2: topRight -> bottomLeft -> bottomRight
            // you can think like its like that https://i0.wp.com/www.simplesimonandco.com/wp-content/uploads/2015/10/HST-Triangle.gif?ssl=1
            mesh.indices[iCounter] = topLeft;
            mesh.indices[iCounter + 1] = bottomLeft;
            mesh.indices[iCounter + 2] = topRight;

            mesh.indices[iCounter + 3] = topRight;
            mesh.indices[iCounter + 4] = bottomLeft;
            mesh.indices[iCounter + 5] = bottomRight;
            iCounter += 6;
        }
    }

    // Set final mesh properties
    mesh.vertexCount = vertices;
    mesh.triangleCount = triangles/3;

    UploadMesh(&mesh, false);

    return mesh;
}