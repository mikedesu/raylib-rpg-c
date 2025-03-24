#pragma once
#include "gamestate.h"
#include "inputstate.h"
#include "specifier.h"
#include "spritegroup.h"
#include <raylib.h>
#include <stdbool.h>

extern Shader shader_grayscale; // Global shader
extern Shader shader_tile_glow; // Global shader

void libdraw_init(gamestate* const g);
void libdraw_update_input(inputstate* const is);
void libdraw_drawframe(gamestate* const g);
void libdraw_close();
bool libdraw_windowshouldclose();

bool libdraw_load_texture(int txkey, int ctxs, int frames, bool do_dither, char* path);
bool libdraw_unload_texture(int txkey);

void libdraw_load_textures();
void libdraw_unload_textures();

// may or may not deprecate specifier_t
void libdraw_create_spritegroup(gamestate* const g,
                                entityid id,
                                int* keys,
                                int num_keys,
                                int offset_x,
                                int offset_y,
                                specifier_t spec);

void libdraw_update_sprites(gamestate* const g);
void libdraw_update_sprite(gamestate* const g, entityid id);
void libdraw_draw_sprite(const gamestate* const g, const entityid id);
void libdraw_draw_sprite_and_shadow(const gamestate* const g, entityid id);
void libdraw_draw_dungeon_floor(const gamestate* const g);

bool libdraw_draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, int x, int y);

void libdraw_draw_debug_panel(gamestate* const g);
void libdraw_calc_debugpanel_size(gamestate* const g);
void libdraw_update_sprite_context(gamestate* const g, entityid id, direction_t dir);
void libdraw_create_sg_byid(gamestate* const g, entityid id);
bool libdraw_check_default_animations(gamestate* const g);
void libdraw_draw_hud(gamestate* const g);

void libdraw_load_shaders(); // Load shader
void libdraw_unload_shaders(); // Cleanup
void libdraw_camera_lock_on(gamestate* const g);
void libdraw_handle_frame_incr(gamestate* const g, spritegroup_t* const sg);
void libdraw_update_sprite_position(spritegroup_t* sg, entity_t* e);
void libdraw_update_sprite_attack(entity_t* e, spritegroup_t* sg);
