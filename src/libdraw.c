#include "libdraw.h"
#include "direction.h"
#include "dungeon_tile_type.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "get_txkey_for_tiletype.h"
#include "hashtable_entityid_spritegroup.h"
#include "mprint.h"
#include "race.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "textureinfo.h"
#include "tx_keys.h"
//#include "raylib.h"
#include "rlgl.h"

#define DEFAULT_SPRITEGROUPS_SIZE 128
//#define DEFAULT_WIN_WIDTH 800
//#define DEFAULT_WIN_HEIGHT 480
#define DEFAULT_WIN_WIDTH 1920
#define DEFAULT_WIN_HEIGHT 1080
#define SPRITEGROUP_DEFAULT_SIZE 32
#define DEFAULT_ANIM_SPEED 4

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

Shader shader_grayscale = {0};
Shader shader_tile_glow = {0};

RenderTexture2D target = {0};
//Rectangle target_src = {0, 0, 800, 480};
Rectangle target_src = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Vector2 target_origin = {0, 0};
Vector2 zero_vec = {0, 0};

int ANIM_SPEED = DEFAULT_ANIM_SPEED;

void DrawTexturedCubeFront(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
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

void DrawTexturedCubeTopOnly(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
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

void DrawTexturedCubeBack(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
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

void DrawTexturedCubeTopOnlyInverted(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
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

void DrawTexturedCubeTopOnlyPlayer(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
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

void DrawTexturedCube(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
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

//void DrawTexturedCube(Texture2D* tex, Vector3 pos, Vector3 size, Rectangle src) {
//    if (!tex || tex->id == 0) return;
//
//    float x = pos.x, y = pos.y, z = pos.z;
//    float w = size.x * 0.5f;
//    float h = size.y * 0.5f;
//    float d = size.z * 0.5f;
//
//    float u0 = src.x / tex->width;
//    float v0 = src.y / tex->height;
//    float u1 = (src.x + src.width) / tex->width;
//    float v1 = (src.y + src.height) / tex->height;
//
//    //rlDisableBackfaceCulling();
//    rlSetTexture(tex->id);
//    rlBegin(RL_QUADS);
//
//    // FRONT
//    rlTexCoord2f(u0, v0);
//    rlVertex3f(x - w, y + h, z - d);
//    rlTexCoord2f(u1, v0);
//    rlVertex3f(x + w, y + h, z - d);
//    rlTexCoord2f(u1, v1);
//    rlVertex3f(x + w, y - h, z - d);
//    rlTexCoord2f(u0, v1);
//    rlVertex3f(x - w, y - h, z - d);
//
//    // BACK
//    rlTexCoord2f(u0, v0);
//    rlVertex3f(x + w, y + h, z + d);
//    rlTexCoord2f(u1, v0);
//    rlVertex3f(x - w, y + h, z + d);
//    rlTexCoord2f(u1, v1);
//    rlVertex3f(x - w, y - h, z + d);
//    rlTexCoord2f(u0, v1);
//    rlVertex3f(x + w, y - h, z + d);
//
//    // TOP
//    rlTexCoord2f(u0, v0);
//    rlVertex3f(x - w, y + h, z + d);
//    rlTexCoord2f(u1, v0);
//    rlVertex3f(x + w, y + h, z + d);
//    rlTexCoord2f(u1, v1);
//    rlVertex3f(x + w, y + h, z - d);
//    rlTexCoord2f(u0, v1);
//    rlVertex3f(x - w, y + h, z - d);
//
//    // BOTTOM
//    rlTexCoord2f(u0, v0);
//    rlVertex3f(x - w, y - h, z - d);
//    rlTexCoord2f(u1, v0);
//    rlVertex3f(x + w, y - h, z - d);
//    rlTexCoord2f(u1, v1);
//    rlVertex3f(x + w, y - h, z + d);
//    rlTexCoord2f(u0, v1);
//    rlVertex3f(x - w, y - h, z + d);
//
//    // LEFT
//    rlTexCoord2f(u0, v0);
//    rlVertex3f(x - w, y + h, z + d);
//    rlTexCoord2f(u1, v0);
//    rlVertex3f(x - w, y + h, z - d);
//    rlTexCoord2f(u1, v1);
//    rlVertex3f(x - w, y - h, z - d);
//    rlTexCoord2f(u0, v1);
//    rlVertex3f(x - w, y - h, z + d);
//
//    // RIGHT
//    rlTexCoord2f(u0, v0);
//    rlVertex3f(x + w, y + h, z - d);
//    rlTexCoord2f(u1, v0);
//    rlVertex3f(x + w, y + h, z + d);
//    rlTexCoord2f(u1, v1);
//    rlVertex3f(x + w, y - h, z + d);
//    rlTexCoord2f(u0, v1);
//    rlVertex3f(x + w, y - h, z - d);
//
//    rlEnd();
//    rlSetTexture(0);
//    //rlEnableBackfaceCulling();
//}

static void draw_dungeon_tiles_3d(const gamestate* const g, const dungeon_floor_t* const floor) {
    const float TILE_WIDTH = 1.0f;
    const float TILE_HEIGHT = 1.0f;
    const float TILE_DEPTH = 1.0f;
    //DrawGrid(10, 1.0f);
    for (int y = 0; y < floor->height; y++) {
        for (int x = 0; x < floor->width; x++) {
            dungeon_tile_t* t = &floor->tiles[y][x];
            if (dungeon_tile_is_wall(t->type)) continue;
            const int txkey = get_txkey_for_tiletype(t->type);
            Texture2D* tex = &txinfo[txkey].texture;
            if (!tex) continue;
            // check if the texture is loaded
            if (tex->id == 0) {
                merrorint("draw_dungeon_tiles_3d: texture not loaded", txkey);
                continue;
            }
            //const Vector3 pos = {x * TILE_SIZE, TILE_HEIGHT / 2.0f, y * TILE_SIZE};
            const int xpos = x * TILE_WIDTH;
            const int ypos = 0;
            const int zpos = y * TILE_DEPTH;

            const Vector3 pos = {xpos, ypos, zpos};
            const Vector3 size = {TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH};
            //DrawTexturedCube(tex, pos, size);
            //DrawTexturedCube(tex, pos, size, (Rectangle){12, 12, 8, 8});
            //DrawTexturedCube(tex, pos, size, (Rectangle){12, 12, 8, 8});
            DrawTexturedCubeTopOnly(tex, pos, size, (Rectangle){12, 12, 8, 8});
            //DrawTexturedCubeTopOnlyInverted(tex, pos, size, (Rectangle){12, 12, 8, 8});
        }
    }
}

static void draw_entities_3d(const gamestate* g, const dungeon_floor_t* floor, bool dead_only) {
    const float TILE_SIZE = 1.0f;
    for (int y = 0; y < floor->height; y++) {
        for (int x = 0; x < floor->width; x++) {
            dungeon_tile_t* t = &floor->tiles[y][x];
            if (dungeon_tile_is_wall(t->type)) continue;
            for (int i = 0; i < dungeon_tile_entity_count(t); i++) {
                entityid id = dungeon_tile_get_entity(t, i);
                entity_t* e = em_get(g->entitymap, id);
                if (!e || e->is_dead != dead_only) continue;
                //Color color = (e->type == ENTITY_PLAYER) ? Fade((Color){0, 0, 255}, 0.8f) : RED;
                Color color = (e->type == ENTITY_PLAYER) ? Fade(BLUE, 0.5f) : RED;
                if (e->type == ENTITY_PLAYER) {
                    //DrawCubeWires(
                    //    (Vector3){e->x * TILE_SIZE, 1, e->y * TILE_SIZE}, TILE_SIZE, TILE_SIZE, TILE_SIZE, WHITE);
                    // get the player's texture
                    // it can be gotten off the player's spritegroup
                    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
                    if (!sg) {
                        merrorint("draw_entities_3d: player spritegroup NULL", id);
                        continue;
                    }
                    Texture2D* tex = sg->sprites[sg->current]->texture;
                    if (!tex) {
                        merrorint("draw_entities_3d: player texture NULL", sg->tx_keys[sg->current]);
                        continue;
                    }
                    const int xpos = e->x * TILE_SIZE;
                    const int ypos = 1;
                    const int zpos = e->y * TILE_SIZE;
                    const Vector3 pos = {xpos, ypos, zpos};
                    const Vector3 size = {TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    Rectangle src = sg->sprites[sg->current]->src;
                    src.width = 8;
                    src.height = 8;
                    src.x = 12;
                    src.y = 12;
                    DrawTexturedCubeBack(tex, pos, size, (Rectangle){12, 12, 8, 8});
                    //DrawCubeWires(
                    //    (Vector3){e->x * TILE_SIZE, 1, e->y * TILE_SIZE}, TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
                } else {
                    //DrawCube((Vector3){e->x * TILE_SIZE, 1, e->y * TILE_SIZE}, TILE_SIZE, TILE_SIZE, TILE_SIZE, color);

                    // get the entity's spritegroup

                    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
                    if (!sg) {
                        merrorint("draw_entities_3d: spritegroup NULL", id);
                        continue;
                    }

                    // get the entity's current sprite's texture

                    Texture2D* tex = sg->sprites[sg->current]->texture;
                    if (!tex) {
                        merrorint("draw_entities_3d: texture NULL", sg->tx_keys[sg->current]);
                        continue;
                    }

                    // get the entity's current sprite's source rectangle
                    const int xpos = e->x * TILE_SIZE;
                    const int ypos = 1;
                    const int zpos = e->y * TILE_SIZE;
                    const Vector3 pos = {xpos, ypos, zpos};
                    const Vector3 size = {TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    Rectangle src = sg->sprites[sg->current]->src;
                    src.width = 8;
                    src.height = 8;
                    src.x = 12;
                    src.y = 12;
                    DrawTexturedCubeBack(tex, pos, size, (Rectangle){12, 12, 8, 8});
                }
            }
        }
    }
}

static void draw_wall_tiles_3d(const gamestate* g, const dungeon_floor_t* floor) {
    const float TILE_SIZE = 1.0f;
    const float TILE_HEIGHT = 2.0f;
    for (int y = 0; y < floor->height; y++) {
        for (int x = 0; x < floor->width; x++) {
            dungeon_tile_t* t = &floor->tiles[y][x];
            if (!dungeon_tile_is_wall(t->type)) continue;
            DrawCube((Vector3){x * TILE_SIZE, 0.5f, y * TILE_SIZE}, TILE_SIZE, TILE_HEIGHT, TILE_SIZE, DARKBROWN);
        }
    }
}

static bool draw_dungeon_tiles_2d(const gamestate* g, dungeon_floor_t* df) {
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            if (df_tile_is_wall(df, x, y)) continue;
            if (!libdraw_draw_dungeon_floor_tile(g, df, x, y)) { merrorint2("draw_dungeon_tiles_2d failed", x, y); }
        }
    }
    return true;
}

static bool draw_entities_2d(const gamestate* g, dungeon_floor_t* df, bool dead) {
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
            if (!tile || dungeon_tile_is_wall(tile->type)) continue;
            for (int i = 0; i < dungeon_tile_entity_count(tile); i++) {
                entityid id = dungeon_tile_get_entity(tile, i);
                entity* e = em_get(g->entitymap, id);
                if (e && e->is_dead == dead) libdraw_draw_sprite_and_shadow(g, id);
            }
        }
    }
    return true;
}

static bool draw_wall_tiles_2d(const gamestate* g, dungeon_floor_t* df) {
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
            if (!tile) {
                merrorint2("draw_wall_tiles_2d: tile NULL", x, y);
                return false;
            }
            if (!dungeon_tile_is_wall(tile->type)) continue;
            libdraw_draw_dungeon_floor_tile(g, df, x, y);
        }
    }
    return true;
}

void libdraw_update_input(inputstate* const is) { inputstate_update(is); }
bool libdraw_windowshouldclose() { return WindowShouldClose(); }

void libdraw_load_shaders() {
    //shader_grayscale = LoadShader(0, "grayscale.frag"); // No vertex shader needed
    //shader_tile_glow = LoadShader(0, "glow.frag");
    //shader_tile_glow = LoadShader(0, "psychedelic_ripple.frag");
}

void libdraw_unload_shaders() {
    UnloadShader(shader_grayscale);
    UnloadShader(shader_tile_glow);
}

bool libdraw_camera_lock_on(gamestate* const g) {
    if (!g || !g->cam_lockon) return false;
    spritegroup_t* grp = hashtable_entityid_spritegroup_get(spritegroups, g->hero_id);
    if (!grp) {
        merrorint("libdraw_camera_lock_on: hero spritegroup NULL", g->hero_id);
        return false;
    }
    g->cam2d.target = (Vector2){grp->dest.x, grp->dest.y};
    return true;
}

bool libdraw_check_default_animations(gamestate* const g) {
    if (!g) {
        merror("libdraw_check_default_animations: gamestate is NULL");
        return false;
    }
    for (int i = 0; i < g->index_entityids; i++) {
        const entityid id = g->entityids[i];
        spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
        if (sg && sg->current != sg->default_anim) { return false; }
    }
    return true;
}

void libdraw_update_sprite(gamestate* const g, entityid id) {
    if (!g) {
        merror("libdraw_update_sprite: gamestate is NULL");
        return;
    }
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merrorint("libdraw_update_sprite: entity not found", id);
        return;
    }
    spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    if (!sg) {
        merrorint("libdraw_update_sprite: spritegroup not found", id);
        return;
    }
    libdraw_update_sprite_ptr(g, e, sg);
    libdraw_handle_frame_incr(g, sg);
}

void libdraw_update_sprite_ptr(gamestate* const g, entity* e, spritegroup_t* sg) {
    if (!e || !sg) {
        merror("libdraw_update_sprite_ptr: entity or spritegroup is NULL");
        return;
    }
    if (e->is_dead && !spritegroup_is_animating(sg)) { return; }
    if (e->do_update) {
        libdraw_update_sprite_context_ptr(g, sg, e->direction);
        e->do_update = false;
    }
    // Copy movement intent from e->sprite_move_x/y if present
    libdraw_update_sprite_position(sg, e);
    libdraw_update_sprite_attack(e, sg);
    // Update movement as long as sg->move.x/y is non-zero
    spritegroup_update_dest(sg);
    // Snap to the tile position only when movement is fully complete
    spritegroup_snap_dest(sg, e->x, e->y);
}

void libdraw_update_sprite_attack(entity_t* e, spritegroup_t* sg) {
    if (e->is_attacking) {
        libdraw_set_sg_is_attacking(e, sg);
    } else if (e->is_damaged) {
        libdraw_set_sg_is_damaged(e, sg);
    } else if (e->is_dead) {
        libdraw_set_sg_is_dead(e, sg);
    }
}

void libdraw_set_sg_is_attacking(entity_t* const e, spritegroup_t* const sg) {
    if (!e || !sg) {
        merror("libdraw_set_sg_is_attacking: entity or spritegroup is NULL");
        return;
    }
    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_ATTACK;
    } else if (e->race == RACE_ORC) {
        sg->current = SPRITEGROUP_ANIM_ORC_ATTACK;
    }
    e->is_attacking = false;
}

void libdraw_set_sg_is_damaged(entity_t* const e, spritegroup_t* const sg) {
    if (!e || !sg) {
        merror("libdraw_set_sg_is_damaged: entity or spritegroup is NULL");
        return;
    }

    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_DMG;
    } else if (e->race == RACE_ORC) {
        sg->current = SPRITEGROUP_ANIM_ORC_DMG;
    }
    e->is_damaged = false;
}

void libdraw_set_sg_is_dead(entity_t* const e, spritegroup_t* const sg) {
    if (!e || !sg) {
        merror("libdraw_set_sg_is_dead: entity or spritegroup is NULL");
        return;
    }

    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_SPINDIE;
        sg->default_anim = SPRITEGROUP_ANIM_HUMAN_SPINDIE;
        spritegroup_set_stop_on_last_frame(sg, true);
    } else if (e->race == RACE_ORC) {
        sg->current = SPRITEGROUP_ANIM_ORC_DIE;
        sg->default_anim = SPRITEGROUP_ANIM_ORC_DIE;
        spritegroup_set_stop_on_last_frame(sg, true);
    }
}

void libdraw_update_sprite_position(spritegroup_t* sg, entity_t* e) {
    if (!sg || !e) {
        merror("libdraw_update_sprite_position: spritegroup or entity is NULL");
        return;
    }
    if (e->sprite_move_x != 0 || e->sprite_move_y != 0) {
        sg->move.x = e->sprite_move_x;
        sg->move.y = e->sprite_move_y;
        e->sprite_move_x = 0;
        e->sprite_move_y = 0;
        // set the current based on e->race
        if (e->race == RACE_HUMAN) {
            sg->current = SPRITEGROUP_ANIM_HUMAN_WALK; // Set animation
        } else if (e->race == RACE_ORC) {
            sg->current = SPRITEGROUP_ANIM_ORC_WALK; // Set animation
        } // else no sprite animation update
    }
}

void libdraw_handle_frame_incr(gamestate* const g, spritegroup_t* const sg) {
    if (!g) {
        merror("libdraw_handle_frame_incr: gamestate is NULL");
        return;
    }
    sprite* const s = sg->sprites[sg->current];
    if (!s) {
        merror("libdraw_update_sprite: sprite is NULL");
        return;
    }
    // attempt to grab the sprite's shadow
    sprite* const s_shadow = sg->sprites[sg->current + 1];
    if (!s_shadow) {
        merror("libdraw_update_sprite: shadow sprite is NULL");
        // don't need to return... we can just continue
    }
    if (g->framecount % ANIM_SPEED == 0) {
        sprite_incrframe(s);
        // Check if the animation has completed one loop
        if (s->num_loops >= 1) {
            sg->current = sg->default_anim;
            s->num_loops = 0;
            // testing switching g->flag
            //if (e->type == ENTITY_PLAYER) {
            //    g->flag = PLAYER_INPUT_FLAG;
            //}
        }
        if (s_shadow) {
            sprite_incrframe(s_shadow);
            if (s_shadow->num_loops >= 1) {
                sg->current = sg->default_anim;
                s_shadow->num_loops = 0;
            }
        }
    }
}

void libdraw_update_sprites(gamestate* const g) {
    if (!g) {
        merror("libdraw_update_sprites: gamestate is NULL");
        return;
    }
    //if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM || g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
    //    ANIM_SPEED = 10;
    //} else if (g->flag == GAMESTATE_FLAG_NPC_ANIM || g->flag == GAMESTATE_FLAG_NPC_TURN) {
    //    ANIM_SPEED = 10;
    //}
    // for each entityid in our entitymap, update the spritegroup
    for (int i = 0; i < g->index_entityids; i++) {
        const entityid id = g->entityids[i];
        libdraw_update_sprite(g, id);
    }

    libdraw_handle_gamestate_flag(g);
}

void libdraw_handle_gamestate_flag(gamestate* const g) {
    const bool done = libdraw_check_default_animations(g);
    if (done) {
        if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            g->flag = GAMESTATE_FLAG_NPC_TURN;
        } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
            g->entity_turn = g->hero_id; // Reset directly to hero
            g->flag = GAMESTATE_FLAG_PLAYER_INPUT;

            g->turn_count++;
        }
    }
}

bool libdraw_draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, int x, int y) {
    if (!g || !df) {
        merror("libdraw_draw_dungeon_floor_tile: gamestate or dungeon_floor is NULL");
        return false;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merrorint2("libdraw_draw_dungeon_floor_tile: x or y out of bounds", x, y);
        return false;
    }
    dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merrorint2("libdraw_draw_dungeon_floor_tile: tile is NULL", x, y);
        return false;
    }
    // check if the tile type is none
    if (tile->type == TILE_NONE) {
        //merrorint2("libdraw_draw_dungeon_floor_tile: tile type is none", x, y);
        return true;
    }
    // just draw the tile itself
    // tile values in get_txkey_for_tiletype.h
    int txkey = get_txkey_for_tiletype(tile->type);
    if (txkey < 0) {
        //merrorint("libdraw_draw_dungeon_floor_tile: txkey is invalid", txkey);
        return false;
    }
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0) {
        merrorint4("libdraw_draw_dungeon_floor_tile: texture id is invalid", texture->id, x, y, txkey);
        return false;
    }
    // atm hard-coding the size of the new tiles and their destinations
    const int sw = DEFAULT_TILE_SIZE * 4, sh = sw, dw = sw, dh = sw;
    const int dx = x * DEFAULT_TILE_SIZE - 12;
    const int dy = y * DEFAULT_TILE_SIZE - 12;
    Rectangle src = (Rectangle){0, 0, sw, sh};
    Rectangle dest = (Rectangle){dx, dy, dw, dh};
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    // draw the pressure plate if it exists
    if (tile->has_pressure_plate) {
        const int txkey2 = tile->pressure_plate_up_tx_key;
        if (txkey2 < 0) {
            merrorint2("libdraw_draw_dungeon_floor_tile: pressure plate up txkey is invalid", x, y);
            return false;
        }
        Texture2D* texture = &txinfo[txkey2].texture;
        if (texture->id <= 0) {
            //merrorint3("libdraw_draw_dungeon_floor_tile: pressure plate texture id is invalid", x, y, txkey);
            return false;
        }
        //const int tile_size_src_w = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_src_h = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_dest_w = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_dest_h = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_dest_x = x * DEFAULT_TILE_SIZE - 12;
        //const int tile_size_dest_y = y * DEFAULT_TILE_SIZE - 12;
        //Rectangle src = (Rectangle){0, 0, tile_size_src_w, tile_size_src_h};
        //Rectangle dest = (Rectangle){tile_size_dest_x, tile_size_dest_y, tile_size_dest_w, tile_size_dest_h};
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }

    // draw the wall switch
    if (tile->has_wall_switch) {
        //const int txkey = tile->wall_switch_up_tx_key;
        int txkey = -1;
        if (tile->wall_switch_on) {
            txkey = tile->wall_switch_down_tx_key;
        } else {
            txkey = tile->wall_switch_up_tx_key;
        }

        if (txkey < 0) {
            merrorint2("libdraw_draw_dungeon_floor_tile: wall switch up txkey is invalid", x, y);
            return false;
        }
        Texture2D* texture = &txinfo[txkey].texture;
        if (texture->id <= 0) {
            merrorint3("libdraw_draw_dungeon_floor_tile: wall switch texture id is invalid", x, y, txkey);
            return false;
        }
        //const int tile_size_src_w = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_src_h = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_dest_w = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_dest_h = DEFAULT_TILE_SIZE * 4;
        //const int tile_size_dest_x = x * DEFAULT_TILE_SIZE - 12;
        //const int tile_size_dest_y = y * DEFAULT_TILE_SIZE - 12;
        //Rectangle src = (Rectangle){0, 0, tile_size_src_w, tile_size_src_h};
        //Rectangle dest = (Rectangle){tile_size_dest_x, tile_size_dest_y, tile_size_dest_w, tile_size_dest_h};
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }
    return true;
}

bool libdraw_draw_dungeon_floor(const gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_dungeon_floor: gamestate is NULL");
        return false;
    }
    dungeon_floor_t* const df = dungeon_get_current_floor(g->dungeon);
    if (!df) {
        merror("libdraw_draw_dungeon_floor: dungeon_floor is NULL");
        return false;
    }
    draw_dungeon_tiles_2d(g, df);
    draw_entities_2d(g, df, true); // dead entities
    draw_entities_2d(g, df, false); // alive entities
    draw_wall_tiles_2d(g, df);
    return true;
}

void libdraw_draw_debug_panel(gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_debug_panel: gamestate is NULL");
        return;
    }
    const Color bg = Fade((Color){0x66, 0x66, 0x66}, 0.8f);
    const Color fg = WHITE;
    const int w0 = g->debugpanel.w + g->debugpanel.pad_left + g->debugpanel.pad_right * 4;
    const int h0 = g->debugpanel.h + g->debugpanel.pad_top + g->debugpanel.pad_bottom;
    const int x1 = g->debugpanel.x + g->debugpanel.pad_left;
    const int y1 = g->debugpanel.y + g->debugpanel.pad_top;
    DrawRectangle(g->debugpanel.x, g->debugpanel.y, w0, h0, bg);
    DrawText(g->debugpanel.buffer, x1, y1, g->debugpanel.font_size, fg);
}

bool libdraw_camera_lock_on_3d(gamestate* const g) {
    if (!g) return false;
    const float TILE_SIZE = 1.0f;
    const float TILE_HEIGHT = 1.0f;
    entity_t* p = em_get(g->entitymap, g->hero_id);
    if (!p) return false;
    g->cam3d.target = (Vector3){p->x * TILE_SIZE, TILE_HEIGHT, p->y * TILE_SIZE};
    return true;
}

bool libdraw_draw_dungeon_floor_3d(gamestate* const g) {
    if (!g) return false;
    dungeon_floor_t* floor = dungeon_get_current_floor(g->dungeon);
    if (!floor) return false;
    draw_dungeon_tiles_3d(g, floor);
    draw_entities_3d(g, floor, true); // dead
    draw_entities_3d(g, floor, false); // alive
    draw_wall_tiles_3d(g, floor);
    return true;
}

bool libdraw_draw_player_target_box_3d(gamestate* const g) {
    if (!g) return false;
    const float w = 1.0f;
    const float h = 1.0f;
    entity_t* p = em_get(g->entitymap, g->hero_id);
    if (!p) return false;
    // Placeholder: draw a wireframe cube at player's position
    DrawCubeWires((Vector3){p->x * w, h, p->y * w}, w, w, w, RED);
    return true;
}

void libdraw_drawframe_3d(gamestate* const g) {
    if (!g) return;
    ClearBackground(BLACK);
    BeginMode3D(g->cam3d);
    if (!libdraw_camera_lock_on_3d(g)) merror("camera lock 3D failed");
    if (!libdraw_draw_dungeon_floor_3d(g)) merror("draw dungeon floor 3D failed");
    //if (!libdraw_draw_player_target_box_3d(g)) merror("draw target box 3D failed");
    EndMode3D();
}

void libdraw_drawframe_2d(gamestate* const g) {
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    if (!libdraw_camera_lock_on(g)) merror("libdraw_drawframe: failed to lock camera on hero");
    if (!libdraw_draw_dungeon_floor(g)) merror("libdraw_drawframe: failed to draw dungeon floor");
    if (!libdraw_draw_player_target_box(g)) merror("libdraw_drawframe: failed to draw player target box");
    EndMode2D();
}

void libdraw_drawframe(gamestate* const g) {
    double start_time = GetTime();
    BeginDrawing();
    ClearBackground(WHITE);
    BeginTextureMode(target);

    if (g->is3d) {
        libdraw_drawframe_3d(g);

    } else {
        libdraw_drawframe_2d(g);
    }
    EndTextureMode();
    //BeginShaderMode(shader_grayscale);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_grayscale, GetShaderLocation(shader_grayscale, "time"), &time, SHADER_UNIFORM_FLOAT);
    //EndShaderMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    libdraw_draw_hud(g);
    //libdraw_draw_msgbox_test(g, "Hello, world!\nLets fucking go!");
    if (g->debugpanelon) { libdraw_draw_debug_panel(g); }
    EndDrawing();
    //double elapsed_time = GetTime() - start_time;
    g->last_frame_time = GetTime() - start_time;
    g->framecount++;
}

bool libdraw_draw_player_target_box(const gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_player_target_box: gamestate is NULL");
        return false;
    }
    const entityid id = g->hero_id;
    if (id == -1) {
        merror("libdraw_draw_player_target_box: id is -1");
        return false;
    }
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merrorint("libdraw_draw_player_target_box: entity not found", id);
        return false;
    }
    direction_t dir = e->direction;
    int x = e->x, y = e->y;
    x += dir == DIR_LEFT || dir == DIR_DOWN_LEFT || dir == DIR_UP_LEFT      ? -1
         : dir == DIR_RIGHT || dir == DIR_DOWN_RIGHT || dir == DIR_UP_RIGHT ? 1
                                                                            : 0;
    y += dir == DIR_UP || dir == DIR_UP_LEFT || dir == DIR_UP_RIGHT         ? -1
         : dir == DIR_DOWN || dir == DIR_DOWN_LEFT || dir == DIR_DOWN_RIGHT ? 1
                                                                            : 0;
    const int ds = DEFAULT_TILE_SIZE;
    const Color base_c = GREEN;
    const float a = 0.5f;
    const Color c = Fade(base_c, a);
    DrawRectangleLinesEx((Rectangle){x * ds, y * ds, ds, ds}, 1, c);
    return true;
}

void libdraw_draw_sprite(const gamestate* const g, const entityid id) {
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    if (!sg) {
        merror("libdraw_draw_sprite: spritegroup is NULL");
        return;
    }
    sprite* s = spritegroup_get(sg, sg->current);
    if (!s) {
        merror("sprite not found");
        return;
    }
    Rectangle new_dest = (Rectangle){sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    DrawTexturePro(*s->texture, s->src, new_dest, (Vector2){0, 0}, 0, WHITE);
}

void libdraw_draw_sprite_and_shadow(const gamestate* const g, entityid id) {
    if (!g) {
        merror("libdraw_draw_sprite_and_shadow: gamestate is NULL");
        return;
    }
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    if (!sg) {
        //merrorint("libdraw_draw_sprite_and_shadow: spritegroup not found", id);
        return;
    }
    sprite* s = spritegroup_get(sg, sg->current);
    if (!s) {
        merrorint("libdraw_draw_sprite_and_shadow: sprite not found at current", sg->current);
        return;
    }
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    sprite* sh = spritegroup_get(sg, sg->current + 1);
    if (sh) {
        DrawTexturePro(*sh->texture, sh->src, dest, (Vector2){0, 0}, 0, WHITE);
    } else {
        merrorint("libdraw_draw_sprite_and_shadow: shadow sprite not found at current+1", sg->current + 1);
    }
    // Draw sprite on top
    DrawTexturePro(*s->texture, s->src, dest, zero_vec, 0, WHITE);
}

void libdraw_close() {
    libdraw_unload_textures();
    libdraw_unload_shaders();
    CloseWindow();
}

bool libdraw_load_texture(int txkey, int ctxs, int frames, bool do_dither, char* path) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_load_texture: txkey out of bounds");
        return false;
    } else if (ctxs < 0) {
        merror("libdraw_load_texture: contexts out of bounds");
        return false;
    } else if (frames < 0) {
        merror("libdraw_load_texture: frames out of bounds");
        return false;
    } else if (txinfo[txkey].texture.id > 0) {
        merror("libdraw_load_texture: texture already loaded");
        return false;
    } else if (strlen(path) == 0) {
        merror("libdraw_load_texture: path is empty");
        return false;
    } else if (strcmp(path, "\n") == 0) {
        merror("libdraw_load_texture: path is newline");
        return false;
    }
    Image image = LoadImage(path);
    if (do_dither) { ImageDither(&image, 4, 4, 4, 4); }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    txinfo[txkey].texture = texture;
    txinfo[txkey].contexts = ctxs;
    txinfo[txkey].num_frames = frames;
    msuccess("libdraw_load_texture: texture loaded successfully");
    return true;
}

bool libdraw_unload_texture(int txkey) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_unload_texture: txkey out of bounds");
        return false;
    }
    UnloadTexture(txinfo[txkey].texture);
    txinfo[txkey].texture = (Texture2D){0};
    txinfo[txkey].contexts = 0;
    msuccess("libdraw_unload_texture: texture unloaded successfully");
    return true;
}

void libdraw_load_textures() {
    const char* textures_file = "textures.txt";
    FILE* file = fopen(textures_file, "r");
    if (!file) {
        merror("libdraw_load_textures: textures.txt not found");
        return;
    }
    char line[1024] = {0};
    while (fgets(line, sizeof(line), file)) {
        int txkey = -1, contexts = -1, frames = -1, do_dither = 0;
        char path[512] = {0};
        // check if the line begins with a #
        if (line[0] == '#') continue;
        sscanf(line, "%d %d %d %d %s", &txkey, &contexts, &frames, &do_dither, path);
        if (txkey < 0 || contexts < 0 || frames < 0) {
            merror("libdraw_load_textures: invalid line in textures.txt");
            continue;
        }
        if (!libdraw_load_texture(txkey, contexts, frames, do_dither, path)) {
            merrorstr("libdraw_load_textures: failed to load texture", path);
        }
    }
    fclose(file);
}

void libdraw_unload_textures() {
    for (int i = 0; i < GAMESTATE_SIZEOFTEXINFOARRAY; i++) libdraw_unload_texture(i);
}

void libdraw_create_spritegroup(gamestate* const g,
                                entityid id,
                                int* keys,
                                int num_keys,
                                int offset_x,
                                int offset_y,
                                specifier_t spec) {
    minfo("libdraw_create_spritegroup");
    if (!g) {
        merror("libdraw_create_spritegroup: gamestate is NULL");
        return;
    }
    msuccess("g was not NULL");
    // can hold up to 32 sprites
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    if (!group) {
        merror("libdraw_create_spritegroup: failed to create spritegroup");
        return;
    }
    msuccess("group was not NULL");
    const entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merrorint("libdraw_create_spritegroup: entity not found", id);
        spritegroup_destroy(group);
        return;
    }
    msuccessint("entity found", id);
    //disabling this check until dungeon_floor created
    dungeon_floor_t* df = dungeon_get_current_floor(g->dungeon);
    if (!df) {
        merror("libdraw_create_spritegroup: dungeon_floor is NULL");
        spritegroup_destroy(group);
        return;
    }
    const int df_w = df->width, df_h = df->height;
    if (e->x < 0 || e->x >= df_w || e->y < 0 || e->y >= df_h) {
        merrorint2("libdraw_create_spritegroup: entity pos out of bounds", e->x, e->y);
        spritegroup_destroy(group);
        return;
    }
    minfoint("libdraw_create_spritegroup: creating spritegroup for entityid", id);
    for (int i = 0; i < num_keys; i++) {
        const int k = keys[i];
        Texture2D* tex = &txinfo[k].texture;
        sprite* s = sprite_create(tex, txinfo[k].contexts, txinfo[k].num_frames);
        spritegroup_add(group, s);
    }
    spritegroup_set_specifier(group, spec);
    group->id = id;
    sprite* s = spritegroup_get(group, 0);
    if (!s) {
        merror("libdraw_create_spritegroup: sprite is NULL");
        spritegroup_destroy(group);
        return;
    }
    const float w = s->width, h = s->height, dst_x = e->x * DEFAULT_TILE_SIZE, dst_y = e->y * DEFAULT_TILE_SIZE;
    group->current = 0;
    group->dest = (Rectangle){dst_x + offset_x, dst_y + offset_y, w, h};
    group->off_x = offset_x, group->off_y = offset_y;
    msuccessint("inserting spritegroup for entity", id);
    hashtable_entityid_spritegroup_insert(spritegroups, id, group);
    msuccessint("Spritegroup created for entity", id);
}

void libdraw_calc_debugpanel_size(gamestate* const g) {
    if (!g) {
        merror("libdraw_calc_debugpanel_size: gamestate is NULL");
        return;
    }
    Vector2 s = MeasureTextEx(GetFontDefault(), g->debugpanel.buffer, g->debugpanel.font_size, 1);
    g->debugpanel.w = s.x;
    g->debugpanel.h = s.y;
}

void libdraw_update_sprite_context(gamestate* const g, entityid id, direction_t dir) {
    if (!g) {
        merror("libdraw_update_sprite_context: gamestate is NULL");
        return;
    }
    //minfoint2("libdraw_update_sprite_context: updating sprite context for entity", id, dir);
    spritegroup_t* group = hashtable_entityid_spritegroup_get(spritegroups, id); // Adjusted for no specifier
    if (!group) {
        merrorint("libdraw_update_sprite_context: group not found", id);
        return;
    }
    libdraw_update_sprite_context_ptr(g, group, dir);
}

void libdraw_update_sprite_context_ptr(gamestate* const g, spritegroup_t* group, direction_t dir) {
    if (!g) {
        merror("libdraw_update_sprite_context: gamestate is NULL");
        return;
    }
    //minfoint2("libdraw_update_sprite_context: updating sprite context for entity", id, dir);
    //spritegroup_t* group = hashtable_entityid_spritegroup_get(spritegroups, id); // Adjusted for no specifier
    if (!group) {
        merrorint("libdraw_update_sprite_context: group is NULL", id);
        return;
    }
    const int old_ctx = group->sprites[group->current]->currentcontext;
    int ctx = old_ctx;
    ctx = dir == DIR_NONE                                      ? old_ctx
          : dir == DIR_DOWN_RIGHT                              ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN_LEFT                               ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_UP_RIGHT                                ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP_LEFT                                 ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_D    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_D    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_U    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_U    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_U
                                                               : old_ctx;
    spritegroup_setcontexts(group, ctx);
}

void libdraw_create_sg_byid(gamestate* const g, entityid id) {
    if (!g) {
        merror("libdraw_create_sg_byid: gamestate is NULL");
        return;
    }
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merrorint("libdraw_create_sg_byid: entity not found", id);
        return;
    }
    int* keys = NULL;
    int num_keys = 0;
    int offset_x = 0;
    int offset_y = 0;
    switch (e->race) {
    case RACE_HUMAN:
        keys = TX_HUMAN_KEYS;
        num_keys = TX_HUMAN_KEY_COUNT;
        offset_x = -12;
        offset_y = -12;
        break;
    // Add cases for other races here
    case RACE_ORC:
        keys = TX_ORC_KEYS;
        num_keys = TX_ORC_KEY_COUNT;
        offset_x = -12;
        offset_y = -12;
        break;
    default: merrorint("libdraw_create_sg_byid: unknown race", e->race); return;
    }

    libdraw_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
}

void libdraw_draw_hud(gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_hud: gamestate is NULL");
        return;
    }
    // Draw the HUD
    const int fontsize = 20, pad = 4, pad2 = pad * 2, x = pad, y = pad;
    char buffer[1024] = {0};
    int hp = -1, maxhp = -1;
    //const char* text = "Name: darkmage\nHP: 1/1";
    entity* const e = em_get(g->entitymap, g->hero_id);
    if (e) {
        hp = e->hp;
        maxhp = e->maxhp;
    }
    //snprintf(buffer, sizeof(buffer), "Name: %s", e->name);
    snprintf(buffer, sizeof(buffer), "Name: %s\nHP: %d/%d\nTurn: %d", e->name, hp, maxhp, g->turn_count);
    //const char* text = "Name: evildojo666\nHP: 1/1";
    const Vector2 size = MeasureTextEx(GetFontDefault(), buffer, fontsize, 1);
    const int w = size.x + pad2 * 4, h = size.y + pad2;
    const Color fg = (Color){0x33, 0x33, 0x33, 0xff}, fg2 = (Color){0x66, 0x66, 0x66, 0xff}, fg3 = WHITE;
    DrawRectangle(x, y, w, h, fg);
    // draw rectangle lines around the box
    DrawRectangleLines(x, y, w, h, fg2);
    const int x2 = x + pad, y2 = y + pad;
    // Draw text
    DrawText(buffer, x2, y2, fontsize, fg3);
}

void libdraw_draw_msgbox_test(gamestate* const g, const char* s) {
    if (!g) {
        merror("libdraw_draw_msgbox_test: gamestate is NULL");
        return;
    }
    const int fontsize = 20, f_offset = 30;
    const Vector2 size = MeasureTextEx(GetFontDefault(), s, fontsize + f_offset, 1);
    //const Vector2 size2 = MeasureTextEx(GetFontDefault(), text, fontsize, 1);
    const int w = size.x, h = size.y, pad = 10, pad2 = 20;
    const int x = g->windowwidth / 4 - w / 2, y = 0 + pad, x2 = x + pad2, y2 = y + pad2;
    const Color bg = (Color){0x33, 0x33, 0x33, 0xff}, fg = WHITE;
    // we need to calculate x and y based on the w and h
    //const int x = g->windowwidth / 2 - w / 2, y = g->windowheight / 2 - h / 2;
    DrawRectangle(x + pad, y + pad, w, h, bg);
    // we need to calculate an x and y for the text based on size2 and a padding
    DrawText(s, x2, y2, fontsize, fg);
}

void libdraw_init(gamestate* const g) {
    if (!g) {
        merror("libdraw_init g is NULL");
        return;
    }
    const int w = DEFAULT_WIN_WIDTH, h = DEFAULT_WIN_HEIGHT, x = w / 4, y = h / 4;
    InitWindow(w, h, "evildojo666");
    g->windowwidth = w;
    g->windowheight = h;
    SetTargetFPS(60);
    target = LoadRenderTexture(w, h);
    target_src = (Rectangle){0, 0, w, -h};
    target_dest = (Rectangle){0, 0, w, h};
    spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_SPRITEGROUPS_SIZE);
    libdraw_load_textures();
    printf("libdraw_init: loaded textures\n");
    for (int i = 0; i < g->index_entityids; i++) { libdraw_create_sg_byid(g, g->entityids[i]); }
    libdraw_calc_debugpanel_size(g);
    libdraw_load_shaders();
    g->cam2d.offset = (Vector2){x, y};
    msuccess("libdraw_init");
}
