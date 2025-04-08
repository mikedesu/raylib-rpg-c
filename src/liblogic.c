#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "em.h"
#include "entity.h"
#include "entity_actions.h"
#include "entitytype.h"
#include "gamestate.h"
#include "keybinding.h"
#include "libgame_defines.h"
#include "liblogic.h"
#include "location.h"
#include "massert.h"
#include "mprint.h"
#include "race.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

static entityid next_entityid = 0; // Start at 0, increment for each new entity

static inline const direction_t liblogic_get_dir_from_xy(int x, int y) {
    if (x == 0 && y == 0) return DIR_NONE;
    if (x == 0 && y == -1) return DIR_UP;
    if (x == 0 && y == 1) return DIR_DOWN;
    if (x == -1 && y == 0) return DIR_LEFT;
    if (x == 1 && y == 0) return DIR_RIGHT;
    // also handle diagonals
    if (x == -1 && y == -1) return DIR_UP_LEFT;
    if (x == 1 && y == -1) return DIR_UP_RIGHT;
    if (x == -1 && y == 1) return DIR_DOWN_LEFT;
    if (x == 1 && y == 1) return DIR_DOWN_RIGHT;
    return DIR_UNKNOWN;
}

static inline int liblogic_get_x_from_dir(direction_t dir) {
    switch (dir) {
    case DIR_UP_LEFT:
    case DIR_DOWN_LEFT:
    case DIR_LEFT: return -1;
    case DIR_UP_RIGHT:
    case DIR_DOWN_RIGHT:
    case DIR_RIGHT: return 1;
    default: return 0;
    }
}

static inline int liblogic_get_y_from_dir(direction_t dir) {
    switch (dir) {
    case DIR_UP:
    case DIR_UP_LEFT:
    case DIR_UP_RIGHT: return -1;
    case DIR_DOWN:
    case DIR_DOWN_LEFT:
    case DIR_DOWN_RIGHT: return 1;
    default: return 0;
    }
}

void liblogic_execute_action(gamestate* const g, entity* const e, entity_action_t action) {
    massert(g, "liblogic_execute_action: gamestate is NULL");
    massert(e, "liblogic_execute_action: entity is NULL");
    switch (action) {
    case ENTITY_ACTION_MOVE_LEFT: liblogic_try_entity_move(g, e, -1, 0); break;
    case ENTITY_ACTION_MOVE_RIGHT: liblogic_try_entity_move(g, e, 1, 0); break;
    case ENTITY_ACTION_MOVE_UP: liblogic_try_entity_move(g, e, 0, -1); break;
    case ENTITY_ACTION_MOVE_DOWN: liblogic_try_entity_move(g, e, 0, 1); break;
    case ENTITY_ACTION_MOVE_UP_LEFT: liblogic_try_entity_move(g, e, -1, -1); break;
    case ENTITY_ACTION_MOVE_UP_RIGHT: liblogic_try_entity_move(g, e, 1, -1); break;
    case ENTITY_ACTION_MOVE_DOWN_LEFT: liblogic_try_entity_move(g, e, -1, 1); break;
    case ENTITY_ACTION_MOVE_DOWN_RIGHT: liblogic_try_entity_move(g, e, 1, 1); break;
    case ENTITY_ACTION_ATTACK_LEFT: liblogic_try_entity_attack(g, e->id, e->x - 1, e->y); break;
    case ENTITY_ACTION_ATTACK_RIGHT: liblogic_try_entity_attack(g, e->id, e->x + 1, e->y); break;
    case ENTITY_ACTION_ATTACK_UP: liblogic_try_entity_attack(g, e->id, e->x, e->y - 1); break;
    case ENTITY_ACTION_ATTACK_DOWN: liblogic_try_entity_attack(g, e->id, e->x, e->y + 1); break;
    case ENTITY_ACTION_ATTACK_UP_LEFT: liblogic_try_entity_attack(g, e->id, e->x - 1, e->y - 1); break;
    case ENTITY_ACTION_ATTACK_UP_RIGHT: liblogic_try_entity_attack(g, e->id, e->x + 1, e->y - 1); break;
    case ENTITY_ACTION_ATTACK_DOWN_LEFT: liblogic_try_entity_attack(g, e->id, e->x - 1, e->y + 1); break;
    case ENTITY_ACTION_ATTACK_DOWN_RIGHT: liblogic_try_entity_attack(g, e->id, e->x + 1, e->y + 1); break;
    case ENTITY_ACTION_MOVE_RANDOM: liblogic_try_entity_move_random(g, e); break;
    case ENTITY_ACTION_ATTACK_RANDOM: {
        liblogic_try_entity_attack_random(g, e);
        break;
    }
    case ENTITY_ACTION_MOVE_PLAYER: {
        liblogic_try_entity_move_player(g, e);
        break;
    }
    case ENTITY_ACTION_ATTACK_PLAYER: {
        liblogic_try_entity_attack_player(g, e);
        break;
    }
    case ENTITY_ACTION_MOVE_ATTACK_PLAYER: {
        liblogic_try_entity_move_attack_player(g, e);
        break;
    }
    case ENTITY_ACTION_WAIT:
        // Do nothing for now (future healing logic can go here)
        break;
    case ENTITY_ACTION_INTERACT_DOWN_LEFT:
    case ENTITY_ACTION_INTERACT_DOWN_RIGHT:
    case ENTITY_ACTION_INTERACT_UP_LEFT:
    case ENTITY_ACTION_INTERACT_UP_RIGHT:
    case ENTITY_ACTION_INTERACT_LEFT:
    case ENTITY_ACTION_INTERACT_RIGHT:
    case ENTITY_ACTION_INTERACT_UP:
    case ENTITY_ACTION_INTERACT_DOWN:
    default:
        //merror("Unknown entity action: %d", action);
        break;
    }
}

void liblogic_try_entity_attack_random(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_attack_random: gamestate is NULL");
    massert(e, "liblogic_try_entity_attack_random: entity is NULL");
    int x = rand() % 3;
    x = x == 0 ? -1 : x == 1 ? 0 : 1;
    // if x is 0, y cannot also be 0
    // if x is 0, y cannot also be 0
    int y;
    if (x == 0) {
        y = rand() % 2;
        y = y == 0 ? -1 : 1;
    } else {
        y = rand() % 3;
        y = y == 0 ? -1 : y == 1 ? 0 : 1;
    }
    liblogic_try_entity_attack(g, e->id, e->x + x, e->y + y);
}

void liblogic_try_entity_move_random(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_move_random: gamestate is NULL");
    massert(e, "liblogic_try_entity_move_random: entity is NULL");
    int x = rand() % 3;
    x = x == 0 ? -1 : x == 1 ? 0 : 1;
    // if x is 0, y cannot also be 0
    int y;
    if (x == 0) {
        y = rand() % 2;
        y = y == 0 ? -1 : 1;
    } else {
        y = rand() % 3;
        y = y == 0 ? -1 : y == 1 ? 0 : 1;
    }
    liblogic_try_entity_move(g, e, x, y);
}

void liblogic_try_entity_move_player(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_move_player: gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    if (h) {
        int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
        int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
        if (dx != 0 || dy != 0) { liblogic_try_entity_move(g, e, dx, dy); }
    }
}

void liblogic_try_entity_attack_player(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_attack_player: gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    if (h) {
        int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
        int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
        if (dx != 0 || dy != 0) { liblogic_try_entity_attack(g, e->id, h->x, h->y); }
    }
}

void liblogic_try_entity_move_attack_player(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_move_attack_player: gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    if (h) {
        // check if we are adjacent to the player
        if (liblogic_entities_adjacent(g, e->id, h->id)) {
            liblogic_try_entity_attack(g, e->id, h->x, h->y);
        } else {
            liblogic_try_entity_move_player(g, e);
        }
    }
}

bool liblogic_entities_adjacent(gamestate* const g, entityid id0, entityid id1) {
    massert(g, "liblogic_entities_adjacent: gamestate is NULL");
    entity* const e0 = em_get(g->entitymap, id0);
    if (!e0) {
        //merrorint("liblogic_entities_adjacent: entity not found", id0);
        return false;
    }
    entity* const e1 = em_get(g->entitymap, id1);
    if (!e1) {
        //merrorint("liblogic_entities_adjacent: entity not found", id1);
        return false;
    }
    // use e0 and check the surrounding 8 tiles
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0) { continue; }
            if (e0->x + x == e1->x && e0->y + y == e1->y) { return true; }
        }
    }
    return false;
}

void liblogic_init(gamestate* const g) {
    massert(g, "liblogic_init: gamestate is NULL");
    srand(time(NULL));
    liblogic_init_dungeon(g);

    gamestate_init_entityids(g);
    g->msg_system.count = 0;
    g->msg_system.index = 0;
    g->msg_system.is_active = false;

    gamestate_load_keybindings(g);

    liblogic_init_em(g);
    liblogic_init_player(g);
    // temporarily disabling
    //liblogic_init_orcs_test(g);
    liblogic_update_debug_panel_buffer(g);
}

void liblogic_add_message(gamestate* g, const char* text) {
    massert(g, "liblogic_add_message: gamestate is NULL");
    massert(text, "liblogic_add_message: text is NULL");
    massert(strlen(text) > 0, "liblogic_add_message: text is empty");

    if (g->msg_system.count >= MAX_MESSAGES) {
        mwarning("Message queue full!");
        return;
    }
    strncpy(g->msg_system.messages[g->msg_system.count], text, MAX_MSG_LENGTH - 1);
    g->msg_system.messages[g->msg_system.count][MAX_MSG_LENGTH - 1] = '\0'; // Ensure null-termination
    g->msg_system.count++;
    g->msg_system.is_active = true;
}

void liblogic_init_dungeon(gamestate* const g) {
    massert(g, "liblogic_init_dungeon: gamestate is NULL");
    g->dungeon = dungeon_create();
    massert(g->dungeon, "liblogic_init_dungeon: failed to init dungeon");
    dungeon_add_floor(g->dungeon, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
}

void liblogic_init_em(gamestate* const g) {
    massert(g, "liblogic_init: gamestate is NULL");
    g->entitymap = em_new();
}

void liblogic_init_player(gamestate* const g) {
    minfo("liblogic_init: initializing player");
    massert(g, "liblogic_init: gamestate is NULL");
    // this is primitive and requires work
    // we cant simply hardcode in player start position anymore
    // without potential assert failure
    // so...

    // setting it up so we can return a loc_t from a function
    // that can scan for an appropriate starting location
    loc_t loc;
    loc.x = 2;
    loc.y = 2;

    const int herox = loc.x;
    const int heroy = loc.y;
    //const int id = liblogic_player_create(g, RACE_HUMAN, herox, heroy, 0, "hero");
    minfo("liblogic_init: creating player...");
    const int id = liblogic_player_create(g, RACE_HUMAN, herox, heroy, 0, "hero");
    minfo("liblogic_init: player id: %d", id);
    massert(id != -1, "liblogic_init: failed to init hero");
    entity* const hero = em_get(g->entitymap, g->hero_id);
    if (hero) {
        entity_set_maxhp(hero, 3);
        entity_set_hp(hero, 3);
    }
    g->entity_turn = g->hero_id;
    msuccess("liblogic_init: hero id %d", g->hero_id);
}

void liblogic_init_orcs_test_naive_loop(gamestate* const g) {
    massert(g, "liblogic_init: gamestate is NULL");
    dungeon_t* const d = g->dungeon;
    massert(d, "liblogic_init: dungeon is NULL");
    //dungeon_floor_t* const df = d->floors[0];
    dungeon_floor_t* const df = dungeon_get_floor(d, 0);
    massert(df, "liblogic_init: dungeon floor is NULL");

    // this works, but is naive and slow
    int count = 0;
    const int total_orcs_to_make = 100;
    while (count < total_orcs_to_make) {
        int x = rand() % df->width;
        int y = rand() % df->height;
        tile_t* const tile = dungeon_floor_tile_at(df, x, y);
        if (dungeon_tile_is_wall(tile->type)) { continue; }
        // check if there is already an entity at this location
        if (tile_entity_count(tile) > 0) { continue; }
        entity* const orc = liblogic_npc_create_ptr(g, RACE_ORC, x, y, 0, "orc");
        //massert(orc, "liblogic_init: failed to create orc");
        if (!orc) {
            merror("liblogic_init: failed to init orc");
            continue;
        }
        entity_action_t action = ENTITY_ACTION_MOVE_ATTACK_PLAYER;
        entity_set_default_action(orc, action);
        entity_set_maxhp(orc, 1);
        entity_set_hp(orc, 1);
        count++;
    }
}

void liblogic_init_orcs_test_intermediate(gamestate* const g) {
    massert(g, "liblogic_init: gamestate is NULL");
    dungeon_t* const d = g->dungeon;
    massert(d, "liblogic_init: dungeon is NULL");
    //dungeon_floor_t* const df = d->floors[0];
    dungeon_floor_t* const df = dungeon_get_floor(d, 0);
    massert(df, "liblogic_init: dungeon floor is NULL");

    // first, we need to count all of the possible tiles we could place an orc on
    // we can loop thru each tile on the dungeon floor and check if it is walkable
    // in the beginning there wont be any entities at all so we are just counting total possible locations right now
    // in order to prepare a list of them

    int count = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = dungeon_floor_tile_at(df, x, y);
            if (dungeon_tile_is_walkable(tile->type)) {
                // there wont be any entities yet so do not check for them
                // do not write an if statement
                count++;
            }
        }
    }

    // now we have the total number of possible locations
    // we can create an array of size count

    loc_t* locations = malloc(sizeof(loc_t) * count);
    massert(locations, "liblogic_init: failed to malloc locations");

    int count2 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = dungeon_floor_tile_at(df, x, y);
            if (dungeon_tile_is_walkable(tile->type)) {
                // there wont be any entities yet so do not check for them
                // do not write an if statement
                locations[count2].x = x;
                locations[count2].y = y;
                count2++;
                massert(count2 <= count, "liblogic_init: count2 is greater than count");
            }
        }
    }
    massert(count2 == count, "liblogic_init: count2 is greater than count");

    // now we can loop thru the array and create an orc at each location
    //for (int i = 0; i < count2; i++) {
    //for (int i = 0; i < count2; i++) {
    for (int i = 0; i < 1; i++) {

        tile_t* const tile = dungeon_floor_tile_at(df, locations[i].x, locations[i].y);
        if (dungeon_tile_is_wall(tile->type)) { continue; }
        // check if there is already an entity at this location
        if (tile_entity_count(tile) > 0) { continue; }
        entity* const orc = liblogic_npc_create_ptr(g, RACE_ORC, locations[i].x, locations[i].y, 0, "orc");
        massert(orc, "liblogic_init: failed to create orc");
        //if (!orc) {
        //    merror("liblogic_init: failed to init orc");
        //    continue;
        //}
        entity_action_t action = ENTITY_ACTION_MOVE_ATTACK_PLAYER;
        entity_set_default_action(orc, action);
        entity_set_maxhp(orc, 1);
        entity_set_hp(orc, 1);
    }

    // we need to free the locations array
    free(locations);
}

void liblogic_init_orcs_test(gamestate* const g) {
    //liblogic_init_orcs_test_naive_loop(g);
    liblogic_init_orcs_test_intermediate(g);
}

void liblogic_handle_input(const inputstate* const is, gamestate* const g) {
    massert(is, "liblogic_handle_input: inputstate is NULL");
    massert(g, "liblogic_handle_input: gamestate is NULL");
    if (inputstate_is_pressed(is, KEY_D)) {
        msuccess("D pressed!");
        g->debugpanelon = !g->debugpanelon;
    }
    //if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) g->is3d = !g->is3d;
    if (g->controlmode == CONTROLMODE_PLAYER) {
        liblogic_handle_input_player(is, g);
    } else if (g->controlmode == CONTROLMODE_CAMERA) {
        liblogic_handle_input_camera(is, g);
    } else {
        merror("Unknown control mode");
    }
}

void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    liblogic_handle_camera_move(g, is);
    liblogic_handle_camera_zoom(g, is);
}

void liblogic_handle_camera_move(gamestate* const g, const inputstate* const is) {
    const float move = g->cam2d.zoom;
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
}

void liblogic_handle_camera_zoom(gamestate* const g, const inputstate* const is) {
    massert(g, "Game state is NULL!");
    massert(is, "Input state is NULL!");
    if (inputstate_is_held(is, KEY_Z)) {
        if (inputstate_is_shift_held(is)) {
            g->cam2d.zoom -= DEFAULT_ZOOM_INCR;
        } else {
            g->cam2d.zoom += DEFAULT_ZOOM_INCR;
        }
    }
}

void liblogic_handle_input_player(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) { return; }

    if (g->msg_system.is_active) {
        if (inputstate_is_pressed(is, KEY_A)) {
            g->msg_system.index++;
            if (g->msg_system.index >= g->msg_system.count) {
                // Reset when all messages read
                g->msg_system.index = 0;
                g->msg_system.count = 0;
                g->msg_system.is_active = false;
            }
        }
        return;
    }

    entity* const e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        //merror("Hero not found!");
        return;
    }
    // check if the player is dead
    if (e->is_dead) {
        //merror("Hero is dead!");
        return;
    }
    const int key = inputstate_get_pressed_key(is);

    if (key != -1) { minfo("Key pressed: %d", key); }

    // can return early if key == -1
    if (key == -1) {
        //merror("Key not found!");
        return;
    }

    const char* action = get_action_for_key(&g->keybinding_list, key);

    if (action) {
        //msuccessstr("Action: --", action);
        if (strcmp(action, "move_w") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_LEFT);
        } else if (strcmp(action, "move_e") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_RIGHT);
        } else if (strcmp(action, "move_n") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_UP);
        } else if (strcmp(action, "move_s") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_DOWN);
        } else if (strcmp(action, "move_nw") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_UP_LEFT);
        } else if (strcmp(action, "move_ne") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_UP_RIGHT);
        } else if (strcmp(action, "move_sw") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_DOWN_LEFT);
        } else if (strcmp(action, "move_se") == 0) {
            liblogic_execute_action(g, e, ENTITY_ACTION_MOVE_DOWN_RIGHT);
        } else if (strcmp(action, "attack") == 0) {
            msuccess("attack pressed!");
            int dx = liblogic_get_x_from_dir(e->direction);
            int dy = liblogic_get_y_from_dir(e->direction);
            int tx = e->x + dx;
            int ty = e->y + dy;
            liblogic_try_entity_attack(g, e->id, tx, ty);
        } else if (strcmp(action, "interact") == 0) {
            // we are hardcoding the flip switch interaction for now
            // but eventually this will be generalized
            // for instance u can talk to an NPC merchant using "interact"
            // or open a door, etc
            msuccess("Space pressed!");
            int dx = liblogic_get_x_from_dir(e->direction);
            int dy = liblogic_get_y_from_dir(e->direction);
            int tx = e->x + dx;
            int ty = e->y + dy;
            liblogic_try_flip_switch(g, e, tx, ty, e->floor);
        }
    } else {
        merror("No action found for key");
    }

    // old
    /*
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
    case KEY_A: {
        msuccess("A pressed!");
        int dx = liblogic_get_x_from_dir(e->direction);
        int dy = liblogic_get_y_from_dir(e->direction);
        int tx = e->x + dx;
        int ty = e->y + dy;
        liblogic_try_entity_attack(g, e->id, tx, ty);
    } break;
    case KEY_SPACE: {
        msuccess("Space pressed!");
        int dx = liblogic_get_x_from_dir(e->direction);
        int dy = liblogic_get_y_from_dir(e->direction);
        int tx = e->x + dx;
        int ty = e->y + dy;
        liblogic_try_flip_switch(g, e, tx, ty, e->floor);
    } break;
    case KEY_C:
        msuccess("C pressed!");
        g->controlmode = CONTROLMODE_CAMERA;
        break;
    default: break; // Ignore unhandled keys
    }
    */
}

void liblogic_try_entity_move(gamestate* const g, entity* const e, int x, int y) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    e->do_update = true;
    e->direction = liblogic_get_dir_from_xy(x, y);
    const int ex = e->x + x, ey = e->y + y, floor = e->floor;
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("Failed to get dungeon floor");
        return;
    }
    // i feel like this might be something we can set elsewhere...like after the player input phase?
    if (e->type == ENTITY_PLAYER) { g->flag = GAMESTATE_FLAG_PLAYER_ANIM; }
    tile_t* const tile = dungeon_floor_tile_at(df, ex, ey);
    if (!tile || ex < 0 || ey < 0) {
        merror(!tile ? "Failed to get tile" : "Cannot move, out of bounds");
        return;
    }
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("Cannot move, tile is not walkable");
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
    // get the entity's new tile
    tile_t* const new_tile = dungeon_floor_tile_at(df, ex, ey);
    if (!new_tile) {
        merror("Failed to get new tile");
        return;
    }
    // check if the tile has a pressure plate
    if (new_tile->has_pressure_plate) {
        msuccess("Pressure plate activated!");
        // do something
        // print the pressure plate event
        //msuccessint("Pressure plate event", new_tile->pressure_plate_event);
    }
    // check if the tile is an ON TRAP
    if (new_tile->type == TILE_FLOOR_STONE_TRAP_ON_00) {
        msuccess("On trap activated!");
        // do something
        e->hp--;
        e->is_damaged = true;
        e->do_update = true;
    }
}

void liblogic_try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "Failed to get dungeon floor");
    tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    massert(tile, "Failed to get tile");
    if (tile->has_wall_switch) {
        tile->wall_switch_on = !tile->wall_switch_on;
        msuccess("Wall switch flipped!");
        int ws_event = tile->wall_switch_event;
        if (ws_event <= -1) {
            merror("Wall switch event is invalid");
            return;
        }
        // grab the event in df
        df_event_t event = df->events[ws_event];
        // grab the tile marked at by the event
        tile_t* const event_tile = dungeon_floor_tile_at(df, event.x, event.y);
        if (!event_tile) {
            merror("Failed to get event tile");
            return;
        }
        tiletype_t type = event_tile->type;
        if (type == event.off_type) {
            event_tile->type = event.on_type;
        } else if (type == event.on_type) {
            event_tile->type = event.off_type;
        }
        //liblogic_add_message(g, "Wall switch flipped!");
        // this is the basis for what we need to do next
        // currently we have no entity passed into this function
        // we need to fix that
        if (e->type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (e->type == ENTITY_NPC) {
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        } else {
            g->flag = GAMESTATE_FLAG_NONE;
        }

        //if (tile->wall_switch_event == 777) {
        //    msuccess("Wall switch event 777!");
        //    // do something
        //    // get the tile at 5,2 and change its type to tile_tYPE_FLOOR_STONE_TRAP_OFF_00
        //    tile_t* const trap_tile = dungeon_floor_tile_at(df, 2, 5);
        //    if (!trap_tile) {
        //        merror("Failed to get trap tile");
        //        return;
        //    }
        //    tiletype_t type = trap_tile->type;
        //    if (type == tile_tYPE_FLOOR_STONE_TRAP_ON_00) {
        //        trap_tile->type = tile_tYPE_FLOOR_STONE_TRAP_OFF_00;
        //    } else if (type == tile_tYPE_FLOOR_STONE_TRAP_OFF_00) {
        //        trap_tile->type = tile_tYPE_FLOOR_STONE_TRAP_ON_00;
        //    }
        //}
    }
}

bool liblogic_player_on_tile(const gamestate* const g, int x, int y, int floor) {
    massert(g, "liblogic_player_on_tile: gamestate is NULL");
    // get the tile at x y
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, 0);
    if (!df) {
        merror("liblogic_player_on_tile: failed to get dungeon floor");
        return false;
    }
    const tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("liblogic_player_on_tile: failed to get tile");
        return false;
    }
    // enumerate entities and check their type
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == -1) { continue; }
        const entity* const e = em_get(g->entitymap, tile->entities[i]);
        if (!e) {
            merror("liblogic_player_on_tile: failed to get entity");
            return false;
        }
        if (e->type == ENTITY_PLAYER) { return true; }
    }
    return false;
}

void liblogic_tick(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    liblogic_update_player_state(g);
    liblogic_update_npcs_state(g);
    liblogic_handle_input(is, g);
    if (g->flag == GAMESTATE_FLAG_NPC_TURN) { liblogic_handle_npcs(g); }
    //liblogic_handle_npc(g);
    liblogic_update_debug_panel_buffer(g);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}

void liblogic_update_player_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        merror("Failed to get hero entity");
        return;
    }
    if (e->is_dead) {

        if (!g->gameover) {

            liblogic_add_message(g, "You died!");
            g->gameover = true;
        }

        //merror("Hero is dead!");
        return;
    }
    if (e->hp <= 0) {
        e->is_dead = true;
        e->do_update = true;
        merror("Hero is dead!");
        return;
    }
}

void liblogic_update_npcs_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    for (int i = 0; i < g->index_entityids; i++) {
        entityid id = g->entityids[i];
        liblogic_update_npc_state(g, id);
    }
}

void liblogic_update_npc_state(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merror("Failed to get entity");
        return;
    }
    if (e->is_dead) { return; }
    if (e->hp <= 0) {
        e->is_dead = true;
        e->do_update = true;
        merror("NPC is dead!");
        return;
    }
}

void liblogic_handle_npc(gamestate* const g) {
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_NPC_TURN) { return; }
    const entityid id = g->entity_turn;
    entity* const e = em_get(g->entitymap, id);
    const int rx = rand() % 3 - 1;
    const int ry = rand() % 3 - 1;
    if (!e) {
        merror("Failed to get entity");
        return;
    }
    if (e->type != ENTITY_NPC) {
        g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
        return;
    }
    liblogic_try_entity_move(g, e, rx, ry);
}

void liblogic_handle_npcs(gamestate* const g) {
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_NPC_TURN) return;
    // Process all NPCs
    for (int i = 0; i < g->index_entityids; i++) {
        entity* e = em_get(g->entitymap, g->entityids[i]);
        if (!e || e->type != ENTITY_NPC || e->floor != 0) continue;
        if (e->is_dead) continue;
        // testing attack logic
        liblogic_execute_action(g, e, e->default_action);
    }
    // After processing all NPCs, set the flag to animate all movements together
    g->flag = GAMESTATE_FLAG_NPC_ANIM;
}

void liblogic_update_debug_panel_buffer(gamestate* const g) {
    massert(g, "liblogic_update_debug_panel_buffer: gamestate is NULL");
    // Static buffers to avoid reallocating every frame
    static const char* control_modes[] = {"Player", "Camera", "Unknown"};
    static const char* flag_names[] = {"GAMESTATE_FLAG_PLAYER_INPUT",
                                       "GAMESTATE_FLAG_PLAYER_ANIM",
                                       "GAMESTATE_FLAG_NONE",
                                       "GAMESTATE_FLAG_COUNT",
                                       "GAMESTATE_FLAG_NPC_TURN",
                                       "GAMESTATE_FLAG_NPC_ANIM",
                                       "Unknown"};
    // Get hero position once
    const entity* const e = em_get(g->entitymap, g->hero_id);
    const int hero_x = e ? e->x : -1;
    const int hero_y = e ? e->y : -1;
    // Determine control mode and flag strings
    const char* control_mode = control_modes[(g->controlmode >= 0 && g->controlmode < 2) ? g->controlmode : 2];
    const char* flag_name = flag_names[(g->flag >= 0 && g->flag <= 5) ? g->flag : 6];
    // Format the string in one pass
    snprintf(g->debugpanel.buffer,
             sizeof(g->debugpanel.buffer),
             "%s\n" // timebeganbuf
             "%s\n" // currenttimebuf
             "Frame: %d (%.1fms)\n"
             "Is3D: %d\n"
             "Cam: (%.0f,%.0f) Zoom: %.1f\n"
             "Mode: %s | Floor: %d/%d\n"
             "Entities: %d | Flag: %s\n"
             "Turn: %d | Hero: (%d,%d)",
             g->timebeganbuf,
             g->currenttimebuf,
             g->framecount,
             g->last_frame_time * 1000,
             g->is3d,
             g->cam2d.offset.x,
             g->cam2d.offset.y,
             g->cam2d.zoom,
             control_mode,
             g->dungeon->current_floor + 1, // More user-friendly 1-based
             g->dungeon->num_floors,
             g->index_entityids,
             flag_name,
             g->entity_turn,
             hero_x,
             hero_y);
}

void liblogic_close(gamestate* const g) {
    massert(g, "liblogic_close: gamestate is NULL");
    massert(g->entitymap, "liblogic_close: entitymap is NULL");
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
    massert(g, "liblogic_add_entityid: gamestate is NULL");
    gamestate_add_entityid(g, id);
    //msuccessint("Added entity ID: ", id);
}

entity_t* const liblogic_npc_create_ptr(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    massert(g, "liblogic_npc_create_ptr: gamestate is NULL");
    entityid id = liblogic_npc_create(g, rt, x, y, fl, name);
    if (id == -1) {
        merror("liblogic_npc_create_ptr: failed to create NPC");
        return NULL;
    }
    return em_get(g->entitymap, id);
}

entityid liblogic_npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    massert(g, "liblogic_npc_create: gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "liblogic_npc_create: entitymap is NULL");
    massert(name && name[0], "liblogic_npc_create: name is NULL or empty");
    massert(rt >= 0, "liblogic_entity_create: race_type is out of bounds");
    massert(rt < RACE_COUNT, "liblogic_entity_create: race_type is out of bounds");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "liblogic_entity_create: failed to get current dungeon floor");
    massert(x >= 0, "liblogic_entity_create: x is out of bounds");
    massert(x < df->width, "liblogic_entity_create: x is out of bounds");
    massert(y >= 0, "liblogic_entity_create: y is out of bounds");
    massert(y < df->height, "liblogic_entity_create: y is out of bounds");
    // can we create an entity at this location? no entities can be made on wall-types etc
    tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    massert(tile, "liblogic_entity_create: failed to get tile");
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("liblogic_entity_create: cannot create entity on wall");
        return -1;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("liblogic_entity_create: cannot create entity on tile with NPC");
        return -1;
    }
    entity* const e = entity_new_npc_at(next_entityid++, rt, x, y, fl,
                                        name); // Assuming entity_new_at takes name next
    if (!e) {
        merror("liblogic_entity_create: failed to create entity");
        return -1;
    }
    em_add(em, e);
    gamestate_add_entityid(g, e->id);
    //dungeon_floor_add_at(df, e->id, x, y);
    if (!dungeon_floor_add_at(df, e->id, x, y)) {
        merror("liblogic_entity_create: failed to add entity to dungeon floor");
        entity_free(e);
        return -1;
    }
    return e->id;
}

entityid liblogic_player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    minfo("liblogic_player_create: creating player");
    massert(g, "liblogic_player_create: gamestate is NULL");
    massert(name, "liblogic_player_create: name is NULL");
    // use the previously-written liblogic_npc_create function
    const entitytype_t type = ENTITY_PLAYER;
    minfo("liblogic_player_create: creating player with type %d", type);
    const entityid id = liblogic_npc_create(g, rt, x, y, fl, name);
    minfo("liblogic_player_create: player id: %d", id);
    minfo("getting entitymap");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "liblogic_player_create: entitymap is NULL");
    entity_t* const e = em_get(em, id);
    if (!e) {
        merror("liblogic_player_create: failed to get entity with id %d", id);
        return ENTITYID_INVALID;
    }
    entity_set_type(e, type);
    gamestate_set_hero_id(g, id);
    return id;
}

void liblogic_try_entity_attack_in_facing_dir(gamestate* const g, entityid attacker_id) {
    massert(g, "liblogic_try_entity_attack_in_facing_dir: gamestate is NULL");
    massert(attacker_id != ENTITYID_INVALID, "liblogic_try_entity_attack_in_facing_dir: attacker_id is invalid");
    entity* const e = em_get(g->entitymap, attacker_id);
    massert(e, "liblogic_try_entity_attack_in_facing_dir: entity is NULL");
    int tx = e->x + liblogic_get_x_from_dir(e->direction);
    int ty = e->y + liblogic_get_y_from_dir(e->direction);
    // if tx = 0 and ty = 0, then we attack ourselves
    liblogic_try_entity_attack(g, e->id, tx, ty);
}

void liblogic_try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y) {
    massert(g, "liblogic_try_entity_attack: gamestate is NULL");
    entity* const attacker = em_get(g->entitymap, attacker_id);
    if (!attacker || attacker->is_dead) {
        merror("liblogic_try_entity_attack: attacker entity not found");
        return;
    }
    dungeon_floor_t* const floor = dungeon_get_floor(g->dungeon, attacker->floor);
    if (!floor) {
        merror("liblogic_try_entity_attack: failed to get dungeon floor");
        return;
    }
    tile_t* const tile = dungeon_floor_tile_at(floor, target_x, target_y);
    if (!tile) {
        merror("liblogic_try_entity_attack: target tile not found");
        return;
    }
    // Calculate direction based on target position
    bool attack_successful = false;
    int dx = target_x - attacker->x, dy = target_y - attacker->y;
    attacker->direction = liblogic_get_dir_from_xy(dx, dy);
    attacker->is_attacking = attacker->do_update = true;
    for (int i = 0; i < tile->entity_max; i++) {
        entityid id = tile->entities[i];
        if (id != -1) {
            entity* const target = em_get(g->entitymap, id);
            if (target && (target->type == ENTITY_NPC || target->type == ENTITY_PLAYER) && !target->is_dead) {
                // Perform attack logic here
                minfo("Attack successful!");
                attack_successful = target->is_damaged = target->do_update = true;
                entity_set_hp(target, entity_get_hp(target) - 1); // Reduce HP by 1

                if (target->type == ENTITY_PLAYER) { liblogic_add_message(g, "Player attacked!"); }

                if (entity_get_hp(target) <= 0) target->is_dead = true;
                break;
            }
        }
    }
    if (!attack_successful) {
        //merrorint2("liblogic_try_entity_attack: no valid target found at the specified location", target_x, target_y);
        if (attacker->type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (attacker->type == ENTITY_NPC) {
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        } else {
            g->flag = GAMESTATE_FLAG_NONE;
        }
    }
}

int liblogic_tile_npc_living_count(const gamestate* const g, int x, int y, int fl) {
    // Validate inputs
    massert(g, "liblogic_tile_npc_living_count: gamestate is NULL");
    massert(fl >= 0, "liblogic_tile_npc_living_count: floor is out of bounds");
    massert(fl < g->dungeon->num_floors, "liblogic_tile_npc_living_count: floor is out of bounds");
    massert(x >= 0, "liblogic_tile_npc_living_count: x is out of bounds");
    massert(x < g->dungeon->floors[fl]->width, "liblogic_tile_npc_living_count: x is out of bounds");
    massert(y >= 0, "liblogic_tile_npc_living_count: y is out of bounds");
    massert(y < g->dungeon->floors[fl]->height, "liblogic_tile_npc_living_count: y is out of bounds");
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "liblogic_tile_npc_living_count: failed to get dungeon floor");
    const tile_t* const t = dungeon_floor_tile_at(df, x, y);
    massert(t, "liblogic_tile_npc_living_count: failed to get tile");
    // Count living NPCs
    int count = 0;
    for (int i = 0; i < t->entity_max; i++) {
        const entityid eid = tile_get_entity(t, i);
        if (eid == -1) continue;
        const entity* const e = em_get(g->entitymap, eid);
        if (!e) {
            //mwarningint("Missing entity", eid); // Warning not error
            continue;
        }
        if (e->type == ENTITY_NPC && !e->is_dead) { count++; }
    }
    return count;
}
