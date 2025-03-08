#include "libdraw.h"
#include "gamestate.h"
#include "hashtable_entityid_spritegroup.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "spritegroup.h"
#include "textureinfo.h"
#include "tx_keys.h"

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

gamestate* g = NULL;

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 480
#define DEFAULT_TILE_SIZE 8
#define SPRITEGROUP_DEFAULT_SIZE 32


void libdraw_init(gamestate* const g) {
    if (g) {
        const char* title = "evildojo666";
        const int window_width = DEFAULT_WINDOW_WIDTH;
        const int window_height = DEFAULT_WINDOW_HEIGHT;
        InitWindow(window_width, window_height, title);
        SetTargetFPS(60);

        spritegroups = hashtable_entityid_spritegroup_create(4);

        libdraw_load_textures();


        libdraw_create_spritegroup(g, g->hero_id, TX_HUMAN_KEYS, TX_HUMAN_KEY_COUNT, -12, -12, SPECIFIER_NONE);

        msuccess("libdraw_init");
    } else {
        merror("libdraw_init g is NULL");
    }
}


void libdraw_update_input(inputstate* const is) {
    inputstate_update(is);
}


void libdraw_drawframe(gamestate* const g) {

    double start_time = GetTime();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    char buffer[1024] = {0};
    snprintf(buffer, sizeof(buffer), "Frame draw time: %.02f ms\n", g->last_frame_time * 1000);
    DrawText(buffer, 10, 10, 20, MAROON);

    spritegroup_t* hero_sg = hashtable_entityid_spritegroup_get(spritegroups, g->hero_id);
    if (hero_sg) {
        msuccess("spritegroup found");
        hero_sg->dest.width *= 1.01f;
        hero_sg->dest.height *= 1.01f;
        sprite* s = spritegroup_get(hero_sg, hero_sg->current);
        if (s) {
            //snprintf(buffer, sizeof(buffer), "Hero x/y/w/h: %d/%d/%d/%d\n", hero_sg->dest.x, hero_sg->dest.y, hero_sg->dest.width, hero_sg->dest.height);
            snprintf(buffer,
                     sizeof(buffer),
                     "Hero x/y/w/h: %0.2f/%0.2f/%0.2f/%0.2f\n",
                     hero_sg->dest.x,
                     hero_sg->dest.y,
                     hero_sg->dest.width,
                     hero_sg->dest.height);
            DrawText(buffer, 10, 40, 20, MAROON);
            DrawTexturePro(*s->texture, s->src, hero_sg->dest, (Vector2){hero_sg->off_x, hero_sg->off_y}, 0, WHITE);
            //DrawTexturePro(*s->texture, s->src, hero_sg->dest, (Vector2){0, 0}, 0, WHITE);
        } else {
            merror("sprite not found");
        }
    }

    EndDrawing();

    double end_time = GetTime();

    double elapsed_time = end_time - start_time;
    g->last_frame_time = elapsed_time;
}


void libdraw_close() {
    g = NULL;
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
    } else if (contexts < 0) {
        merror("libdraw_load_texture: contexts out of bounds");
    } else if (frames < 0) {
        merror("libdraw_load_texture: frames out of bounds");
    } else if (txinfo[txkey].texture.id > 0) {
        merror("libdraw_load_texture: texture already loaded");
    } else if (strlen(path) == 0) {
        merror("libdraw_load_texture: path is empty");
    } else if (strcmp(path, "\n") == 0) {
        merror("libdraw_load_texture: path is newline");
    } else {
        Image image = LoadImage(path);
        if (do_dither) {
            ImageDither(&image, 8, 8, 8, 8);
        }
        Texture2D texture = LoadTextureFromImage(image);
        UnloadImage(image);
        txinfo[txkey].texture = texture;
        txinfo[txkey].contexts = contexts;
        txinfo[txkey].num_frames = frames;
        msuccess("libdraw_load_texture: texture loaded successfully");
    }
}


void libdraw_unload_texture(const int txkey) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_unload_texture: txkey out of bounds");
    } else {
        UnloadTexture(txinfo[txkey].texture);
        txinfo[txkey].texture = (Texture2D){0};
        txinfo[txkey].contexts = 0;
        msuccess("libdraw_unload_texture: texture unloaded successfully");
    }
}


void libdraw_load_textures() {
    const char* textures_file = "textures.txt";
    FILE* file = fopen(textures_file, "r");
    if (file) {
        char line[1024] = {0};
        while (fgets(line, sizeof(line), file)) {
            int txkey = -1;
            int contexts = -1;
            int frames = -1;
            int do_dither = 0;
            char path[512] = {0};
            // check if the line begins with a #
            if (line[0] == '#') {
                minfo("libdraw_load_textures: skipping comment line");
                continue;
            }

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
    } else {
        merror("libdraw_load_textures: could not open textures.txt");
    }
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
        //sprite_width = tex->width / 4; // 4 frames
        //sprite_height = tex->height / 4; // 4 directions
        //sprite_width = tex->width; // 4 frames
        //sprite_height = tex->height; // 4 directions
        //minfo("got width/height");
        //minfoint("sprite_width", sprite_width);
        //minfoint("sprite_height", sprite_height);
        //minfoint("contexts", txinfo[k].contexts);
        //minfoint("num_frames", txinfo[k].num_frames);

        sprite* s = sprite_create(tex, txinfo[k].contexts, txinfo[k].num_frames);
        minfo("created sprite");
        //s->src = (Rectangle){k * sprite_width, 0, sprite_width, sprite_height}; // Row 0 (up)
        //minfo("set src");
        spritegroup_add(group, s);
        minfo("added sprite");
    }
    minfo("loop done");
    //group->dest = (Rectangle){
    //    e->x * DEFAULT_TILE_SIZE + offset_x, e->y * DEFAULT_TILE_SIZE + offset_y, sprite_width, sprite_height};
    //group->off_x = offset_x;
    //group->off_y = offset_y;
    //minfo("set dest");
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
    //    spritegroup_destroy(group); // Hashtable copies it
}
