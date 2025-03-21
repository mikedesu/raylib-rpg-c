#include "liblogic.h"
#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "em.h"
#include "entity.h"
#include "entitytype.h"
#include "gamestate.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "race.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_ZOOM_INCR 0.01f

static entityid next_entityid = 0; // Start at 0, increment for each new entity


static inline const direction_t liblogic_get_dir_from_xy(const int x, const int y) {
    if (x == 0 && y == 0) return DIRECTION_NONE;
    if (x == 0 && y == -1) return DIRECTION_UP;
    if (x == 0 && y == 1) return DIRECTION_DOWN;
    if (x == -1 && y == 0) return DIRECTION_LEFT;
    if (x == 1 && y == 0) return DIRECTION_RIGHT;
    // also handle diagonals
    if (x == -1 && y == -1) return DIRECTION_UP_LEFT;
    if (x == 1 && y == -1) return DIRECTION_UP_RIGHT;
    if (x == -1 && y == 1) return DIRECTION_DOWN_LEFT;
    if (x == 1 && y == 1) return DIRECTION_DOWN_RIGHT;
    return DIRECTION_NONE;
}


static inline int liblogic_get_x_from_dir(const direction_t dir) {
    switch (dir) {
    case DIRECTION_UP_LEFT:
    case DIRECTION_DOWN_LEFT:
    case DIRECTION_LEFT:
        return -1;
    case DIRECTION_UP_RIGHT:
    case DIRECTION_DOWN_RIGHT:
    case DIRECTION_RIGHT:
        return 1;
    default:
        return 0;
    }
}


static inline int liblogic_get_y_from_dir(const direction_t dir) {
    switch (dir) {
    case DIRECTION_UP:
    case DIRECTION_UP_LEFT:
    case DIRECTION_UP_RIGHT:
        return -1;
    case DIRECTION_DOWN:
    case DIRECTION_DOWN_LEFT:
    case DIRECTION_DOWN_RIGHT:
        return 1;
    default:
        return 0;
    }
}


// liblogic.c (add this function)
static void liblogic_execute_action(gamestate* const g, entity* e, entity_action_t action) {
    switch (action) {
    case ENTITY_ACTION_MOVE_LEFT:
        liblogic_try_entity_move(g, e, -1, 0);
        break;
    case ENTITY_ACTION_MOVE_RIGHT:
        liblogic_try_entity_move(g, e, 1, 0);
        break;
    case ENTITY_ACTION_MOVE_UP:
        liblogic_try_entity_move(g, e, 0, -1);
        break;
    case ENTITY_ACTION_MOVE_DOWN:
        liblogic_try_entity_move(g, e, 0, 1);
        break;
    case ENTITY_ACTION_MOVE_UP_LEFT:
        liblogic_try_entity_move(g, e, -1, -1);
        break;
    case ENTITY_ACTION_MOVE_UP_RIGHT:
        liblogic_try_entity_move(g, e, 1, -1);
        break;
    case ENTITY_ACTION_MOVE_DOWN_LEFT:
        liblogic_try_entity_move(g, e, -1, 1);
        break;
    case ENTITY_ACTION_MOVE_DOWN_RIGHT:
        liblogic_try_entity_move(g, e, 1, 1);
        break;
    case ENTITY_ACTION_ATTACK_LEFT:
        liblogic_try_entity_attack(g, e->id, e->x - 1, e->y);
        break;
    case ENTITY_ACTION_ATTACK_RIGHT:
        liblogic_try_entity_attack(g, e->id, e->x + 1, e->y);
        break;
    case ENTITY_ACTION_ATTACK_UP:
        liblogic_try_entity_attack(g, e->id, e->x, e->y - 1);
        break;
    case ENTITY_ACTION_ATTACK_DOWN:
        liblogic_try_entity_attack(g, e->id, e->x, e->y + 1);
        break;
    case ENTITY_ACTION_ATTACK_UP_LEFT:
        liblogic_try_entity_attack(g, e->id, e->x - 1, e->y - 1);
        break;
    case ENTITY_ACTION_ATTACK_UP_RIGHT:
        liblogic_try_entity_attack(g, e->id, e->x + 1, e->y - 1);
        break;
    case ENTITY_ACTION_ATTACK_DOWN_LEFT:
        liblogic_try_entity_attack(g, e->id, e->x - 1, e->y + 1);
        break;
    case ENTITY_ACTION_ATTACK_DOWN_RIGHT:
        liblogic_try_entity_attack(g, e->id, e->x + 1, e->y + 1);
        break;
    case ENTITY_ACTION_MOVE_RANDOM: {
        int rx = rand() % 3 - 1; // -1, 0, 1
        int ry = rand() % 3 - 1;
        if (rx != 0 || ry != 0) liblogic_try_entity_move(g, e, rx, ry);
        break;
    }
    case ENTITY_ACTION_ATTACK_RANDOM: {
        int rx = rand() % 3 - 1;
        int ry = rand() % 3 - 1;
        if (rx != 0 || ry != 0) liblogic_try_entity_attack(g, e->id, e->x + rx, e->y + ry);
        break;
    }
    case ENTITY_ACTION_MOVE_PLAYER: {
        entity* hero = em_get(g->entitymap, g->hero_id);
        if (hero) {
            int dx = (hero->x > e->x) ? 1 : (hero->x < e->x) ? -1 : 0;
            int dy = (hero->y > e->y) ? 1 : (hero->y < e->y) ? -1 : 0;
            if (dx != 0 || dy != 0) liblogic_try_entity_move(g, e, dx, dy);
        }
        break;
    }
    case ENTITY_ACTION_ATTACK_PLAYER: {
        entity* hero = em_get(g->entitymap, g->hero_id);
        if (hero) {
            int dx = hero->x - e->x;
            int dy = hero->y - e->y;
            if (abs(dx) <= 1 && abs(dy) <= 1) // Adjacent check
                liblogic_try_entity_attack(g, e->id, hero->x, hero->y);
        }
        break;
    }
    case ENTITY_ACTION_WAIT:
        // Do nothing for now (future healing logic can go here)
        break;
    default:
        //merror("Unknown entity action: %d", action);
        break;
    }
}


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

    int herox = 7;
    int heroy = 2;
    if (liblogic_player_create(g, RACE_HUMAN, herox, heroy, 0, "hero") == -1) {
        merror("liblogic_init: failed to init hero");
    }
    g->entity_turn = g->hero_id;

    // create some orcs with names
    //entityid orc0 = liblogic_npc_create(g, RACE_ORC, 8, 2, 0, "orc-mover");
    //entityid orc1 = liblogic_npc_create(g, RACE_ORC, 8, 3, 0, "orc-attacker");
    //entityid orc2 = liblogic_npc_create(g, RACE_ORC, 8, 4, 0, "orc-mover");

    int orc_basex = 8;
    int orc_basey = 2;
    //int rows = 10;
    //int cols = 10;
    //int rows = 34;
    //int cols = 30;
    int count = 0;
    int total_orcs_to_make = 2048 + 1024;

    dungeon_t* d = g->dungeon;
    dungeon_floor_t* df = d->floors[0];
    if (!df) {
        merror("liblogic_init: failed to get dungeon floor");
        return;
    }

    for (int y = orc_basey; y < df->height; y += 1) {
        for (int x = orc_basex; x < df->width; x += 1) {
            entityid orc_id = liblogic_npc_create(g, RACE_ORC, orc_basex + x, orc_basey + y, 0, "orc");
            if (orc_id == -1) {
                merror("liblogic_init: failed to init orc");
            }
            entity* const orc = em_get(g->entitymap, orc_id);
            if (orc) {
                //entity_action_t action = ENTITY_ACTION_MOVE_RANDOM;
                entity_action_t action = ENTITY_ACTION_MOVE_PLAYER;
                entity_set_default_action(orc, action);
                entity_set_maxhp(orc, 3);
                entity_set_hp(orc, 3);
                count++;
            }

            if (count >= total_orcs_to_make) {
                break;
            }
        }

        if (count >= total_orcs_to_make) {
            break;
        }
    }

    //entity* const e0 = em_get(g->entitymap, orc0);
    //entity* const e1 = em_get(g->entitymap, orc1);
    //entity* const e2 = em_get(g->entitymap, orc2);

    //entity_set_maxhp(e0, 3);
    //entity_set_maxhp(e1, 3);
    //entity_set_maxhp(e2, 3);

    //entity_set_hp(e0, 3);
    //entity_set_hp(e1, 3);
    //entity_set_hp(e2, 3);


    //entity_action_t action = ENTITY_ACTION_MOVE_PLAYER;

    //entity_set_default_action(e0, action);
    //entity_set_default_action(e1, action);
    //entity_set_default_action(e2, action);


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


void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g) {
    //minfo("Handling camera input");
    const float move = g->cam2d.zoom;

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
        //msuccess("C pressed!");
        g->cam2d.zoom = roundf(g->cam2d.zoom);
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }

    if (inputstate_is_held(is, KEY_Z)) {
        //msuccess("Z held!");
        if (inputstate_is_shift_held(is)) {
            g->cam2d.zoom += DEFAULT_ZOOM_INCR;
        } else {
            g->cam2d.zoom -= DEFAULT_ZOOM_INCR;
        }
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

    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) {
        return;
    }

    entity* const e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        //merror("Hero not found!");
        return;
    }

    const int key = inputstate_get_pressed_key(is);

    switch (key) {
    case KEY_KP_6:
    case KEY_RIGHT:
        minfo("Right pressed!");
        liblogic_try_entity_move(g, e, 1, 0);
        break;
    case KEY_KP_4:
    case KEY_LEFT:
        minfo("left  pressed!");
        liblogic_try_entity_move(g, e, -1, 0);
        break;
    case KEY_KP_8:
    case KEY_UP:
        minfo("up pressed!");
        liblogic_try_entity_move(g, e, 0, -1);
        break;
    case KEY_KP_2:
    case KEY_DOWN:
        minfo("down pressed!");
        liblogic_try_entity_move(g, e, 0, 1);
        break;
    // diagonals on the numpad
    case KEY_KP_7:
        minfo("Numpad 7 pressed!");
        liblogic_try_entity_move(g, e, -1, -1);
        break;
    case KEY_KP_9:
        minfo("Numpad 9 pressed!");
        liblogic_try_entity_move(g, e, 1, -1);
        break;
    case KEY_KP_1:
        minfo("Numpad 1 pressed!");
        liblogic_try_entity_move(g, e, -1, 1);
        break;
    case KEY_KP_3:
        minfo("Numpad 3 pressed!");
        liblogic_try_entity_move(g, e, 1, 1);
        break;
    case KEY_A:
        msuccess("A pressed!");
        int dx = liblogic_get_x_from_dir(e->direction);
        int dy = liblogic_get_y_from_dir(e->direction);
        int tx = e->x + dx;
        int ty = e->y + dy;
        liblogic_try_entity_attack(g, e->id, tx, ty);
        break;
    case KEY_SPACE:
        msuccess("Space pressed!");
        break;
    case KEY_ENTER:
        msuccess("Enter pressed!");
        break;
    case KEY_C:
        msuccess("C pressed!");
        g->controlmode = CONTROLMODE_CAMERA;
        break;
    default:
        break; // Ignore unhandled keys
    }
}


void liblogic_try_entity_move(gamestate* const g, entity* const e, int x, int y) {
    if (!g || !e) {
        merror(!g ? "Game state is NULL!" : "Entity is NULL!");
        return;
    }

    e->do_update = true;
    e->direction = liblogic_get_dir_from_xy(x, y);
    const int ex = e->x + x, ey = e->y + y, floor = e->floor;

    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("Failed to get dungeon floor");
        return;
    }

    // i feel like this might be something we can set elsewhere...like after the player input phase?
    if (e->type == ENTITY_PLAYER) {
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }

    dungeon_tile_t* const tile = dungeon_floor_tile_at(df, ex, ey);
    if (!tile || ex < 0 || ey < 0) {
        merror(!tile ? "Failed to get tile" : "Cannot move, out of bounds");
        return;
    }

    if (dungeon_tile_is_wall(tile->type)) {
        merror("Cannot move, wall");
        return;
    }

    if (liblogic_tile_npc_living_count(g, ex, ey, floor) > 0) {
        merror("Cannot move, NPC in the way");
        return;
    }

    if (liblogic_player_on_tile(g, ex, ey, floor)) {
        merror("Cannot move, player on tile");
        return;
    }

    dungeon_floor_remove_at(df, e->id, e->x, e->y);
    dungeon_floor_add_at(df, e->id, ex, ey);
    e->x = ex, e->y = ey;
    e->sprite_move_x = x * DEFAULT_TILE_SIZE, e->sprite_move_y = y * DEFAULT_TILE_SIZE;
}


const bool liblogic_player_on_tile(const gamestate* const g, const int x, const int y, const int floor) {
    if (!g) {
        merror("liblogic_player_on_tile: gamestate is NULL");
        return false;
    }

    // get the tile at x y
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, 0);
    if (!df) {
        merror("liblogic_player_on_tile: failed to get dungeon floor");
        return false;
    }

    const dungeon_tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("liblogic_player_on_tile: failed to get tile");
        return false;
    }

    // enumerate entities and check their type
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == -1) {
            continue;
        }
        const entity* const e = em_get(g->entitymap, tile->entities[i]);
        if (!e) {
            merror("liblogic_player_on_tile: failed to get entity");
            return false;
        }
        if (e->type == ENTITY_PLAYER) {
            return true;
        }
    }
    return false;
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

    if (g->flag == GAMESTATE_FLAG_NPC_TURN) {
        liblogic_handle_npcs(g);
    }
    //liblogic_handle_npc(g);

    liblogic_update_debug_panel_buffer(g);

    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}


void liblogic_handle_npc(gamestate* const g) {
    if (!g) {
        merror("Game state is NULL!");
        return;
    }

    if (g->flag != GAMESTATE_FLAG_NPC_TURN) {
        return;
    }

    const entityid id = g->entity_turn;
    entity* const e = em_get(g->entitymap, id);
    const int rx = rand() % 3 - 1;
    const int ry = rand() % 3 - 1;

    if (!e) {
        merror("Failed to get entity");
        return;
    }

    if (e->type != ENTITY_NPC) {
        merrorint("Entity is not an NPC. entityid", e->id);
        merrorstr("Entity type", e->type == ENTITY_PLAYER ? "PLAYER" : e->type == ENTITY_NPC ? "NPC" : "UNKNOWN");
        g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
        return;
    }

    liblogic_try_entity_move(g, e, rx, ry);
}


void liblogic_handle_npcs(gamestate* const g) {
    if (!g) {
        merror("Game state is NULL!");
        return;
    }
    if (g->flag != GAMESTATE_FLAG_NPC_TURN) return;
    // Process all NPCs
    for (int i = 0; i < g->index_entityids; i++) {
        entity* e = em_get(g->entitymap, g->entityids[i]);
        if (!e || e->type != ENTITY_NPC || e->floor != 0) {
            continue;
        }

        if (e->is_dead) {
            continue;
        }

        // testing attack logic
        //if (strcmp(e->name, "orc-attacker") == 0) {
        //    liblogic_try_entity_attack(g, e->id, e->x - 1, e->y);
        //} else {
        //    liblogic_try_entity_move(g, e, -1, 0);
        //}
        liblogic_execute_action(g, e, e->default_action);
    }
    // After processing all NPCs, set the flag to animate all movements together
    g->flag = GAMESTATE_FLAG_NPC_ANIM;
}


/*
void liblogic_handle_npcs(gamestate* const g) {
    if (!g || g->flag != GAMESTATE_FLAG_NPC_TURN) return;

    int current_index = gamestate_get_entityid_index(g, g->entity_turn);
    for (int i = current_index; i < g->index_entityids; i++) {
        entity* e = em_get(g->entitymap, g->entityids[i]);
        if (!e || e->type != ENTITY_NPC || e->floor != 0) continue;

        g->entity_turn = e->id; // Track the current NPC

        if (strcmp(e->name, "orc-attacker") == 0) {
            bool attack_connected = liblogic_try_entity_attack(g, e->id, e->x - 1, e->y);
            if (attack_connected) {
                // Set attack animation for this NPC
                e->is_attacking = true;
                e->do_update = true;

                // Find the victim (assume it’s the entity at x-1, y for simplicity)
                entity* victim = liblogic_get_entity_at(g, e->x - 1, e->y);
                if (victim) {
                    victim->is_damaged = true;
                    victim->do_update = true;
                }

                // Switch to animation state and wait
                g->flag = GAMESTATE_FLAG_NPC_ANIM;
                return; // Exit to let animations play
            }
        } else {
            // Non-interactive action (e.g., move left)
            liblogic_try_entity_move(g, e, -1, 0);
        }
    }

    // All NPCs processed, animate remaining actions
    g->flag = GAMESTATE_FLAG_NPC_ANIM;
    g->entity_turn = g->hero_id; // Reset for player turn
}
*/


void liblogic_update_debug_panel_buffer(gamestate* const g) {
    if (!g) {
        merror("Game state is NULL!");
        return;
    }
    // grab a pointer to the hero
    entity* const e = em_get(g->entitymap, g->hero_id);
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
             "flag: %s\n"
             "entity_turn: %d\n"
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
             g->flag == GAMESTATE_FLAG_PLAYER_INPUT  ? "GAMESTATE_FLAG_PLAYER_INPUT"
             : g->flag == GAMESTATE_FLAG_PLAYER_ANIM ? "GAMESTATE_FLAG_PLAYER_ANIM"
             : g->flag == GAMESTATE_FLAG_NONE        ? "GAMESTATE_FLAG_NONE"
             : g->flag == GAMESTATE_FLAG_COUNT       ? "GAMESTATE_FLAG_COUNT"
             : g->flag == GAMESTATE_FLAG_NPC_TURN    ? "GAMESTATE_FLAG_NPC_TURN"
             : g->flag == GAMESTATE_FLAG_NPC_ANIM    ? "GAMESTATE_FLAG_NPC_ANIM"
                                                     : "Unknown",

             g->entity_turn,
             x,
             y);
}


void liblogic_close(gamestate* const g) {
    if (!g) {
        merror("Game state is NULL!");
        return;
    }

    if (!g->entitymap) {
        merror("Entity map is NULL!");
        return;
    }

    em_free(g->entitymap);
    g->entitymap = NULL;

    if (!g->dungeon) {
        merror("Dungeon is NULL!");
        return;
    }

    dungeon_destroy(g->dungeon);
    g->dungeon = NULL;
}


void liblogic_add_entityid(gamestate* const g, entityid id) {
    if (!g) {
        merror("liblogic_add_entityid: gamestate is NULL");
        return;
    }
    gamestate_add_entityid(g, id);
    msuccessint("Added entity ID: ", id);
}


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
    dungeon_tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("liblogic_entity_create: failed to get tile");
        return -1;
    }

    if (dungeon_tile_is_wall(tile->type)) {
        merror("liblogic_entity_create: cannot create entity on wall");
        return -1;
    }

    if (liblogic_tile_npc_count(g, x, y, floor) > 0) {
        merror("liblogic_entity_create: cannot create entity on tile with NPC");
        return -1;
    }

    entity* const e = entity_new_npc_at(next_entityid++,
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
    const entitytype_t type = ENTITY_PLAYER;
    const entityid id = liblogic_npc_create(g, race_type, x, y, floor, name);
    entity_t* const e = em_get(gamestate_get_entitymap(g), id);
    if (!e) {
        merror("liblogic_player_create: failed to get entity");
        return -1;
    }
    entity_set_type(e, type);
    gamestate_set_hero_id(g, id);
    return id;
}


void liblogic_try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y) {
    if (!g) {
        merror("liblogic_try_entity_attack: gamestate is NULL");
        return;
    }
    entity* const attacker = em_get(g->entitymap, attacker_id);
    //if (!attacker) {
    if (!attacker || attacker->is_dead) {
        merror("liblogic_try_entity_attack: attacker entity not found");
        return;
    }
    dungeon_floor_t* const floor = dungeon_get_floor(g->dungeon, attacker->floor);
    if (!floor) {
        merror("liblogic_try_entity_attack: failed to get dungeon floor");
        return;
    }
    dungeon_tile_t* const tile = dungeon_floor_tile_at(floor, target_x, target_y);
    if (!tile) {
        merror("liblogic_try_entity_attack: target tile not found");
        return;
    }

    // Calculate direction based on target position
    int dx = target_x - attacker->x;
    int dy = target_y - attacker->y;
    attacker->direction =
        liblogic_get_dir_from_xy(dx, dy); // Assumes this function exists to convert dx, dy to a direction

    attacker->is_attacking = true;
    attacker->do_update = true;

    bool attack_successful = false;

    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] != -1) {
            entity* const target = em_get(g->entitymap, tile->entities[i]);
            if (target && (target->type == ENTITY_NPC || target->type == ENTITY_PLAYER) && !target->is_dead) {
                // Perform attack logic here
                minfo("Attack successful!");
                attack_successful = true;
                target->is_damaged = true;
                target->do_update = true;

                entity_set_hp(target, entity_get_hp(target) - 1); // Reduce HP by 1

                if (entity_get_hp(target) <= 0) { // Check if target is dead
                    target->is_dead = true;
                }
                break;
            }
        }
    }

    if (!attack_successful)
        merrorint2("liblogic_try_entity_attack: no valid target found at the specified location", target_x, target_y);

    if (attacker->type == ENTITY_PLAYER) {
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    } else if (attacker->type == ENTITY_NPC) {
        g->flag = GAMESTATE_FLAG_NPC_ANIM;
    } else {
        g->flag = GAMESTATE_FLAG_NONE;
    }
}


const int liblogic_tile_npc_dead_count(const gamestate* const g, const int x, const int y, const int floor) {
    if (!g) {
        merror("liblogic_tile_npc_dead_count: gamestate is NULL");
        return -1;
    }

    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("liblogic_tile_npc_dead_count: failed to get dungeon floor");
        return -1;
    }

    const dungeon_tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("liblogic_tile_npc_dead_count: failed to get tile");
        return -1;
    }

    // enumerate entities and count dead NPCs
    int count = 0;
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == -1) {
            continue;
        }
        const entity* const e = em_get(g->entitymap, tile->entities[i]);
        if (!e) {
            merror("liblogic_tile_npc_dead_count: failed to get entity");
            return -1;
        }
        if (e->type == ENTITY_NPC && e->is_dead) {
            count++;
        }
    }
    return count;
}


const int liblogic_tile_npc_living_count(const gamestate* const g, const int x, const int y, const int floor) {
    if (!g) {
        merror("liblogic_tile_npc_living_count: gamestate is NULL");
        return -1;
    }

    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("liblogic_tile_npc_living_count: failed to get dungeon floor");
        return -1;
    }

    const dungeon_tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("liblogic_tile_npc_living_count: failed to get tile");
        return -1;
    }

    // enumerate entities and count living NPCs
    int count = 0;
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == -1) {
            continue;
        }
        const entity* const e = em_get(g->entitymap, tile->entities[i]);
        if (!e) {
            merror("liblogic_tile_npc_living_count: failed to get entity");
            return -1;
        }
        if (e->type == ENTITY_NPC && !e->is_dead) {
            count++;
        }
    }
    return count;
}
