#include "liblogic.h"
#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
#include "em.h"
#include "entity.h"
#include "entity_actions.h"
#include "entitytype.h"
#include "gamestate.h"
#include "libgame_defines.h"
#include "mprint.h"
#include "race.h"
#include <math.h>
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
    return DIR_NONE;
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
    default:
        //merror("Unknown entity action: %d", action);
        break;
    }
}

void liblogic_try_entity_move_random(gamestate* const g, entity* const e) {
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
    entity* h = em_get(g->entitymap, g->hero_id);
    if (h) {
        int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
        int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
        if (dx != 0 || dy != 0) { liblogic_try_entity_move(g, e, dx, dy); }
    }
}

void liblogic_try_entity_attack_player(gamestate* const g, entity* const e) {
    entity* h = em_get(g->entitymap, g->hero_id);
    if (h) {
        int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
        int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
        if (dx != 0 || dy != 0) { liblogic_try_entity_attack(g, e->id, h->x, h->y); }
    }
}

void liblogic_try_entity_move_attack_player(gamestate* const g, entity* const e) {
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

bool liblogic_entities_adjacent(gamestate* const g, entityid eid0, entityid eid1) {
    if (!g) {
        merror("liblogic_entities_adjacent: gamestate is NULL");
        return false;
    }
    entity* const e0 = em_get(g->entitymap, eid0);
    if (!e0) {
        merrorint("liblogic_entities_adjacent: entity not found", eid0);
        return false;
    }
    entity* const e1 = em_get(g->entitymap, eid1);
    if (!e1) {
        merrorint("liblogic_entities_adjacent: entity not found", eid1);
        return false;
    }
    //if (e0->x == e1->x && abs(e0->y - e1->y) == 1) {
    //    return true;
    //}
    //if (e0->y == e1->y && abs(e0->x - e1->x) == 1) {
    //    return true;
    //}
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
    if (!g) {
        merror("liblogic_init: gamestate is NULL");
        return;
    }

    srand(time(NULL));

    // init the dungeon and dungeon floor
    g->dungeon = dungeon_create();
    if (!g->dungeon) {
        merror("liblogic_init: failed to init dungeon");
        return;
    }

    dungeon_add_floor(g->dungeon, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    gamestate_init_entityids(g);
    g->entitymap = em_new();

    int herox = 2;
    int heroy = 2;
    if (liblogic_player_create(g, RACE_HUMAN, herox, heroy, 0, "hero") == -1) {
        merror("liblogic_init: failed to init hero");
    }
    entity* const hero = em_get(g->entitymap, g->hero_id);
    if (hero) {
        //entity_set_default_action(hero, ENTITY_ACTION_MOVE_PLAYER);
        entity_set_maxhp(hero, 3);
        entity_set_hp(hero, 3);
    }
    g->entity_turn = g->hero_id;

    // create some orcs with names
    //entityid orc0 = liblogic_npc_create(g, RACE_ORC, 8, 2, 0, "orc-mover");
    //entityid orc1 = liblogic_npc_create(g, RACE_ORC, 8, 3, 0, "orc-attacker");
    //entityid orc2 = liblogic_npc_create(g, RACE_ORC, 8, 4, 0, "orc-mover");

    int orc_x = 0;
    int orc_y = 6;
    //int rows = 10;
    //int cols = 10;
    //int rows = 34;
    //int cols = 30;
    int count = 0;
    int total_orcs_to_make = 4;

    dungeon_t* d = g->dungeon;
    dungeon_floor_t* df = d->floors[0];
    if (!df) {
        merror("liblogic_init: failed to get dungeon floor");
        return;
    }

    // lets test setting a pressure plate
    //dungeon_floor_set_pressure_plate(df, 9, 2, TX_PRESSURE_PLATE_UP_00, TX_PRESSURE_PLATE_DOWN_00, 666);
    //dungeon_floor_set_wall_switch(df, herox, heroy - 2, TX_WALL_SWITCH_UP_00, TX_WALL_SWITCH_DOWN_00, 777);

    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            if (count >= total_orcs_to_make) { break; }

            entity* const orc = liblogic_npc_create_ptr(g, RACE_ORC, orc_x + x, orc_y + y, 0, "orc");
            entity_action_t action = ENTITY_ACTION_MOVE_ATTACK_PLAYER;
            entity_set_default_action(orc, action);
            entity_set_maxhp(orc, 1);
            entity_set_hp(orc, 1);
            count++;

            if (count >= total_orcs_to_make) { break; }
        }

        if (count >= total_orcs_to_make) { break; }
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
            g->cam2d.zoom -= DEFAULT_ZOOM_INCR;
        } else {
            g->cam2d.zoom += DEFAULT_ZOOM_INCR;
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

    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) { return; }

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

        liblogic_try_flip_switch(g, tx, ty, e->floor);

    } break;
    case KEY_C:
        msuccess("C pressed!");
        g->controlmode = CONTROLMODE_CAMERA;
        break;
    default: break; // Ignore unhandled keys
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
    if (e->type == ENTITY_PLAYER) { g->flag = GAMESTATE_FLAG_PLAYER_ANIM; }

    dungeon_tile_t* const tile = dungeon_floor_tile_at(df, ex, ey);
    if (!tile || ex < 0 || ey < 0) {
        merror(!tile ? "Failed to get tile" : "Cannot move, out of bounds");
        return;
    }

    //if (dungeon_tile_is_wall(tile->type)) {
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
    dungeon_tile_t* const new_tile = dungeon_floor_tile_at(df, ex, ey);
    if (!new_tile) {
        merror("Failed to get new tile");
        return;
    }

    // check if the tile has a pressure plate
    if (new_tile->has_pressure_plate) {
        msuccess("Pressure plate activated!");
        // do something
        // print the pressure plate event
        msuccessint("Pressure plate event", new_tile->pressure_plate_event);
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

void liblogic_try_flip_switch(gamestate* const g, int x, int y, int fl) {
    if (!g) {
        merror("Game state is NULL!");
        return;
    }
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    if (!df) {
        merror("Failed to get dungeon floor");
        return;
    }
    dungeon_tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merror("Failed to get tile");
        return;
    }
    if (tile->has_wall_switch) {
        tile->wall_switch_on = !tile->wall_switch_on;
        msuccess("Wall switch flipped!");
        int ws_event = tile->wall_switch_event;
        msuccessint("Wall switch event", ws_event);

        if (ws_event <= -1) {
            merror("Wall switch event is invalid");
            return;
        }

        // grab the event in df
        df_event_t event = df->events[ws_event];
        // grab the tile marked at by the event
        dungeon_tile_t* const event_tile = dungeon_floor_tile_at(df, event.x, event.y);
        if (!event_tile) {
            merror("Failed to get event tile");
            return;
        }

        dungeon_tile_type_t type = event_tile->type;
        if (type == event.off_type) {
            event_tile->type = event.on_type;
        } else if (type == event.on_type) {
            event_tile->type = event.off_type;
        }

        //if (tile->wall_switch_event == 777) {
        //    msuccess("Wall switch event 777!");
        //    // do something
        //    // get the tile at 5,2 and change its type to DUNGEON_TILE_TYPE_FLOOR_STONE_TRAP_OFF_00
        //    dungeon_tile_t* const trap_tile = dungeon_floor_tile_at(df, 2, 5);
        //    if (!trap_tile) {
        //        merror("Failed to get trap tile");
        //        return;
        //    }
        //    dungeon_tile_type_t type = trap_tile->type;
        //    if (type == DUNGEON_TILE_TYPE_FLOOR_STONE_TRAP_ON_00) {
        //        trap_tile->type = DUNGEON_TILE_TYPE_FLOOR_STONE_TRAP_OFF_00;
        //    } else if (type == DUNGEON_TILE_TYPE_FLOOR_STONE_TRAP_OFF_00) {
        //        trap_tile->type = DUNGEON_TILE_TYPE_FLOOR_STONE_TRAP_ON_00;
        //    }
        //}
    }
}

bool liblogic_player_on_tile(const gamestate* const g, int x, int y, int floor) {
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

int liblogic_tile_npc_count(const gamestate* const g, int x, int y, int floor) {
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
        if (tile->entities[i] == -1) { continue; }
        const entity* const e = em_get(g->entitymap, tile->entities[i]);
        if (!e) {
            merror("liblogic_tile_npc_count: failed to get entity");
            return -1;
        }
        if (e->type == ENTITY_NPC) { count++; }
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
    if (!g) {
        merror("Game state is NULL!");
        return;
    }

    entity* const e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        merror("Failed to get hero entity");
        return;
    }

    if (e->is_dead) {
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
    if (!g) {
        merror("Game state is NULL!");
        return;
    }

    for (int i = 0; i < g->index_entityids; i++) {
        entityid id = g->entityids[i];
        liblogic_update_npc_state(g, id);
    }
}

void liblogic_update_npc_state(gamestate* const g, entityid id) {
    if (!g) {
        merror("Game state is NULL!");
        return;
    }

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
    if (!g) {
        merror("Game state is NULL!");
        return;
    }

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
        if (!e || e->type != ENTITY_NPC || e->floor != 0) { continue; }

        if (e->is_dead) { continue; }

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

void liblogic_update_debug_panel_buffer(gamestate* const g) {
    if (!g) return;

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
             "Cam: (%.0f,%.0f) Zoom: %.1f\n"
             "Mode: %s | Floor: %d/%d\n"
             "Entities: %d | Flag: %s\n"
             "Turn: %d | Hero: (%d,%d)",
             g->timebeganbuf,
             g->currenttimebuf,
             g->framecount,
             g->last_frame_time * 1000,
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

//void liblogic_update_debug_panel_buffer(gamestate* const g) {
//    if (!g) {
//        merror("Game state is NULL!");
//        return;
//    }
//    // grab a pointer to the hero
//    entity* const e = em_get(g->entitymap, g->hero_id);
//    int x = -1;
//    int y = -1;
//    if (e) {
//        x = e->x;
//        y = e->y;
//    }
//
//    snprintf(g->debugpanel.buffer,
//             sizeof(g->debugpanel.buffer),
//             "%s\n"
//             "%s\n"
//             "Frame count: %d\n"
//             "Frame draw time: %.02f ms\n"
//             "debug font size: %d\n"
//             "Cam: (%.02f, %.02f)\n"
//             "Zoom: %.02f\n"
//             "Control mode: %s\n"
//             "Current floor: %d\n"
//             "Dungeon num floors: %d\n"
//             "Num entityids: %d\n"
//             "flag: %s\n"
//             "entity_turn: %d\n"
//             "Hero: (%d, %d)\n",
//             g->timebeganbuf,
//             g->currenttimebuf,
//             g->framecount,
//             g->last_frame_time * 1000,
//             g->debugpanel.font_size,
//             g->cam2d.offset.x,
//             g->cam2d.offset.y,
//             g->cam2d.zoom,
//             g->controlmode == CONTROLMODE_PLAYER   ? "Player"
//             : g->controlmode == CONTROLMODE_CAMERA ? "Camera"
//                                                    : "Unknown",
//             g->dungeon->current_floor,
//             g->dungeon->num_floors,
//             g->index_entityids,
//             g->flag == GAMESTATE_FLAG_PLAYER_INPUT  ? "GAMESTATE_FLAG_PLAYER_INPUT"
//             : g->flag == GAMESTATE_FLAG_PLAYER_ANIM ? "GAMESTATE_FLAG_PLAYER_ANIM"
//             : g->flag == GAMESTATE_FLAG_NONE        ? "GAMESTATE_FLAG_NONE"
//             : g->flag == GAMESTATE_FLAG_COUNT       ? "GAMESTATE_FLAG_COUNT"
//             : g->flag == GAMESTATE_FLAG_NPC_TURN    ? "GAMESTATE_FLAG_NPC_TURN"
//             : g->flag == GAMESTATE_FLAG_NPC_ANIM    ? "GAMESTATE_FLAG_NPC_ANIM"
//                                                     : "Unknown",
//
//             g->entity_turn,
//             x,
//             y);
//}

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

entity_t* const liblogic_npc_create_ptr(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {

    if (!g) {
        merror("liblogic_npc_create_ptr: gamestate is NULL");
        return NULL;
    }

    entityid id = liblogic_npc_create(g, rt, x, y, fl, name);
    if (id == -1) {
        merror("liblogic_npc_create_ptr: failed to create NPC");
        return NULL;
    }

    return em_get(g->entitymap, id);
}

entityid liblogic_npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
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
    if (rt < 0 || rt >= RACE_COUNT) {
        merror("liblogic_entity_create: race_type is out of bounds");
        return -1;
    }
    // check x and y
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    if (!df) {
        merror("liblogic_entity_create: failed to get current dungeon floor");
        return -1;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        merrorint2("liblogic_entity_create: x or y is out of bounds", x, y);
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

    if (liblogic_tile_npc_count(g, x, y, fl) > 0) {
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
    dungeon_floor_add_at(df, e->id, x, y);

    msuccessint2("Created entity at", x, y);
    return e->id;
}

//const entityid liblogic_player_create(gamestate* const g,
//                                      const race_t race_type,
//                                      const int x,
//                                      const int y,
//                                      const int floor,
//                                      const char* name) {
//
//    if (!g) {
//        merror("liblogic_player_create: gamestate is NULL");
//        return -1;
//    }
//    // use the previously-written liblogic_npc_create function
//    const entitytype_t type = ENTITY_PLAYER;
//    const entityid id = liblogic_npc_create(g, race_type, x, y, floor, name);
//    entity_t* const e = em_get(gamestate_get_entitymap(g), id);
//    if (!e) {
//        merror("liblogic_player_create: failed to get entity");
//        return -1;
//    }
//    entity_set_type(e, type);
//    gamestate_set_hero_id(g, id);
//    return id;
//}

entityid liblogic_player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {

    if (!g) {
        merror("liblogic_player_create: gamestate is NULL");
        return ENTITYID_INVALID;
    }

    if (!name) {
        merror("liblogic_player_create: name is NULL");
        return ENTITYID_INVALID;
    }

    // use the previously-written liblogic_npc_create function
    const entitytype_t type = ENTITY_PLAYER;
    const entityid id = liblogic_npc_create(g, rt, x, y, fl, name);
    entity_t* const e = em_get(gamestate_get_entitymap(g), id);
    if (!e) {
        merrorint("liblogic_player_create: failed to get entity with id", id);
        return ENTITYID_INVALID;
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
                if (entity_get_hp(target) <= 0) target->is_dead = true;
                break;
            }
        }
    }
    if (!attack_successful)
        merrorint2("liblogic_try_entity_attack: no valid target found at the specified location", target_x, target_y);
    if (attacker->type == ENTITY_PLAYER)
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    else if (attacker->type == ENTITY_NPC)
        g->flag = GAMESTATE_FLAG_NPC_ANIM;
    else
        g->flag = GAMESTATE_FLAG_NONE;
}

int liblogic_tile_npc_living_count(const gamestate* const g, int x, int y, int fl) {
    // Validate inputs
    if (!g) {
        merror("Null gamestate");
        return TILE_COUNT_ERROR;
    }
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    if (!df) {
        merrorint("Invalid floor", fl);
        return TILE_COUNT_ERROR;
    }
    const dungeon_tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        merrorint2("Invalid tile at", x, y);
        return TILE_COUNT_ERROR;
    }
    // Count living NPCs
    int count = 0;
    const int max_entities = tile->entity_max;
    for (int i = 0; i < max_entities; i++) {
        const entityid eid = tile->entities[i];
        if (eid == -1) continue;
        const entity* const e = em_get(g->entitymap, eid);
        if (!e) {
            mwarningint("Missing entity", eid); // Warning not error
            continue;
        }
        if (e->type == ENTITY_NPC && !e->is_dead) { count++; }
    }
    return count;
}
