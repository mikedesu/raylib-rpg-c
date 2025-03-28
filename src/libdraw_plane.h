#pragma once
#include "raylib.h"
#include "rlgl.h"

static inline void DrawTexturedPlane(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
    if (!tex || tex->id == 0) return;

    float x = pos.x, y = pos.y, z = pos.z;
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;
    float d = size.z * 0.5f;

    float u0 = src.x / tex->width;
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    // Flip vertically: just swap v0 <-> v1
    float vt0 = v1;
    float vt1 = v0;

    rlSetTexture(tex->id);
    rlBegin(RL_QUADS);

    // Top face with flipped UVs
    rlTexCoord2f(u0, vt0);
    rlVertex3f(x - w, y + h, z + d);
    rlTexCoord2f(u1, vt0);
    rlVertex3f(x + w, y + h, z + d);
    rlTexCoord2f(u1, vt1);
    rlVertex3f(x + w, y + h, z - d);
    rlTexCoord2f(u0, vt1);
    rlVertex3f(x - w, y + h, z - d);

    rlEnd();
    rlSetTexture(0);
}
