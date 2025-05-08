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
#include "path_node.h"
#include "potiontype.h"
#include "race.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

static entityid next_entityid = 0; // Start at 0, increment for each new entity

static inline size_t tile_npc_count_at(gamestate* const g, dungeon_floor_t* const df, int x, int y);

static inline tile_t* get_first_empty_tile_around_entity(gamestate* const g, entityid id);

static inline bool is_traversable(gamestate* const g, int x, int y, int z);

static inline void reset_player_blocking(gamestate* const g);
static inline void reset_player_block_success(gamestate* const g);
static inline void update_npc_state(gamestate* const g, entityid id);
static inline void handle_camera_zoom(gamestate* const g, const inputstate* const is);
static inline void try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl);

static loc_t* get_empty_locs(dungeon_floor_t* const df, int* count);
static loc_t* get_walkable_locs(dungeon_floor_t* df, int* cnt);
static loc_t* get_empty_non_wall_locs(dungeon_floor_t* const df, int* count);
static loc_t* get_empty_non_wall_locs_in_area(dungeon_floor_t* const df, int* count, int x0, int y0, int w, int h);
static loc_t* get_locs_around_entity(gamestate* const g, entityid id);

static entity* const get_door_from_tile(const gamestate* const g, int x, int y, int fl);
static entity* create_shield(gamestate* g);
static entity* create_sword(gamestate* g);
static entity* create_sword_at(gamestate* g, loc_t loc);
static entity* create_shield_at(gamestate* g, loc_t loc);
static entity* create_elf_at(gamestate* g, loc_t loc);
static entity* create_orc_at(gamestate* g, loc_t loc);
static entity* create_dwarf_at(gamestate* g, loc_t loc);
static entity* create_human_at(gamestate* g, loc_t loc);
static entity* create_goblin_at(gamestate* g, loc_t loc);
static entity* create_halfling_at(gamestate* g, loc_t loc);

static void init_potion_test(gamestate* const g, potiontype_t potion_type, const char* name);
static void init_npcs_test_by_room(gamestate* const g);

static void init_em(gamestate* const g);
static void init_dungeon(gamestate* const g);
static void update_player_state(gamestate* const g);
static void update_debug_panel_buffer(gamestate* const g);
static void handle_camera_move(gamestate* const g, const inputstate* const is);
static void handle_input(const inputstate* const is, gamestate* const g);
static void handle_input_camera(const inputstate* const is, gamestate* const g);
static void handle_input_player(const inputstate* const is, gamestate* const g);
static void add_message_history(gamestate* const g, const char* fmt, ...);
static void add_message_and_history(gamestate* g, const char* fmt, ...);
static void add_message(gamestate* g, const char* fmt, ...);
static void try_entity_open_door(gamestate* g, entity* e, int x, int y);
static void try_entity_move_a_star(gamestate* const g, entity* const e);
static void try_entity_move(gamestate* const g, entity* const e, int x, int y);
static void try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y);

static const char* get_action_key(const inputstate* const is, gamestate* const g);

static entityid player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);
static entityid create_potion_at(gamestate* const g, potiontype_t potion_type, const char* name, loc_t loc);
static entityid door_create(gamestate* const g, int x, int y, int fl, const char* name);
static entityid npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);

static loc_t get_random_empty_non_wall_loc_in_area(gamestate* const g, int floor, int x, int y, int w, int h);
static loc_t get_random_empty_non_wall_loc(gamestate* const g, int floor);

static bool entities_adjacent(gamestate* const g, entityid id0, entityid id1);
static bool player_on_tile(gamestate* g, int x, int y, int floor);
static bool tile_has_closed_door(const gamestate* const g, int x, int y, int fl);
static bool tile_has_door(const gamestate* const g, int x, int y, int fl);

static void add_message_history(gamestate* const g, const char* fmt, ...) {
    massert(g, "gamestate is NULL");
    massert(fmt, "fmt is NULL");
    massert(g->msg_history.messages, "g->msg_history.messages is NULL");
    massert(g->msg_history.count >= 0, "g->msg_history.count is negative");
    if (g->msg_history.count >= g->msg_history.max_count) {
        merror("Message history full!");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(g->msg_history.messages[g->msg_history.count], MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    g->msg_history.messages[g->msg_history.count][MAX_MSG_LENGTH - 1] = '\0'; // null term
    g->msg_history.count++;
}

static void add_message_and_history(gamestate* g, const char* fmt, ...) {
    massert(g, "gamestate is NULL");
    massert(fmt, "format string is NULL");
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

static void add_message(gamestate* g, const char* fmt, ...) {
    massert(g, "gamestate is NULL");
    massert(fmt, "format string is NULL");
    if (g->msg_system.count >= MAX_MESSAGES) {
        mwarning("Message queue full!");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(g->msg_system.messages[g->msg_system.count], MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    g->msg_system.count++;
    g->msg_system.is_active = true;
}

static void update_equipped_shield_dir(gamestate* g, entity* e) {
    massert(g, "gamestate is NULL");
    massert(e, "entity is NULL");
    if (e->shield != ENTITYID_INVALID) {
        entity* shield = em_get(g->entitymap, e->shield);
        if (shield) {
            shield->direction = e->direction;
            shield->do_update = true;
        }
    }
}

static bool player_on_tile(gamestate* g, int x, int y, int floor) {
    massert(g, "gamestate is NULL");
    // get the tile at x y
    dungeon_floor_t* df = dungeon_get_floor(g->dungeon, 0);
    massert(df, "dungeon floor is NULL");
    tile_t* tile = df_tile_at(df, x, y);
    if (!tile) { return false; }
    // enumerate entities and check their type
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == ENTITYID_INVALID) continue;
        entity* e = em_get(g->entitymap, tile->entities[i]);
        massert(e, "failed to get entity");
        if (e->type == ENTITY_PLAYER) return true;
    }
    return false;
}

static bool tile_has_door(const gamestate* const g, int x, int y, int fl) {
    // Validate inputs
    massert(g, "gamestate is NULL");
    massert(fl >= 0, "floor is out of bounds");
    massert(fl < g->dungeon->num_floors, "floor is out of bounds");
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get dungeon floor");
    const tile_t* const t = df_tile_at(df, x, y);
    massert(t, "failed to get tile");
    // check for a door entity
    for (int i = 0; i < t->entity_max; i++) {
        const entityid eid = tile_get_entity(t, i);
        if (eid == ENTITYID_INVALID) continue;
        const entity* const e = em_get(g->entitymap, eid);
        if (!e) continue;
        if (e->type == ENTITY_DOOR) return true;
    }
    return false;
}

static entity* const get_door_from_tile(const gamestate* const g, int x, int y, int fl) {
    // Validate inputs
    massert(g, "gamestate is NULL");
    massert(fl >= 0, "floor is out of bounds");
    massert(fl < g->dungeon->num_floors, "floor is out of bounds");
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get dungeon floor");
    const tile_t* const t = df_tile_at(df, x, y);
    massert(t, "failed to get tile");
    // check for a door entity
    for (int i = 0; i < t->entity_max; i++) {
        const entityid eid = tile_get_entity(t, i);
        if (eid == ENTITYID_INVALID) continue;
        entity* const e = em_get(g->entitymap, eid);
        if (!e) continue;
        if (e->type == ENTITY_DOOR) return e;
    }
    return NULL;
}

static bool tile_has_closed_door(const gamestate* const g, int x, int y, int fl) {
    // Validate inputs
    massert(g, "gamestate is NULL");
    massert(fl >= 0, "floor is out of bounds");
    massert(fl < g->dungeon->num_floors, "floor is out of bounds");
    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get dungeon floor");
    const tile_t* const t = df_tile_at(df, x, y);
    massert(t, "failed to get tile");
    // check for a door entity
    for (int i = 0; i < t->entity_max; i++) {
        const entityid eid = tile_get_entity(t, i);
        if (eid == ENTITYID_INVALID) continue;
        const entity* const e = em_get(g->entitymap, eid);
        if (!e) continue;
        if (e->type == ENTITY_DOOR && !e->door_is_open) return true;
    }
    return false;
}

static inline int tile_npc_living_count(const gamestate* const g, int x, int y, int fl) {
    // Validate inputs
    massert(g, "gamestate is NULL");
    massert(fl >= 0, "floor is out of bounds");
    massert(fl < g->dungeon->num_floors, "floor is out of bounds");
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
    if (!tile_is_walkable(tile->type)) {
        merror("Cannot move, tile is not walkable");
        return;
    }
    if (tile_has_closed_door(g, ex, ey, floor)) {
        merror("Cannot move, tile has a closed door");
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
        e->stats.hp--;
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

static void try_entity_move_a_star(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    massert(e, "entity is NULL");
    if (e->target_path) { e_free_target_path(e); }
    // for testing, we will hardcode an update to the entity's target
    // realistically, we should actually use a target ID and do location lookups on every update
    // however, for this test, we will instead hardcode the target to point to the hero's location
    // first, grab the hero id and then the hero entity pointer
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "hero is NULL");
    // set the target to the hero's location
    e->target.x = h->x;
    e->target.y = h->y;
    e->target_path = find_path((loc_t){e->x, e->y, e->floor}, e->target, g->dungeon->floors[g->dungeon->current_floor], &e->target_path_length);
    if (e->target_path) {
        if (e->target_path_length > 0) {
            // instead of grabbing index 0, which is the target destination, AND
            // instead of grabbing index target_path_length -1, which is the entity's current location,
            // we want to grab the second to last index, which is the next location to move to
            if (e->target_path_length >= 2) {
                loc_t loc = e->target_path[e->target_path_length - 2];
                int dx = loc.x - e->x;
                int dy = loc.y - e->y;

                if (entities_adjacent(g, e->id, h->id)) {
                    // if the entity is adjacent to the hero, try to attack
                    try_entity_attack(g, e->id, loc.x, loc.y);
                } else {
                    // if the entity is not adjacent to the hero, try to move
                    // there might be a door in the way...

                    if (tile_has_closed_door(g, loc.x, loc.y, e->floor)) {
                        try_entity_open_door(g, e, loc.x, loc.y);
                    } else {
                        try_entity_move(g, e, dx, dy);
                    }
                }
            }
            //else if (e->target_path_length == 1) {
            // we are at the destination
            //    minfo("Entity is at the destination");
            //} else {
            // find path could not return a valid path
            //    merror("find_path returned an invalid path");
            //}
        }
    }
}

static void try_entity_move_random(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    massert(e, "entity is NULL");
    int x = rand() % 3;
    int y = 0;
    x = x == 0 ? -1 : x == 1 ? 0 : 1;
    // if x is 0, y cannot also be 0
    if (x == 0) {
        y = rand() % 2 == 0 ? -1 : 1;
    } else {
        y = rand() % 3;
        y = y == 0 ? -1 : y == 1 ? 0 : 1;
    }
    try_entity_move(g, e, x, y);
}

static inline void handle_attack_success_gamestate_flag(gamestate* const g, entitytype_t type, bool success) {
    if (!success) {
        if (type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (type == ENTITY_NPC) {
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        } else {
            g->flag = GAMESTATE_FLAG_NONE;
        }
    } else if (type == ENTITY_PLAYER) {
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
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
    //if (target->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked you for %d damage!", attacker->name, dmg);
    //if (attacker->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked %s for %d damage!", attacker->name, target->name, dmg);
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
    //if (target->type == ENTITY_PLAYER) { add_message_and_history(g, "%s blocked %s's attack!", target->name, attacker->name); }
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
    massert(target, "target entity is NULL");

    entitytype_t type = target->type;
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) { return false; }

    if (target->dead) { return false; }

    // lets try an experiment...
    if (target->shield != ENTITYID_INVALID) {
        // introducing a random chance to block if you have a shield...
        int block_chance = rand() % 100;

        if (block_chance < 50) {
            msuccess("Block successful");
            handle_attack_blocked(g, attacker, target, attack_successful);
            return false;
        }
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
    massert(!e->dead, "attacker entity is dead");
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
    x = x == 0 ? -1 : x == 1 ? 0 : 1;
    // if x is 0, y cannot also be 0
    if (x == 0) {
        y = rand() % 2;
        y = y == 0 ? -1 : 1;
    } else {
        y = rand() % 3;
        y = y == 0 ? -1 : y == 1 ? 0 : 1;
    }
    try_entity_attack(g, e->id, e->x + x, e->y + y);
}

static void try_entity_attack_player(gamestate* const g, entity* const e) {
    massert(g, "gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "hero is NULL");
    int dx = h->x > e->x ? 1 : h->x < e->x ? -1 : 0;
    int dy = h->y > e->y ? 1 : h->y < e->y ? -1 : 0;
    if (dx != 0 || dy != 0) try_entity_attack(g, e->id, h->x, h->y);
}

static bool entities_adjacent(gamestate* const g, entityid id0, entityid id1) {
    massert(g, "gamestate is NULL");
    massert(id0 != ENTITYID_INVALID, "id0 is invalid");
    massert(id1 != ENTITYID_INVALID, "id1 is invalid");
    massert(id0 != id1, "id0 and id1 are the same");

    entity* const e0 = em_get(g->entitymap, id0);
    massert(e0, "liblogic_entities_adjacent: e0 is NULL");
    entity* const e1 = em_get(g->entitymap, id1);
    massert(e1, "liblogic_entities_adjacent: e1 is NULL");
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
    if (entities_adjacent(g, e->id, h->id)) {
        try_entity_attack(g, e->id, h->x, h->y);
    } else {
        try_entity_move_player(g, e);
    }
}

static void try_entity_wait(gamestate* const g, entity* const e) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
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
    case ENTITY_ACTION_MOVE_A_STAR: try_entity_move_a_star(g, e); break;
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

static entityid create_potion_at(gamestate* const g, potiontype_t potion_type, const char* name, loc_t loc) {
    massert(g, "gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "entitymap is NULL");
    massert(name && name[0], "name is NULL or empty");
    massert(potion_type >= 0, "potion_type is NONE or less than 0");
    massert(potion_type < POTION_COUNT, "potion_type is out of bounds");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, loc.z);
    massert(df, "failed to get current dungeon floor");
    massert(loc.x >= 0, "x is out of bounds");
    massert(loc.x < df->width, "x is out of bounds");
    massert(loc.y >= 0, "y is out of bounds");
    massert(loc.y < df->height, "y is out of bounds");
    tile_t* const tile = df_tile_at(df, loc.x, loc.y);
    massert(tile, "failed to get tile");
    if (!tile_is_walkable(tile->type)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("cannot create entity on tile with NPC");
        return ENTITYID_INVALID;
    }
    entity* const e = e_new_potion_at(next_entityid++, potion_type, name, loc.x, loc.y, loc.z);
    if (!e) {
        merror("failed to create entity");
        return ENTITYID_INVALID;
    }
    // FIRST try to add to dungeon floor
    if (!df_add_at(df, e->id, loc.x, loc.y)) {
        merror("failed to add entity to dungeon floor");
        free(e); // Free immediately since EM doesn't own it yet
        return ENTITYID_INVALID;
    }
    // ONLY add to EM after dungeon placement succeeds
    em_add(gamestate_get_entitymap(g), e);
    gs_add_entityid(g, e->id);
    return e->id;
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
    if (!tile_is_walkable(tile->type)) {
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

static entityid shield_create(gamestate* const g, int x, int y, int fl, const char* name) {
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
    if (!tile_is_walkable(tile->type)) {
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

static loc_t* get_locs_around_entity(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    entity* const e = em_get(g->entitymap, id);
    massert(e, "entity is NULL");
    loc_t* locs = malloc(sizeof(loc_t) * 8);
    massert(locs, "failed to allocate memory for locs");
    int index = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) { continue; }
            locs[index] = (loc_t){e->x + i, e->y + j};
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
        if (!tile_is_walkable(tile->type)) continue;
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

static entity* create_shield(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = next_entityid++;
    entity* e = e_new_shield(id, "shield");
    massert(e, "failed to create shield");
    em_add(g->entitymap, e);
    gs_add_entityid(g, id);
    return e;
}

static entity* create_shield_at(gamestate* g, loc_t loc) {
    entityid id = shield_create(g, loc.x, loc.y, 0, "shield");
    massert(id != ENTITYID_INVALID, "shield create fail");
    entity* s = em_get(g->entitymap, id);
    massert(s, "shield is NULL");
    return s;
}

static entity* create_sword(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = next_entityid++;
    entity_t* e = e_new_weapon(id, "sword");
    massert(e, "failed to create sword");
    em_add(g->entitymap, e);
    gs_add_entityid(g, id);
    return e;
}

static entity* create_sword_at(gamestate* g, loc_t loc) {
    entityid id = weapon_create(g, loc.x, loc.y, 0, "sword");
    massert(id != ENTITYID_INVALID, "weapon create fail");
    entity* s = em_get(g->entitymap, id);
    massert(s, "sword is NULL");
    return s;
}

static void init_shield_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    dungeon_t* d = g->dungeon;
    massert(d, "dungeon is NULL");
    dungeon_floor_t* df = dungeon_get_floor(d, 0);
    massert(df, "floor is NULL");
    entity* e = em_get(g->entitymap, g->hero_id);
    massert(e, "e is NULL");

    loc_t loc = get_random_empty_non_wall_loc(g, g->dungeon->current_floor);
    create_shield_at(g, loc);
}

static void init_shield_test2(gamestate* g) {
    massert(g, "gamestate is NULL");
    // get the hero entity
    entity* e = em_get(g->entitymap, g->hero_id);
    massert(e, "hero entity is NULL");
    entity* shield = create_shield(g);
    e_add_item_to_inventory(e, shield->id);
    e->shield = shield->id;
}

static void init_potion_test(gamestate* const g, potiontype_t potion_type, const char* name) {
    massert(g, "gamestate is NULL");
    dungeon_t* d = g->dungeon;
    massert(d, "dungeon is NULL");
    dungeon_floor_t* df = dungeon_get_floor(d, 0);
    massert(df, "floor is NULL");
    entity* e = em_get(g->entitymap, g->hero_id);
    massert(e, "e is NULL");

    loc_t loc = get_random_empty_non_wall_loc(g, 0);
    create_potion_at(g, potion_type, name, loc);
}

static loc_t get_random_empty_non_wall_loc_in_area(gamestate* const g, int floor, int x, int y, int w, int h) {
    massert(g, "gamestate is NULL");
    massert(floor >= 0, "floor is out of bounds");
    massert(floor < g->dungeon->num_floors, "floor is out of bounds");
    massert(x >= 0, "x is out of bounds");
    massert(x < g->dungeon->floors[floor]->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < g->dungeon->floors[floor]->height, "y is out of bounds");
    massert(w > 0, "w is out of bounds");
    massert(h > 0, "h is out of bounds");
    massert(x + w <= g->dungeon->floors[floor]->width, "x + w is out of bounds");
    massert(y + h <= g->dungeon->floors[floor]->height, "y + h is out of bounds");

    int c = -1;
    //loc_t* locations = get_empty_non_wall_locs(g->dungeon->floors[floor], &c);
    loc_t* locations = get_empty_non_wall_locs_in_area(g->dungeon->floors[floor], &c, x, y, w, h);
    massert(locations, "locations is NULL");
    massert(c > 0, "locations count is 0 or less");
    // pick a random location
    int index = rand() % c;
    loc_t loc = locations[index];
    free(locations);
    massert(loc.x >= 0, "loc.x is out of bounds");
    massert(loc.x < g->dungeon->floors[floor]->width, "loc.x is out of bounds");
    massert(loc.y >= 0, "loc.y is out of bounds");
    massert(loc.y < g->dungeon->floors[floor]->height, "loc.y is out of bounds");
    loc.z = floor;
    return loc;
}

static loc_t get_random_empty_non_wall_loc(gamestate* const g, int floor) {
    massert(g, "gamestate is NULL");
    massert(floor >= 0, "floor is out of bounds");
    massert(floor < g->dungeon->num_floors, "floor is out of bounds");
    return get_random_empty_non_wall_loc_in_area(g, floor, 0, 0, g->dungeon->floors[floor]->width, g->dungeon->floors[floor]->height);
}

static void init_weapon_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entity* e = em_get(g->entitymap, g->hero_id);
    massert(e, "hero entity is NULL");
    // pick a random location
    loc_t loc = get_random_empty_non_wall_loc(g, g->dungeon->current_floor);
    create_sword_at(g, loc);
}

static void init_weapon_test2(gamestate* g) {
    massert(g, "gamestate is NULL");
    entity* e = em_get(g->entitymap, g->hero_id);
    massert(e, "hero entity is NULL");
    entity* sword = create_sword(g);
    e_add_item_to_inventory(e, sword->id);
    // equip the sword
    e->weapon = sword->id;
}

static void init_dungeon(gamestate* const g) {
    massert(g, "gamestate is NULL");
    g->dungeon = dungeon_create();
    massert(g->dungeon, "failed to init dungeon");
    dungeon_add_floor(g->dungeon, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
}

static void init_em(gamestate* const g) {
    massert(g, "gamestate is NULL");
    g->entitymap = em_new();
}

static entityid npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    massert(g, "gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "entitymap is NULL");
    massert(name && name[0], "name is NULL or empty");
    massert(rt >= 0, "race_type is out of bounds: %s: %d", name, rt);
    massert(rt < RACE_COUNT, "race_type is out of bounds: %s: %d", name, rt);
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get current dungeon floor");
    massert(x >= 0, "x is out of bounds: %s: %d", name, x);
    massert(x < df->width, "x is out of bounds: %s: %d", name, x);
    massert(y >= 0, "y is out of bounds: %s: %d", name, y);
    massert(y < df->height, "y is out of bounds: %s: %d", name, y);
    // can we create an entity at this location? no entities can be made on wall-types etc
    tile_t* const tile = df_tile_at(df, x, y);
    massert(tile, "failed to get tile");
    if (!tile_is_walkable(tile->type)) {
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

static entityid door_create(gamestate* const g, int x, int y, int fl, const char* name) {
    massert(g, "gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "entitymap is NULL");
    massert(name && name[0], "name is NULL or empty");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "failed to get current dungeon floor");
    massert(x >= 0, "x is out of bounds: %s: %d", name, x);
    massert(x < df->width, "x is out of bounds: %s: %d", name, x);
    massert(y >= 0, "y is out of bounds: %s: %d", name, y);
    massert(y < df->height, "y is out of bounds: %s: %d", name, y);
    // can we create an entity at this location? no entities can be made on wall-types etc
    tile_t* const tile = df_tile_at(df, x, y);
    massert(tile, "failed to get tile");
    if (!tile_is_walkable(tile->type)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("cannot create entity on tile with NPC");
        return ENTITYID_INVALID;
    }
    // check for existing door
    if (tile_has_door(g, x, y, fl)) {
        merror("cannot create entity on tile with door");
        return ENTITYID_INVALID;
    }

    entity* const e = e_new_door_at(next_entityid++, x, y, fl, name);
    //massert(e, "failed to create entity");
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

static entityid player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    //minfo("creating player");
    massert(g, "gamestate is NULL");
    massert(name, "name is NULL");
    // use the previously-written liblogic_npc_create function
    const entitytype_t type = ENTITY_PLAYER;
    const entityid id = npc_create(g, rt, x, y, fl, name);
    massert(id != ENTITYID_INVALID, "failed to create player");
    entity_t* const e = em_get(g->entitymap, id);
    massert(e, "entity is NULL");
    e_set_type(e, type);
    gamestate_set_hero_id(g, id);
    return id;
}

static void init_player(gamestate* const g) {
    massert(g, "gamestate is NULL");
    // setting it up so we can return a loc_t from a function
    // that can scan for an appropriate starting location
    loc_t loc = df_get_upstairs(g->dungeon->floors[g->dungeon->current_floor]);
    const int id = player_create(g, RACE_HUMAN, loc.x, loc.y, 0, "hero");
    massert(id != ENTITYID_INVALID, "failed to init hero");
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "hero is NULL");
    e_set_maxhp(hero, 10);
    e_set_hp(hero, 10);
    g->entity_turn = g->hero_id;
    msuccess("hero id %d", g->hero_id);
}

static entity_t* const npc_create_ptr(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    massert(g, "gamestate is NULL");
    entityid id = npc_create(g, rt, x, y, fl, name);
    if (id == ENTITYID_INVALID) {
        merror("failed to create NPC");
        return NULL;
    }
    return em_get(g->entitymap, id);
}

static inline size_t tile_npc_count_at(gamestate* const g, dungeon_floor_t* const df, int x, int y) {
    massert(g, "gamestate is NULL");
    massert(df, "dungeon floor is NULL");
    massert(x >= 0, "x is out of bounds");
    massert(x < df->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < df->height, "y is out of bounds");

    tile_t* const tile = df_tile_at(df, x, y);
    return tile_live_npc_count(tile, g->entitymap);
}

static loc_t* get_walkable_locs(dungeon_floor_t* df, int* cnt) {
    massert(df, "dungeon floor is NULL");
    massert(cnt, "cnt is NULL");
    int c = df_count_walkable(df);
    loc_t* locs = malloc(sizeof(loc_t) * c);
    massert(locs, "malloc failed");
    int i = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* t = df_tile_at(df, x, y);
            massert(t, "tile is NULL");
            if (tile_is_walkable(t->type)) locs[i++] = (loc_t){x, y};
        }
    }
    massert(i == c, "count mismatch");
    *cnt = c;
    return locs;
}

static loc_t* get_empty_locs(dungeon_floor_t* const df, int* count) {
    massert(df, "dungeon floor is NULL");
    massert(count, "count is NULL");
    int c = df_count_empty(df);
    loc_t* locs = malloc(sizeof(loc_t) * c);
    massert(locs, "malloc failed");
    int i = 0;
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* t = df_tile_at(df, x, y);
            if (tile_entity_count(t) == 0) { locs[i++] = (loc_t){x, y}; }
        }
    }
    massert(i == c, "count mismatch");
    *count = c;
    return locs;
}

static loc_t* get_empty_non_wall_locs_in_area(dungeon_floor_t* const df, int* count, int x0, int y0, int w, int h) {
    massert(df, "dungeon floor is NULL");
    massert(count, "count is NULL");
    int c = df_count_empty_non_walls_in_area(df, x0, y0, w, h);
    loc_t* locs = malloc(sizeof(loc_t) * c);
    massert(locs, "malloc failed");
    int i = 0;
    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            int newx = x + x0;
            int newy = y + y0;

            tile_t* t = df_tile_at(df, newx, newy);
            tiletype_t type = t->type;
            if (tile_entity_count(t) == 0 && tile_is_walkable(type)) locs[i++] = (loc_t){newx, newy};
            if (i >= c) break;
        }
    }
    massert(i == c, "count mismatch: expected %d, got %d", c, i);
    *count = c;
    return locs;
}

static loc_t* get_empty_non_wall_locs(dungeon_floor_t* const df, int* count) {
    massert(df, "dungeon floor is NULL");
    massert(count, "count is NULL");
    return get_empty_non_wall_locs_in_area(df, count, 0, 0, df->width, df->height);
}

static entity* create_orc_at(gamestate* g, loc_t loc) {
    entity* e = npc_create_ptr(g, RACE_ORC, loc.x, loc.y, loc.z, "orc");
    massert(e, "orc create fail");
    //e_set_default_action(e, ENTITY_ACTION_MOVE_ATTACK_PLAYER);
    e_set_default_action(e, ENTITY_ACTION_MOVE_A_STAR);
    e_set_maxhp(e, 1);
    e_set_hp(e, 1);

    entity* sword = create_sword(g);
    e_add_item_to_inventory(e, sword->id);
    e->weapon = sword->id;

    return e;

    //entity* shield = create_shield(g);
    //e_add_item_to_inventory(e, shield->id);
    //e->shield = shield->id;
}

static entity* create_human_at(gamestate* g, loc_t loc) {
    entity* e = npc_create_ptr(g, RACE_HUMAN, loc.x, loc.y, loc.z, "human");
    massert(e, "human create fail");
    e_set_default_action(e, ENTITY_ACTION_MOVE_A_STAR);
    e_set_maxhp(e, 1);
    e_set_hp(e, 1);

    entity* sword = create_sword(g);
    e_add_item_to_inventory(e, sword->id);
    e->weapon = sword->id;

    return e;
}

static entity* create_elf_at(gamestate* g, loc_t loc) {
    entity* e = npc_create_ptr(g, RACE_ELF, loc.x, loc.y, loc.z, "elf");
    massert(e, "elf create fail");
    e_set_default_action(e, ENTITY_ACTION_MOVE_A_STAR);
    e_set_maxhp(e, 1);
    e_set_hp(e, 1);

    entity* sword = create_sword(g);
    e_add_item_to_inventory(e, sword->id);
    e->weapon = sword->id;

    return e;
}

static entity* create_dwarf_at(gamestate* g, loc_t loc) {
    entity* e = npc_create_ptr(g, RACE_DWARF, loc.x, loc.y, loc.z, "dwarf");
    massert(e, "dwarf create fail");
    e_set_default_action(e, ENTITY_ACTION_MOVE_A_STAR);
    e_set_maxhp(e, 1);
    e_set_hp(e, 1);

    entity* sword = create_sword(g);
    e_add_item_to_inventory(e, sword->id);
    e->weapon = sword->id;

    return e;
}

static entity* create_halfling_at(gamestate* g, loc_t loc) {
    entity* e = npc_create_ptr(g, RACE_HALFLING, loc.x, loc.y, loc.z, "halfling");
    massert(e, "halfling create fail");
    e_set_default_action(e, ENTITY_ACTION_MOVE_A_STAR);
    e_set_maxhp(e, 1);
    e_set_hp(e, 1);

    entity* sword = create_sword(g);
    e_add_item_to_inventory(e, sword->id);
    e->weapon = sword->id;

    return e;
}

static entity* create_goblin_at(gamestate* g, loc_t loc) {
    entity* e = npc_create_ptr(g, RACE_GOBLIN, loc.x, loc.y, loc.z, "goblin");
    massert(e, "goblin create fail");
    e_set_default_action(e, ENTITY_ACTION_MOVE_A_STAR);
    e_set_maxhp(e, 1);
    e_set_hp(e, 1);

    entity* sword = create_sword(g);
    e_add_item_to_inventory(e, sword->id);
    e->weapon = sword->id;

    return e;
}

static void init_npcs_test_by_room(gamestate* const g) {
    massert(g, "gamestate is NULL");
    dungeon_floor_t* df = dungeon_get_floor(g->dungeon, 0);
    massert(df, "floor is NULL");

    int hallway_count = 0;
    int room_count = 0;
    int random_choice = 0;

    room_data_t* hallways = NULL;
    room_data_t* rooms = NULL;
    entity* player = NULL;
    entity* e = NULL;

    hallways = df_get_rooms_with_prefix(df, &hallway_count, "hallway");
    massert(hallways, "hallways is NULL");

    rooms = df_get_rooms_with_prefix(df, &room_count, "room");
    massert(rooms, "rooms is NULL");

    player = em_get(g->entitymap, g->hero_id);
    massert(player, "player is NULL");

    // create doors in hallways
    if (hallway_count > 0) {
        for (int i = 0; i < hallway_count; i++) {
            room_data_t hallway = hallways[i];
            loc_t loc = get_random_empty_non_wall_loc_in_area(g, g->dungeon->current_floor, hallway.x, hallway.y, hallway.w, hallway.h);
            entityid doorid = door_create(g, loc.x, loc.y, loc.z, "door");
            massert(doorid != ENTITYID_INVALID, "door create fail");
        }
    }

    // one NPC per room past the first
    const int max_npcs = 300;
    for (int i = 0; i < max_npcs; i++) {
        room_data_t room = rooms[0];
        loc_t loc = get_random_empty_non_wall_loc_in_area(g, g->dungeon->current_floor, room.x, room.y, room.w, room.h);

        random_choice = rand() % 5;

        if (random_choice == 0) {
            e = create_orc_at(g, loc);
        } else if (random_choice == 1) {
            e = create_elf_at(g, loc);
        } else if (random_choice == 2) {
            e = create_dwarf_at(g, loc);
        } else if (random_choice == 3) {
            e = create_halfling_at(g, loc);
        } else if (random_choice == 4) {
            e = create_goblin_at(g, loc);
        } else if (random_choice == 4) {
            e = create_human_at(g, loc);
        }

        massert(e != ENTITYID_INVALID, "NPC create fail");
        e->target = (loc_t){player->x, player->y, player->floor};
    }
}

static const char* get_action_key(const inputstate* const is, gamestate* const g) {
    const int key = inputstate_get_pressed_key(is);
    // can return early if key == -1
    if (key == -1) return "none";
    return get_action_for_key(&g->keybinding_list, key);
}

static void handle_camera_move(gamestate* const g, const inputstate* const is) {
    const float move = g->cam2d.zoom;
    const char* action = get_action_key(is, g);
    if (inputstate_is_held(is, KEY_RIGHT)) {
        g->cam2d.offset.x += move;
    } else if (inputstate_is_held(is, KEY_LEFT)) {
        g->cam2d.offset.x -= move;
    } else if (inputstate_is_held(is, KEY_UP)) {
        g->cam2d.offset.y -= move;
    } else if (inputstate_is_held(is, KEY_DOWN)) {
        g->cam2d.offset.y += move;
    } else if (action && strcmp(action, "toggle_camera") == 0) {
        g->cam2d.zoom = roundf(g->cam2d.zoom);
        g->controlmode = CONTROLMODE_PLAYER;
    }
}

static inline void handle_camera_zoom(gamestate* const g, const inputstate* const is) {
    massert(g, "Game state is NULL!");
    massert(is, "Input state is NULL!");
    if (inputstate_is_held(is, KEY_Z)) {
        if (inputstate_is_shift_held(is)) {
            if (g->cam2d.zoom > 1.0) { g->cam2d.zoom -= DEFAULT_ZOOM_INCR; }
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

static void handle_input_inventory(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_is_pressed(is, KEY_I)) {
        g->controlmode = CONTROLMODE_PLAYER;
        g->display_inventory_menu = false;
        return;
    }
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "hero is NULL");
    if (hero->inventory_count == 0) { return; }
    if (inputstate_is_pressed(is, KEY_DOWN)) {
        g->inventory_menu_selection++;
        if (g->inventory_menu_selection >= hero->inventory_count) { g->inventory_menu_selection = 0; }
    } else if (inputstate_is_pressed(is, KEY_UP)) {
        g->inventory_menu_selection--;
        if (g->inventory_menu_selection < 0) { g->inventory_menu_selection = hero->inventory_count - 1; }
    } else if (inputstate_is_pressed(is, KEY_ENTER)) {
        // we need to grab the entityid of the selected item
        entityid item_id = hero->inventory[g->inventory_menu_selection];
        entity* item = em_get(g->entitymap, item_id);
        massert(item, "item is NULL");

        if (item->type == ENTITY_WEAPON) {
            // attempt to equip the weapon
            // check if the hero is already equipped with the weapon
            if (hero->weapon == item_id) {
                add_message(g, "You are already using %s", item->name);
                g->controlmode = CONTROLMODE_PLAYER;
                g->display_inventory_menu = false;
                return;
            }

            hero->weapon = item_id;
            add_message_and_history(g, "%s equipped %s", hero->name, item->name);
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_inventory_menu = false;
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (item->type == ENTITY_SHIELD) {
            // attempt to equip the shield
            // check if the hero is already equipped with the shield
            if (hero->shield == item_id) {
                add_message(g, "You are already using %s", item->name);
                g->controlmode = CONTROLMODE_PLAYER;
                g->display_inventory_menu = false;
                return;
            }

            hero->shield = item_id;
            add_message_and_history(g, "%s equipped %s", hero->name, item->name);
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_inventory_menu = false;
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (item->type == ENTITY_POTION) {
            add_message(g, "Potion use is not handled yet!");
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_inventory_menu = false;
        } else {
            add_message(g, "Unhandled item type: %d", item->type);
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_inventory_menu = false;
        }

    } else if (inputstate_is_pressed(is, KEY_U)) {
        // we need to grab the entityid of the selected item
        entityid item_id = hero->inventory[g->inventory_menu_selection];
        entity* item = em_get(g->entitymap, item_id);
        massert(item, "item is NULL");

        if (item->type == ENTITY_WEAPON) {
            // unequip the weapon
            hero->weapon = ENTITYID_INVALID;
            add_message_and_history(g, "%s unequipped %s", hero->name, item->name);
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_inventory_menu = false;
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (item->type == ENTITY_SHIELD) {
            // equip the shield
            hero->shield = ENTITYID_INVALID;
            add_message_and_history(g, "%s unequipped %s", hero->name, item->name);
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_inventory_menu = false;
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
    }
}

static inline void change_player_dir(gamestate* const g, direction_t dir) {
    massert(g, "Game state is NULL!");
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "hero is NULL");
    if (hero->dead) return;
    hero->direction = dir;
    hero->do_update = true;
    update_equipped_shield_dir(g, hero);
}

static bool try_entity_pickup(gamestate* const g, entity* const e) {
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
        if (it->type == ENTITY_WEAPON || it->type == ENTITY_SHIELD || it->type == ENTITY_POTION) {
            add_message_and_history(g, "%s picked up a %s", e->name, it->name);
            tile_remove(tile, id);
            e_add_item_to_inventory(e, id);
            if (e->type == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        } else {
            add_message(g, "Unhandled item type cannot be picked up: %d", it->type);
            return false;
        }
    }
    add_message(g, "No items to pick up");
    return false;
}

static inline void try_open_close_door(gamestate* const g, entity* const e, int x, int y, int fl) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "Failed to get dungeon floor");
    tile_t* const tile = df_tile_at(df, x, y);
    if (!tile) {
        merror("Failed to get tile");
        return;
    }
}

static inline void try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "Failed to get dungeon floor");
    tile_t* const tile = df_tile_at(df, x, y);
    if (tile && tile->has_wall_switch) {
        tile->wall_switch_on = !tile->wall_switch_on;
        msuccess("Wall switch flipped!");
        int ws_event = tile->wall_switch_event;
        if (ws_event <= -1) {
            merror("Wall switch event is invalid");
            return;
        }
        df_event_t event = df->events[ws_event];
        tile_t* const event_tile = df_tile_at(df, event.x, event.y);
        //massert(event_tile, "Failed to get event tile");
        tiletype_t type = event_tile->type;
        if (type == event.off_type) {
            event_tile->type = event.on_type;
        } else if (type == event.on_type) {
            event_tile->type = event.off_type;
        }
        //add_message(g, "Wall switch flipped!");
        // this is the basis for what we need to do next
        // currently we have no entity passed into this function
        // we need to fix that
        if (e->type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (e->type == ENTITY_NPC) {
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        }
        //else {
        //    g->flag = GAMESTATE_FLAG_NONE;
        //}
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

static void handle_input_player(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) { return; }
    if (g->msg_system.is_active) {
        if (inputstate_is_pressed(is, KEY_I) || inputstate_is_pressed(is, KEY_U) || inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_A) ||
            inputstate_is_pressed(is, KEY_SPACE) || inputstate_is_pressed(is, KEY_COMMA) || inputstate_is_pressed(is, KEY_PERIOD)) {
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
    const char* action = get_action_key(is, g);
    if (!action) {
        merror("No action found for key");
        return;
    }
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "hero is NULL");
    // check if the player is dead
    if (hero->dead) return;
    if (action) {
        if (g->player_changing_direction) {
            if (strcmp(action, "wait") == 0) {
                execute_action(g, hero, ENTITY_ACTION_WAIT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_w") == 0) {
                change_player_dir(g, DIR_LEFT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_e") == 0) {
                change_player_dir(g, DIR_RIGHT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_s") == 0) {
                change_player_dir(g, DIR_DOWN);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_n") == 0) {
                change_player_dir(g, DIR_UP);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_nw") == 0) {
                change_player_dir(g, DIR_UP_LEFT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_ne") == 0) {
                change_player_dir(g, DIR_UP_RIGHT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_sw") == 0) {
                change_player_dir(g, DIR_DOWN_LEFT);
                g->player_changing_direction = false;
            } else if (strcmp(action, "move_se") == 0) {
                change_player_dir(g, DIR_DOWN_RIGHT);
                g->player_changing_direction = false;
            }
            return;
        }

        if (strcmp(action, "wait") == 0) {
            g->player_changing_direction = true;
        } else if (strcmp(action, "inventory_menu") == 0) {
            //g->player_changing_direction = true;
            g->display_inventory_menu = true;
            g->controlmode = CONTROLMODE_INVENTORY;
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
            loc_t loc = get_loc_from_dir(hero->direction);
            try_entity_attack(g, hero->id, hero->x + loc.x, hero->y + loc.y);
        } else if (strcmp(action, "interact") == 0) {
            // we are hardcoding the flip switch interaction for now
            // but eventually this will be generalized
            // for instance u can talk to an NPC merchant using "interact"
            // or open a door, etc
            //msuccess("Space pressed!");
            int tx = hero->x + get_x_from_dir(hero->direction);
            int ty = hero->y + get_y_from_dir(hero->direction);
            // check to see if there is a door
            entity* door = get_door_from_tile(g, tx, ty, hero->floor);
            if (door) {
                door->door_is_open = !door->door_is_open;
                door->do_update = true;
            }
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            //try_flip_switch(g, hero, tx, ty, hero->floor);
        } else if (strcmp(action, "pickup") == 0) {
            try_entity_pickup(g, hero);
        } else if (strcmp(action, "toggle_camera") == 0) {
            g->controlmode = CONTROLMODE_CAMERA;
        }
    } else {
        merror("No action found for key");
    }
}

static void try_entity_open_door(gamestate* g, entity* e, int x, int y) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    dungeon_floor_t* df = dungeon_get_floor(g->dungeon, e->floor);
    massert(df, "Failed to get dungeon floor");
    tile_t* tile = df_tile_at(df, x, y);
    if (!tile) {
        merror("Failed to get tile");
        return;
    }
    if (tile_has_closed_door(g, x, y, e->floor)) {
        entity* door = get_door_from_tile(g, x, y, e->floor);
        massert(door, "door is NULL");
        door->door_is_open = !door->door_is_open;
        door->do_update = true;
    }
}

static void handle_input(const inputstate* const is, gamestate* const g) {
    massert(is, "inputstate is NULL");
    massert(g, "gamestate is NULL");
    // no matter which mode we are in, we can toggle the debug panel
    if (inputstate_is_pressed(is, KEY_D)) {
        msuccess("D pressed!");
        g->debugpanelon = !g->debugpanelon;
    }
    if (g->controlmode == CONTROLMODE_PLAYER) {
        handle_input_player(is, g);
    } else if (g->controlmode == CONTROLMODE_CAMERA) {
        handle_input_camera(is, g);
    } else if (g->controlmode == CONTROLMODE_INVENTORY) {
        handle_input_inventory(is, g);
    } else {
        merror("Unknown control mode");
    }
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
    init_dungeon(g);
    gamestate_init_entityids(g);
    g->msg_system.count = 0;
    g->msg_system.index = 0;
    g->msg_system.is_active = false;
    gamestate_load_keybindings(g);
    init_em(g);
    init_player(g);
    // test to create a weapon
    //init_weapon_test(g);
    init_weapon_test2(g);
    //init_shield_test(g);
    init_shield_test2(g);
    //init_potion_test(g, POTION_HP_SMALL, "small healing potion");
    //init_potion_test(g, POTION_HP_MEDIUM, "medium healing potion");
    //init_potion_test(g, POTION_HP_LARGE, "large healing potion");
    init_npcs_test_by_room(g);
    update_debug_panel_buffer(g);
}

static void update_player_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    massert(e, "liblogic_update_player_state: hero is NULL");
    if (!g->gameover) {
        if (e->dead) {
            add_message_and_history(g, "You died!");
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
    if (e->stats.hp <= 0) {
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
    massert(e, "entity is NULL");
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

static inline void reset_player_blocking(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    massert(e, "hero is NULL");
    e->is_blocking = false;
    g->test_guard = false;
}

static inline void reset_player_block_success(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    massert(e, "hero is NULL");
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

// Check if a location is traversable (walkable and unoccupied)
static inline bool is_traversable(gamestate* const g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    // get the dungeon floor
    dungeon_floor_t* df = dungeon_get_floor(g->dungeon, z);
    massert(df, "floor is NULL");
    // Check map bounds
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) return false;
    // Get the current tile
    tile_t* tile = df_tile_at(df, x, y);
    massert(tile, "tile is NULL");
    // Check if the tile type is walkable
    if (!tile_is_walkable(tile->type)) return false;
    // Check for blocking entities
    // Comment out the next line if entity blocking is interfering with pathfinding tests
    if (tile_has_live_npcs(tile, g->entitymap)) return false;
    return true;
}
