#pragma once

#include "raylib.h"
#include "rlgl.h"

static inline void DrawTexturedCubeFront(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
    if (!tex || tex->id == 0) return;

    float x = pos.x, y = pos.y, z = pos.z;
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;
    float d = size.z * 0.5f;

    float u0 = src.x / tex->width;
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    rlEnableTexture(tex->id);
    rlBegin(RL_QUADS);

    // Front face
    rlTexCoord2f(u0, v0);
    rlVertex3f(x - w, y + h, z + d); // top-left
    rlTexCoord2f(u1, v0);
    rlVertex3f(x + w, y + h, z + d); // top-right
    rlTexCoord2f(u1, v1);
    rlVertex3f(x + w, y - h, z + d); // bottom-right
    rlTexCoord2f(u0, v1);
    rlVertex3f(x - w, y - h, z + d); // bottom-left

    rlEnd();
    rlDisableTexture();
}

static inline void DrawTexturedCubeTopOnly(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
    if (!tex || tex->id == 0) return;

    float x = pos.x, y = pos.y, z = pos.z;
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;
    float d = size.z * 0.5f;

    float u0 = src.x / tex->width;
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    //rlEnableTexture(tex->id);
    rlSetTexture(tex->id);
    rlBegin(RL_QUADS);

    // Top face using correct source region
    rlTexCoord2f(u0, v0);
    rlVertex3f(x - w, y + h, z + d); // top-left
    rlTexCoord2f(u1, v0);
    rlVertex3f(x + w, y + h, z + d); // top-right
    rlTexCoord2f(u1, v1);
    rlVertex3f(x + w, y + h, z - d); // bottom-right
    rlTexCoord2f(u0, v1);
    rlVertex3f(x - w, y + h, z - d); // bottom-left

    rlEnd();
    //rlDisableTexture();
    rlSetTexture(0);
}

//void DrawTexturedCubeFrontOnly(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
//}

static inline void DrawTexturedCubeBack(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
    if (!tex || tex->id == 0) return;

    float x = pos.x, y = pos.y, z = pos.z;
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;
    float d = size.z * 0.5f;

    float u0 = src.x / tex->width;
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    rlSetTexture(tex->id);
    rlBegin(RL_QUADS);

    // FRONT (-Z)
    //rlTexCoord2f(u0, v0);
    //rlVertex3f(x - w, y + h, z - d);
    //rlTexCoord2f(u1, v0);
    //rlVertex3f(x + w, y + h, z - d);
    //rlTexCoord2f(u1, v1);
    //rlVertex3f(x + w, y - h, z - d);
    //rlTexCoord2f(u0, v1);
    //rlVertex3f(x - w, y - h, z - d);

    // BACK (+Z)
    rlTexCoord2f(u0, v0);
    rlVertex3f(x + w, y + h, z + d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x - w, y + h, z + d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x - w, y - h, z + d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x + w, y - h, z + d);

    // TOP (+Y)
    //rlTexCoord2f(u0, v0);
    //rlVertex3f(x - w, y + h, z + d);
    //rlTexCoord2f(u1, v0);
    //rlVertex3f(x + w, y + h, z + d);
    //rlTexCoord2f(u1, v1);
    //rlVertex3f(x + w, y + h, z - d);
    //rlTexCoord2f(u0, v1);
    //rlVertex3f(x - w, y + h, z - d);

    // BOTTOM (-Y)
    //rlTexCoord2f(u0, v0);
    //rlVertex3f(x - w, y - h, z - d);
    //rlTexCoord2f(u1, v0);
    //rlVertex3f(x + w, y - h, z - d);
    //rlTexCoord2f(u1, v1);
    //rlVertex3f(x + w, y - h, z + d);
    //rlTexCoord2f(u0, v1);
    //rlVertex3f(x - w, y - h, z + d);

    // LEFT (-X)
    //rlTexCoord2f(u0, v0);
    //rlVertex3f(x - w, y + h, z + d);
    //rlTexCoord2f(u1, v0);
    //rlVertex3f(x - w, y + h, z - d);
    //rlTexCoord2f(u1, v1);
    //rlVertex3f(x - w, y - h, z - d);
    //rlTexCoord2f(u0, v1);
    //rlVertex3f(x - w, y - h, z + d);

    // RIGHT (+X)
    //rlTexCoord2f(u0, v0);
    //rlVertex3f(x + w, y + h, z - d);
    //rlTexCoord2f(u1, v0);
    //rlVertex3f(x + w, y + h, z + d);
    //rlTexCoord2f(u1, v1);
    //rlVertex3f(x + w, y - h, z + d);
    //rlTexCoord2f(u0, v1);
    //rlVertex3f(x + w, y - h, z - d);

    rlEnd();
    rlSetTexture(0);
}

static inline void DrawTexturedCubeTopOnlyInverted(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
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

static inline void DrawTexturedCubeTopOnlyPlayer(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
    if (!tex || tex->id == 0) return;

    float x = pos.x, y = pos.y, z = pos.z;
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;
    float d = size.z * 0.5f;

    // Assume sprite frames are laid out horizontally across the texture
    //float u0 = src.x / (tex->width);
    //float v0 = src.y / tex->height;
    //float u1 = (src.x + src.width) / tex->width;
    //float v1 = (src.y + src.height) / tex->height;

    float u0 = src.x / (tex->width);
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    //float u1 = (src.x + src.width);
    //float v1 = (src.y + src.height);

    rlSetTexture(tex->id);
    rlBegin(RL_QUADS);

    // Top face only
    rlTexCoord2f(u0, v0);
    rlVertex3f(x - w, y + h, z + d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x + w, y + h, z + d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x + w, y + h, z - d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x - w, y + h, z - d);

    rlEnd();
    rlSetTexture(0);
}

static inline void DrawTexturedCube(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
    if (!tex || tex->id == 0) return;

    float x = pos.x, y = pos.y, z = pos.z;
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;
    float d = size.z * 0.5f;

    float u0 = src.x / tex->width;
    float v0 = src.y / tex->height;
    float u1 = (src.x + src.width) / tex->width;
    float v1 = (src.y + src.height) / tex->height;

    rlSetTexture(tex->id);
    rlBegin(RL_QUADS);

    // FRONT (-Z)
    rlTexCoord2f(u0, v0);
    rlVertex3f(x - w, y + h, z - d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x + w, y + h, z - d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x + w, y - h, z - d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x - w, y - h, z - d);

    // BACK (+Z)
    rlTexCoord2f(u0, v0);
    rlVertex3f(x + w, y + h, z + d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x - w, y + h, z + d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x - w, y - h, z + d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x + w, y - h, z + d);

    // TOP (+Y)
    rlTexCoord2f(u0, v0);
    rlVertex3f(x - w, y + h, z + d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x + w, y + h, z + d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x + w, y + h, z - d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x - w, y + h, z - d);

    // BOTTOM (-Y)
    rlTexCoord2f(u0, v0);
    rlVertex3f(x - w, y - h, z - d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x + w, y - h, z - d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x + w, y - h, z + d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x - w, y - h, z + d);

    // LEFT (-X)
    rlTexCoord2f(u0, v0);
    rlVertex3f(x - w, y + h, z + d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x - w, y + h, z - d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x - w, y - h, z - d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x - w, y - h, z + d);

    // RIGHT (+X)
    rlTexCoord2f(u0, v0);
    rlVertex3f(x + w, y + h, z - d);
    rlTexCoord2f(u1, v0);
    rlVertex3f(x + w, y + h, z + d);
    rlTexCoord2f(u1, v1);
    rlVertex3f(x + w, y - h, z + d);
    rlTexCoord2f(u0, v1);
    rlVertex3f(x + w, y - h, z - d);

    rlEnd();
    rlSetTexture(0);
}
