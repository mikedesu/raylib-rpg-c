#include "liblogic.h"
#include "controlmode.h"
#include "em.h"
#include "entity.h"
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

    // init the dungeon floor

    g->dungeon_floor = dungeon_floor_create(DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    if (!g->dungeon_floor) {
        merror("liblogic_init: failed to init dungeon floor");
        return;
    }
    dungeon_floor_init(g->dungeon_floor);


    g->entitymap = em_new();

    const int x = 0;
    const int y = 0;
    entityid hero_id = liblogic_entity_create(g, ENTITY_PLAYER, x, y, "hero");
    if (hero_id != -1) {
        g->hero_id = hero_id;
        msuccessint("Logic Init! Hero ID: ", g->hero_id);
    } else
        merror("Logic Init: failed to init hero");
}


void liblogic_handle_input(const inputstate* const is, gamestate* const g) {
    if (g->controlmode == CONTROLMODE_PLAYER) {
        liblogic_handle_input_player(is, g);
    } else if (g->controlmode == CONTROLMODE_CAMERA) {
        liblogic_handle_input_camera(is, g);
    } else {
        merror("Unknown control mode");
    }
}


void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g) {

    //minfo("Handling camera input");

    if (!is) {
        merror("Input state is NULL!");
        return;
    }

    if (!g) {
        merror("Game state is NULL!");
        return;
    }

    const float move = 2.0f;

    if (inputstate_is_held(is, KEY_RIGHT)) {
        // move camera right
        g->cam2d.offset.x += move;
    }
    if (inputstate_is_held(is, KEY_LEFT)) {
        // move camera left
        g->cam2d.offset.x -= move;
    }
    if (inputstate_is_held(is, KEY_UP)) {
        // move camera up
        g->cam2d.offset.y -= move;
    }
    if (inputstate_is_held(is, KEY_DOWN)) {
        // move camera down
        g->cam2d.offset.y += move;
    }

    if (inputstate_is_pressed(is, KEY_C)) {
        msuccess("C pressed!");
        g->controlmode = CONTROLMODE_PLAYER;
    }

#define DEFAULT_ZOOM_INCR 0.1f
    if (inputstate_is_held(is, KEY_Z)) {
        msuccess("Z held!");
        g->cam2d.zoom += DEFAULT_ZOOM_INCR;
    } else if (inputstate_is_held(is, KEY_X)) {
        msuccess("X held!");
        g->cam2d.zoom -= DEFAULT_ZOOM_INCR;
    } else if (inputstate_is_pressed(is, KEY_V)) {
        msuccess("V pressed!");
        // we want to round up to the next nearest integer value
        // we can use math.h roundf() function
        g->cam2d.zoom = roundf(g->cam2d.zoom);
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

    entity* e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        merror("Hero not found!");
        return;
    }

    if (inputstate_is_pressed(is, KEY_RIGHT)) {
        e->sprite_move_x = DEFAULT_TILE_SIZE;
        entity_incr_x(e);
    } else if (inputstate_is_pressed(is, KEY_LEFT)) {
        e->sprite_move_x = -DEFAULT_TILE_SIZE;
        entity_decr_x(e);
    } else if (inputstate_is_pressed(is, KEY_UP)) {
        e->sprite_move_y = -DEFAULT_TILE_SIZE;
        entity_decr_y(e);
    } else if (inputstate_is_pressed(is, KEY_DOWN)) {
        e->sprite_move_y = DEFAULT_TILE_SIZE;
        entity_incr_y(e);
    } else if (inputstate_is_pressed(is, KEY_SPACE)) {
        msuccess("Space pressed!");
    } else if (inputstate_is_pressed(is, KEY_ENTER)) {
        msuccess("Enter pressed!");
    } else if (inputstate_is_pressed(is, KEY_C)) {
        msuccess("C pressed!");
        g->controlmode = CONTROLMODE_CAMERA;
    }
}

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

    snprintf(g->debugpanel.buffer,
             sizeof(g->debugpanel.buffer),
             "%s\n"
             "%s\n"
             "Frame count: %d\n"
             "Frame draw time: %.02f ms\n"
             "Cam: (%.02f, %.02f)\n"
             "Zoom: %.02f\n"
             "Control mode: %s\n",

             g->timebeganbuf,
             g->currenttimebuf,
             g->framecount,
             g->last_frame_time * 1000,
             g->cam2d.offset.x,
             g->cam2d.offset.y,
             g->cam2d.zoom,
             g->controlmode == CONTROLMODE_PLAYER   ? "Player"
             : g->controlmode == CONTROLMODE_CAMERA ? "Camera"
                                                    : "Unknown");
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
    if (g->index_entityids >= g->max_entityids) {
        int new_max = g->max_entityids * 2;
        entityid* new_ids = realloc(g->entityids, new_max * sizeof(entityid));
        if (!new_ids) {
            merror("liblogic_add_entityid: realloc failed");
            return;
        }
        g->entityids = new_ids;
        g->max_entityids = new_max;
        msuccessint("Expanded entityids to ", new_max);
    }
    g->entityids[g->index_entityids++] = id;
    msuccessint("Added entity ID: ", id);
}


entityid liblogic_entity_create(gamestate* const g, entitytype_t type, int x, int y, const char* name) {
    if (!g || !g->entitymap) {
        merror("liblogic_entity_create: gamestate or entitymap is NULL");
        return -1;
    }
    if (!name || !name[0]) {
        merror("liblogic_entity_create: name is NULL or empty");
        return -1;
    }
    entity* e = entity_new_at(next_entityid++, type, x, y); // Assuming entity_new_at takes name next
    if (!e) {
        merror("liblogic_entity_create: failed to create entity");
        return -1;
    }
    // Assuming entity struct has a name field—copy it
    bzero(e->name, ENTITY_NAME_LEN_MAX); // Clear name field
    strncpy(e->name, name, ENTITY_NAME_LEN_MAX - 1); // Adjust ENTITY_NAME_MAXLEN if defined elsewhere
    e->name[ENTITY_NAME_LEN_MAX - 1] = '\0'; // Ensure null-terminated
    em_add(g->entitymap, e);
    liblogic_add_entityid(g, e->id);
    msuccessint2("Created entity at", x, y);
    return e->id;
}
