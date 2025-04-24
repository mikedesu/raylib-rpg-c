#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "em.h"
#include "entity.h"
#include "entity_actions.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "gamestate_flag.h"
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

static inline tile_t* get_first_empty_tile_around_entity(gamestate* const g, entityid id);
static inline loc_t* get_locs_around_entity(gamestate* const g, entityid id);
static inline bool e_has_weapon(gamestate* const g, entityid id);
static inline bool e_has_shield(gamestate* const g, entityid id);
static inline bool player_on_tile(gamestate* g, int x, int y, int floor);
static inline void reset_player_block_success(gamestate* const g);
static inline void update_npc_state(gamestate* const g, entityid id);
static inline void handle_camera_zoom(gamestate* const g, const inputstate* const is);
static inline void add_message_history(gamestate* const g, const char* msg);
static inline void try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl);

static void update_player_state(gamestate* const g);
static void update_debug_panel_buffer(gamestate* const g);
static void handle_camera_move(gamestate* const g, const inputstate* const is);
static void handle_input(const inputstate* const is, gamestate* const g);
static void handle_input_camera(const inputstate* const is, gamestate* const g);
static void handle_input_player(const inputstate* const is, gamestate* const g);

static inline void add_message_history(gamestate* const g, const char* msg) {
    massert(g, "gamestate is NULL");
    massert(msg, "msg is NULL");
    massert(strlen(msg) > 0, "msg is empty");
    massert(g->msg_history.messages, "g->msg_history.messages is NULL");
    massert(g->msg_history.count >= 0, "g->msg_history.count is negative");
    if (g->msg_history.count >= g->msg_history.max_count) {
        merror("Message history full!");
        return;
    }
    strncpy(g->msg_history.messages[g->msg_history.count], msg, MAX_MSG_LENGTH - 1);
    g->msg_history.messages[g->msg_history.count][MAX_MSG_LENGTH - 1] = '\0'; // null term
    g->msg_history.count++;
}

static void add_message(gamestate* g, const char* fmt, ...) {
    massert(g, "add_message: gamestate is NULL");
    massert(fmt, "add_message: format string is NULL");
    if (g->msg_system.count >= MAX_MESSAGES) {
        mwarning("Message queue full!");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(g->msg_system.messages[g->msg_system.count], MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    add_message_history(g, g->msg_system.messages[g->msg_system.count]);
    g->msg_system.count++;
    g->msg_system.is_active = true;
}

static void update_equipped_shield_dir(gamestate* g, entity* e) {
    massert(g, "gamestate is NULL");
    massert(e, "entity is NULL");
    if (e->shield != ENTITYID_INVALID) {
        entity* shield = em_get(g->entitymap, e->shield);
        if (!shield) {
            merror("Failed to get shield entity");
            return;
        }
        shield->direction = e->direction;
        shield->do_update = true;
    }
}

static inline bool player_on_tile(gamestate* g, int x, int y, int floor) {
    massert(g, "gamestate is NULL");
    // get the tile at x y
    dungeon_floor_t* df = dungeon_get_floor(g->dungeon, 0);
    if (!df) {
        merror("failed to get dungeon floor");
        return false;
    }
    tile_t* tile = df_tile_at(df, x, y);
    if (!tile) {
        merror("failed to get tile");
        return false;
    }
    // enumerate entities and check their type
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == ENTITYID_INVALID) continue;
        entity* e = em_get(g->entitymap, tile->entities[i]);
        if (!e) {
            merror("failed to get entity");
            return false;
        }
        if (e->type == ENTITY_PLAYER) return true;
    }
    return false;
}

static inline int tile_npc_living_count(const gamestate* const g, int x, int y, int fl) {
    // Validate inputs
    massert(g, "gamestate is NULL");
    massert(fl >= 0, "floor is out of bounds");
    massert(fl < g->dungeon->num_floors, "floor is out of bounds");
    massert(x >= 0, "x is out of bounds");
    massert(x < g->dungeon->floors[fl]->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < g->dungeon->floors[fl]->height, "y is out of bounds");
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get dungeon floor");
    const tile_t* const t = df_tile_at(df, x, y);
    massert(t, "failed to get tile");
    // Count living NPCs
    int count = 0;
    for (int i = 0; i < t->entity_max; i++) {
        const entityid eid = tile_get_entity(t, i);
        if (eid == ENTITYID_INVALID) continue;
        const entity* const e = em_get(g->entitymap, eid);
        if (!e) continue;
        if (e->type == ENTITY_NPC && !e->dead) count++;
    }
    return count;
}

static void try_entity_move(gamestate* const g, entity* const e, int x, int y) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    e->do_update = true;
    e->direction = get_dir_from_xy(x, y);
    int ex = e->x + x;
    int ey = e->y + y;
    int floor = e->floor;
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("Failed to get dungeon floor");
        return;
    }
    // i feel like this might be something we can set elsewhere...like after the player input phase?
    if (e->type == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    tile_t* const tile = df_tile_at(df, ex, ey);
    if (!tile) {
        merror("Cannot move, tile is NULL");
        return;
    }
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("Cannot move, tile is not walkable");
        return;
    }
    if (tile_npc_living_count(g, ex, ey, floor) > 0) {
        merror("Cannot move, NPC in the way");
        return;
    }
    if (player_on_tile(g, ex, ey, floor)) {
        merror("Cannot move, player on tile");
        return;
    }
    if (!df_remove_at(df, e->id, e->x, e->y)) {
        merror("Failed to remove entity from old tile");
        return;
    }
    if (!df_add_at(df, e->id, ex, ey)) {
        merror("Failed to add entity to new tile");
        return;
    }
    e->x = ex;
    e->y = ey;
    e->sprite_move_x = x * DEFAULT_TILE_SIZE;
    e->sprite_move_y = y * DEFAULT_TILE_SIZE;
    // at this point the move is 'successful'
    update_equipped_shield_dir(g, e);
    // get the entity's new tile
    tile_t* const new_tile = df_tile_at(df, ex, ey);
    if (!new_tile) {
        merror("Failed to get new tile");
        return;
    }
    // do not remove!!!
    // check if the tile has a pressure plate
    //if (new_tile->has_pressure_plate) {
    //    msuccess("Pressure plate activated!");
    // do something
    // print the pressure plate event
    //msuccessint("Pressure plate event", new_tile->pressure_plate_event);
    //}
    // check if the tile is an ON TRAP
    if (new_tile->type == TILE_FLOOR_STONE_TRAP_ON_00) {
        msuccess("On trap activated!");
        // do something
        e->hp--;
        e->is_damaged = true;
        e->do_update = true;
    }
}

static void try_entity_move_player(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "hero is NULL");
    int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
    int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
    if (dx != 0 || dy != 0) try_entity_move(g, e, dx, dy);
}

static void try_entity_move_random(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    massert(e, "entity is NULL");
    int x = rand() % 3;
    int y = 0;
    if (x == 0)
        x = -1;
    else if (x == 1)
        x = 0;
    else
        x = 1;

    // if x is 0, y cannot also be 0
    if (x == 0) {
        y = rand() % 2;
        y = y == 0 ? -1 : 1;
    } else {
        y = rand() % 3;
        if (y == 0)
            y = -1;
        else if (y == 1)
            y = 0;
        else
            y = 1;
    }
    try_entity_move(g, e, x, y);
}

static inline void handle_attack_success_gamestate_flag(gamestate* const g, entitytype_t type, bool success) {
    if (!success) {
        if (type == ENTITY_PLAYER)
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        else if (type == ENTITY_NPC)
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        else
            g->flag = GAMESTATE_FLAG_NONE;
    } else if (type == ENTITY_PLAYER)
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
}

static void handle_attack_success(gamestate* const g, entity* attacker, entity* target, bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(attacker, "attacker entity is NULL");
    massert(target, "target entity is NULL");
    massert(attack_successful, "attack_successful is NULL");
    msuccess("Successful Attack on target: %s", target->name);
    *attack_successful = true;
    target->is_damaged = true;
    target->do_update = true;
    int dmg = 1;
    e_set_hp(target, e_get_hp(target) - dmg); // Reduce HP by 1
    if (target->type == ENTITY_PLAYER) add_message(g, "You took %d damage!", dmg);
    if (e_get_hp(target) <= 0) target->dead = true;
}

static void handle_attack_blocked(gamestate* const g, entity* attacker, entity* target, bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(attacker, "attacker entity is NULL");
    massert(target, "target entity is NULL");
    massert(attack_successful, "attack_successful is NULL");
    msuccess("Successful Block from target: %s", target->name);
    *attack_successful = false;
    target->is_damaged = false;
    target->block_success = true;
    target->do_update = true;
    if (target->type == ENTITY_PLAYER) add_message_history(g, "You blocked the attack!");
}

static inline bool handle_attack_helper_innerloop(gamestate* const g, tile_t* tile, int i, entity* attacker, bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(i >= 0, "i is out of bounds");
    massert(i < tile->entity_max, "i is out of bounds");
    massert(attacker, "attacker is NULL");
    massert(attack_successful, "attack_successful is NULL");

    entityid id = tile->entities[i];
    if (id == ENTITYID_INVALID) return false;

    entity* target = em_get(g->entitymap, id);
    if (!target) {
        merror("target entity is NULL");
        return false;
    }

    entitytype_t type = target->type;
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) {
        merror("target entity is not a player or NPC");
        return false;
    }

    minfo("Attacking target: %s", target->name);
    if (target->dead) {
        merror("Target is dead, cannot attack");
        return false;
    }

    if (target->is_blocking) {
        msuccess("Block successful");
        handle_attack_blocked(g, attacker, target, attack_successful);
        return false;
    }

    msuccess("Attack successful");
    handle_attack_success(g, attacker, target, attack_successful);
    return true;
}

static void handle_attack_helper(gamestate* const g, tile_t* tile, entity* attacker, bool* successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(attacker, "attacker is NULL");
    massert(successful, "attack_successful is NULL");
    for (int i = 0; i < tile->entity_max; i++) handle_attack_helper_innerloop(g, tile, i, attacker, successful);
}

static void try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y) {
    massert(g, "gamestate is NULL");
    entity* e = em_get(g->entitymap, attacker_id);
    massert(e, "attacker entity is NULL");
    if (e->dead) {
        merror("attacker entity is dead");
        return;
    }
    dungeon_floor_t* const floor = dungeon_get_floor(g->dungeon, e->floor);
    massert(floor, "failed to get dungeon floor");
    tile_t* const tile = df_tile_at(floor, target_x, target_y);
    if (!tile) {
        merror("target tile not found");
        return;
    }
    // Calculate direction based on target position
    bool ok = false;
    int dx = target_x - e->x;
    int dy = target_y - e->y;
    e->direction = get_dir_from_xy(dx, dy);
    e->is_attacking = true;
    e->do_update = true;
    handle_attack_helper(g, tile, e, &ok);
    handle_attack_success_gamestate_flag(g, e->type, ok);
}

static void try_entity_attack_random(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    massert(e, "entity is NULL");
    int x = rand() % 3;
    int y = 0;
    if (x == 0)
        x = -1;
    else if (x == 1)
        x = 0;
    else
        x = 1;

    // if x is 0, y cannot also be 0
    if (x == 0) {
        y = rand() % 2;
        if (y == 0)
            y = -1;
        else
            y = 1;
    } else {
        y = rand() % 3;
        if (y == 0)
            y = -1;
        else if (y == 1)
            y = 0;
        else
            y = 1;
    }
    try_entity_attack(g, e->id, e->x + x, e->y + y);
}

static void try_entity_attack_player(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "hero is NULL");
    int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
    int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
    if (dx != 0 || dy != 0) try_entity_attack(g, e->id, h->x, h->y);
}

static bool entities_adjacent(gamestate* const g, entityid id0, entityid id1) {
    massert(g, "liblogic_entities_adjacent: gamestate is NULL");
    entity* const e0 = em_get(g->entitymap, id0);
    if (!e0) return false;
    entity* const e1 = em_get(g->entitymap, id1);
    if (!e1) return false;
    // use e0 and check the surrounding 8 tiles
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0) continue;
            if (e0->x + x == e1->x && e0->y + y == e1->y) return true;
        }
    }
    return false;
}

static void try_entity_move_attack_player(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "hero is NULL");
    if (entities_adjacent(g, e->id, h->id))
        try_entity_attack(g, e->id, h->x, h->y);
    else
        try_entity_move_player(g, e);
}

static void try_entity_wait(gamestate* const g, entity* const e) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    if (e->type == ENTITY_PLAYER || e->type == ENTITY_NPC) e_incr_hp(e, 1);
    if (e->type == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    e->do_update = true;
}

static void execute_action(gamestate* const g, entity* const e, entity_action_t action) {
    massert(g, "gamestate is NULL");
    massert(e, "entity is NULL");
    switch (action) {
    case ENTITY_ACTION_MOVE_LEFT: try_entity_move(g, e, -1, 0); break;
    case ENTITY_ACTION_MOVE_RIGHT: try_entity_move(g, e, 1, 0); break;
    case ENTITY_ACTION_MOVE_UP: try_entity_move(g, e, 0, -1); break;
    case ENTITY_ACTION_MOVE_DOWN: try_entity_move(g, e, 0, 1); break;
    case ENTITY_ACTION_MOVE_UP_LEFT: try_entity_move(g, e, -1, -1); break;
    case ENTITY_ACTION_MOVE_UP_RIGHT: try_entity_move(g, e, 1, -1); break;
    case ENTITY_ACTION_MOVE_DOWN_LEFT: try_entity_move(g, e, -1, 1); break;
    case ENTITY_ACTION_MOVE_DOWN_RIGHT: try_entity_move(g, e, 1, 1); break;
    case ENTITY_ACTION_ATTACK_LEFT: try_entity_attack(g, e->id, e->x - 1, e->y); break;
    case ENTITY_ACTION_ATTACK_RIGHT: try_entity_attack(g, e->id, e->x + 1, e->y); break;
    case ENTITY_ACTION_ATTACK_UP: try_entity_attack(g, e->id, e->x, e->y - 1); break;
    case ENTITY_ACTION_ATTACK_DOWN: try_entity_attack(g, e->id, e->x, e->y + 1); break;
    case ENTITY_ACTION_ATTACK_UP_LEFT: try_entity_attack(g, e->id, e->x - 1, e->y - 1); break;
    case ENTITY_ACTION_ATTACK_UP_RIGHT: try_entity_attack(g, e->id, e->x + 1, e->y - 1); break;
    case ENTITY_ACTION_ATTACK_DOWN_LEFT: try_entity_attack(g, e->id, e->x - 1, e->y + 1); break;
    case ENTITY_ACTION_ATTACK_DOWN_RIGHT: try_entity_attack(g, e->id, e->x + 1, e->y + 1); break;
    case ENTITY_ACTION_MOVE_RANDOM: try_entity_move_random(g, e); break;
    case ENTITY_ACTION_WAIT: try_entity_wait(g, e); break;
    case ENTITY_ACTION_ATTACK_RANDOM: try_entity_attack_random(g, e); break;
    case ENTITY_ACTION_MOVE_PLAYER: try_entity_move_player(g, e); break;
    case ENTITY_ACTION_ATTACK_PLAYER: try_entity_attack_player(g, e); break;
    case ENTITY_ACTION_MOVE_ATTACK_PLAYER: try_entity_move_attack_player(g, e); break;
    case ENTITY_ACTION_INTERACT_DOWN_LEFT:
    case ENTITY_ACTION_INTERACT_DOWN_RIGHT:
    case ENTITY_ACTION_INTERACT_UP_LEFT:
    case ENTITY_ACTION_INTERACT_UP_RIGHT:
    case ENTITY_ACTION_INTERACT_LEFT:
    case ENTITY_ACTION_INTERACT_RIGHT:
    case ENTITY_ACTION_INTERACT_UP:
    case ENTITY_ACTION_INTERACT_DOWN:
    default: merror("Unknown entity action: %d", action); break;
    }
}

static entityid weapon_create(gamestate* const g, int x, int y, int fl, const char* name) {
    massert(g, "gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "entitymap is NULL");
    massert(name && name[0], "name is NULL or empty");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get current dungeon floor");
    massert(x >= 0, "x is out of bounds");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < df->height, "y is out of bounds");
    tile_t* const tile = df_tile_at(df, x, y);
    massert(tile, "failed to get tile");
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("cannot create entity on tile with NPC");
        return ENTITYID_INVALID;
    }
    entity* const e = e_new_weapon_at(next_entityid++, x, y, fl, name);
    if (!e) {
        merror("failed to create entity");
        return ENTITYID_INVALID;
    }
    // FIRST try to add to dungeon floor
    if (!df_add_at(df, e->id, x, y)) {
        merror("failed to add entity to dungeon floor");
        free(e); // Free immediately since EM doesn't own it yet
        return ENTITYID_INVALID;
    }
    // ONLY add to EM after dungeon placement succeeds
    em_add(gamestate_get_entitymap(g), e);
    gs_add_entityid(g, e->id);
    return e->id;
}

static entityid liblogic_shield_create(gamestate* const g, int x, int y, int fl, const char* name) {
    massert(g, "gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "entitymap is NULL");
    massert(name && name[0], "name is NULL or empty");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get current dungeon floor");
    massert(x >= 0, "x is out of bounds");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < df->height, "y is out of bounds");
    // can we create an entity at this location? no entities can be made on wall-types etc
    tile_t* const tile = df_tile_at(df, x, y);
    massert(tile, "failed to get tile");
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("cannot create entity on tile with NPC");
        return ENTITYID_INVALID;
    }
    entity* const e = e_new_shield_at(next_entityid++, x, y, fl, name);
    if (!e) {
        merror("failed to create entity");
        return ENTITYID_INVALID;
    }
    em_add(em, e);
    gs_add_entityid(g, e->id);
    if (!df_add_at(df, e->id, x, y)) {
        merror("failed to add entity to dungeon floor");
        free(e);
        return ENTITYID_INVALID;
    }
    return e->id;
}

static inline loc_t* get_locs_around_entity(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    entity* const e = em_get(g->entitymap, id);
    massert(e, "entity is NULL");
    int x = e->x;
    int y = e->y;
    loc_t* locs = malloc(sizeof(loc_t) * 8);
    if (!locs) {
        merror("failed to allocate memory for locs");
        return NULL;
    }
    int index = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) { continue; }
            locs[index].x = x + i;
            locs[index].y = y + j;
            index++;
        }
    }
    return locs;
}

static inline tile_t* get_first_empty_tile_around_entity(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    entity* const e = em_get(g->entitymap, id);
    massert(e, "entity is NULL");
    loc_t* locs = get_locs_around_entity(g, id);
    massert(locs, "locs is NULL");
    bool found = false;
    tile_t* tile = NULL;
    for (int i = 0; i < 8; i++) {
        tile = df_tile_at(g->dungeon->floors[e->floor], locs[i].x, locs[i].y);
        if (!tile) continue;
        if (!dungeon_tile_is_walkable(tile->type)) continue;
        if (tile_entity_count(tile) > 0) continue;
        // found an empty tile
        found = true;
        break;
    }
    free(locs);
    if (!found) {
        merror("no empty tile found");
        return NULL;
    }
    return tile;
}

static void init_weapon_test(gamestate* const g) {
    massert(g, "gamestate is NULL");
    dungeon_t* const d = g->dungeon;
    massert(d, "dungeon is NULL");
    dungeon_floor_t* const df = dungeon_get_floor(d, 0);
    massert(df, "dungeon floor is NULL");
    entity* const e = em_get(g->entitymap, g->hero_id);
    massert(e, "e is NULL");
    bool found = false;
    // place the shield somewhere around the player
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (found) break;
            if (i == 0 && j == 0) continue;
            int nx = e->x + i;
            int ny = e->y + j;
            tile_t* tile = df_tile_at(df, nx, ny);
            if (tile_entity_count(tile) > 0) continue;
            if (!dungeon_tile_is_walkable(tile->type)) continue;
            // create the shield
            entityid shield_id = liblogic_shield_create(g, nx, ny, 0, "shield");
            massert(shield_id != ENTITYID_INVALID, "failed to create shield");
            entity* const shield = em_get(g->entitymap, shield_id);
            massert(shield, "shield is NULL");
            // set the shield direction to the player direction
            found = true;
        }
        if (found) break;
    }

    // place the sword somewhere around the player
    found = false;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (found) break;
            if (i == 0 && j == 0) continue;
            int nx = e->x + i;
            int ny = e->y + j;
            tile_t* const tile = df_tile_at(df, nx, ny);
            if (tile_entity_count(tile) > 0) continue;
            // check if the tile is walkable
            if (!dungeon_tile_is_walkable(tile->type)) continue;
            // create the sword
            entityid sword_id = weapon_create(g, nx, ny, 0, "sword");
            massert(sword_id != ENTITYID_INVALID, "failed to create weapon");
            entity* const sword = em_get(g->entitymap, sword_id);
            massert(sword, "sword is NULL");
            found = true;
        }
        if (found) break;
    }
}

static void liblogic_init_dungeon(gamestate* const g) {
    massert(g, "gamestate is NULL");
    g->dungeon = dungeon_create();
    massert(g->dungeon, "failed to init dungeon");
    dungeon_add_floor(g->dungeon, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
}

static void liblogic_init_em(gamestate* const g) {
    massert(g, "gamestate is NULL");
    g->entitymap = em_new();
}

static entityid liblogic_npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    massert(g, "gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "entitymap is NULL");
    massert(name && name[0], "name is NULL or empty");
    massert(rt >= 0, "race_type is out of bounds");
    massert(rt < RACE_COUNT, "race_type is out of bounds");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get current dungeon floor");
    massert(x >= 0, "x is out of bounds");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < df->height, "y is out of bounds");
    // can we create an entity at this location? no entities can be made on wall-types etc
    tile_t* const tile = df_tile_at(df, x, y);
    massert(tile, "failed to get tile");
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("cannot create entity on tile with NPC");
        return ENTITYID_INVALID;
    }
    entity* const e = e_new_npc_at(next_entityid++, rt, x, y, fl,
                                   name); // Assuming entity_new_at takes name next
    if (!e) {
        merror("failed to create entity");
        return ENTITYID_INVALID;
    }
    em_add(em, e);
    gs_add_entityid(g, e->id);
    if (!df_add_at(df, e->id, x, y)) {
        merror("failed to add entity to dungeon floor");
        free(e);
        return ENTITYID_INVALID;
    }
    return e->id;
}

static entityid liblogic_player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    minfo("creating player");
    massert(g, "gamestate is NULL");
    massert(name, "name is NULL");
    // use the previously-written liblogic_npc_create function
    const entitytype_t type = ENTITY_PLAYER;
    minfo("creating player with type %d", type);
    const entityid id = liblogic_npc_create(g, rt, x, y, fl, name);
    minfo("player id: %d", id);
    minfo("getting entitymap");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "entitymap is NULL");
    entity_t* const e = em_get(em, id);
    if (!e) {
        merror("failed to get entity with id %d", id);
        return ENTITYID_INVALID;
    }
    e_set_type(e, type);
    gamestate_set_hero_id(g, id);
    return id;
}

static void init_player(gamestate* const g) {
    minfo("initializing player");
    massert(g, "gamestate is NULL");
    // setting it up so we can return a loc_t from a function
    // that can scan for an appropriate starting location
    loc_t loc = df_get_upstairs(g->dungeon->floors[g->dungeon->current_floor]);
    minfo("creating player...");
    const int id = liblogic_player_create(g, RACE_HUMAN, loc.x, loc.y, 0, "hero");
    msuccess("player id: %d", id);
    massert(id != ENTITYID_INVALID, "failed to init hero");
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "hero is NULL");
    e_set_maxhp(hero, 10);
    e_set_hp(hero, 10);
    g->entity_turn = g->hero_id;
    msuccess("hero id %d", g->hero_id);
}

static entity_t* const liblogic_npc_create_ptr(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    massert(g, "gamestate is NULL");
    entityid id = liblogic_npc_create(g, rt, x, y, fl, name);
    if (id == ENTITYID_INVALID) {
        merror("failed to create NPC");
        return NULL;
    }
    return em_get(g->entitymap, id);
}

static int liblogic_tile_npc_count(gamestate* const g, dungeon_floor_t* const df, tile_t* const tile) {
    massert(g, "gamestate is NULL");
    massert(df, "dungeon floor is NULL");
    massert(tile, "failed to get tile");

    const int t_entity_count = tile_entity_count(tile);
    int tile_npc_count = 0;

    for (int i = 0; i < t_entity_count; i++) {
        entityid id = tile_get_entity(tile, i);
        massert(id != ENTITYID_INVALID, "entity id is invalid");
        entity* const e = em_get(g->entitymap, id);
        massert(e, "entity is NULL");
        if (e->type == ENTITY_NPC) tile_npc_count++;
    }
    return tile_npc_count;
}

static int liblogic_tile_npc_count_xy(gamestate* const g, dungeon_floor_t* const df, int x, int y) {
    massert(g, "gamestate is NULL");
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is out of bounds");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < df->height, "y is out of bounds");

    tile_t* const tile = df_tile_at(df, x, y);
    massert(tile, "failed to get tile");

    return liblogic_tile_npc_count(g, df, tile);
}

static void liblogic_init_orcs_test_intermediate(gamestate* const g) {
    massert(g, "gamestate is NULL");
    dungeon_t* const d = g->dungeon;
    massert(d, "dungeon is NULL");
    dungeon_floor_t* const df = dungeon_get_floor(d, 0);
    massert(df, "dungeon floor is NULL");
    // first, we need to count all of the possible tiles we could place an orc on
    // we can loop thru each tile on the dungeon floor and check if it is walkable
    // in the beginning there wont be any entities at all so we are just counting total possible locations right now
    // in order to prepare a list of them
    const int count = df_count_walkable(df);
    // now we have the total number of possible locations
    // we can create an array of size count
    loc_t* locations = malloc(sizeof(loc_t) * count);
    massert(locations, "failed to malloc locations");
    int count2 = 0;
    // now we can loop thru the dungeon floor again and fill the array with the locations
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* const tile = df_tile_at(df, x, y);
            if (dungeon_tile_is_walkable(tile->type)) {
                // there wont be any entities yet so do not check for them
                // do not write an if statement
                locations[count2].x = x;
                locations[count2].y = y;
                count2++;
                massert(count2 <= count, "count2 is greater than count");
            }
        }
    }
    massert(count2 == count, "count2 is greater than count");
    // now we can loop thru the array and create an orc at each location
    int max_orcs = 1;
    //int max_orcs = count2;

    int created_orcs = 0;
    int i = 0;
    massert(max_orcs < count2, "max_orcs is greater than count2");

    while (created_orcs < max_orcs && i < count2) {
        int tile_npc_count = liblogic_tile_npc_count_xy(g, df, locations[i].x, locations[i].y);
        if (tile_npc_count > 0) {
            //merror("liblogic_init: tile has %d NPCs", tile_npc_count);
            i++;
            continue;
        }

        // also cant spawn on top of a player
        entity* const player = em_get(g->entitymap, g->hero_id);
        massert(player, "liblogic_init: player is NULL");
        if (locations[i].x == player->x && locations[i].y == player->y) {
            merror("liblogic_init: cannot spawn on top of player");
            i++;
            continue;
        }
        entity* const orc = liblogic_npc_create_ptr(g, RACE_ORC, locations[i].x, locations[i].y, 0, "orc");
        massert(orc, "liblogic_init: failed to create orc");
        entity_action_t action = ENTITY_ACTION_MOVE_ATTACK_PLAYER;
        e_set_default_action(orc, action);
        e_set_maxhp(orc, 1);
        e_set_hp(orc, 1);
        i++;
        created_orcs++;
    }
    // we need to free the locations array
    free(locations);
}

static void init_orcs_test(gamestate* const g) {
    //liblogic_init_orcs_test_naive_loop(g);
    liblogic_init_orcs_test_intermediate(g);
}

static const char* liblogic_get_action_key(const inputstate* const is, gamestate* const g) {
    const int key = inputstate_get_pressed_key(is);
    //if (key != -1) { minfo("Key pressed: %d", key); }
    // can return early if key == -1
    if (key == -1) return "none";
    return get_action_for_key(&g->keybinding_list, key);
}

static void handle_camera_move(gamestate* const g, const inputstate* const is) {
    const float move = g->cam2d.zoom;
    const char* action = liblogic_get_action_key(is, g);
    if (!action) {
        merror("No action found for key");
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
    if (strcmp(action, "toggle_camera") == 0) {
        g->cam2d.zoom = roundf(g->cam2d.zoom);
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }
}

static inline void handle_camera_zoom(gamestate* const g, const inputstate* const is) {
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

static void handle_input_camera(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    handle_camera_move(g, is);
    handle_camera_zoom(g, is);
}

static inline void liblogic_change_player_dir(gamestate* const g, direction_t dir) {
    massert(g, "Game state is NULL!");
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "liblogic_change_player_dir: hero is NULL");
    if (hero->dead) return;
    hero->direction = dir;
    hero->do_update = true;
    update_equipped_shield_dir(g, hero);
}

static void try_entity_block(gamestate* const g, entity* const e) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    if (!e_has_shield(g, e->id)) {
        add_message(g, "You have no shield to block with!");
        return;
    }
    e->do_update = true;
    e->is_blocking = true;
    g->test_guard = true;
    update_equipped_shield_dir(g, e);
    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
}

static bool try_entity_pickup(gamestate* const g, entity* const e) {
    minfo("try_entity_pickup: trying to pick up item");
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    e->do_update = true;
    // check if the player is on a tile with an item
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, e->floor);
    if (!df) {
        merror("Failed to get dungeon floor");
        return false;
    }
    tile_t* const tile = df_tile_at(df, e->x, e->y);
    if (!tile) {
        merror("Failed to get tile");
        return false;
    }
    if (tile->entity_count == 0) {
        merror("No items on tile");
        return false;
    }
    for (int i = 0; i < tile->entity_count; i++) {
        entityid id = tile->entities[i];
        entity* it = em_get(g->entitymap, id);
        if (!it) {
            merror("Failed to get entity");
            return false;
        }
        if (it->type == ENTITY_WEAPON) {
            // pick up the item
            // picking up an item requires:
            // 1. removing its id from the tile
            // 2. adding its id to the entity inventory
            // remove the item from the tile
            add_message(g, "Picked up %s", it->name);
            tile_remove(tile, id);
            // add the item to the entity inventory
            e_add_item_to_inventory(e, id);
            e->weapon = id;
            msuccess("Picked up item: %s", it->name);
            if (e->type == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        } else if (it->type == ENTITY_SHIELD) {
            // pick up the item
            // picking up an item requires:
            // 1. removing its id from the tile
            // 2. adding its id to the entity inventory
            // remove the item from the tile
            add_message(g, "Picked up %s", it->name);
            tile_remove(tile, id);
            // add the item to the entity inventory
            e_add_item_to_inventory(e, id);
            update_equipped_shield_dir(g, e);
            e->shield = id;
            msuccess("Picked up item: %s", it->name);
            if (e->type == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
    }
    add_message(g, "No items to pick up");
    return false;
}

static inline void try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "Failed to get dungeon floor");
    tile_t* const tile = df_tile_at(df, x, y);
    massert(tile, "Failed to get tile");
    if (tile->has_wall_switch) {
        tile->wall_switch_on = !tile->wall_switch_on;
        msuccess("Wall switch flipped!");
        int ws_event = tile->wall_switch_event;
        if (ws_event <= -1) {
            merror("Wall switch event is invalid");
            return;
        }
        df_event_t event = df->events[ws_event];
        tile_t* const event_tile = df_tile_at(df, event.x, event.y);
        massert(event_tile, "Failed to get event tile");
        tiletype_t type = event_tile->type;
        if (type == event.off_type)
            event_tile->type = event.on_type;
        else if (type == event.on_type)
            event_tile->type = event.off_type;
        //add_message(g, "Wall switch flipped!");
        // this is the basis for what we need to do next
        // currently we have no entity passed into this function
        // we need to fix that
        if (e->type == ENTITY_PLAYER)
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        else if (e->type == ENTITY_NPC)
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        else
            g->flag = GAMESTATE_FLAG_NONE;
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

static inline bool e_has_weapon(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity ID is invalid");
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merror(" entity not found");
        return false;
    }
    entityid id0 = e->weapon;
    if (id0 == ENTITYID_INVALID) {
        merror("entity has no weapon");
        return false;
    }
    return true;
}

static inline bool e_has_shield(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity ID is invalid");
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merror("entity not found");
        return false;
    }
    entityid id0 = e->shield;
    if (id0 == ENTITYID_INVALID) {
        merror("entity has no shield");
        return false;
    }
    return true;
}

static void handle_input_player(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) { return; }
    const char* action = liblogic_get_action_key(is, g);
    if (!action) {
        merror("1 No action found for key");
        return;
    }
    if (g->msg_system.is_active) {
        if (strcmp(action, "attack") == 0 || strcmp(action, "pickup") == 0) {
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
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "liblogic_handle_input_player: hero is NULL");
    // check if the player is dead
    if (hero->dead) return;
    if (action) {
        if (g->player_changing_direction) {
            if (strcmp(action, "wait") == 0) {
                execute_action(g, hero, ENTITY_ACTION_WAIT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_w") == 0) {
                liblogic_change_player_dir(g, DIR_LEFT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_e") == 0) {
                liblogic_change_player_dir(g, DIR_RIGHT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_s") == 0) {
                liblogic_change_player_dir(g, DIR_DOWN);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_n") == 0) {
                liblogic_change_player_dir(g, DIR_UP);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_nw") == 0) {
                liblogic_change_player_dir(g, DIR_UP_LEFT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_ne") == 0) {
                liblogic_change_player_dir(g, DIR_UP_RIGHT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_sw") == 0) {
                liblogic_change_player_dir(g, DIR_DOWN_LEFT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_se") == 0) {
                liblogic_change_player_dir(g, DIR_DOWN_RIGHT);
                g->player_changing_direction = false;
            }
            return;
        }
        if (strcmp(action, "wait") == 0) {
            g->player_changing_direction = true;
        } else if (strcmp(action, "move_w") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_LEFT);
        } else if (strcmp(action, "move_e") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_RIGHT);
        } else if (strcmp(action, "move_n") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_UP);
        } else if (strcmp(action, "move_s") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_DOWN);
        } else if (strcmp(action, "move_nw") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_UP_LEFT);
        } else if (strcmp(action, "move_ne") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_UP_RIGHT);
        } else if (strcmp(action, "move_sw") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_DOWN_LEFT);
        } else if (strcmp(action, "move_se") == 0) {
            execute_action(g, hero, ENTITY_ACTION_MOVE_DOWN_RIGHT);
        } else if (strcmp(action, "attack") == 0) {
            msuccess("attack pressed!");
            if (e_has_weapon(g, hero->id)) {
                msuccess("Entity has weapon");
                int tx = hero->x + get_x_from_dir(hero->direction);
                int ty = hero->y + get_y_from_dir(hero->direction);
                try_entity_attack(g, hero->id, tx, ty);
            } else {
                merror("Entity has no weapon");
                add_message(g, "You have no weapon to attack with!");
                // add a message to the message system
            }
        } else if (strcmp(action, "block") == 0) {
            msuccess("Block pressed!");
            try_entity_block(g, hero);
        } else if (strcmp(action, "interact") == 0) {
            // we are hardcoding the flip switch interaction for now
            // but eventually this will be generalized
            // for instance u can talk to an NPC merchant using "interact"
            // or open a door, etc
            msuccess("Space pressed!");
            int tx = hero->x + get_x_from_dir(hero->direction);
            int ty = hero->y + get_y_from_dir(hero->direction);
            try_flip_switch(g, hero, tx, ty, hero->floor);
        } else if (strcmp(action, "pickup") == 0) {
            try_entity_pickup(g, hero);
        } else if (strcmp(action, "toggle_camera") == 0) {
            g->controlmode = CONTROLMODE_CAMERA;
        }
    } else {
        merror("No action found for key");
    }
}

static void handle_input(const inputstate* const is, gamestate* const g) {
    massert(is, "handle_input: inputstate is NULL");
    massert(g, "handle_input: gamestate is NULL");
    // no matter which mode we are in, we can toggle the debug panel
    if (inputstate_is_pressed(is, KEY_D)) {
        msuccess("D pressed!");
        g->debugpanelon = !g->debugpanelon;
    }
    if (g->controlmode == CONTROLMODE_PLAYER)
        handle_input_player(is, g);
    else if (g->controlmode == CONTROLMODE_CAMERA)
        handle_input_camera(is, g);
    else
        merror("Unknown control mode");
}

static void update_debug_panel_buffer(gamestate* const g) {
    massert(g, "gamestate is NULL");
    // Static buffers to avoid reallocating every frame
    static const char* control_modes[] = {"Player", "Camera", "Unknown"};
    static const char* flag_names[] = {"GAMESTATE_FLAG_NONE",
                                       "GAMESTATE_FLAG_PLAYER_INPUT",
                                       "GAMESTATE_FLAG_PLAYER_ANIM",
                                       "GAMESTATE_FLAG_COUNT",
                                       "GAMESTATE_FLAG_NPC_TURN",
                                       "GAMESTATE_FLAG_NPC_ANIM",
                                       "Unknown"};
    // Get hero position once
    const entity* const e = em_get(g->entitymap, g->hero_id);
    int x = -1;
    int y = -1;
    int inventory_count = -1;
    entityid shield_id = -1;
    direction_t player_dir = DIR_NONE;
    direction_t shield_dir = DIR_NONE;
    bool is_b = false;
    bool test_guard = g->test_guard;
    if (e) {
        x = e->x;
        y = e->y;
        inventory_count = e->inventory_count;
        player_dir = e->direction;
        shield_id = e->shield;
        if (shield_id != -1) {
            entity* const shield = em_get(g->entitymap, shield_id);
            massert(shield, "shield is NULL");
            shield_dir = shield->direction;
        }
        is_b = e->is_blocking;
    }
    // Determine control mode and flag strings
    const char* control_mode = control_modes[(g->controlmode >= 0 && g->controlmode < 2) ? g->controlmode : 2];
    const char* flag_name = flag_names[(g->flag >= GAMESTATE_FLAG_NONE && g->flag < GAMESTATE_FLAG_COUNT) ? g->flag : GAMESTATE_FLAG_COUNT];
    // zero out the buffer
    memset(g->debugpanel.buffer, 0, sizeof(g->debugpanel.buffer));
    // Format the string in one pass
    snprintf(g->debugpanel.buffer,
             sizeof(g->debugpanel.buffer),
             "%s\n" // timebeganbuf
             "%s\n" // currenttimebuf
             "Frame: %d\n"
             "Draw Time: %.1fms\n"
             "Is3D: %d\n"
             "Cam: (%.0f,%.0f) Zoom: %.1f\n"
             "Mode: %s | Floor: %d/%d\n"
             "Entities: %d | Flag: %s\n"
             "Turn: %d | Hero: (%d,%d)\n"
             "Inventory: %d\n"
             "msg_history.count: %d\n"
             "shield_dir_str: %s\n"
             "player_dir_str: %s\n"
             "is_blocking: %d\n"
             "test_guard: %d\n"
             "TEST 12345\n"
             "block_success: %d\n"
             "TEST 66666\n",
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
             x,
             y,
             inventory_count,
             g->msg_history.count,
             get_dir_as_string(shield_dir),
             get_dir_as_string(player_dir),
             is_b,
             test_guard,
             e->block_success);
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
    init_player(g);
    // test to create a weapon
    init_weapon_test(g);
    // temporarily disabling
    init_orcs_test(g);
    update_debug_panel_buffer(g);
}

static void update_player_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    massert(e, "liblogic_update_player_state: hero is NULL");
    if (!g->gameover) {
        if (e->dead) {
            add_message(g, "You died!");
            g->gameover = true;
        }
        return;
    }
    if (e->dead) return;
    if (e_get_hp(e) <= 0) {
        e->dead = true;
        e->do_update = true;
        return;
    }
}

static inline void update_npc_state(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, id);
    massert(e, "update_npc_state: entity is NULL");
    if (e->dead) return;
    if (e->hp <= 0) {
        e->dead = true;
        e->do_update = true;
        merror("NPC is dead!");
        return;
    }
}

static void update_npcs_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    for (int i = 0; i < g->index_entityids; i++) {
        entityid id = g->entityids[i];
        if (id == g->hero_id) continue;
        update_npc_state(g, id);
    }
}

static void handle_nth_npc(gamestate* const g, int i) {
    massert(g, "Game state is NULL!");
    massert(i >= 0, "Index is out of bounds!");
    massert(i < g->index_entityids, "Index is out of bounds!");
    entityid id = g->entityids[i];
    if (id == g->hero_id) return; // Skip the hero
    entity* e = em_get(g->entitymap, id);
    massert(e, "liblogic_handle_nth_npc: entity is NULL");
    if (e->type == ENTITY_NPC && !e->dead) execute_action(g, e, e->default_action);
}

static void handle_npcs(gamestate* const g) {
    massert(g, "Game state is NULL!");
    massert(g->flag == GAMESTATE_FLAG_NPC_TURN, "Game state is not in NPC turn!");
    // Process all NPCs
    for (int i = 0; i < g->index_entityids; i++) handle_nth_npc(g, i);
    // After processing all NPCs, set the flag to animate all movements together
    g->flag = GAMESTATE_FLAG_NPC_ANIM;
}

static void reset_player_blocking(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    massert(e, "hero is NULL");
    e->is_blocking = false;
    g->test_guard = false;
}

static inline void reset_player_block_success(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    massert(e, "liblogic_reset_player_block_success: hero is NULL");
    e->block_success = false;
}

void liblogic_tick(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");

    update_player_state(g);
    update_npcs_state(g);

    if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
        reset_player_blocking(g);
        reset_player_block_success(g);
    }

    handle_input(is, g);

    if (g->flag == GAMESTATE_FLAG_NPC_TURN) handle_npcs(g);

    update_debug_panel_buffer(g);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}

void liblogic_close(gamestate* const g) {
    massert(g, "liblogic_close: gamestate is NULL");

    // free the entitymap
    em_free(g->entitymap);

    // free the dungeon
    dungeon_free(g->dungeon);
}
