#include "libdraw.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "hashtable_entityid_spritegroup.h"
#include "mprint.h"
#include "spritegroup.h"
#include "textureinfo.h"
#include "tx_keys.h"
#include <ctype.h>

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

RenderTexture2D target = {0};
Rectangle target_src = {0, 0, 800, 480};
Rectangle target_dest = {0, 0, 800, 480};
Vector2 target_origin = {0, 0};

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 480
#define DEFAULT_TILE_SIZE 8
#define SPRITEGROUP_DEFAULT_SIZE 32
#define ANIM_SPEED 10


void libdraw_init(gamestate* const g) {
    if (!g) {
        merror("libdraw_init g is NULL");
        return;
    }
    const char* t = "evildojo666";
    const int w = DEFAULT_WINDOW_WIDTH, h = DEFAULT_WINDOW_HEIGHT;
    InitWindow(w, h, t);
    SetTargetFPS(60);
    target = LoadRenderTexture(w, h);
    target_src = (Rectangle){0, 0, w, -h};
    target_dest = (Rectangle){0, 0, w, h};
    spritegroups = hashtable_entityid_spritegroup_create(4);
    libdraw_load_textures();
    libdraw_create_spritegroup(g, g->hero_id, TX_HUMAN_KEYS, TX_HUMAN_KEY_COUNT, -12, -12, SPECIFIER_NONE);
    msuccess("libdraw_init");
}


void libdraw_update_input(inputstate* const is) {
    inputstate_update(is);
}


void libdraw_update_sprite(gamestate* const g, entityid id) {
    if (!g) {
        merror("libdraw_update_sprite: gamestate is NULL");
        return;
    }
    entity* const e = em_get(g->entitymap, id);
    if (e) {
        spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
        if (sg) {

            if (!e->sprite_move_x && !e->sprite_move_y) {
                sg->dest.x = e->x * DEFAULT_TILE_SIZE;
                sg->dest.y = e->y * DEFAULT_TILE_SIZE;
            } else {
                if (e->sprite_move_x) {
                    if (e->sprite_move_x > 0) {
                        sg->dest.x++;
                        e->sprite_move_x--;
                    } else if (e->sprite_move_x < 0) {
                        sg->dest.x--;
                        e->sprite_move_x++;
                    }
                }

                if (e->sprite_move_y) {
                    if (e->sprite_move_y > 0) {
                        sg->dest.y++;
                        e->sprite_move_y--;
                    } else if (e->sprite_move_y < 0) {
                        sg->dest.y--;
                        e->sprite_move_y++;
                    }
                }
            }

            //sg->dest.x = e->x * DEFAULT_TILE_SIZE;
            //sg->dest.y = e->y * DEFAULT_TILE_SIZE;
            sprite* s = sg->sprites[sg->current];
            if (!s) {
                merror("libdraw_update_sprite: sprite is NULL");
                return;
            }
            if (g->framecount % ANIM_SPEED == 0) sprite_incrframe(s);
        }
    }
}


void libdraw_update_sprites(gamestate* const g) {
    if (!g) {
        merror("libdraw_update_sprites: gamestate is NULL");
        return;
    }
    //minfo("libdraw_update_sprites");
    // for each entityid in our entitymap, update the spritegroup
    libdraw_update_sprite(g, g->hero_id);
    for (int i = 0; i < g->index_entityids; i++) {
        entityid id = g->entityids[i];
        libdraw_update_sprite(g, id);
    }
}


void libdraw_draw_dungeon_floor_tile(const gamestate* const g, const int x, const int y) {
    if (!g) {
        merror("libdraw_draw_dungeon_floor_tile: gamestate is NULL");
        return;
    }
    if (x < 0 || x >= g->dungeon_floor->width || y < 0 || y >= g->dungeon_floor->height) {
        merrorint2("libdraw_draw_dungeon_floor_tile: x or y out of bounds", x, y);
        return;
    }
    dungeon_tile_t* tile = dungeon_floor_tile_at(g->dungeon_floor, x, y);
    if (!tile) {
        merrorint2("libdraw_draw_dungeon_floor_tile: tile is NULL", x, y);
        return;
    }
    // just draw the tile itself
    // tile values in get_txkey_for_tiletype.h
    int txkey = get_txkey_for_tiletype(tile->type);
    if (txkey < 0) {
        merrorint("libdraw_draw_dungeon_floor_tile: txkey is invalid", txkey);
        return;
    }
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0) {
        merrorint("libdraw_draw_dungeon_floor_tile: texture id is invalid", texture->id);
        return;
    }
    Rectangle src = (Rectangle){0, 0, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    const int dest_x = x * DEFAULT_TILE_SIZE;
    const int dest_y = y * DEFAULT_TILE_SIZE;
    Rectangle dest = (Rectangle){dest_x, dest_y, DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE};
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
}


void libdraw_draw_dungeon_floor(const gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_dungeon_floor: gamestate is NULL");
        return;
    }
    if (!g->dungeon_floor) {
        merror("libdraw_draw_dungeon_floor: dungeon_floor is NULL");
        return;
    }
    for (int y = 0; y < g->dungeon_floor->height; y++) {
        for (int x = 0; x < g->dungeon_floor->width; x++) {
            libdraw_draw_dungeon_floor_tile(g, x, y);
        }
    }
}


void libdraw_drawframe(gamestate* const g) {
    double start_time = GetTime();
    BeginDrawing();
    BeginTextureMode(target);
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    char buffer[1024] = {0};
    char buffer2[1024] = {0};
    libdraw_draw_dungeon_floor(g);
    libdraw_draw_sprite_and_shadow(g, g->hero_id);
    EndMode2D();
    EndTextureMode();
    snprintf(buffer, sizeof(buffer), "Frame draw time: %.02f ms\n", g->last_frame_time * 1000);
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    DrawText(buffer, 10, 10, 20, WHITE);
    EndDrawing();
    double elapsed_time = GetTime() - start_time;
    g->last_frame_time = elapsed_time;
    g->framecount++;
}


//void libdraw_draw_dungeon_tile(const gamestate* const g, const int x, const int y) {
//}


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
    const int scale = 4;
    Rectangle new_dest = (Rectangle){sg->dest.x, sg->dest.y, scale * sg->dest.width, scale * sg->dest.height};
    DrawTexturePro(*s->texture, s->src, new_dest, (Vector2){0, 0}, 0, WHITE);
}


void libdraw_draw_sprite_and_shadow(const gamestate* const g, entityid id) {
    if (!g) {
        merror("libdraw_draw_sprite_and_shadow: gamestate is NULL");
        return;
    }
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    if (!sg) {
        merrorint("libdraw_draw_sprite_and_shadow: spritegroup not found", id);
        return;
    }
    sprite* s = spritegroup_get(sg, sg->current);
    if (!s) {
        merrorint("libdraw_draw_sprite_and_shadow: sprite not found at current", sg->current);
        return;
    }
    //const int scale = 4;
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    sprite* sh = spritegroup_get(sg, sg->current + 1);
    if (sh) {
        DrawTexturePro(*sh->texture, sh->src, dest, (Vector2){0, 0}, 0, WHITE);
    } else {
        merrorint("libdraw_draw_sprite_and_shadow: shadow sprite not found at current+1", sg->current + 1);
    }
    // Draw sprite on top
    DrawTexturePro(*s->texture, s->src, dest, (Vector2){0, 0}, 0, WHITE);
}


void libdraw_close() {
    //g = NULL;
    libdraw_unload_textures();
    CloseWindow();
}


bool libdraw_windowshouldclose() {
    return WindowShouldClose();
}


void libdraw_load_texture(
    const int txkey, const int contexts, const int frames, const bool do_dither, const char* path) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_load_texture: txkey out of bounds");
        return;
    }
    if (contexts < 0) {
        merror("libdraw_load_texture: contexts out of bounds");
        return;
    }
    if (frames < 0) {
        merror("libdraw_load_texture: frames out of bounds");
        return;
    }
    if (txinfo[txkey].texture.id > 0) {
        merror("libdraw_load_texture: texture already loaded");
        return;
    }
    if (strlen(path) == 0) {
        merror("libdraw_load_texture: path is empty");
        return;
    }
    if (strcmp(path, "\n") == 0) {
        merror("libdraw_load_texture: path is newline");
        return;
    }
    Image image = LoadImage(path);
    if (do_dither) ImageDither(&image, 8, 8, 8, 8);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    txinfo[txkey].texture = texture;
    txinfo[txkey].contexts = contexts;
    txinfo[txkey].num_frames = frames;
    msuccess("libdraw_load_texture: texture loaded successfully");
}


void libdraw_unload_texture(const int txkey) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_unload_texture: txkey out of bounds");
        return;
    }
    UnloadTexture(txinfo[txkey].texture);
    txinfo[txkey].texture = (Texture2D){0};
    txinfo[txkey].contexts = 0;
    msuccess("libdraw_unload_texture: texture unloaded successfully");
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
        int txkey = -1;
        int contexts = -1;
        int frames = -1;
        int do_dither = 0;
        char path[512] = {0};
        // check if the line begins with a #
        if (line[0] == '#') continue;
        sscanf(line, "%d %d %d %d %s", &txkey, &contexts, &frames, &do_dither, path);
        if (txkey < 0 || contexts < 0 || frames < 0) {
            merror("libdraw_load_textures: invalid line in textures.txt");
            continue;
        }
        libdraw_load_texture(txkey, contexts, frames, do_dither, path);
        minfo("libdraw_load_textures: loaded texture");
        minfoint("txkey", txkey);
        minfoint("contexts", contexts);
        minfoint("frames", frames);
        minfo("path");
        minfo(path);
    }
    fclose(file);
}


bool libdraw_line_begins_with_whitespace_and_char(const char* l) {
    if (!l) {
        merror("libdraw_line_begins_with_whitespace_and_char: line is NULL");
        return false;
    }
    while (*l != 0 && isspace(*l))
        l++;
    return *l != 0 && !isspace(*l);
}


void libdraw_unload_textures() {
    for (int i = 0; i < GAMESTATE_SIZEOFTEXINFOARRAY; i++)
        libdraw_unload_texture(i);
}


void libdraw_create_spritegroup(
    gamestate* const g, entityid id, int* keys, int num_keys, int offset_x, int offset_y, specifier_t spec) {
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
    entity* e = em_get(g->entitymap, id);
    if (!e) {
        merrorint("libdraw_create_spritegroup: entity not found", id);
        spritegroup_destroy(group);
        return;
    }
    msuccessint("entity found", id);
    //disabling this check until dungeon_floor created
    //if (e->x < 0 || e->x >= g->dungeon_floor->width || e->y < 0 || e->y >= g->dungeon_floor->height) {
    //    merrorint2("libdraw_create_spritegroup: entity pos out of bounds", e->x, e->y);
    //    spritegroup_destroy(group);
    //    return;
    //}
    //    // Use TX_HUMAN_WALK (assume index 2—check textures.txt)
    //int tx_key = keys[0]; // First key (e.g., TX_HUMAN_WALK)
    int sprite_width = -1;
    int sprite_height = -1;
    minfo("libdraw_create_spritegroup: creating spritegroup");
    for (int i = 0; i < TX_HUMAN_KEY_COUNT; i++) {
        int k = keys[i];
        minfo("beginning loop");
        minfoint("k", k);
        Texture2D* tex = &txinfo[k].texture;
        minfo("got texture");
        sprite* s = sprite_create(tex, txinfo[k].contexts, txinfo[k].num_frames);
        minfo("created sprite");
        spritegroup_add(group, s);
        minfo("added sprite");
    }
    minfo("loop done");
    spritegroup_set_specifier(group, spec);
    group->id = id;
    const float w = spritegroup_get(group, 0)->width;
    const float h = spritegroup_get(group, 0)->height;
    const float dst_x = e->x * DEFAULT_TILE_SIZE;
    const float dst_y = e->y * DEFAULT_TILE_SIZE;
    group->current = 0;
    group->dest = (Rectangle){dst_x + offset_x, dst_y + offset_y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    msuccessint("inserting spritegroup for entity", id);
    hashtable_entityid_spritegroup_insert(spritegroups, id, group);
    msuccessint("Spritegroup created for entity", id);
}
