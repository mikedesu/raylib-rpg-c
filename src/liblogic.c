#include "liblogic.h"
#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "em.h"
#include "entity.h"
#include "gamestate.h"
#include "libgame_defines.h"
#include "mprint.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static entityid next_entityid = 0; // Start at 0, increment for each new entity

void liblogic_init(gamestate* const g) {
    if (!g) {
        merror("liblogic_init: gamestate is NULL");
        return;
    }
    // init the dungeon and dungeon floor
    g->dungeon = dungeon_create();
    if (!g->dungeon) {
        merror("liblogic_init: failed to init dungeon");
        return;
    }

    dungeon_add_floor(g->dungeon, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);

    gamestate_init_entityids(g);

    g->entitymap = em_new();

    if (liblogic_player_create(g, RACE_HUMAN, 1, 1, 0, "hero") == -1) {
        merror("liblogic_init: failed to init hero");
    }
    if (liblogic_npc_create(g, RACE_ORC, 2, 2, 0, "orc") == -1) {
        merror("liblogic_init: failed to create orc");
    }
    liblogic_update_debug_panel_buffer(g);
}


void liblogic_handle_input(const inputstate* const is, gamestate* const g) {
    if (inputstate_is_pressed(is, KEY_D)) {
        msuccess("D pressed!");
        g->debugpanelon = !g->debugpanelon;
    }

    if (g->controlmode == CONTROLMODE_PLAYER) {
        liblogic_handle_input_player(is, g);
    } else if (g->controlmode == CONTROLMODE_CAMERA) {
        liblogic_handle_input_camera(is, g);
    } else {
        merror("Unknown control mode");
    }
}


#define DEFAULT_ZOOM_INCR 0.1f
void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g) {
    //minfo("Handling camera input");
    const float move = 2.0f;
    if (!is) {
        merror("Input state is NULL!");
        return;
    }

    if (!g) {
        merror("Game state is NULL!");
        return;
    }

    if (inputstate_is_held(is, KEY_RIGHT)) {
        g->cam2d.offset.x += move;
        return;
    }

    if (inputstate_is_held(is, KEY_LEFT)) {
        g->cam2d.offset.x -= move;
        return;
    }

    if (inputstate_is_held(is, KEY_UP)) {
        g->cam2d.offset.y -= move;
        return;
    }

    if (inputstate_is_held(is, KEY_DOWN)) {
        g->cam2d.offset.y += move;
        return;
    }

    if (inputstate_is_pressed(is, KEY_C)) {
        msuccess("C pressed!");
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }

    if (inputstate_is_held(is, KEY_Z)) {
        msuccess("Z held!");
        g->cam2d.zoom += DEFAULT_ZOOM_INCR;
        return;
    } else if (inputstate_is_held(is, KEY_X)) {
        msuccess("X held!");
        g->cam2d.zoom -= DEFAULT_ZOOM_INCR;
        return;
    } else if (inputstate_is_pressed(is, KEY_V)) {
        msuccess("V pressed!");
        // we want to round up to the next nearest integer value
        // we can use math.h roundf() function
        g->cam2d.zoom = roundf(g->cam2d.zoom);
        return;
    }
}


void liblogic_handle_input_player(const inputstate* const is, gamestate* const g) {
    if (!is) {
        merror("Input state is NULL!");
        return;
    }
    if (!g) {
        merror("Game state is NULL!");
        return;
    }
    entity* const e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        //merror("Hero not found!");
        return;
    }
    if (inputstate_is_pressed(is, KEY_RIGHT)) {
        minfo("Right pressed!");
        //e->do_update = true;
        liblogic_try_entity_move(g, e, 1, 0);
    } else if (inputstate_is_pressed(is, KEY_LEFT)) {
        minfo("left  pressed!");
        //e->do_update = true;
        liblogic_try_entity_move(g, e, -1, 0);
    } else if (inputstate_is_pressed(is, KEY_UP)) {
        minfo("up pressed!");
        //e->do_update = true;
        liblogic_try_entity_move(g, e, 0, -1);
    } else if (inputstate_is_pressed(is, KEY_DOWN)) {
        minfo("down pressed!");
        //e->do_update = true;
        liblogic_try_entity_move(g, e, 0, 1);
    } else if (inputstate_is_pressed(is, KEY_SPACE)) {
        msuccess("Space pressed!");
    } else if (inputstate_is_pressed(is, KEY_ENTER)) {
        msuccess("Enter pressed!");
    } else if (inputstate_is_pressed(is, KEY_C)) {
        msuccess("C pressed!");
        g->controlmode = CONTROLMODE_CAMERA;
    }
}


static inline const direction_t liblogic_get_dir_from_xy(const int x, const int y) {
    if (x == 0 && y == 0) return DIRECTION_NONE;
    if (x == 0 && y == -1) return DIRECTION_UP;
    if (x == 0 && y == 1) return DIRECTION_DOWN;
    if (x == -1 && y == 0) return DIRECTION_LEFT;
    if (x == 1 && y == 0) return DIRECTION_RIGHT;
    return DIRECTION_NONE;
}


void liblogic_try_entity_move(gamestate* const g, entity* const e, int x, int y) {
    if (!g || !e) {
        merror(!g ? "Game state is NULL!" : "Entity is NULL!");
        return;
    }

    e->do_update = true;
    e->direction = liblogic_get_dir_from_xy(x, y);

    const int ex = e->x + x, ey = e->y + y, floor = e->floor;

    dungeon_floor_t* df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("Failed to get dungeon floor");
        return;
    }

    dungeon_tile_t* tile = dungeon_floor_tile_at(df, ex, ey);
    if (!tile || ex < 0 || ey < 0) {
        merror(!tile ? "Failed to get tile" : "Cannot move, out of bounds");
        return;
    }

    if (dungeon_tile_is_wall(tile->type)) {
        merror("Cannot move, wall");
        return;
    }

    if (liblogic_tile_npc_count(g, ex, ey, floor) > 0) {
        merror("Cannot move, NPC in the way");
        return;
    }

    dungeon_floor_remove_at(df, e->id, e->x, e->y);
    dungeon_floor_add_at(df, e->id, ex, ey);
    e->x = ex;
    e->y = ey;
    e->sprite_move_x = x * DEFAULT_TILE_SIZE;
    e->sprite_move_y = y * DEFAULT_TILE_SIZE;
}


const int liblogic_tile_npc_count(const gamestate* const g, const int x, const int y, const int floor) {
    if (!g) {
        merror("liblogic_tile_npc_count: gamestate is NULL");
        return -1;
    }

    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("liblogic_tile_npc_count: failed to get dungeon floor");
        return -1;
    }

    const dungeon_tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("liblogic_tile_npc_count: failed to get tile");
        return -1;
    }

    // enumerate entities and check their type
    int count = 0;
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == -1) {
            continue;
        }
        const entity* const e = em_get(g->entitymap, tile->entities[i]);
        if (!e) {
            merror("liblogic_tile_npc_count: failed to get entity");
            return -1;
        }
        if (e->type == ENTITY_NPC) {
            count++;
        }
    }
    return count;
}


//void liblogic_try_entity_move_left(gamestate* const g, entity* const e) {
//    liblogic_try_entity_move(g, e, -1, 0);
//}


//void liblogic_try_entity_move_right(gamestate* const g, entity* const e) {
//    liblogic_try_entity_move(g, e, 1, 0);
//}


//void liblogic_try_entity_move_up(gamestate* const g, entity* const e) {
//    liblogic_try_entity_move(g, e, 0, -1);
//}


//void liblogic_try_entity_move_down(gamestate* const g, entity* const e) {
//    liblogic_try_entity_move(g, e, 0, 1);
//}


void liblogic_tick(const inputstate* const is, gamestate* const g) {
    if (!is) {
        merror("Input state is NULL!");
        return;
    }
    if (!g) {
        merror("Game state is NULL!");
        return;
    }
    liblogic_handle_input(is, g);

    liblogic_update_debug_panel_buffer(g);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    //g->currenttimebuf = strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}


void liblogic_update_debug_panel_buffer(gamestate* const g) {
    if (!g) {
        merror("Game state is NULL!");
        return;
    }

    // grab a pointer to the hero
    entity* e = em_get(g->entitymap, g->hero_id);
    int x = -1;
    int y = -1;
    if (e) {
        x = e->x;
        y = e->y;
    }


    snprintf(g->debugpanel.buffer,
             sizeof(g->debugpanel.buffer),
             "%s\n"
             "%s\n"
             "Frame count: %d\n"
             "Frame draw time: %.02f ms\n"
             "debug font size: %d\n"
             "Cam: (%.02f, %.02f)\n"
             "Zoom: %.02f\n"
             "Control mode: %s\n"
             "Current floor: %d\n"
             "Dungeon num floors: %d\n"
             "Num entityids: %d\n"
             "Hero: (%d, %d)\n",
             g->timebeganbuf,
             g->currenttimebuf,
             g->framecount,
             g->last_frame_time * 1000,
             g->debugpanel.font_size,
             g->cam2d.offset.x,
             g->cam2d.offset.y,
             g->cam2d.zoom,
             g->controlmode == CONTROLMODE_PLAYER   ? "Player"
             : g->controlmode == CONTROLMODE_CAMERA ? "Camera"
                                                    : "Unknown",
             g->dungeon->current_floor,
             g->dungeon->num_floors,
             g->index_entityids,
             x,
             y);
}


void liblogic_close(gamestate* const g) {
    if (!g) {
        merror("liblogic_close: gamestate is NULL");
        return;
    }
    if (g->entitymap) {
        em_free(g->entitymap);
        g->entitymap = NULL;
    }
    msuccess("Logic Close!");
}


void liblogic_add_entityid(gamestate* const g, entityid id) {
    if (!g) {
        merror("liblogic_add_entityid: gamestate is NULL");
        return;
    }
    //if (g->index_entityids >= g->max_entityids) {
    //    int new_max = g->max_entityids * 2;
    //    entityid* new_ids = realloc(g->entityids, new_max * sizeof(entityid));
    //    if (!new_ids) {
    //        merror("liblogic_add_entityid: realloc failed");
    //        return;
    //    }
    //    g->entityids = new_ids;
    //    g->max_entityids = new_max;
    //    msuccessint("Expanded entityids to ", new_max);
    //}
    // g->entityids[g->index_entityids++] = id;
    gamestate_add_entityid(g, id);
    msuccessint("Added entity ID: ", id);
}


//entityid liblogic_entity_create(gamestate* const g, entitytype_t type, int x, int y, const char* name) {
const entityid liblogic_npc_create(gamestate* const g,
                                   const race_t race_type,
                                   const int x,
                                   const int y,
                                   const int floor,
                                   const char* name) {

    if (!g) {
        merror("liblogic_entity_create: gamestate is NULL");
        return -1;
    }

    em_t* em = gamestate_get_entitymap(g);
    if (!em) {
        merror("liblogic_entity_create: em is NULL");
        return -1;
    }

    if (!name || !name[0]) {
        merror("liblogic_entity_create: name is NULL or empty");
        return -1;
    }

    // check type
    if (race_type < 0 || race_type >= RACE_COUNT) {
        merror("liblogic_entity_create: race_type is out of bounds");
        return -1;
    }

    // check x and y
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("liblogic_entity_create: failed to get current dungeon floor");
        return -1;
    }

    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merror("liblogic_entity_create: x or y is out of bounds");
        return -1;
    }

    // can we create an entity at this location? no entities can be made on wall-types etc
    dungeon_tile_t* tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("liblogic_entity_create: failed to get tile");
        return -1;
    }
    if (tile->type == DUNGEON_TILE_TYPE_STONE_WALL_00) {
        merror("liblogic_entity_create: cannot create entity on stone wall 00");
        return -1;
    }
    if (tile->type == DUNGEON_TILE_TYPE_STONE_WALL_01) {
        merror("liblogic_entity_create: cannot create entity on stone wall 01");
        return -1;
    }
    if (tile->type == DUNGEON_TILE_TYPE_STONE_WALL_02) {
        merror("liblogic_entity_create: cannot create entity on stone wall 02");
        return -1;
    }

    entity* e = entity_new_npc_at(next_entityid++,
                                  race_type,
                                  x,
                                  y,
                                  floor,
                                  name); // Assuming entity_new_at takes name next
    if (!e) {
        merror("liblogic_entity_create: failed to create entity");
        return -1;
    }
    em_add(em, e);
    gamestate_add_entityid(g, e->id);
    dungeon_floor_add_at(df, e->id, x, y);
    msuccessint2("Created entity at", x, y);
    return e->id;
}


const entityid liblogic_player_create(gamestate* const g,
                                      const race_t race_type,
                                      const int x,
                                      const int y,
                                      const int floor,
                                      const char* name) {

    if (!g) {
        merror("liblogic_player_create: gamestate is NULL");
        return -1;
    }

    // use the previously-written liblogic_npc_create function
    entityid id = liblogic_npc_create(g, race_type, x, y, floor, name);
    entity_set_type(em_get(gamestate_get_entitymap(g), id), ENTITY_PLAYER);
    gamestate_set_hero_id(g, id);
    return id;
}
