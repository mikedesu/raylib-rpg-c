#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

// Draw a billboard quad using a source rectangle from a texture (spritesheet-style)
static inline void
DrawBillboardFromRect(Camera camera, Texture2D* tex, Vector3 pos, Vector2 size, Rectangle src, Color tint) {
    if (!tex || tex->id == 0) return;

    // Direction from object to camera
    Vector2 dir = Vector2Normalize((Vector2){camera.position.x - pos.x, camera.position.z - pos.z});

    // Build right vector
    Vector3 right = {dir.y, 0.0f, -dir.x}; // perpendicular in XZ plane

    float w = size.x * 0.5f;
    float h = size.y * 0.5f;

    // Texture UVs
    float u0 = src.x / tex->width;
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    // Corners
    Vector3 topLeft = Vector3Add(pos, Vector3Add(Vector3Scale(right, -w), (Vector3){0, h, 0}));
    Vector3 topRight = Vector3Add(pos, Vector3Add(Vector3Scale(right, w), (Vector3){0, h, 0}));
    Vector3 bottomRight = Vector3Add(pos, Vector3Add(Vector3Scale(right, w), (Vector3){0, -h, 0}));
    Vector3 bottomLeft = Vector3Add(pos, Vector3Add(Vector3Scale(right, -w), (Vector3){0, -h, 0}));

    rlSetTexture(tex->id);
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    rlTexCoord2f(u0, v0);
    rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
    rlTexCoord2f(u1, v0);
    rlVertex3f(topRight.x, topRight.y, topRight.z);
    rlTexCoord2f(u1, v1);
    rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);
    rlTexCoord2f(u0, v1);
    rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

    rlEnd();
    rlSetTexture(0);
    rlColor4ub(255, 255, 255, 255);
}

/*
static inline void
DrawBillboardFromRect(Camera camera, Texture2D* tex, Vector3 pos, Vector2 size, Rectangle src, Color tint) {
    if (!tex || tex->id == 0) return;

    // Camera-facing direction
    Vector3 up = {0.0f, 1.0f, 0.0f};

    Vector3 toCam = Vector3Normalize(Vector3Subtract(camera.position, pos));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(up, toCam));
    //Vector3 forward = Vector3CrossProduct(right, up);

    // Half extents
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;

    // Texture coords (normalized)
    float u0 = src.x / tex->width;
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    // Four corners of the quad in 3D space
    Vector3 topLeft = Vector3Add(pos, Vector3Add(Vector3Scale(right, -w), Vector3Scale(up, h)));
    Vector3 topRight = Vector3Add(pos, Vector3Add(Vector3Scale(right, w), Vector3Scale(up, h)));
    Vector3 bottomRight = Vector3Add(pos, Vector3Add(Vector3Scale(right, w), Vector3Scale(up, -h)));
    Vector3 bottomLeft = Vector3Add(pos, Vector3Add(Vector3Scale(right, -w), Vector3Scale(up, -h)));

    rlSetTexture(tex->id);
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    rlTexCoord2f(u0, v0);
    rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
    rlTexCoord2f(u1, v0);
    rlVertex3f(topRight.x, topRight.y, topRight.z);
    rlTexCoord2f(u1, v1);
    rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);
    rlTexCoord2f(u0, v1);
    rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

    rlEnd();
    rlSetTexture(0);
    rlColor4ub(255, 255, 255, 255); // reset color state
}
*/
