#include "libdraw.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "hashtable_entityid_spritegroup.h"
//#include "libgame_defines.h"
#include "mprint.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "textureinfo.h"
#include "tx_keys.h"
#include <ctype.h>

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

RenderTexture2D target = {0};
Rectangle target_src = {0, 0, 800, 480};
Rectangle target_dest = {0, 0, 800, 480};
Vector2 target_origin = {0, 0};
Vector2 zero_vector = {0, 0};

//#define DEFAULT_WINDOW_WIDTH 800
//#define DEFAULT_WINDOW_HEIGHT 480
#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720
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
    //SetExitKey(KEY_Q);
    target = LoadRenderTexture(w, h);
    target_src = (Rectangle){0, 0, w, -h};
    target_dest = (Rectangle){0, 0, w, h};
    spritegroups = hashtable_entityid_spritegroup_create(4);
    libdraw_load_textures();

    // here we are hard-coding the creation of the hero sprite
    //libdraw_create_spritegroup(g, g->hero_id, TX_HUMAN_KEYS, TX_HUMAN_KEY_COUNT, -12, -12, SPECIFIER_NONE);

    // use the new function to create the spritegroup
    //libdraw_create_sg_byid(g, g->hero_id);


    // we will need to write a function that, given an entityid, does a lookup on the entity
    // for NPCs, we need to look at the entity race to determine which texture keys, texture key count, offset to use
    // so we might write a `libdraw_create_sg_byid(gamestate* const g, entityid id)` function


    // instead, we want to iterate across our entitymap and create spritegroups for each entity
    for (int i = 0; i < g->index_entityids; i++) {
        entityid id = g->entityids[i];
        // use the new function to create the spritegroup
        libdraw_create_sg_byid(g, id);
    }


    libdraw_calc_debugpanel_size(g); // Calculate size
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
    if (!e) {
        merrorint("libdraw_update_sprite: entity not found", id);
        return;
    }

    spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    if (!sg) {
        merrorint("libdraw_update_sprite: spritegroup not found", id);
        return;
    }

    if (e->do_update) {
        libdraw_update_sprite_context(g, id, e->direction);
        e->do_update = false;
    }

    // Copy movement intent from e->sprite_move_x/y if present
    if (e->sprite_move_x != 0 || e->sprite_move_y != 0) {
        sg->move.x = e->sprite_move_x;
        sg->move.y = e->sprite_move_y;
        e->sprite_move_x = 0;
        e->sprite_move_y = 0;
        sg->current = SPRITEGROUP_ANIM_HUMAN_WALK; // Set animation
    }

    // Update movement as long as sg->move.x/y is non-zero
    if (sg->move.x > 0) {
        sg->dest.x++;
        sg->move.x--;
    } else if (sg->move.x < 0) {
        sg->dest.x--;
        sg->move.x++;
    }

    if (sg->move.y > 0) {
        sg->dest.y++;
        sg->move.y--;
    } else if (sg->move.y < 0) {
        sg->dest.y--;
        sg->move.y++;
    }

    // Snap to the tile position only when movement is fully complete
    if (sg->move.x == 0 && sg->move.y == 0) {
        sg->dest.x = e->x * DEFAULT_TILE_SIZE + sg->off_x;
        sg->dest.y = e->y * DEFAULT_TILE_SIZE + sg->off_y;
    }

    sprite* s = sg->sprites[sg->current];
    if (!s) {
        merror("libdraw_update_sprite: sprite is NULL");
        return;
    }

    if (g->framecount % ANIM_SPEED == 0) {
        sprite_incrframe(s);
        // Check if the animation has completed one loop
        if (s->num_loops >= 1) {
            sg->current = sg->default_anim;
            s->num_loops = 0;
        }
    }
}


void libdraw_update_sprites(gamestate* const g) {
    if (!g) {
        merror("libdraw_update_sprites: gamestate is NULL");
        return;
    }
    // for each entityid in our entitymap, update the spritegroup
    for (int i = 0; i < g->index_entityids; i++) {
        const entityid id = g->entityids[i];
        libdraw_update_sprite(g, id);
    }
}


void libdraw_draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, const int x, const int y) {
    if (!g) {
        merror("libdraw_draw_dungeon_floor_tile: gamestate is NULL");
        return;
    }

    if (!df) {
        merror("libdraw_draw_dungeon_floor_tile: dungeon_floor is NULL");
        return;
    }

    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merrorint2("libdraw_draw_dungeon_floor_tile: x or y out of bounds", x, y);
        return;
    }

    dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
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
    // atm hard-coding the size of the new tiles and their destinations

    const int tile_size_src_w = DEFAULT_TILE_SIZE * 4;
    const int tile_size_src_h = DEFAULT_TILE_SIZE * 4;
    const int tile_size_dest_w = DEFAULT_TILE_SIZE * 4;
    const int tile_size_dest_h = DEFAULT_TILE_SIZE * 4;
    const int tile_size_dest_x = x * DEFAULT_TILE_SIZE - 12;
    const int tile_size_dest_y = y * DEFAULT_TILE_SIZE - 12;

    Rectangle src = (Rectangle){0, 0, tile_size_src_w, tile_size_src_h};
    Rectangle dest = (Rectangle){tile_size_dest_x, tile_size_dest_y, tile_size_dest_w, tile_size_dest_h};
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
}


void libdraw_draw_dungeon_floor(const gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_dungeon_floor: gamestate is NULL");
        return;
    }

    dungeon_floor_t* const df = dungeon_get_current_floor(g->dungeon);
    if (!df) {
        merror("libdraw_draw_dungeon_floor: dungeon_floor is NULL");
        return;
    }

    // draw all non-wall tiles
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            // draw the entities on the tile
            dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
            if (!tile) {
                merrorint2("libdraw_draw_dungeon_floor: tile is NULL", x, y);
                return;
            }
            if (dungeon_tile_is_wall(tile->type)) continue;
            libdraw_draw_dungeon_floor_tile(g, df, x, y);
        }
    }


    // draw all non-wall tile entities
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            // draw the entities on the tile
            dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
            if (!tile) {
                merrorint2("libdraw_draw_dungeon_floor: tile is NULL", x, y);
                return;
            }
            if (dungeon_tile_is_wall(tile->type)) continue;
            const int num_entities = dungeon_tile_entity_count(tile);
            for (int i = 0; i < num_entities; i++) {
                const entityid id = dungeon_tile_get_entity(tile, i);
                if (id == -1) {
                    merrorint2("libdraw_draw_dungeon_floor: entity id is -1", x, y);
                    return;
                }
                libdraw_draw_sprite_and_shadow(g, id);
            }
        }
    }


    // walls etc
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
            if (!tile) {
                merrorint2("libdraw_draw_dungeon_floor: tile is NULL", x, y);
                return;
            }
            if (!dungeon_tile_is_wall(tile->type)) continue;
            libdraw_draw_dungeon_floor_tile(g, df, x, y);
        }
    }
}


void libdraw_draw_debug_panel(gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_debug_panel: gamestate is NULL");
        return;
    }

    //libdraw_calc_debugpanel_size(g); // Calculate size
    // Draw background
    const int x = g->debugpanel.x, y = g->debugpanel.y, w = g->debugpanel.w, h = g->debugpanel.h,
              fontsize = g->debugpanel.font_size;

    const Color c = Fade(RED, 0.8f), c2 = WHITE;
    // calculate the size of the bg box of the debug panel
    // use g->debugpanel.pad_left, top, bottom, right to do this

    const int pad_left = g->debugpanel.pad_left, pad_top = g->debugpanel.pad_top, pad_right = g->debugpanel.pad_right,
              pad_bottom = g->debugpanel.pad_bottom;
    // the root of the box should be at x,y

    const int w0 = w + pad_left + pad_right * 4, h0 = h + pad_top + pad_bottom;

    DrawRectangle(x, y, w0, h0, c);
    // Draw text

    const int x1 = x + pad_left, y1 = y + pad_top;

    DrawText(g->debugpanel.buffer, x1, y1, fontsize, c2);
}


void libdraw_drawframe(gamestate* const g) {
    double start_time = GetTime();

    BeginDrawing();
    BeginTextureMode(target);
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);

    libdraw_draw_dungeon_floor(g);

    //libdraw_draw_sprite_and_shadow(g, g->hero_id);
    EndMode2D();
    EndTextureMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);

    if (g->debugpanelon) {
        libdraw_draw_debug_panel(g);
    }

    EndDrawing();

    double elapsed_time = GetTime() - start_time;
    g->last_frame_time = elapsed_time;
    g->framecount++;
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
    //const int scale = 1;
    //Rectangle new_dest = (Rectangle){sg->dest.x, sg->dest.y, scale * sg->dest.width, scale * sg->dest.height};
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
    DrawTexturePro(*s->texture, s->src, dest, zero_vector, 0, WHITE);
}


void libdraw_close() {
    //g = NULL;
    libdraw_unload_textures();
    CloseWindow();
}


bool libdraw_windowshouldclose() {
    return WindowShouldClose();
}


void libdraw_load_texture(const int txkey,
                          const int contexts,
                          const int frames,
                          const bool do_dither,
                          const char* path) {

    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_load_texture: txkey out of bounds");
        return;
    } else if (contexts < 0) {
        merror("libdraw_load_texture: contexts out of bounds");
        return;
    } else if (frames < 0) {
        merror("libdraw_load_texture: frames out of bounds");
        return;
    } else if (txinfo[txkey].texture.id > 0) {
        merror("libdraw_load_texture: texture already loaded");
        return;
    } else if (strlen(path) == 0) {
        merror("libdraw_load_texture: path is empty");
        return;
    } else if (strcmp(path, "\n") == 0) {
        merror("libdraw_load_texture: path is newline");
        return;
    }

    Image image = LoadImage(path);
    if (do_dither) ImageDither(&image, 4, 4, 4, 4);
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
        int txkey = -1, contexts = -1, frames = -1, do_dither = 0;
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
    sprite* s = spritegroup_get(group, 0);
    if (!s) {
        merror("libdraw_create_spritegroup: sprite is NULL");
        spritegroup_destroy(group);
        return;
    }
    const float w = s->width, h = s->height, dst_x = e->x * DEFAULT_TILE_SIZE, dst_y = e->y * DEFAULT_TILE_SIZE;
    group->current = 0;
    group->dest = (Rectangle){dst_x + offset_x, dst_y + offset_y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    msuccessint("inserting spritegroup for entity", id);
    hashtable_entityid_spritegroup_insert(spritegroups, id, group);
    msuccessint("Spritegroup created for entity", id);
}


void libdraw_calc_debugpanel_size(gamestate* const g) {
    if (!g) {
        merror("libdraw_calc_debugpanel_size: gamestate is NULL");
        return;
    }
    const Vector2 size = MeasureTextEx(GetFontDefault(), g->debugpanel.buffer, g->debugpanel.font_size, 1);
    g->debugpanel.w = size.x;
    g->debugpanel.h = size.y;
}


void libdraw_update_sprite_context(gamestate* const g, entityid id, direction_t dir) {
    if (!g) {
        merror("libdraw_update_sprite_context: gamestate is NULL");
        return;
    }

    minfoint2("libdraw_update_sprite_context: updating sprite context for entity", id, dir);


    spritegroup_t* group = hashtable_entityid_spritegroup_get(spritegroups, id); // Adjusted for no specifier
    if (!group) {
        merrorint("libdraw_update_sprite_context: group not found", id);
        return;
    }

    const int old_ctx = group->sprites[group->current]->currentcontext;
    int ctx = old_ctx;
    ctx = dir == DIRECTION_NONE                                      ? old_ctx
          : dir == DIRECTION_DOWN_RIGHT                              ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_DOWN_LEFT                               ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_UP_RIGHT                                ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_UP_LEFT                                 ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_DOWN && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_R_D    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_L_D    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_R_U    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_UP && ctx == SPRITEGROUP_CONTEXT_L_U    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIRECTION_LEFT && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_U
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

    default:
        merrorint("libdraw_create_sg_byid: unknown race", e->race);
        return;
    }

    libdraw_create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
}
