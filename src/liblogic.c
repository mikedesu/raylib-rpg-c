#include "component.h"
#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
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
//#include "path_node.h"
#include "path_node.h"
#include "race.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

static entityid next_entityid = 0; // Start at 0, increment for each new entity

//static inline size_t tile_npc_count_at(gamestate* const g, int x, int y, int z);

static inline tile_t* get_first_empty_tile_around_entity(gamestate* const g, entityid id);

static inline bool is_traversable(gamestate* const g, int x, int y, int z);

static void handle_attack_blocked(gamestate* const g, entityid attacker_id, entityid target_id, bool* atk_successful);
static inline void reset_player_blocking(gamestate* const g);
static inline void reset_player_block_success(gamestate* const g);
static inline void update_npc_state(gamestate* const g, entityid id);
static inline void handle_camera_zoom(gamestate* const g, const inputstate* const is);
//static inline void try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl);

//static loc_t* get_empty_locs(dungeon_floor_t* const df, int* count);
//static loc_t* get_walkable_locs(dungeon_floor_t* df, int* cnt);
//static loc_t* get_empty_non_wall_locs(dungeon_floor_t* const df, int* count);
static loc_t* get_empty_non_wall_locs_in_area(dungeon_floor_t* const df, int* count, int x0, int y0, int w, int h);
static loc_t* get_locs_around_entity(gamestate* const g, entityid id);

//static entity* create_shield(gamestate* g);
//static entity* create_sword(gamestate* g);
//static entity* create_sword_at(gamestate* g, loc_t loc);
//static entity* create_shield_at(gamestate* g, loc_t loc);
//static entity* create_elf_at(gamestate* g, loc_t loc);
//static entity* create_orc_at(gamestate* g, loc_t loc);
//static entity* create_dwarf_at(gamestate* g, loc_t loc);
//static entity* create_human_at(gamestate* g, loc_t loc);
//static entity* create_goblin_at(gamestate* g, loc_t loc);
//static entity* create_halfling_at(gamestate* g, loc_t loc);

//static void init_potion_test(gamestate* const g, potiontype_t potion_type, const char* name);
//static void init_npcs_test_by_room(gamestate* const g);
static void init_npc_test(gamestate* g);
static void init_sword_test(gamestate* g);

//static void init_em(gamestate* const g);
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
//static void try_entity_open_door(gamestate* g, entity* e, int x, int y);
static void try_entity_move_a_star(gamestate* const g, entityid id);
static void try_entity_move(gamestate* const g, entityid id, int x, int y);
static void try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y);

static const char* get_action_key(const inputstate* const is, gamestate* const g);

static entityid player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);
//static entityid create_potion_at(gamestate* const g, potiontype_t potion_type, const char* name, loc_t loc);
//static entityid door_create(gamestate* const g, int x, int y, int fl, const char* name);
//static entityid npc_create(gamestate* const g, race_t rt, int x, int y, int z, const char* name);
static entityid npc_create(gamestate* const g, race_t rt, loc_t loc, const char* name);
static entityid item_create(gamestate* const g, itemtype type, loc_t loc, const char* name);
static entityid weapon_create(gamestate* const g, weapontype type, loc_t loc, const char* name);

static loc_t get_random_empty_non_wall_loc_in_area(gamestate* const g, int floor, int x, int y, int w, int h);
static loc_t get_random_empty_non_wall_loc(gamestate* const g, int floor);

static bool entities_adjacent(gamestate* const g, entityid id0, entityid id1);
static bool player_on_tile(gamestate* g, int x, int y, int floor);
//static bool tile_has_closed_door(const gamestate* const g, int x, int y, int fl);
//static bool tile_has_door(const gamestate* const g, int x, int y, int fl);

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

//static void update_equipped_shield_dir(gamestate* g, entity* e) {
static void update_equipped_shield_dir(gamestate* g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    //massert(e, "entity is NULL");
    //if (e->shield != ENTITYID_INVALID) {
    //    if (shield) {
    //shield->direction = e->direction;
    //        g_update_direction(g, shield->id, g_get_direction(g, e->id));
    //shield->do_update = true;
    //        g_set_update(g, shield->id, true);
    //    }
    //}
}

static bool player_on_tile(gamestate* g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    // get the tile at x y
    dungeon_floor_t* df = d_get_floor(g->d, z);
    massert(df, "dungeon floor is NULL");
    tile_t* tile = df_tile_at(df, (loc_t){x, y, z});
    if (!tile) {
        return false;
    }
    // enumerate entities and check their type
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == ENTITYID_INVALID) continue;
        //massert(e, "failed to get entity");
        entityid id = tile->entities[i];
        //if (e->type == ENTITY_PLAYER) return true;

        if (g_is_type(g, id, ENTITY_PLAYER)) {
            // check if the entity is on the same floor
            // hardcoded, FIX THIS!!!
            if (z == 0) {
                return true;
            }
        }
    }
    return false;
}

//static bool tile_has_door(const gamestate* const g, int x, int y, int fl) {
//    // Validate inputs
//    massert(g, "gamestate is NULL");
//    massert(fl >= 0, "floor is out of bounds");
//    massert(fl < g->dungeon->num_floors, "floor is out of bounds");
//    const dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
//    massert(df, "failed to get dungeon floor");
//    const tile_t* const t = df_tile_at(df, x, y);
//    massert(t, "failed to get tile");
//    // check for a door entity
//    for (int i = 0; i < t->entity_max; i++) {
//        const entityid eid = tile_get_entity(t, i);
//        if (eid == ENTITYID_INVALID) continue;
//        if (!e) continue;
//        //if (e->type == ENTITY_DOOR) return true;
//        if (g_is_type(g, eid, ENTITY_DOOR)) return true;
//    }
//    return false;
//}

//static entity* const get_door_from_tile(const gamestate* const g, int x, int y, int fl) {
//    // Validate inputs
//    massert(g, "gamestate is NULL");
//    massert(fl >= 0, "floor is out of bounds");
//    massert(fl < g->d->num_floors, "floor is out of bounds");
//    const dungeon_floor_t* const df = d_get_floor(g->d, fl);
//    massert(df, "failed to get dungeon floor");
//    const tile_t* const t = df_tile_at(df, x, y);
//    massert(t, "failed to get tile");
//    // check for a door entity
//    for (int i = 0; i < t->entity_max; i++) {
//        const entityid eid = tile_get_entity(t, i);
//        if (eid == ENTITYID_INVALID) continue;
//        if (!e) continue;
//        if (g_is_type(g, eid, ENTITY_DOOR)) return e;
//    }
//    return NULL;
//}

//static bool tile_has_closed_door(const gamestate* const g, int x, int y, int fl) {
//    // Validate inputs
//    massert(g, "gamestate is NULL");
//    massert(fl >= 0, "floor is out of bounds");
//    massert(fl < g->d->num_floors, "floor is out of bounds");
//    const dungeon_floor_t* const df = d_get_floor(g->d, fl);
//    massert(df, "failed to get dungeon floor");
//    const tile_t* const t = df_tile_at(df, x, y);
//    massert(t, "failed to get tile");
//    // check for a door entity
//    for (int i = 0; i < t->entity_max; i++) {
//        const entityid eid = tile_get_entity(t, i);
//        if (eid == ENTITYID_INVALID) continue;
//        if (!e) continue;
//        //if (e->type == ENTITY_DOOR && !e->door_is_open) return true;
//        if (g_is_type(g, eid, ENTITY_DOOR) && !e->door_is_open) return true;
//    }
//    return false;
//}

static inline int tile_npc_living_count(const gamestate* const g, int x, int y, int z) {
    // Validate inputs
    massert(g, "gamestate is NULL");
    massert(z >= 0, "floor is out of bounds");
    massert(z < g->d->num_floors, "floor is out of bounds");
    const dungeon_floor_t* const df = d_get_floor(g->d, z);
    massert(df, "failed to get dungeon floor");
    const tile_t* const t = df_tile_at(df, (loc_t){x, y, z});
    massert(t, "failed to get tile");
    // Count living NPCs
    int count = 0;
    for (int i = 0; i < t->entity_max; i++) {
        const entityid eid = tile_get_entity(t, i);
        if (eid == ENTITYID_INVALID) continue;
        //if (!e) continue;
        //if (e->type == ENTITY_NPC && !e->dead) count++;
        //if (g_is_type(g, eid, ENTITY_NPC) && !e->dead) count++;
        //minfo("calling g_is_dead 4");
        if (g_is_type(g, eid, ENTITY_NPC) && !g_is_dead(g, eid)) count++;
    }
    return count;
}

//static void try_entity_move(gamestate* const g, entity* const e, int x, int y) {
static void try_entity_move(gamestate* const g, entityid id, int x, int y) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    //massert(e, "Entity is NULL!");
    //e->do_update = true;
    g_set_update(g, id, true);
    //e->direction = get_dir_from_xy(x, y);
    g_update_direction(g, id, get_dir_from_xy(x, y));
    loc_t loc = g_get_location(g, id);
    int ex = loc.x + x;
    int ey = loc.y + y;
    int z = loc.z;
    dungeon_floor_t* const df = d_get_floor(g->d, z);
    if (!df) {
        merror("Failed to get dungeon floor");
        return;
    }
    // i feel like this might be something we can set elsewhere...like after the player input phase?

    tile_t* const tile = df_tile_at(df, (loc_t){ex, ey, z});
    if (!tile) {
        merror("Cannot move, tile is NULL");
        return;
    }
    if (!tile_is_walkable(tile->type)) {
        merror("Cannot move, tile is not walkable");
        return;
    }
    //if (tile_has_closed_door(g, ex, ey, floor)) {
    //    merror("Cannot move, tile has a closed door");
    //    return;
    //}
    if (tile_npc_living_count(g, ex, ey, z) > 0) {
        merror("Cannot move, NPC in the way");
        return;
    }
    if (player_on_tile(g, ex, ey, z)) {
        merror("Cannot move, player on tile");
        return;
    }
    if (!df_remove_at(df, id, loc.x, loc.y)) {
        merror("Failed to remove entity from old tile");
        return;
    }
    if (!df_add_at(df, id, ex, ey)) {
        merror("Failed to add entity to new tile");
        return;
    }
    g_update_location(g, id, (loc_t){ex, ey, z});
    g_update_sprite_move(g, id, (loc_t){x * DEFAULT_TILE_SIZE, y * DEFAULT_TILE_SIZE, 0});
    // at this point the move is 'successful'
    //update_equipped_shield_dir(g, id);
    // get the entity's new tile
    //tile_t* const new_tile = df_tile_at(df, (loc_t){ex, ey, z});
    //if (!new_tile) {
    //    merror("Failed to get new tile");
    //    return;
    //}
    // do not remove!!!
    // check if the tile has a pressure plate
    //if (new_tile->has_pressure_plate) {
    //    msuccess("Pressure plate activated!");
    // do something
    // print the pressure plate event
    //msuccessint("Pressure plate event", new_tile->pressure_plate_event);
    //}
    // check if the tile is an ON TRAP
    //if (new_tile->type == TILE_FLOOR_STONE_TRAP_ON_00) {
    //    msuccess("On trap activated!");
    // do something
    //e->stats.hp--;
    //e->is_damaged = true;
    //    g_set_damaged(g, id, true);
    //e->do_update = true;
    //    g_set_update(g, id, true);
    //}

    if (g_is_type(g, id, ENTITY_PLAYER)) {
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
}

//static void try_entity_move_player(gamestate* const g, entity* const e) {
//    massert(g, "gamestate is NULL");
//    massert(h, "hero is NULL");
//    loc_t hl = g_get_location(g, h->id);
//    loc_t el = g_get_location(g, e->id);
//    int dx = (hl.x > el.x) ? 1 : (hl.x < el.x) ? -1 : 0;
//    int dy = (hl.y > el.y) ? 1 : (hl.y < el.y) ? -1 : 0;
//    if (dx != 0 || dy != 0) try_entity_move(g, e, dx, dy);
//}

//static void try_entity_move_a_star(gamestate* const g, entity* const e) {
static void try_entity_move_a_star(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    //    if (e->target_path) { e_free_target_path(e); }
    // for testing, we will hardcode an update to the entity's target
    // realistically, we should actually use a target ID and do location lookups on every update
    // however, for this test, we will instead hardcode the target to point to the hero's location
    // first, grab the hero id and then the hero entity pointer
    //    massert(h, "hero is NULL");
    loc_t hloc = g_get_location(g, g->hero_id);
    loc_t eloc = g_get_location(g, id);

    dungeon_floor_t* df = d_get_floor(g->d, eloc.z);
    massert(df, "dungeon floor is NULL");

    int target_path_length = 0;
    loc_t* target_path = find_path(eloc, hloc, df, &target_path_length);
    if (target_path) {
        if (target_path_length >= 2) {
            loc_t loc = target_path[target_path_length - 2];
            int dx = loc.x - eloc.x;
            int dy = loc.y - eloc.y;
            if (entities_adjacent(g, id, g->hero_id)) {
                //                    // if the entity is adjacent to the hero, try to attack
                try_entity_attack(g, id, loc.x, loc.y);
            } else {
                //                    // if the entity is not adjacent to the hero, try to move
                //                    // there might be a door in the way...
                //                    //if (tile_has_closed_door(g, loc.x, loc.y, e->floor)) {
                //                    if (tile_has_closed_door(g, loc.x, loc.y, loc.z)) {
                //                        try_entity_open_door(g, e, loc.x, loc.y);
                //                    } else {
                int dx = loc.x - eloc.x;
                int dy = loc.y - eloc.y;
                try_entity_move(g, id, dx, dy);
                //                    }
            }
            //            }
        }
    }
    // set the target to the hero's location
    //    e->target.x = hloc.x;
}
//    e->target.y = hloc.y;
//    e->target_path = find_path(eloc, e->target, g->dungeon->floors[g->dungeon->current_floor], &e->target_path_length);
//    if (e->target_path) {
//        if (e->target_path_length > 0) {
//            // instead of grabbing index 0, which is the target destination, AND
//            // instead of grabbing index target_path_length -1, which is the entity's current location,
//            // we want to grab the second to last index, which is the next location to move to
//            if (e->target_path_length >= 2) {
//                loc_t loc = e->target_path[e->target_path_length - 2];
//                int dx = loc.x - eloc.x;
//                int dy = loc.y - eloc.y;
//                if (entities_adjacent(g, e->id, h->id)) {
//                    // if the entity is adjacent to the hero, try to attack
//                    try_entity_attack(g, e->id, loc.x, loc.y);
//                } else {
//                    // if the entity is not adjacent to the hero, try to move
//                    // there might be a door in the way...
//                    //if (tile_has_closed_door(g, loc.x, loc.y, e->floor)) {
//                    if (tile_has_closed_door(g, loc.x, loc.y, loc.z)) {
//                        try_entity_open_door(g, e, loc.x, loc.y);
//                    } else {
//                        try_entity_move(g, e, dx, dy);
//                    }
//                }
//            }
//else if (e->target_path_length == 1) {
// we are at the destination
//    minfo("Entity is at the destination");
//} else {
// find path could not return a valid path
//    merror("find_path returned an invalid path");
//}
//        }
//    }
//}

static void try_entity_move_random(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
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
    try_entity_move(g, id, x, y);
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

//static void handle_attack_success(gamestate* const g, entity* attacker, entity* target, bool* attack_successful) {
static void handle_attack_success(gamestate* const g, entityid atk_id, entityid tgt_id, bool* atk_successful) {
    massert(g, "gamestate is NULL");
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
    massert(atk_successful, "attack_successful is NULL");
    *atk_successful = true;
    g_set_damaged(g, tgt_id, true);
    g_set_update(g, tgt_id, true);
    //int dmg = 1;
    //e_set_hp(target, e_get_hp(target) - dmg); // Reduce HP by 1
    //if (target->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked you for %d damage!", attacker->name, dmg);
    //if (attacker->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked %s for %d damage!", attacker->name, target->name, dmg);
    //if (e_get_hp(target) <= 0) {
    //target->dead = true;
    g_update_dead(g, tgt_id, true);
    //}
}

static void handle_attack_blocked(gamestate* const g, entityid attacker_id, entityid target_id, bool* atk_successful) {
    massert(g, "gamestate is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(target_id != ENTITYID_INVALID, "target entity id is invalid");
    massert(atk_successful, "attack_successful is NULL");
    *atk_successful = false;
    g_set_damaged(g, target_id, false);
    g_set_block_success(g, target_id, true);
    g_set_update(g, target_id, true);
    //if (target->type == ENTITY_PLAYER) { add_message_and_history(g, "%s blocked %s's attack!", target->name, attacker->name); }
}

static inline bool handle_attack_helper_innerloop(gamestate* const g, tile_t* tile, int i, entityid attacker_id, bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(i >= 0, "i is out of bounds");
    massert(i < tile->entity_max, "i is out of bounds");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(attack_successful, "attack_successful is NULL");
    entityid target_id = tile->entities[i];
    if (target_id == ENTITYID_INVALID) return false;
    entitytype_t type = g_get_type(g, target_id);
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) {
        return false;
    }
    if (g_is_dead(g, target_id)) {
        return false;
    }
    // lets try an experiment...
    //if (target->shield != ENTITYID_INVALID) {
    // introducing a random chance to block if you have a shield...
    //    int block_chance = rand() % 100;
    //    if (block_chance < 50) {
    //        msuccess("Block successful");
    //        handle_attack_blocked(g, attacker, target, attack_successful);
    //        return false;
    //    }
    //}
    msuccess("Attack successful");
    handle_attack_success(g, attacker_id, target_id, attack_successful);
    return true;
}

static void handle_attack_helper(gamestate* const g, tile_t* tile, entityid attacker_id, bool* successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(successful, "attack_successful is NULL");
    for (int i = 0; i < tile->entity_max; i++) handle_attack_helper_innerloop(g, tile, i, attacker_id, successful);
}

static void try_entity_attack(gamestate* const g, entityid atk_id, int tgt_x, int tgt_y) {
    massert(g, "gamestate is NULL");
    massert(!g_is_dead(g, atk_id), "attacker entity is dead");
    loc_t loc = g_get_location(g, atk_id);
    dungeon_floor_t* const floor = d_get_floor(g->d, loc.z);
    massert(floor, "failed to get dungeon floor");
    tile_t* const tile = df_tile_at(floor, (loc_t){tgt_x, tgt_y, loc.z});
    if (!tile) {
        merror("target tile not found");
        return;
    }
    // Calculate direction based on target position
    bool ok = false;
    loc_t eloc = g_get_location(g, atk_id);
    int dx = tgt_x - eloc.x;
    int dy = tgt_y - eloc.y;
    g_update_direction(g, atk_id, get_dir_from_xy(dx, dy));
    g_set_attacking(g, atk_id, true);
    g_set_update(g, atk_id, true);
    handle_attack_helper(g, tile, atk_id, &ok);
    handle_attack_success_gamestate_flag(g, g_get_type(g, atk_id), ok);
}

static void try_entity_attack_random(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
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
    loc_t loc = g_get_location(g, id);
    try_entity_attack(g, id, loc.x + x, loc.y + y);
}

//static void try_entity_attack_player(gamestate* const g, entity* const e) {
//massert(g, "gamestate is NULL");
//massert(h, "hero is NULL");
//loc_t hl = g_get_location(g, h->id);
//loc_t el = g_get_location(g, e->id);
//int dx = hl.x > el.x ? 1 : hl.x < el.x ? -1 : 0;
//int dy = hl.y > el.y ? 1 : hl.y < el.y ? -1 : 0;
//if (dx != 0 || dy != 0) try_entity_attack(g, e->id, hl.x, hl.y);
//}

static bool entities_adjacent(gamestate* const g, entityid id0, entityid id1) {
    massert(g, "gamestate is NULL");
    massert(id0 != ENTITYID_INVALID, "id0 is invalid");
    massert(id1 != ENTITYID_INVALID, "id1 is invalid");
    massert(id0 != id1, "id0 and id1 are the same");
    // use e0 and check the surrounding 8 tiles
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0) continue;
            loc_t loc0 = g_get_location(g, id0);
            loc_t loc1 = g_get_location(g, id1);
            if (loc0.x + x == loc1.x && loc0.y + y == loc1.y) return true;
        }
    }
    return false;
}

//static void try_entity_move_attack_player(gamestate* const g, entity* const e) {
//    massert(g, "gamestate is NULL");
//    massert(h, "hero is NULL");
//    if (entities_adjacent(g, e->id, h->id)) {
//        loc_t loc = g_get_location(g, h->id);
//        try_entity_attack(g, e->id, loc.x, loc.y);
//    }
//else {
//try_entity_move_player(g, e);
//}
//}

static void try_entity_wait(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    if (g_is_type(g, id, ENTITY_PLAYER)) {
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
    g_set_update(g, id, true);
}

static void execute_action(gamestate* const g, entityid id, entity_action_t action) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    //massert(e, "entity is NULL");
    //loc_t loc = g_get_location(g, id);
    switch (action) {
    case ENTITY_ACTION_MOVE_LEFT: try_entity_move(g, id, -1, 0); break;
    case ENTITY_ACTION_MOVE_RIGHT: try_entity_move(g, id, 1, 0); break;
    case ENTITY_ACTION_MOVE_UP: try_entity_move(g, id, 0, -1); break;
    case ENTITY_ACTION_MOVE_DOWN: try_entity_move(g, id, 0, 1); break;
    case ENTITY_ACTION_MOVE_UP_LEFT: try_entity_move(g, id, -1, -1); break;
    case ENTITY_ACTION_MOVE_UP_RIGHT: try_entity_move(g, id, 1, -1); break;
    case ENTITY_ACTION_MOVE_DOWN_LEFT: try_entity_move(g, id, -1, 1); break;
    case ENTITY_ACTION_MOVE_DOWN_RIGHT: try_entity_move(g, id, 1, 1); break;
    //case ENTITY_ACTION_ATTACK_LEFT: try_entity_attack(g, e->id, loc.x - 1, loc.y); break;
    //case ENTITY_ACTION_ATTACK_RIGHT: try_entity_attack(g, e->id, loc.x + 1, loc.y); break;
    //case ENTITY_ACTION_ATTACK_UP: try_entity_attack(g, e->id, loc.x, loc.y - 1); break;
    //case ENTITY_ACTION_ATTACK_DOWN: try_entity_attack(g, e->id, loc.x, loc.y + 1); break;
    //case ENTITY_ACTION_ATTACK_UP_LEFT: try_entity_attack(g, e->id, loc.x - 1, loc.y - 1); break;
    //case ENTITY_ACTION_ATTACK_UP_RIGHT: try_entity_attack(g, e->id, loc.x + 1, loc.y - 1); break;
    //case ENTITY_ACTION_ATTACK_DOWN_LEFT: try_entity_attack(g, e->id, loc.x - 1, loc.y + 1); break;
    //case ENTITY_ACTION_ATTACK_DOWN_RIGHT: try_entity_attack(g, e->id, loc.x + 1, loc.y + 1); break;
    case ENTITY_ACTION_MOVE_RANDOM: try_entity_move_random(g, id); break;
    case ENTITY_ACTION_WAIT: try_entity_wait(g, id); break;
    //case ENTITY_ACTION_ATTACK_RANDOM: try_entity_attack_random(g, e); break;
    //case ENTITY_ACTION_MOVE_PLAYER:
    //    try_entity_move_player(g, e);
    //    break;
    //case ENTITY_ACTION_ATTACK_PLAYER: try_entity_attack_player(g, e); break;
    //case ENTITY_ACTION_MOVE_ATTACK_PLAYER: try_entity_move_attack_player(g, e); break;
    case ENTITY_ACTION_MOVE_A_STAR:
        try_entity_move_a_star(g, id);
        break;
        //case ENTITY_ACTION_INTERACT_DOWN_LEFT:
        //case ENTITY_ACTION_INTERACT_DOWN_RIGHT:
        //case ENTITY_ACTION_INTERACT_UP_LEFT:
        //case ENTITY_ACTION_INTERACT_UP_RIGHT:
        //case ENTITY_ACTION_INTERACT_LEFT:
        //case ENTITY_ACTION_INTERACT_RIGHT:
        //case ENTITY_ACTION_INTERACT_UP:
        //case ENTITY_ACTION_INTERACT_DOWN:
        //default: merror("Unknown entity action: %d", action); break;
    }
}

//static entityid create_potion_at(gamestate* const g, potiontype_t potion_type, const char* name, loc_t loc) {
//    massert(g, "gamestate is NULL");
//    massert(name && name[0], "name is NULL or empty");
//    massert(potion_type >= 0, "potion_type is NONE or less than 0");
//    massert(potion_type < POTION_COUNT, "potion_type is out of bounds");
//    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, loc.z);
//    massert(df, "failed to get current dungeon floor");
//    massert(loc.x >= 0, "x is out of bounds");
//    massert(loc.x < df->width, "x is out of bounds");
//    massert(loc.y >= 0, "y is out of bounds");
//    massert(loc.y < df->height, "y is out of bounds");
//    tile_t* const tile = df_tile_at(df, loc.x, loc.y);
//    massert(tile, "failed to get tile");
//    if (!tile_is_walkable(tile->type)) {
//        merror("cannot create entity on wall");
//        return ENTITYID_INVALID;
//    }
//    if (tile_has_live_npcs(g, tile, em)) {
//        merror("cannot create entity on tile with NPC");
//        return ENTITYID_INVALID;
//    }
//    entity* const e = e_new_potion_at(next_entityid++, potion_type, name, loc.x, loc.y, loc.z);
//    if (!e) {
//        merror("failed to create entity");
//        return ENTITYID_INVALID;
//    }
//    // FIRST try to add to dungeon floor
//    if (!df_add_at(df, e->id, loc.x, loc.y)) {
//        merror("failed to add entity to dungeon floor");
//        free(e); // Free immediately since EM doesn't own it yet
//        return ENTITYID_INVALID;
//    }
//    // ONLY add to EM after dungeon placement succeeds
//    gs_add_entityid(g, e->id);
//    return e->id;
//}

//static entityid weapon_create(gamestate* const g, int x, int y, int fl, const char* name) {
//    massert(g, "gamestate is NULL");
//    em_t* em = gamestate_get_entitymap(g);
//    massert(em, "entitymap is NULL");
//    massert(name && name[0], "name is NULL or empty");
//    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
//    massert(df, "failed to get current dungeon floor");
//    massert(x >= 0, "x is out of bounds");
//    massert(x < df->width, "x is out of bounds");
//    massert(y >= 0, "y is out of bounds");
//    massert(y < df->height, "y is out of bounds");
//    tile_t* const tile = df_tile_at(df, x, y);
//    massert(tile, "failed to get tile");
//    if (!tile_is_walkable(tile->type)) {
//        merror("cannot create entity on wall");
//        return ENTITYID_INVALID;
//    }
//    if (tile_has_live_npcs(g, tile, em)) {
//        merror("cannot create entity on tile with NPC");
//        return ENTITYID_INVALID;
//    }
//    entity* const e = e_new_weapon_at(next_entityid++, x, y, fl, name);
//    if (!e) {
//        merror("failed to create entity");
//        return ENTITYID_INVALID;
//    }
//    // FIRST try to add to dungeon floor
//    if (!df_add_at(df, e->id, x, y)) {
//        merror("failed to add entity to dungeon floor");
//        free(e); // Free immediately since EM doesn't own it yet
//        return ENTITYID_INVALID;
//    }
//    // ONLY add to EM after dungeon placement succeeds
//    gs_add_entityid(g, e->id);
//    return e->id;
//}

//static entityid shield_create(gamestate* const g, int x, int y, int fl, const char* name) {
//    massert(g, "gamestate is NULL");
//    massert(name && name[0], "name is NULL or empty");
//    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
//    massert(df, "failed to get current dungeon floor");
//    massert(x >= 0, "x is out of bounds");
//    massert(x < df->width, "x is out of bounds");
//    massert(y >= 0, "y is out of bounds");
//    massert(y < df->height, "y is out of bounds");
//    // can we create an entity at this location? no entities can be made on wall-types etc
//    tile_t* const tile = df_tile_at(df, x, y);
//    massert(tile, "failed to get tile");
//    if (!tile_is_walkable(tile->type)) {
//        merror("cannot create entity on wall");
//        return ENTITYID_INVALID;
//    }
//    if (tile_has_live_npcs(g, tile, em)) {
//        merror("cannot create entity on tile with NPC");
//        return ENTITYID_INVALID;
//    }
//    entity* const e = e_new_shield_at(next_entityid++, x, y, fl, name);
//    if (!e) {
//        merror("failed to create entity");
//        return ENTITYID_INVALID;
//    }
//    em_add(em, e);
//    gs_add_entityid(g, e->id);
//    if (!df_add_at(df, e->id, x, y)) {
//        merror("failed to add entity to dungeon floor");
//        free(e);
//        return ENTITYID_INVALID;
//    }
//    return e->id;
//}

static loc_t* get_locs_around_entity(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    loc_t* locs = malloc(sizeof(loc_t) * 8);
    massert(locs, "failed to allocate memory for locs");
    int index = 0;
    loc_t oldloc = g_get_location(g, id);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            locs[index] = (loc_t){oldloc.x + i, oldloc.y + j, oldloc.z};
            index++;
        }
    }
    return locs;
}

static inline tile_t* get_first_empty_tile_around_entity(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    loc_t* locs = get_locs_around_entity(g, id);
    massert(locs, "locs is NULL");
    bool found = false;
    tile_t* tile = NULL;
    for (int i = 0; i < 8; i++) {
        loc_t loc = g_get_location(g, id);
        //tile = df_tile_at(g->d->floors[loc.z], locs[i].x, locs[i].y);
        tile = df_tile_at(g->d->floors[loc.z], locs[i]);
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

static loc_t get_random_empty_non_wall_loc_in_area(gamestate* const g, int z, int x, int y, int w, int h) {
    massert(g, "gamestate is NULL");
    massert(z >= 0, "floor is out of bounds");
    massert(z < g->d->num_floors, "floor is out of bounds");
    massert(x >= 0, "x is out of bounds");
    massert(x < g->d->floors[z]->width, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(y < g->d->floors[z]->height, "y is out of bounds");
    massert(w > 0, "w is out of bounds");
    massert(h > 0, "h is out of bounds");
    massert(x + w <= g->d->floors[z]->width, "x + w is out of bounds");
    massert(y + h <= g->d->floors[z]->height, "y + h is out of bounds");
    int c = -1;
    //    //loc_t* locations = get_empty_non_wall_locs(g->dungeon->floors[floor], &c);
    loc_t* locations = get_empty_non_wall_locs_in_area(g->d->floors[z], &c, x, y, w, h);
    massert(locations, "locations is NULL");
    massert(c > 0, "locations count is 0 or less");
    //    // pick a random location
    int index = rand() % c;
    loc_t loc = locations[index];
    free(locations);
    massert(loc.x >= 0, "loc.x is out of bounds");
    massert(loc.x < g->d->floors[z]->width, "loc.x is out of bounds");
    massert(loc.y >= 0, "loc.y is out of bounds");
    massert(loc.y < g->d->floors[z]->height, "loc.y is out of bounds");
    loc.z = z;
    return loc;
    //}
}
static loc_t get_random_empty_non_wall_loc(gamestate* const g, int floor) {
    massert(g, "gamestate is NULL");
    massert(floor >= 0, "floor is out of bounds");
    massert(floor < g->d->num_floors, "floor is out of bounds");
    return get_random_empty_non_wall_loc_in_area(g, floor, 0, 0, g->d->floors[floor]->width, g->d->floors[floor]->height);
}

//static void init_weapon_test(gamestate* g) {
//    massert(g, "gamestate is NULL");
//    // pick a random location
//    loc_t loc = get_random_empty_non_wall_loc(g, g->dungeon->current_floor);
//    create_sword_at(g, loc);
//}

//static void init_weapon_test2(gamestate* g) {
//    massert(g, "gamestate is NULL");
//    entity* sword = create_sword(g);
//    e_add_item_to_inventory(e, sword->id);
//    // equip the sword
//    e->weapon = sword->id;
//}

static void init_dungeon(gamestate* const g) {
    massert(g, "gamestate is NULL");
    g->d = d_create();
    massert(g->d, "failed to init dungeon");
    d_add_floor(g->d, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
}

static entityid npc_create(gamestate* const g, race_t rt, loc_t loc, const char* name) {
    massert(g, "gamestate is NULL");
    massert(name && name[0], "name is NULL or empty");
    massert(rt >= 0, "race_type is out of bounds: %s: %d", name, rt);
    massert(rt < RACE_COUNT, "race_type is out of bounds: %s: %d", name, rt);
    massert(loc.z >= 0, "z is out of bounds: %s: %d", name, loc.z);
    massert(loc.z < g->d->num_floors, "z is out of bounds: %s: %d", name, loc.z);
    dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    massert(df, "failed to get current dungeon floor");
    massert(loc.x >= 0, "x is out of bounds: %s: %d", name, loc.x);
    massert(loc.x < df->width, "x is out of bounds: %s: %d", name, loc.x);
    massert(loc.y >= 0, "y is out of bounds: %s: %d", name, loc.y);
    massert(loc.y < df->height, "y is out of bounds: %s: %d", name, loc.y);
    // can we create an entity at this location? no entities can be made on wall-types etc
    //tile_t* const tile = df_tile_at(df, x, y);
    tile_t* const tile = df_tile_at(df, loc);
    massert(tile, "failed to get tile");
    if (!tile_is_walkable(tile->type) || tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on tile with NPC");
        return ENTITYID_INVALID;
    }
    entityid id = next_entityid++;
    gs_add_entityid(g, id);
    //minfo("registering name: %s", name);
    g_register_comp(g, id, C_NAME);
    //minfo("registering type: %d", ENTITY_NPC);
    g_register_comp(g, id, C_TYPE);
    //minfo("registering race: %d", rt);
    g_register_comp(g, id, C_RACE);
    //minfo("registering direction: %d", DIR_RIGHT);
    g_register_comp(g, id, C_DIRECTION);
    //minfo("registering location: %d, %d, %d", loc.x, loc.y, loc.z);
    g_register_comp(g, id, C_LOCATION);
    //minfo("registering sprite_move: %d, %d", 0, 0);
    g_register_comp(g, id, C_SPRITE_MOVE);
    //minfo("registering dead: %d", false);
    g_register_comp(g, id, C_DEAD);
    //minfo("registering update: %d", false);
    g_register_comp(g, id, C_UPDATE);
    //minfo("registering attacking: %d", false);
    g_register_comp(g, id, C_ATTACKING);
    //minfo("registering blocking: %d", false);
    g_register_comp(g, id, C_BLOCKING);
    //minfo("registering block_success: %d", false);
    g_register_comp(g, id, C_BLOCK_SUCCESS);
    //minfo("registering damaged: %d", false);
    g_register_comp(g, id, C_DAMAGED);
    //minfo("registering default_action: %d", ENTITY_ACTION_WAIT);
    g_register_comp(g, id, C_DEFAULT_ACTION);
    //minfo("registering inventory");
    g_register_comp(g, id, C_INVENTORY);
    //minfo("registering target");
    g_register_comp(g, id, C_TARGET);
    //minfo("registering target_path");
    g_register_comp(g, id, C_TARGET_PATH);
    //minfo("registering equipment");
    g_register_comp(g, id, C_EQUIPMENT);

    g_register_comp(g, id, C_STATS);

    //minfo("adding name for id %d: %s", id, name);
    g_add_name(g, id, name);
    //minfo("adding type for id %d: %d", id, ENTITY_NPC);
    g_add_type(g, id, ENTITY_NPC);
    //minfo("adding race for id %d: %d", id, rt);
    g_add_race(g, id, rt);
    //minfo("adding direction for id %d: %d", id, DIR_RIGHT);
    g_add_direction(g, id, DIR_RIGHT);
    //minfo("adding location for id %d: %d, %d, %d", id, loc.x, loc.y, loc.z);
    g_add_location(g, id, loc);
    //minfo("adding sprite_move: %d, %d", 0, 0);
    g_add_sprite_move(g, id, (loc_t){0, 0}); // default
    //minfo("adding dead: %d", false);
    g_add_dead(g, id, false);
    //minfo("adding update: %d", false);
    g_add_update(g, id, false);
    //minfo("adding ");
    g_add_attacking(g, id, false);
    //minfo("adding ");
    g_add_blocking(g, id, false);
    //minfo("adding ");
    g_add_block_success(g, id, false);
    //minfo("adding ");
    g_add_damaged(g, id, false);
    //minfo("adding ");
    g_add_default_action(g, id, ENTITY_ACTION_WAIT);
    //minfo("adding ");
    g_add_inventory(g, id);
    //minfo("adding ");
    g_add_target(g, id, (loc_t){-1, -1, -1});
    //minfo("adding ");
    g_add_target_path(g, id);
    //minfo("adding ");
    g_add_equipment(g, id);

    g_add_stats(g, id);
    g_set_stat(g, id, STATS_LEVEL, 1);
    g_set_stat(g, id, STATS_MAXHP, 3);
    g_set_stat(g, id, STATS_HP, 3);

    if (!df_add_at(df, id, loc.x, loc.y)) {
        merror("failed to add entity to dungeon floor");
        //free(e);
        return ENTITYID_INVALID;
    }
    return id;
}

static entityid item_create(gamestate* const g, itemtype type, loc_t loc, const char* name) {
    massert(g, "gamestate is NULL");
    massert(name && name[0], "name is NULL or empty");
    massert(type >= 0, "item_type is out of bounds: %s: %d", name, type);
    massert(type < ITEM_TYPE_COUNT, "item_type is out of bounds: %s: %d", name, type);
    massert(loc.z >= 0, "z is out of bounds: %s: %d", name, loc.z);
    massert(loc.z < g->d->num_floors, "z is out of bounds: %s: %d", name, loc.z);
    dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    massert(df, "failed to get current dungeon floor");
    massert(loc.x >= 0, "x is out of bounds: %s: %d", name, loc.x);
    massert(loc.x < df->width, "x is out of bounds: %s: %d", name, loc.x);
    massert(loc.y >= 0, "y is out of bounds: %s: %d", name, loc.y);
    massert(loc.y < df->height, "y is out of bounds: %s: %d", name, loc.y);
    // can we create an entity at this location? no entities can be made on wall-types etc
    //tile_t* const tile = df_tile_at(df, x, y);
    tile_t* const tile = df_tile_at(df, loc);
    massert(tile, "failed to get tile");
    if (!tile_is_walkable(tile->type) || tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    if (tile_has_live_npcs(g, tile)) {
        merror("cannot create entity on tile with NPC");
        return ENTITYID_INVALID;
    }
    entityid id = next_entityid++;
    gs_add_entityid(g, id);
    //minfo("registering name: %s", name);
    g_register_comp(g, id, C_NAME);
    //minfo("registering type: %d", ENTITY_NPC);
    g_register_comp(g, id, C_TYPE);
    //minfo("registering race: %d", rt);
    //g_register_comp(g, id, C_RACE);
    //minfo("registering direction: %d", DIR_RIGHT);
    g_register_comp(g, id, C_DIRECTION);
    //minfo("registering location: %d, %d, %d", loc.x, loc.y, loc.z);
    g_register_comp(g, id, C_LOCATION);
    //minfo("registering sprite_move: %d, %d", 0, 0);
    g_register_comp(g, id, C_SPRITE_MOVE);
    //minfo("registering dead: %d", false);
    //g_register_comp(g, id, C_DEAD);
    //minfo("registering update: %d", false);
    g_register_comp(g, id, C_UPDATE);
    g_register_comp(g, id, C_ITEMTYPE);
    //minfo("registering attacking: %d", false);
    //g_register_comp(g, id, C_ATTACKING);
    //minfo("registering blocking: %d", false);
    //g_register_comp(g, id, C_BLOCKING);
    //minfo("registering block_success: %d", false);
    //g_register_comp(g, id, C_BLOCK_SUCCESS);
    //minfo("registering damaged: %d", false);
    //g_register_comp(g, id, C_DAMAGED);
    //minfo("registering default_action: %d", ENTITY_ACTION_WAIT);
    //g_register_comp(g, id, C_DEFAULT_ACTION);
    //minfo("registering inventory");
    //g_register_comp(g, id, C_INVENTORY);
    //minfo("registering target");
    //g_register_comp(g, id, C_TARGET);
    //minfo("registering target_path");
    //g_register_comp(g, id, C_TARGET_PATH);
    //minfo("registering equipment");
    //g_register_comp(g, id, C_EQUIPMENT);
    //g_register_comp(g, id, C_STATS);

    //minfo("adding name for id %d: %s", id, name);
    g_add_name(g, id, name);
    //minfo("adding type for id %d: %d", id, ENTITY_NPC);
    g_add_type(g, id, ENTITY_ITEM);
    //minfo("adding race for id %d: %d", id, rt);
    //g_add_race(g, id, rt);
    //minfo("adding direction for id %d: %d", id, DIR_RIGHT);
    g_add_direction(g, id, DIR_RIGHT);
    //minfo("adding location for id %d: %d, %d, %d", id, loc.x, loc.y, loc.z);
    g_add_location(g, id, loc);
    //minfo("adding sprite_move: %d, %d", 0, 0);
    g_add_sprite_move(g, id, (loc_t){0, 0}); // default
    //minfo("adding dead: %d", false);
    //g_add_dead(g, id, false);
    //minfo("adding update: %d", false);
    g_add_update(g, id, false);
    //minfo("adding ");
    //g_add_attacking(g, id, false);
    //minfo("adding ");
    //g_add_blocking(g, id, false);
    //minfo("adding ");
    //g_add_block_success(g, id, false);
    //minfo("adding ");
    //g_add_damaged(g, id, false);
    //minfo("adding ");
    //g_add_default_action(g, id, ENTITY_ACTION_WAIT);
    //minfo("adding ");
    //g_add_inventory(g, id);
    //minfo("adding ");
    //g_add_target(g, id, (loc_t){-1, -1, -1});
    //minfo("adding ");
    //g_add_target_path(g, id);
    //minfo("adding ");
    //g_add_equipment(g, id);
    g_add_itemtype(g, id, type);

    //g_add_stats(g, id);
    //g_set_stat(g, id, STATS_LEVEL, 1);
    //g_set_stat(g, id, STATS_MAXHP, 3);
    //g_set_stat(g, id, STATS_HP, 3);

    if (!df_add_at(df, id, loc.x, loc.y)) {
        merror("failed to add entity to dungeon floor");
        //free(e);
        return ENTITYID_INVALID;
    }
    return id;
}

static entityid weapon_create(gamestate* const g, weapontype type, loc_t loc, const char* name) {
    massert(g, "gamestate is NULL");

    entityid id = item_create(g, ITEM_WEAPON, loc, name);
    massert(id != ENTITYID_INVALID, "failed to create weapon");

    g_register_comp(g, id, C_WEAPONTYPE);
    g_add_weapontype(g, id, type);

    return id;
}

static entityid player_create(gamestate* const g, race_t rt, int x, int y, int z, const char* name) {
    massert(g, "gamestate is NULL");
    massert(name, "name is NULL");
    // use the previously-written liblogic_npc_create function
    //const entitytype_t type = ENTITY_PLAYER;
    //const entityid id = npc_create(g, rt, x, y, fl, name);
    const entityid id = npc_create(g, rt, (loc_t){x, y, z}, name);
    massert(id != ENTITYID_INVALID, "failed to create player");
    gamestate_set_hero_id(g, id);
    g_set_type(g, id, ENTITY_PLAYER);
    return id;
}

static void init_player(gamestate* const g) {
    massert(g, "gamestate is NULL");
    // setting it up so we can return a loc_t from a function
    // that can scan for an appropriate starting location
    loc_t loc = df_get_upstairs(g->d->floors[g->d->current_floor]);
    //minfo("loc: %d, %d, %d", loc.x, loc.y, loc.z);
    const int id = player_create(g, RACE_HUMAN, loc.x, loc.y, 0, "hero");
    massert(id != ENTITYID_INVALID, "failed to init hero");
    //minfo("hero id: %d", id);
    //e_set_maxhp(hero, 10);
    //e_set_hp(hero, 10);
    g->entity_turn = g->hero_id;
    msuccess("hero id %d", g->hero_id);
}

//static inline size_t tile_npc_count_at(gamestate* const g, int x, int y, int z) {
//    massert(g, "gamestate is NULL");
//    massert(df, "dungeon floor is NULL");
//    massert(x >= 0, "x is out of bounds");
//    massert(x < df->width, "x is out of bounds");
//    massert(y >= 0, "y is out of bounds");
//    massert(y < df->height, "y is out of bounds");
//    massert(z >= 0, "z is out of bounds");
//    massert(z < g->d->num_floors, "z is out of bounds");
//    dungeon_floor_t* const df = d_get_floor(g->d, z);
//    massert(df, "failed to get current dungeon floor");
//    tile_t* const tile = df_tile_at(df, x, y);
//    massert(tile, "failed to get tile");
//    return tile_live_npc_count(g, tile);
//}

//static loc_t* get_walkable_locs(dungeon_floor_t* df, int* cnt) {
//    massert(df, "dungeon floor is NULL");
//    massert(cnt, "cnt is NULL");
//    int c = df_count_walkable(df);
//    loc_t* locs = malloc(sizeof(loc_t) * c);
//    massert(locs, "malloc failed");
//    int i = 0;
//    for (int y = 0; y < df->height; y++) {
//        for (int x = 0; x < df->width; x++) {
//            tile_t* t = df_tile_at(df, x, y);
//            massert(t, "tile is NULL");
//            if (tile_is_walkable(t->type)) locs[i++] = (loc_t){x, y};
//        }
//    }
//    massert(i == c, "count mismatch");
//    *cnt = c;
//    return locs;
//}

//static loc_t* get_empty_locs(dungeon_floor_t* const df, int* count) {
//    massert(df, "dungeon floor is NULL");
//    massert(count, "count is NULL");
//    int c = df_count_empty(df);
//    loc_t* locs = malloc(sizeof(loc_t) * c);
//    massert(locs, "malloc failed");
//    int i = 0;
//    for (int y = 0; y < df->height; y++) {
//        for (int x = 0; x < df->width; x++) {
//            tile_t* t = df_tile_at(df, x, y);
//            if (tile_entity_count(t) == 0) { locs[i++] = (loc_t){x, y}; }
//        }
//    }
//    massert(i == c, "count mismatch");
//    *count = c;
//    return locs;
//}

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

            tile_t* t = df_tile_at(df, (loc_t){newx, newy, -1});
            tiletype_t type = t->type;
            if (tile_entity_count(t) == 0 && tile_is_walkable(type)) locs[i++] = (loc_t){newx, newy};
            if (i >= c) break;
        }
    }
    massert(i == c, "count mismatch: expected %d, got %d", c, i);
    *count = c;
    return locs;
}

//static loc_t* get_empty_non_wall_locs(dungeon_floor_t* const df, int* count) {
//    massert(df, "dungeon floor is NULL");
//    massert(count, "count is NULL");
//    return get_empty_non_wall_locs_in_area(df, count, 0, 0, df->width, df->height);
//}

//static entity* create_orc_at(gamestate* g, loc_t loc) {
//    entity* e = npc_create_ptr(g, RACE_ORC, loc.x, loc.y, loc.z, "orc");
//    massert(e, "orc create fail");
//    return e;
//}

//static entity* create_human_at(gamestate* g, loc_t loc) {
//    entity* e = npc_create_ptr(g, RACE_HUMAN, loc.x, loc.y, loc.z, "human");
//    massert(e, "human create fail");
//entity* sword = create_sword(g);
//e_add_item_to_inventory(e, sword->id);
//e->weapon = sword->id;
//    return e;
//}

//static entity* create_elf_at(gamestate* g, loc_t loc) {
//    entity* e = npc_create_ptr(g, RACE_ELF, loc.x, loc.y, loc.z, "elf");
//    massert(e, "elf create fail");
//entity* sword = create_sword(g);
//e_add_item_to_inventory(e, sword->id);
//e->weapon = sword->id;
//    return e;
//}

//static entity* create_dwarf_at(gamestate* g, loc_t loc) {
//    entity* e = npc_create_ptr(g, RACE_DWARF, loc.x, loc.y, loc.z, "dwarf");
//    massert(e, "dwarf create fail");
//entity* sword = create_sword(g);
//e_add_item_to_inventory(e, sword->id);
//e->weapon = sword->id;
//    return e;
//}

//static entity* create_halfling_at(gamestate* g, loc_t loc) {
//    entity* e = npc_create_ptr(g, RACE_HALFLING, loc.x, loc.y, loc.z, "halfling");
//    massert(e, "halfling create fail");
//entity* sword = create_sword(g);
//e_add_item_to_inventory(e, sword->id);
//e->weapon = sword->id;
//    return e;
//}

//static entity* create_goblin_at(gamestate* g, loc_t loc) {
//    entity* e = npc_create_ptr(g, RACE_GOBLIN, loc.x, loc.y, loc.z, "goblin");
//    massert(e, "goblin create fail");
//entity* sword = create_sword(g);
//e_add_item_to_inventory(e, sword->id);
//e->weapon = sword->id;
//    return e;
//}

//static void init_npcs_test_by_room(gamestate* const g) {
//    massert(g, "gamestate is NULL");
//    dungeon_floor_t* df = dungeon_get_floor(g->dungeon, 0);
//    massert(df, "floor is NULL");
//    int hallway_count = 0;
//    int room_count = 0;
//    int random_choice = 0;
//    room_data_t* hallways = NULL;
//    room_data_t* rooms = NULL;
//    entity* player = NULL;
//    entity* e = NULL;
//    hallways = df_get_rooms_with_prefix(df, &hallway_count, "hallway");
//    massert(hallways, "hallways is NULL");
//    rooms = df_get_rooms_with_prefix(df, &room_count, "room");
//    massert(rooms, "rooms is NULL");
//    // create doors in hallways
//    if (hallway_count > 0) {
//        for (int i = 0; i < hallway_count; i++) {
//            room_data_t hallway = hallways[i];
//            loc_t loc = get_random_empty_non_wall_loc_in_area(g, g->dungeon->current_floor, hallway.x, hallway.y, hallway.w, hallway.h);
//            entityid doorid = door_create(g, loc.x, loc.y, loc.z, "door");
//            massert(doorid != ENTITYID_INVALID, "door create fail");
//        }
//    }
//    // one NPC per room past the first
//    const int max_npcs = 3;
//    for (int i = 0; i < max_npcs; i++) {
//        room_data_t room = rooms[2];
//        loc_t loc = get_random_empty_non_wall_loc_in_area(g, g->dungeon->current_floor, room.x, room.y, room.w, room.h);
//        random_choice = rand() % 5;
//        if (random_choice == 0) {
//            e = create_orc_at(g, loc);
//        } else if (random_choice == 1) {
//            e = create_elf_at(g, loc);
//        } else if (random_choice == 2) {
//            e = create_dwarf_at(g, loc);
//        } else if (random_choice == 3) {
//            e = create_halfling_at(g, loc);
//        } else if (random_choice == 4) {
//            e = create_goblin_at(g, loc);
//        } else if (random_choice == 4) {
//            e = create_human_at(g, loc);
//        }
//        massert(e != ENTITYID_INVALID, "NPC create fail");
//        //e->target = (loc_t){player->x, player->y, player->floor};
//        e->target = g_get_location(g, player->id);
//    }
//}

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
            if (g->cam2d.zoom > 1.0) {
                g->cam2d.zoom -= DEFAULT_ZOOM_INCR;
            }
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
    //if (hero->inventory_count == 0) { return; }
    //if (inputstate_is_pressed(is, KEY_DOWN)) {
    //    g->inventory_menu_selection++;
    //    if (g->inventory_menu_selection >= hero->inventory_count) { g->inventory_menu_selection = 0; }
    //} else if (inputstate_is_pressed(is, KEY_UP)) {
    //    g->inventory_menu_selection--;
    //    if (g->inventory_menu_selection < 0) { g->inventory_menu_selection = hero->inventory_count - 1; }
    //} else if (inputstate_is_pressed(is, KEY_ENTER)) {
    //    // we need to grab the entityid of the selected item
    //    entityid item_id = hero->inventory[g->inventory_menu_selection];
    //    if (g_is_type(g, item_id, ENTITY_WEAPON)) {
    //        // attempt to equip the weapon
    //        // check if the hero is already equipped with the weapon
    //        if (hero->weapon == item_id) {
    //            add_message(g, "You are already using %s", "[placeholder]");
    //            g->controlmode = CONTROLMODE_PLAYER;
    //            g->display_inventory_menu = false;
    //            return;
    //        }
    //        hero->weapon = item_id;
    //        add_message_and_history(g, "%s equipped %s", "[placeholder]", "[placeholder]");
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->display_inventory_menu = false;
    //        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //        //} else if (item->type == ENTITY_SHIELD) {
    //    } else if (g_is_type(g, item_id, ENTITY_SHIELD)) {
    //        // attempt to equip the shield
    //        // check if the hero is already equipped with the shield
    //        if (hero->shield == item_id) {
    //            add_message(g, "You are already using %s", "[placeholder]");
    //            g->controlmode = CONTROLMODE_PLAYER;
    //            g->display_inventory_menu = false;
    //            return;
    //        }
    //        hero->shield = item_id;
    //        add_message_and_history(g, "%s equipped %s", "[placeholder]", "[placeholder]");
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->display_inventory_menu = false;
    //        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //        //} else if (item->type == ENTITY_POTION) {
    //    } else if (g_is_type(g, item_id, ENTITY_POTION)) {
    //        add_message(g, "Potion use is not handled yet!");
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->display_inventory_menu = false;
    //    } else {
    //        //add_message(g, "Unhandled item type: %d", item->type);
    //        add_message(g, "Unhandled item type: %d", g_get_type(g, item_id));
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->display_inventory_menu = false;
    //    }
    //} else if (inputstate_is_pressed(is, KEY_U)) {
    //    // we need to grab the entityid of the selected item
    //    entityid item_id = hero->inventory[g->inventory_menu_selection];
    //    //if (item->type == ENTITY_WEAPON) {
    //    if (g_is_type(g, item_id, ENTITY_WEAPON)) {
    //        // unequip the weapon
    //        hero->weapon = ENTITYID_INVALID;
    //        add_message_and_history(g, "%s unequipped %s", "[placeholder]", "[placeholder]");
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->display_inventory_menu = false;
    //        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //    } else if (g_is_type(g, item_id, ENTITY_SHIELD)) {
    //        // equip the shield
    //        hero->shield = ENTITYID_INVALID;
    //        add_message_and_history(g, "%s unequipped %s", "[placeholder]", "[placeholder]");
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->display_inventory_menu = false;
    //        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //    }
    //}
}

static inline void change_player_dir(gamestate* const g, direction_t dir) {
    massert(g, "Game state is NULL!");
    if (g_is_dead(g, g->hero_id)) return;
    g_update_direction(g, g->hero_id, dir);
    g_set_update(g, g->hero_id, true);
    update_equipped_shield_dir(g, g->hero_id);
}

static bool try_entity_pickup(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    g_set_update(g, id, true);
    // check if the player is on a tile with an item
    loc_t loc = g_get_location(g, id);
    dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    if (!df) {
        merror("Failed to get dungeon floor");
        return false;
    }
    tile_t* const tile = df_tile_at(df, loc);
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
        entitytype_t type = g_get_type(g, id);
        //if (type == ENTITY_WEAPON || type == ENTITY_SHIELD || type == ENTITY_POTION) {
        //    add_message_and_history(g, "%s picked up a %s", "[placeholder]", "[placeholder]");
        //    tile_remove(tile, id);
        //    e_add_item_to_inventory(e, id);
        //    if (g_is_type(g, e->id, ENTITY_PLAYER)) { g->flag = GAMESTATE_FLAG_PLAYER_ANIM; }
        //    return true;
        //} else {
        //    add_message(g, "Unhandled item type cannot be picked up: %d", type);
        //    return false;
        //}
    }
    add_message(g, "No items to pick up");
    return false;
}

//static inline void try_open_close_door(gamestate* const g, entity* const e, int x, int y, int fl) {
//    massert(g, "Game state is NULL!");
//    massert(e, "Entity is NULL!");
//    dungeon_floor_t* const df = d_get_floor(g->d, fl);
//    massert(df, "Failed to get dungeon floor");
//    tile_t* const tile = df_tile_at(df, x, y);
//    if (!tile) {
//        merror("Failed to get tile");
//        return;
//    }
//}

//static inline void try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl) {
//    massert(g, "Game state is NULL!");
//    massert(e, "Entity is NULL!");
//    dungeon_floor_t* const df = d_get_floor(g->d, fl);
//    massert(df, "Failed to get dungeon floor");
//    tile_t* const tile = df_tile_at(df, x, y);
//    if (tile && tile->has_wall_switch) {
//        tile->wall_switch_on = !tile->wall_switch_on;
//        msuccess("Wall switch flipped!");
//        int ws_event = tile->wall_switch_event;
//        if (ws_event <= -1) {
//            merror("Wall switch event is invalid");
//            return;
//        }
//        df_event_t event = df->events[ws_event];
//        tile_t* const event_tile = df_tile_at(df, event.x, event.y);
//        //massert(event_tile, "Failed to get event tile");
//        tiletype_t type = event_tile->type;
//        if (type == event.off_type) {
//            event_tile->type = event.on_type;
//        } else if (type == event.on_type) {
//            event_tile->type = event.off_type;
//        }
//        //add_message(g, "Wall switch flipped!");
//        // this is the basis for what we need to do next
//        // currently we have no entity passed into this function
//        // we need to fix that
//        entitytype_t etype = g_get_type(g, e->id);
//        if (etype == ENTITY_PLAYER) {
//            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
//        } else if (etype == ENTITY_NPC) {
//            g->flag = GAMESTATE_FLAG_NPC_ANIM;
//        }
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
//    }
//}

static void handle_input_player(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) {
        return;
    }
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
    if (strcmp(action, "none") != 0) {
        minfo("action: %s", action);
    }
    // check if the player is dead
    //if (hero->dead) return;
    //minfo("calling g_is_dead 8");
    if (g_is_dead(g, g->hero_id)) return;
    if (action) {
        if (g->player_changing_direction) {
            if (strcmp(action, "wait") == 0) {
                execute_action(g, g->hero_id, ENTITY_ACTION_WAIT);
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
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_LEFT);
        } else if (strcmp(action, "move_e") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_RIGHT);
        } else if (strcmp(action, "move_n") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_UP);
        } else if (strcmp(action, "move_s") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_DOWN);
        } else if (strcmp(action, "move_nw") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_UP_LEFT);
        } else if (strcmp(action, "move_ne") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_UP_RIGHT);
        } else if (strcmp(action, "move_sw") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_DOWN_LEFT);
        } else if (strcmp(action, "move_se") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_DOWN_RIGHT);
        } else if (strcmp(action, "attack") == 0) {
            loc_t loc = get_loc_from_dir(g_get_direction(g, g->hero_id));
            loc_t hloc = g_get_location(g, g->hero_id);
            try_entity_attack(g, g->hero_id, hloc.x + loc.x, hloc.y + loc.y);
        } else if (strcmp(action, "interact") == 0) {
            // we are hardcoding the flip switch interaction for now
            // but eventually this will be generalized
            // for instance u can talk to an NPC merchant using "interact"
            // or open a door, etc
            //msuccess("Space pressed!");
            //int tx = hero->x + get_x_from_dir(hero->direction);
            //direction_t dir = g_get_direction(g, g->hero_id);
            //loc_t hloc = g_get_location(g, g->hero_id);
            //int tx = hloc.x + get_x_from_dir(dir);
            //int ty = hloc.y + get_y_from_dir(dir);
            // check to see if there is a door
            //entity* door = get_door_from_tile(g, tx, ty, hloc.z);
            //if (door) {
            //    door->door_is_open = !door->door_is_open;
            //    //door->do_update = true;
            //    g_set_update(g, door->id, true);
            //}
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            //try_flip_switch(g, hero, tx, ty, hero->floor);
        } else if (strcmp(action, "pickup") == 0) {
            //try_entity_pickup(g, hero);
            try_entity_pickup(g, g->hero_id);
        } else if (strcmp(action, "toggle_camera") == 0) {
            g->controlmode = CONTROLMODE_CAMERA;
        }
    } else {
        merror("No action found for key");
    }
}

//static void try_entity_open_door(gamestate* g, entity* e, int x, int y) {
//    massert(g, "Game state is NULL!");
//    massert(e, "Entity is NULL!");
//    loc_t loc = g_get_location(g, e->id);
//    dungeon_floor_t* df = d_get_floor(g->d, loc.z);
//    massert(df, "Failed to get dungeon floor");
//    tile_t* tile = df_tile_at(df, x, y);
//    if (!tile) {
//        merror("Failed to get tile");
//        return;
//    }
//    if (tile_has_closed_door(g, x, y, loc.z)) {
//        entity* door = get_door_from_tile(g, x, y, loc.z);
//        massert(door, "door is NULL");
//        door->door_is_open = !door->door_is_open;
//        //door->do_update = true;
//        g_set_update(g, door->id, true);
//    }
//}

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
    int x = -1;
    int y = -1;
    int inventory_count = -1;
    entityid shield_id = -1;
    direction_t player_dir = DIR_NONE;
    direction_t shield_dir = DIR_NONE;
    bool is_b = false;
    bool test_guard = g->test_guard;
    //if (e) {
    //    loc_t loc = g_get_location(g, e->id);
    //    x = loc.x;
    //    y = loc.y;
    //    inventory_count = e->inventory_count;
    //    //player_dir = e->direction;
    //    player_dir = g_get_direction(g, e->id);
    //    shield_id = e->shield;
    //    if (shield_id != -1) {
    //        //shield_dir = shield->direction;
    //        shield_dir = g_get_direction(g, shield->id);
    //    }
    //    //is_b = e->is_blocking;
    //    is_b = g_get_blocking(g, e->id);
    //}
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
             g->d->current_floor + 1, // More user-friendly 1-based
             g->d->num_floors,
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
             test_guard);
    //e->block_success);
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
    //init_em(g);
    //minfo("liblogic_init: em initialized");
    init_player(g);
    //minfo("liblogic_init: player initialized");
    // test to create a weapon
    //init_weapon_test(g);
    //init_weapon_test2(g);
    //init_shield_test(g);
    //init_shield_test2(g);
    //init_potion_test(g, POTION_HP_SMALL, "small healing potion");
    //init_potion_test(g, POTION_HP_MEDIUM, "medium healing potion");
    //init_potion_test(g, POTION_HP_LARGE, "large healing potion");
    //init_npcs_test_by_room(g);
    //init_npc_test(g);
    init_sword_test(g);
    update_debug_panel_buffer(g);
}

static void init_npc_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    //loc_t loc = g_get_location(g, g->hero_id);

    loc_t* locs = get_locs_around_entity(g, g->hero_id);
    for (int i = 0; i < 8; i++) {
        entityid id = npc_create(g, RACE_ORC, locs[i], "orc");
        if (id != ENTITYID_INVALID) {
            g_set_default_action(g, id, ENTITY_ACTION_MOVE_A_STAR);
            break;
        }
    }
}

static void init_sword_test(gamestate* g) {
    massert(g, "gamestate is NULL");

    loc_t* locs = get_locs_around_entity(g, g->hero_id);
    for (int i = 0; i < 8; i++) {
        entityid id = weapon_create(g, WEAPON_SWORD, locs[i], "dummy sword");
        if (id != ENTITYID_INVALID) {
            // set item properties
            break;
        }
    }
}

static void update_player_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    //massert(e, "liblogic_update_player_state: hero is NULL");
    if (!g->gameover) {
        //if (e->dead) {
        //minfo("calling g_is_dead 0");
        if (g_is_dead(g, g->hero_id)) {
            add_message_and_history(g, "You died!");
            g->gameover = true;
        }
        return;
    }
    //if (e->dead) return;
    //minfo("calling g_is_dead 1");
    if (g_is_dead(g, g->hero_id)) return;
    //if (e_get_hp(e) <= 0) {
    //    g_update_dead(g, e->id, true);
    //    e->do_update = true;
    //}
}

static inline void update_npc_state(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    //massert(e, "update_npc_state: entity is NULL");
    //if (e->dead) return;
    //minfo("calling g_is_dead 2");
    if (g_is_dead(g, id)) return;
    //if (e->stats.hp <= 0) {
    //    e->dead = true;
    //    e->do_update = true;
    //    merror("NPC is dead!");
    //    return;
    //}
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
    if (id == g->hero_id) {
        minfo("Skipping hero");
        return; // Skip the hero
    }
    if (g_is_type(g, id, ENTITY_NPC) && !g_is_dead(g, id)) {
        minfo("Handling NPC %d", id);
        execute_action(g, id, g_get_default_action(g, id));
    }
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
    g_set_blocking(g, g->hero_id, false);
    g->test_guard = false;
}

static inline void reset_player_block_success(gamestate* const g) {
    massert(g, "Game state is NULL!");
    g_set_block_success(g, g->hero_id, false);
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
    // free the dungeon
    d_free(g->d);
}

// Check if a location is traversable (walkable and unoccupied)
static inline bool is_traversable(gamestate* const g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    // get the dungeon floor
    dungeon_floor_t* df = d_get_floor(g->d, z);
    massert(df, "floor is NULL");
    // Check map bounds
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) return false;
    // Get the current tile
    tile_t* tile = df_tile_at(df, (loc_t){x, y, z});
    massert(tile, "tile is NULL");
    // Check if the tile type is walkable
    if (!tile_is_walkable(tile->type)) return false;
    // Check for blocking entities
    // Comment out the next line if entity blocking is interfering with pathfinding tests
    if (tile_has_live_npcs(g, tile)) return false;
    return true;
}
