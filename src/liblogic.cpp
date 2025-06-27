//#include "bonus_table.h"
//#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
//#include "dungeon_tile_type.h"
//#include "entity_actions.h"
//#include "entityid.h"
//#include "entitytype.h"
#include "gamestate.h"
//#include "gamestate_flag.h"
#include "inputstate.h"
//#include "inventory_sort.h"
//#include "keybinding.h"
//#include "libgame_defines.h"
#include "liblogic.h"
#include "massert.h"
//#include "mprint.h"
//#include "path_node.h"
//#include "potion.h"
//#include "race.h"
//#include "roll.h"
//#include "stats_slot.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <raylib.h>

using std::shared_ptr;
using std::string;
//using std::vector;

int liblogic_restart_count = 0;

/*
static inline bool
is_traversable(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(z >= 0, "floor is out of bounds");
    massert(z < g->d->num_floors, "floor is out of bounds");
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, z);
    massert(df, "failed to get dungeon floor");
    shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "failed to get tile");
    return tile_is_walkable(t->type);
}
*/


/*
static inline shared_ptr<tile_t>
get_first_empty_tile_around_entity(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    vec3 loc = g_get_location(g, id);
    massert(loc.z >= 0 && loc.z < g->d->num_floors, "floor is out of bounds");
    //dungeon_floor_t* const df = d_get_floor(g->dungeon, loc.z);
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);
    massert(df, "failed to get dungeon floor");
    // check the 8 surrounding tiles
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // skip the entity's own tile
            int x = loc.x + dx;
            int y = loc.y + dy;
            if (is_traversable(g, x, y, loc.z)) {
                shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, loc.z});
                if (t && t->entities->empty()) {
                    return t; // found an empty tile
                }
            }
        }
    }
    return nullptr; // no empty tile found
}
*/

/*
static inline void reset_player_blocking(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    g_set_blocking(g, g->hero_id, false);
    g_set_block_success(g, g->hero_id, false);
    g_set_update(g, g->hero_id, true);
}
*/

/*
static inline void reset_player_block_success(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    g_set_block_success(g, g->hero_id, false);
    g_set_update(g, g->hero_id, true);
}
*/

/*
static inline void update_npc_state(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // check if the NPC is dead
    if (g_is_dead(g, id)) return;
    // check if the NPC is blocking
    if (g_get_blocking(g, id)) {
        // reset the blocking state
        g_set_blocking(g, id, false);
        g_set_update(g, id, true);
    }
    // check if the NPC is blocked
    if (g_get_block_success(g, id)) {
        g_set_block_success(g, id, false);
        g_set_update(g, id, true);
    }
}
*/

//static void update_player_tiles_explored(shared_ptr<gamestate> g);
//static vec3 get_random_available_loc_in_area(shared_ptr<gamestate> g,
//                                             int floor,
//                                             int x0,
//                                             int y0,
//                                             int w,
//                                             int h);
//static vec3 get_base_attack_damage_for_race(race_t race);
//static vec3* get_available_locs_in_area(shared_ptr<gamestate> g,
//                                        dungeon_floor_t* const df,
//                                        int* count,
//                                        int x0,
//                                        int y0,
//                                        int w,
//                                        int h);
//static int calc_challenge_rating(shared_ptr<gamestate> g, entityid id);
//static vec3* get_locs_around_entity(gamestate* const g, entityid id);
//static vec3*
//get_locs_around_entity(shared_ptr<gamestate> g, entityid id, int* count);
//static void handle_input_gameplay_settings(const inputstate* const is, gamestate* const g);
//static void handle_input_gameplay_settings(shared_ptr<inputstate> is,
//                                           shared_ptr<gamestate> g);
//static void handle_attack_blocked(gamestate* const g, entityid attacker_id, entityid target_id, bool* atk_successful);
//static void handle_attack_blocked(shared_ptr<gamestate> g,
//                                  entityid attacker_id,
//                                  entityid target_id,
//                                  bool* atk_successful);
//static void handle_input_help_menu(const inputstate* const is, gamestate* const g);
//static void handle_input_help_menu(shared_ptr<inputstate> is,
//                                   shared_ptr<gamestate> g);
//static void handle_level_up(gamestate* const g, entityid id);
//static void handle_level_up(shared_ptr<gamestate> g, entityid id);
//static void handle_input_sort_inventory(const inputstate* const is, gamestate* const g);
//static void handle_input_sort_inventory(shared_ptr<inputstate> is,
//                                        shared_ptr<gamestate> g);
//static void init_dungeon(gamestate* const g);
//static void init_dungeon(shared_ptr<gamestate> g);
//static void update_player_state(gamestate* const g);
//static void update_player_state(shared_ptr<gamestate> g);
//static void update_debug_panel_buffer(gamestate* const g);
static void update_debug_panel_buffer(shared_ptr<gamestate> g);
//static void handle_camera_move(gamestate* const g, const inputstate* const is);
//static void handle_camera_move(shared_ptr<gamestate> g,
//                               shared_ptr<inputstate> is);
//static void handle_input(const inputstate* const is, gamestate* const g);
//static void handle_input(shared_ptr<gamestate> g, shared_ptr<inputstate> is);
//static void handle_input_camera(shared_ptr<gamestate> g,
//                                shared_ptr<inputstate> is);
//static void handle_input_player(shared_ptr<gamestate> g,
//                                shared_ptr<inputstate> is);
//static void handle_input_inventory(shared_ptr<gamestate> g,
//                                   shared_ptr<inputstate> is);
//static void add_message_history(gamestate* const g, const char* fmt, ...);
//static void add_message_history(shared_ptr<gamestate> g, const char* fmt, ...);
//static void add_message_and_history(gamestate* g, const char* fmt, ...);
//static void
//add_message_and_history(shared_ptr<gamestate> g, const char* fmt, ...);
//static void add_message(gamestate* g, const char* fmt, ...);
//static void add_message(shared_ptr<gamestate> g, const char* fmt, ...);
//static void try_entity_move_a_star(gamestate* const g, entityid id);
//static void try_entity_move_a_star(shared_ptr<gamestate> g, entityid id);
//static void try_entity_move(gamestate* const g, entityid id, int x, int y);
//static void try_entity_move(shared_ptr<gamestate> g, entityid id, int x, int y);
//static void try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y);
//static void try_entity_attack(shared_ptr<gamestate> g,
//                              entityid attacker_id,
//                              int target_x,
//                              int target_y);
//static void try_entity_traverse_floors(gamestate* const g, entityid id);
//static void try_entity_traverse_floors(shared_ptr<gamestate> g, entityid id);
//static void check_and_handle_level_up(gamestate* const g, entityid id);
//static void check_and_handle_level_up(shared_ptr<gamestate> g, entityid id);
//static const char* get_action_key(const inputstate* const is, gamestate* const g);
//static const char* get_action_key(shared_ptr<gamestate> g,
//                                  shared_ptr<inputstate> is);
//static entityid player_create(gamestate* const g, race_t rt, int x, int y, int fl, string name);
//static entityid player_create(shared_ptr<gamestate> g,
//                              race_t rt,
//                              int x,
//                              int y,
//                              int fl,
//                              string name);
//static entityid npc_create(gamestate* const g, race_t rt, vec3 loc, string name);
//static entityid
//npc_create(shared_ptr<gamestate> g, race_t rt, vec3 loc, string name);
//static entityid item_create(gamestate* const g, itemtype type, vec3 loc, const char* name);
//static entityid
//item_create(shared_ptr<gamestate> g, itemtype type, vec3 loc, const char* name);
//static entityid potion_create(gamestate* const g, vec3 loc, potiontype type, const char* name);
//static entityid potion_create(shared_ptr<gamestate> g,
//                              vec3 loc,
//                              potiontype type,
//                              const char* name);

//static bool entities_adjacent(gamestate* const g, entityid id0, entityid id1);
//static bool npc_create_set_stats(gamestate* const g, vec3 loc, race_t race);
//static int calc_next_lvl_xp(gamestate* const g, entityid id);
//static int calc_reward_xp(gamestate* const g, entityid attacker_id, entityid target_id);
//static bool
//entities_adjacent(shared_ptr<gamestate> g, entityid id0, entityid id1);
//static entityid
//npc_create_set_stats(shared_ptr<gamestate> g, vec3 loc, race_t race);

//static int get_hitdie_for_race(race_t race);
//static int calc_next_lvl_xp(shared_ptr<gamestate> g, entityid id);
//static int calc_reward_xp(shared_ptr<gamestate> g,
//                          entityid attacker_id,
//                          entityid target_id);

//static race_t get_random_race();

/*
static int calc_reward_xp(shared_ptr<gamestate> g,
                          entityid attacker_id,
                          entityid target_id) {
    massert(g, "gamestate is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker id is invalid");
    massert(target_id != ENTITYID_INVALID, "target id is invalid");
    // get the entity type
    entitytype_t attacker_type = g_get_type(g, attacker_id);
    massert(attacker_type == ENTITY_NPC || attacker_type == ENTITY_PLAYER,
            "attacker type is not NPC or Player");
    entitytype_t target_type = g_get_type(g, target_id);
    massert(target_type == ENTITY_NPC || target_type == ENTITY_PLAYER,
            "target type is not NPC or Player");
    if (target_type != ENTITY_NPC && target_type != ENTITY_PLAYER) {
        merror("Target type is not NPC or Player");
        return 0; // no reward for non-NPC/Player targets
    }
    if (attacker_type == ENTITY_PLAYER && target_type == ENTITY_PLAYER) {
        merror("Cannot calculate reward xp for player vs player combat");
        return 0; // no reward for player vs player combat
    }

    // get the challenge rating
    int challenge_rating = calc_challenge_rating(g, target_id);
    massert(challenge_rating >= 0, "challenge rating is negative");
    minfo("Challenge rating for target %d is %d", target_id, challenge_rating);
    // get the attacker's level
    int attacker_level = g_get_stat(g, attacker_id, STATS_LEVEL);
    massert(attacker_level >= 0, "attacker level is negative");
    // calculate the reward xp
    int base_xp = challenge_rating * 2;
    //int xp_modifier = pow(1.5, challenge_rating - attacker_level);
    int base = 2;
    int exponent = challenge_rating - attacker_level;
    float xp_modifier = pow(base, exponent);
    int reward_xp = (int)round(base_xp * xp_modifier);
    massert(reward_xp >= 0, "reward xp is negative");
    return reward_xp;
}
*/

//static int calc_challenge_rating(gamestate* const g, entityid id) {
/*
static int calc_challenge_rating(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // get the entity type
    entitytype_t type = g_get_type(g, id);
    massert(type == ENTITY_NPC || type == ENTITY_PLAYER,
            "entity type is not NPC or Player");
    if (type != ENTITY_NPC || type != ENTITY_PLAYER) {
        merror("Entity type is not NPC or Player");
        return 0; // no challenge rating for non-NPC entities
    }
    // get the current level
    int lvl = g_get_stat(g, id, STATS_LEVEL);
    massert(lvl >= 0, "level is negative");
    int wpn_bonus = 0;
    // check to see if the NPC has an equipped weapon
    entityid weapon_id = g_get_equipment(g, id, EQUIP_SLOT_WEAPON);
    if (weapon_id != ENTITYID_INVALID) {
        wpn_bonus = 1;
    }
    // check to see if the NPC has an equipped shield
    int shield_bonus = 0;
    entityid shield_id = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield_id != ENTITYID_INVALID) {
        shield_bonus = 1;
    }
    // calculate the challenge rating
    int challenge_rating = lvl + wpn_bonus + shield_bonus;
    massert(challenge_rating >= 0, "challenge rating is negative");
    // set the challenge rating
    return challenge_rating;
}
*/

//static int calc_next_lvl_xp(gamestate* const g, entityid id) {
/*
static int calc_next_lvl_xp(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // get the current level
    int lvl = g_get_stat(g, id, STATS_LEVEL);
    massert(lvl >= 0, "level is negative");
    // calculate the next level's xp
    int base_xp = 10;
    int next_lvl_xp = base_xp * (powl(2, lvl) - 1);
    massert(next_lvl_xp >= 0, "next level xp is negative");
    // set the next level's xp
    return next_lvl_xp;
}
*/

/*
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
    vsnprintf(g->msg_history.messages[g->msg_history.count],
              MAX_MSG_LENGTH - 1,
              fmt,
              args);
    va_end(args);
    g->msg_history.messages[g->msg_history.count][MAX_MSG_LENGTH - 1] =
        '\0'; // null term
    g->msg_history.count++;
}
*/

/*
static void add_message_and_history(gamestate* g, const char* fmt, ...) {
    massert(g, "gamestate is NULL");
    massert(fmt, "format string is NULL");
    if (g->msg_system.count >= MAX_MESSAGES) {
        mwarning("Message queue full!");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(g->msg_system.messages[g->msg_system.count],
              MAX_MSG_LENGTH - 1,
              fmt,
              args);
    va_end(args);
    add_message_history(g, g->msg_system.messages[g->msg_system.count]);
    g->msg_system.count++;
    g->msg_system.is_active = true;
}
*/

/*
static void add_message(gamestate* g, const char* fmt, ...) {
    massert(g, "gamestate is NULL");
    massert(fmt, "format string is NULL");
    if (g->msg_system.count >= MAX_MESSAGES) {
        mwarning("Message queue full!");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(g->msg_system.messages[g->msg_system.count],
              MAX_MSG_LENGTH - 1,
              fmt,
              args);
    va_end(args);
    g->msg_system.count++;
    g->msg_system.is_active = true;
}
*/

//static void update_equipped_shield_dir(gamestate* g, entity* e) {
//static void update_equipped_shield_dir(gamestate* g, entityid id) {
//    massert(g, "gamestate is NULL");
//    massert(id != ENTITYID_INVALID, "entity id is invalid");
//massert(e, "entity is NULL");
//if (e->shield != ENTITYID_INVALID) {
//    if (shield) {
//shield->direction = e->direction;
//        g_update_direction(g, shield->id, g_get_direction(g, e->id));
//shield->do_update = true;
//        g_set_update(g, shield->id, true);
//    }
//}
//}

//static inline int tile_npc_living_count(const gamestate* const g, int x, int y, int z) {
/*
static inline int
tile_npc_living_count(shared_ptr<gamestate> g, int x, int y, int z) {
    // Validate inputs
    massert(g, "gamestate is NULL");
    massert(z >= 0, "floor is out of bounds");
    massert(z < g->d->num_floors, "floor is out of bounds");
    //const dungeon_floor_t* const df = d_get_floor(g->dungeon, z);
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, z);
    massert(df, "failed to get dungeon floor");
    //const tile_t* const t = df_tile_at(df, (vec3){x, y, z});
    shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "failed to get tile");
    // Count living NPCs
    int count = 0;
    for (int i = 0; (size_t)i < t->entities->size(); i++) {
        const entityid eid = tile_get_entity(t, i);
        if (eid == ENTITYID_INVALID) continue;
        if (g_is_type(g, eid, ENTITY_NPC) && !g_is_dead(g, eid)) count++;
        if (g_is_type(g, eid, ENTITY_PLAYER) && !g_is_dead(g, eid)) count++;
    }
    return count;
}
*/

//static void try_entity_move(gamestate* const g, entity* const e, int x, int y) {
//static void try_entity_move(gamestate* const g, entityid id, int x, int y) {
/*
static void
try_entity_move(shared_ptr<gamestate> g, entityid id, int x, int y) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    //massert(e, "Entity is NULL!");
    //e->do_update = true;
    //g_set_update(g, id, true);
    g_set_update(g, id, true);
    //e->direction = get_dir_from_xy(x, y);
    g_update_direction(g, id, get_dir_from_xy(x, y));
    vec3 loc = g_get_location(g, id);
    int ex = loc.x + x;
    int ey = loc.y + y;
    int z = loc.z;
    //dungeon_floor_t* const df = d_get_floor(g->dungeon, z);
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, z);
    if (!df) return;
    // i feel like this might be something we can set elsewhere...like after the player input phase?
    //tile_t* const tile = df_tile_at(df, (vec3){ex, ey, z});
    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){ex, ey, z});
    if (!tile) return;
    if (!tile_is_walkable(tile->type)) return;
    //if (tile_has_closed_door(g, ex, ey, floor)) {
    //    merror("Cannot move, tile has a closed door");
    //    return;
    //}
    if (tile_npc_living_count(g, ex, ey, z) > 0) return;
    //if (player_on_tile(g, ex, ey, z)) {
    //    merror("Cannot move, player on tile");
    //    return;
    //}
    //if (!df_remove_at(df, id, loc.x, loc.y)) return;
    if (!df_remove_at(df, id, loc.x, loc.y)) return;
    if (!df_add_at(df, id, ex, ey)) return;
    g_update_location(g, id, (vec3){ex, ey, z});

    //g_update_sprite_move(g, id, (vec3){x * DEFAULT_TILE_SIZE, y * DEFAULT_TILE_SIZE, 0});
    g_update_sprite_move(g,
                         id,
                         (Rectangle){(float)(x * DEFAULT_TILE_SIZE),
                                     (float)(y * DEFAULT_TILE_SIZE),
                                     0,
                                     0});

    if (id == g->hero_id) {
        update_player_tiles_explored(g);
    }
    // at this point the move is 'successful'
    //update_equipped_shield_dir(g, id);
    // get the entity's new tile
    //tile_t* const new_tile = df_tile_at(df, (vec3){ex, ey, z});
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
    if (g_is_type(g, id, ENTITY_PLAYER)) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
}
*/

//static void try_entity_move_a_star(gamestate* const g, entityid id) {
/*
static void try_entity_move_a_star(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // for testing, we will hardcode an update to the entity's target
    // realistically, we should actually use a target ID and do location lookups on every update
    // however, for this test, we will instead hardcode the target to point to the hero's location
    // first, grab the hero id and then the hero entity pointer
    vec3 hloc = g_get_location(g, g->hero_id);
    vec3 eloc = g_get_location(g, id);
    vec3 hloc_cast = {hloc.x, hloc.y, hloc.z};
    vec3 eloc_cast = {eloc.x, eloc.y, eloc.z};
    //dungeon_floor_t* df = d_get_floor(g->dungeon, eloc.z);
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, eloc.z);
    //massert(df, "dungeon floor is NULL");
    int target_path_length = 0;
    vec3* target_path =
        find_path(eloc_cast, hloc_cast, df, &target_path_length);
    if (target_path) {
        if (target_path_length >= 2) {
            vec3 loc = target_path[target_path_length - 2];
            if (entities_adjacent(g, id, g->hero_id)) {
                try_entity_attack(g, id, loc.x, loc.y);
            } else {
                int dx = loc.x - eloc.x;
                int dy = loc.y - eloc.y;
                try_entity_move(g, id, dx, dy);
            }
        }
    }
}
*/

//static void try_entity_move_random(gamestate* const g, entityid id) {
/*
static void try_entity_move_random(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    int x = (rand() % 3) - 1;
    int y = 0;
    //x = x == 0 ? -1 : x == 1 ? 0 : 1;
    // if x is 0, y cannot also be 0
    if (x == 0) {
        y = rand() % 2 == 0 ? -1 : 1;
    } else {
        y = rand() % 3;
        y = y == 0 ? -1 : y == 1 ? 0 : 1;
    }
    try_entity_move(g, id, x, y);
}
*/

/*
static inline void handle_attack_success_gamestate_flag(shared_ptr<gamestate> g,
                                                        entitytype_t type,
                                                        bool success) {
    //if (!success) {
    //    if (type == ENTITY_PLAYER)
    //        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //    else if (type == ENTITY_NPC)
    //        g->flag = GAMESTATE_FLAG_NPC_ANIM;
    //    else
    //        g->flag = GAMESTATE_FLAG_NONE;
    //} else {
    //    if (type == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //}
    g->flag = success && type == ENTITY_PLAYER    ? GAMESTATE_FLAG_PLAYER_ANIM
              : !success && type == ENTITY_PLAYER ? GAMESTATE_FLAG_PLAYER_ANIM
              : !success && type == ENTITY_NPC    ? GAMESTATE_FLAG_NPC_ANIM
              : !success                          ? GAMESTATE_FLAG_NONE
                                                  : g->flag;
}
*/

//static void handle_attack_success(gamestate* const g, entityid atk_id, entityid tgt_id, bool* atk_successful) {
/*
static void handle_attack_success(shared_ptr<gamestate> g,
                                  entityid atk_id,
                                  entityid tgt_id,
                                  bool* atk_successful) {
    massert(g, "gamestate is NULL");
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
    massert(atk_successful, "attack_successful is NULL");
    entitytype_t tgttype = g_get_type(g, tgt_id);
    if (*atk_successful) {
        entityid attacker_weapon_id =
            g_get_equipment(g, atk_id, EQUIP_SLOT_WEAPON);
        int dmg = 1;
        if (attacker_weapon_id == ENTITYID_INVALID) {
            // no weapon
            // get the entity's base attack damage
            vec3 dmg_roll = g_get_base_attack_damage(g, atk_id);
            dmg = do_roll(dmg_roll);
            int atk_bonus = g_get_stat(g, atk_id, STATS_ATTACK_BONUS);
            dmg += atk_bonus;
        } else {
            // weapon
            // we will calculate damage based on weapon attributes
            massert(g_has_damage(g, attacker_weapon_id),
                    "attacker weapon does not have damage attached");
            vec3 dmg_roll = g_get_damage(g, attacker_weapon_id);
            dmg = do_roll(dmg_roll);
            int atk_bonus = g_get_stat(g, atk_id, STATS_ATTACK_BONUS);
            dmg += atk_bonus;
        }
        g_set_damaged(g, tgt_id, true);
        g_set_update(g, tgt_id, true);
        int hp = g_get_stat(g, tgt_id, STATS_HP);
        if (hp <= 0) {
            merror("Target is already dead, hp was: %d", hp);
            g_update_dead(g, tgt_id, true);
            return;
        }
        hp -= dmg;
        g_set_stat(g, tgt_id, STATS_HP, hp);
        if (tgttype == ENTITY_PLAYER) {
            add_message_history(g,
                                "%s attacked you for %d damage!",
                                g_get_name(g, atk_id).c_str(),
                                dmg);
        } else if (tgttype == ENTITY_NPC) {
            add_message_history(g,
                                "%s attacked %s for %d damage!",
                                g_get_name(g, atk_id).c_str(),
                                g_get_name(g, tgt_id).c_str(),
                                dmg);
        }
        if (hp <= 0) {
            g_update_dead(g, tgt_id, true);
            if (tgttype == ENTITY_NPC) {
                add_message_history(g,
                                    "%s killed %s!",
                                    g_get_name(g, atk_id).c_str(),
                                    g_get_name(g, tgt_id).c_str());
                // increment attacker's xp
                int old_xp = g_get_stat(g, atk_id, STATS_XP);
                massert(old_xp >= 0, "attacker's xp is negative");
                int reward_xp = calc_reward_xp(g, atk_id, tgt_id);
                massert(reward_xp >= 0, "reward xp is negative");
                int new_xp = old_xp + reward_xp;
                massert(new_xp >= 0, "new xp is negative");
                g_set_stat(g, atk_id, STATS_XP, new_xp);
                vec3 loc = g_get_location(g, tgt_id);
                vec3 loc_cast = {loc.x, loc.y, loc.z};
                entityid id = ENTITYID_INVALID;
                while (id == ENTITYID_INVALID)
                    id = potion_create(g,
                                       loc_cast,
                                       POTION_HEALTH_SMALL,
                                       "small health potion");
            }
        } else {
            g_update_dead(g, tgt_id, false);
        }
    } else {
        // handle attack miss
        if (tgttype == ENTITY_PLAYER)
            add_message_history(
                g, "%s's attack missed!", g_get_name(g, atk_id).c_str());
        else if (tgttype == ENTITY_NPC)
            add_message_history(g,
                                "%s missed %s!",
                                g_get_name(g, atk_id).c_str(),
                                g_get_name(g, tgt_id).c_str());
    }
    //e_set_hp(target, e_get_hp(target) - dmg); // Reduce HP by 1
    //if (target->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked you for %d damage!", attacker->name, dmg);
    //if (attacker->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked %s for %d damage!", attacker->name, target->name, dmg);
    //if (e_get_hp(target) <= 0) {
    //target->dead = true;
    //g_update_dead(g, tgt_id, true);
    //}
}
*/

//static void handle_attack_blocked(gamestate* const g, entityid attacker_id, entityid target_id, bool* atk_successful) {
/*
static void handle_attack_blocked(shared_ptr<gamestate> g,
                                  entityid attacker_id,
                                  entityid target_id,
                                  bool* atk_successful) {
    massert(g, "gamestate is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(target_id != ENTITYID_INVALID, "target entity id is invalid");
    massert(atk_successful, "attack_successful is NULL");
    *atk_successful = false;
    g_set_damaged(g, target_id, false);
    g_set_block_success(g, target_id, true);
    g_set_update(g, target_id, true);
    //entitytype_t tgttype = g_get_type(g, target_id);
    //if (tgttype == ENTITY_PLAYER) {
    //} else if (tgttype == ENTITY_NPC) {
    add_message_history(g,
                        "%s blocked %s's attack!",
                        g_get_name(g, target_id).c_str(),
                        g_get_name(g, attacker_id).c_str());
    //}
    //if (target->type == ENTITY_PLAYER) { add_message_and_history(g, "%s blocked %s's attack!", target->name, attacker->name); }
}
*/

/*
static bool handle_shield_check(shared_ptr<gamestate> g,
                                entityid attacker_id,
                                entityid target_id,
                                int attack_roll,
                                int base_ac,
                                bool* attack_successful) {
    // if you have a shield at all, the attack will get auto-blocked
    entityid shield_id = g_get_equipment(g, target_id, EQUIP_SLOT_SHIELD);
    if (shield_id != ENTITYID_INVALID) {
        int shield_ac = g_get_ac(g, shield_id);
        int total_ac = base_ac + shield_ac;
        if (attack_roll < total_ac) {
            *attack_successful = false;
            handle_attack_blocked(g, attacker_id, target_id, attack_successful);
            return false;
        }
    }
    msuccess("Attack successful");
    *attack_successful = true;
    handle_attack_success(g, attacker_id, target_id, attack_successful);
    return true;
}
*/

//static inline bool handle_attack_helper_innerloop(gamestate* const g, tile_t* tile, int i, entityid attacker_id, bool* attack_successful) {
/*
static inline bool handle_attack_helper_innerloop(shared_ptr<gamestate> g,
                                                  shared_ptr<tile_t> tile,
                                                  int i,
                                                  entityid attacker_id,
                                                  bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(i >= 0, "i is out of bounds");
    //massert((size_t)i < tile->entity_max, "i is out of bounds");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(attack_successful, "attack_successful is NULL");

    entityid target_id = tile->entities->at(i);

    if (target_id == ENTITYID_INVALID) return false;
    entitytype_t type = g_get_type(g, target_id);
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) return false;
    if (g_is_dead(g, target_id)) return false;
    // lets try an experiment...
    // get the armor class of the target
    int base_ac = g_get_stat(g, target_id, STATS_AC);
    int base_str = g_get_stat(g, attacker_id, STATS_STR);
    int str_bonus = bonus_calc(base_str);
    int atk_bonus = g_get_stat(g, attacker_id, STATS_ATTACK_BONUS);
    int attack_roll = rand() % 20 + 1 + str_bonus +
                      atk_bonus; // 1d20 + str bonus + attack bonus
    *attack_successful = false;
    if (attack_roll >= base_ac)
        return handle_shield_check(
            g, attacker_id, target_id, attack_roll, base_ac, attack_successful);
    // attack misses
    handle_attack_success(g, attacker_id, target_id, attack_successful);
    return false;
}
*/

//static void handle_attack_helper(gamestate* const g, tile_t* tile, entityid attacker_id, bool* successful) {
/*
static void handle_attack_helper(shared_ptr<gamestate> g,
                                 shared_ptr<tile_t> tile,
                                 entityid attacker_id,
                                 bool* successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(successful, "attack_successful is NULL");
    //for (size_t i = 0; i < tile->entity_max; i++) {
    for (int i = 0; (size_t)i < tile->entities->size(); i++) {
        handle_attack_helper_innerloop(g, tile, i, attacker_id, successful);
    }
}
*/

/*
static void try_entity_attack(shared_ptr<gamestate> g,
                              entityid atk_id,
                              int tgt_x,
                              int tgt_y) {
    massert(g, "gamestate is NULL");
    massert(!g_is_dead(g, atk_id), "attacker entity is dead");
    vec3 loc = g_get_location(g, atk_id);
    //dungeon_floor_t* const floor = d_get_floor(g->dungeon, loc.z);
    shared_ptr<dungeon_floor_t> floor = d_get_floor(g->dungeon, loc.z);
    massert(floor, "failed to get dungeon floor");
    //tile_t* const tile = df_tile_at(floor, (vec3){tgt_x, tgt_y, loc.z});
    shared_ptr<tile_t> tile = df_tile_at(floor, (vec3){tgt_x, tgt_y, loc.z});
    if (!tile) return;
    // Calculate direction based on target position
    bool ok = false;
    vec3 eloc = g_get_location(g, atk_id);
    int dx = tgt_x - eloc.x;
    int dy = tgt_y - eloc.y;
    g_update_direction(g, atk_id, get_dir_from_xy(dx, dy));
    g_set_attacking(g, atk_id, true);
    g_set_update(g, atk_id, true);
    handle_attack_helper(g, tile, atk_id, &ok);
    handle_attack_success_gamestate_flag(g, g_get_type(g, atk_id), ok);
}
*/

/*
static bool
entities_adjacent(shared_ptr<gamestate> g, entityid id0, entityid id1) {
    massert(g, "gamestate is NULL");
    massert(id0 != ENTITYID_INVALID, "id0 is invalid");
    massert(id1 != ENTITYID_INVALID, "id1 is invalid");
    massert(id0 != id1, "id0 and id1 are the same");
    // use e0 and check the surrounding 8 tiles
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0) continue;
            vec3 loc0 = g_get_location(g, id0);
            vec3 loc1 = g_get_location(g, id1);
            if (loc0.x + x == loc1.x && loc0.y + y == loc1.y) return true;
        }
    }
    return false;
}
*/

/*
static void try_entity_wait(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    if (g_is_type(g, id, ENTITY_PLAYER)) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    g_set_update(g, id, true);
}
*/

/*
static void
execute_action(shared_ptr<gamestate> g, entityid id, entity_action_t action) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
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
    case ENTITY_ACTION_NONE:
        // do nothing
        break;
    default: merror("Unknown entity action: %d", action); break;
    }
}
*/

//static vec3* get_locs_around_entity(gamestate* const g, entityid id) {
/*
static vec3* get_locs_around_entity(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    //vec3* locs = malloc(sizeof(vec3) * 8);
    vec3* locs = (vec3*)malloc(sizeof(vec3) * 8);
    massert(locs, "failed to allocate memory for locs");
    int index = 0;
    vec3 oldloc = g_get_location(g, id);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            locs[index] = (vec3){oldloc.x + i, oldloc.y + j, oldloc.z};
            index++;
        }
    }
    return locs;
}
*/

//static inline tile_t* get_first_empty_tile_around_entity(gamestate* const g, entityid id) {
//static inline shared_ptr<tile_t> get_first_empty_tile_around_entity(shared_ptr<gamestate> g, entityid id) {
//    massert(g, "gamestate is NULL");
//    vec3* locs = get_locs_around_entity(g, id);
//    massert(locs, "locs is NULL");
//    bool found = false;
//    //tile_t* tile = NULL;
//    shared_ptr<tile_t> tile = nullptr;
//    for (int i = 0; i < 8; i++) {
//        vec3 loc = g_get_location(g, id);
//        //tile = df_tile_at(g->d->floors[loc.z], locs[i].x, locs[i].y);
//        tile = df_tile_at(g->d->floors[loc.z], locs[i]);
//        if (!tile) continue;
//        if (!tile_is_walkable(tile->type)) continue;
//        if (tile_entity_count(tile) > 0) continue;
//        // found an empty tile
//        found = true;
//        break;
//    }
//    free(locs);
//    if (!found) {
//        merror("no empty tile found");
//        return NULL;
//    }
//    return tile;
//}

//static vec3 get_random_empty_non_wall_loc_in_area(gamestate* const g, int z, int x, int y, int w, int h) {
//    massert(g, "gamestate is NULL");
//    massert(z >= 0, "floor is out of bounds");
//    massert(z < g->d->num_floors, "floor is out of bounds");
//    massert(x >= 0, "x is out of bounds");
//    massert(x < g->d->floors[z]->width, "x is out of bounds");
//    massert(y >= 0, "y is out of bounds");
//    massert(y < g->d->floors[z]->height, "y is out of bounds");
//    massert(w > 0, "w is out of bounds");
//    massert(h > 0, "h is out of bounds");
//    massert(x + w <= g->d->floors[z]->width, "x + w is out of bounds");
//    massert(y + h <= g->d->floors[z]->height, "y + h is out of bounds");
//    int c = -1;
//    vec3* locations = get_empty_non_wall_locs_in_area(g->d->floors[z], &c, x, y, w, h);
//    massert(locations, "locations is NULL");
//    massert(c > 0, "locations count is 0 or less");
//    //    // pick a random location
//    int index = rand() % c;
//    vec3 loc = locations[index];
//    free(locations);
//    massert(loc.x >= 0, "loc.x is out of bounds");
//    massert(loc.x < g->d->floors[z]->width, "loc.x is out of bounds");
//    massert(loc.y >= 0, "loc.y is out of bounds");
//    massert(loc.y < g->d->floors[z]->height, "loc.y is out of bounds");
//    loc.z = z;
//    return loc;
//}

//static vec3 get_random_empty_non_wall_loc(gamestate* const g, int floor) {
//    massert(g, "gamestate is NULL");
//    massert(floor >= 0, "floor is out of bounds");
//    massert(floor < g->d->num_floors, "floor is out of bounds");
//    return get_random_empty_non_wall_loc_in_area(
//        g, floor, 0, 0, g->d->floors[floor]->width, g->d->floors[floor]->height);
//}

//static vec3 get_random_available_loc(gamestate* const g, int floor) {
/*
static vec3 get_random_available_loc(shared_ptr<gamestate> g, int floor) {
    massert(g, "gamestate is NULL");
    massert(floor >= 0, "floor is out of bounds");
    massert(floor < g->d->num_floors, "floor is out of bounds");

    int w = g->dungeon->floors->at(floor)->width;
    int h = g->dungeon->floors->at(floor)->height;

    return get_random_available_loc_in_area(g, floor, 0, 0, w, h);
}
*/

static void init_dungeon(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    g->dungeon = d_create();

    massert(g->dungeon, "failed to init dungeon");
    msuccess("Dungeon initialized successfully");

    minfo("adding floors...");
    int df_count = 1;
    for (int i = 0; i < df_count; i++) {
        d_add_floor(g->dungeon, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    }
    msuccess("Added %d floors to dungeon", df_count);
}

/*
static entityid
npc_create(shared_ptr<gamestate> g, race_t rt, vec3 loc, string name) {
    massert(g, "gamestate is NULL");
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, loc.z);
    shared_ptr<tile_t> tile = df_tile_at(df, loc);
    massert(tile, "failed to get tile");
    printf("checking if tile is walkable and empty...\n");
    if (!tile_is_walkable(tile->type) || tile_has_live_npcs(g, tile)) {
        return ENTITYID_INVALID;
    }
    //if (tile_has_live_npcs(g, tile)) {
    //merror("cannot create entity on tile with NPC");
    //    return ENTITYID_INVALID;
    //}

    entityid id = g_add_entity(g);

    g_add_name(g, id, name);

    //printf("adding entity type...\n");
    g_add_type(g, id, ENTITY_NPC);
    //printf("adding entity race...\n");
    g_add_race(g, id, rt);
    //printf("adding entity dir...\n");
    g_add_direction(g, id, DIR_RIGHT);

    //vec3 loc_vec = {loc.x, loc.y, loc.z};
    //printf("adding entity loc...\n");
    //g_add_location(g, id, loc_vec);

    //printf("adding remaining entity components...\n");
    //g_add_sprite_move(g, id, (Rectangle){0, 0, 0, 0}); // default
    //g_add_dead(g, id, 0);
    //g_add_update(g, id, false);
    //g_add_attacking(g, id, false);
    //g_add_zapping(g, id, false);
    //g_add_block_success(g, id, false);
    //g_add_damaged(g, id, false);
    //g_add_default_action(g, id, ENTITY_ACTION_WAIT);
    //g_add_inventory(g, id);
    //g_add_target(g, id, (vec3){-1, -1, -1});
    //g_add_target_path(g, id);
    //g_add_equipment(g, id);
    //g_add_base_attack_damage(g, id, (vec3){1, 4, 0});
    //g_add_vision_distance(g, id, 0);
    //g_add_light_radius(g, id, 0);

    //g_add_stats(g, id);
    //g_set_stat(g, id, STATS_LEVEL, 1);
    //g_set_stat(g, id, STATS_XP, 0);
    //g_set_stat(g, id, STATS_NEXT_LEVEL_XP, calc_next_lvl_xp(g, id));
    //g_set_stat(g, id, STATS_MAXHP, 1);
    //g_set_stat(g, id, STATS_HP, 1);
    //g_set_stat(g, id, STATS_HITDIE, 1);
    //g_set_stat(g, id, STATS_STR, 10);
    //g_set_stat(g, id, STATS_DEX, 10);
    //g_set_stat(g, id, STATS_CON, 10);
    //g_set_stat(g, id, STATS_ATTACK_BONUS, 0);
    //g_set_stat(g, id, STATS_AC, 10);

    //printf("adding entity to dungeon floor...\n");
    if (!df_add_at(df, id, loc.x, loc.y)) {
        return ENTITYID_INVALID;
    }

    return id;
}
*/

/*
static entityid
item_create(gamestate* const g, itemtype type, vec3 loc, const char* name) {
    massert(g, "gamestate is NULL");
    massert(name && name[0], "name is NULL or empty");
    massert(type >= 0, "item_type is out of bounds: %s: %d", name, type);
    massert(type < ITEM_TYPE_COUNT,
            "item_type is out of bounds: %s: %d",
            name,
            type);
    massert(loc.z >= 0, "z is out of bounds: %s: %d", name, loc.z);
    massert(
        loc.z < g->d->num_floors, "z is out of bounds: %s: %d", name, loc.z);
    dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    massert(df, "failed to get current dungeon floor");
    massert(loc.x >= 0, "x is out of bounds: %s: %d", name, loc.x);
    massert(loc.x < df->width, "x is out of bounds: %s: %d", name, loc.x);
    massert(loc.y >= 0, "y is out of bounds: %s: %d", name, loc.y);
    massert(loc.y < df->height, "y is out of bounds: %s: %d", name, loc.y);
    //tile_t* const tile = df_tile_at(df, loc);
    shared_ptr<tile_t> tile = df_tile_at(df, loc);
    massert(tile, "failed to get tile");
    //if (!tile_is_walkable(tile->type) || tile_has_live_npcs(g, tile)) {
    if (!tile_is_walkable(tile->type)) {
        merror("cannot create entity on wall");
        return ENTITYID_INVALID;
    }
    //if (tile_has_live_npcs(g, tile)) {
    //    merror("cannot create entity on tile with NPC");
    //    return ENTITYID_INVALID;
    //}
    entityid id = g_add_entity(g);
    g_add_name(g, id, name);
    g_add_type(g, id, ENTITY_ITEM);
    g_add_direction(g, id, DIR_RIGHT);
    vec3 loc_vec = {loc.x, loc.y, loc.z};
    g_add_location(g, id, loc_vec);
    g_add_sprite_move(g, id, (Rectangle){0, 0, 0, 0});
    g_add_update(g, id, false);
    g_add_itemtype(g, id, type);
    if (!df_add_at(df, id, loc.x, loc.y)) return ENTITYID_INVALID;
    return id;
}
*/

//static entityid weapon_create(gamestate* const g, weapontype type, vec3 loc, const char* name) {
//    massert(g, "gamestate is NULL");
//    entityid id = item_create(g, ITEM_WEAPON, loc, name);
//    if (id == ENTITYID_INVALID) return ENTITYID_INVALID;
//    g_add_weapontype(g, id, type);
//    g_add_damage(g, id, (vec3){0, 0, 0});
//    return id;
//}

//static entityid ring_create(gamestate* const g, ringtype type, vec3 loc, const char* name) {
//    massert(g, "gamestate is NULL");
//    entityid id = item_create(g, ITEM_RING, loc, name);
//    if (id == ENTITYID_INVALID) return ENTITYID_INVALID;
//    g_add_ringtype(g, id, type);
//    // rings can have various effects, so we will not set any default stats here
//    return id;
//}

/*
static entityid
potion_create(gamestate* const g, vec3 loc, potiontype type, const char* name) {
    minfo("potion create...");
    massert(g, "gamestate is NULL");
    entityid id = item_create(g, ITEM_POTION, loc, name);
    if (id == ENTITYID_INVALID) return ENTITYID_INVALID;
    g_add_potiontype(g, id, type);
    msuccess("potion created!");
    return id;
}
*/

//static entityid shield_create(gamestate* const g, shieldtype type, vec3 loc, const char* name) {
//    massert(g, "gamestate is NULL");
//    int random_ac = do_roll((vec3){1, 4, 0});
//    char name_buffer[128];
//    if (name && name[0]) {
//        snprintf(name_buffer, sizeof(name_buffer), "%s + %d", name, random_ac);
//    }
//    entityid id = item_create(g, ITEM_SHIELD, loc, name_buffer);
//    if (id == ENTITYID_INVALID) {
//        merror("failed to create shield");
//        return ENTITYID_INVALID;
//    }
//    g_add_shieldtype(g, id, type);
//    g_add_ac(g, id, random_ac);
//    return id;
//}

// this should only take into account any equipment that has light radius bonus equipment
//static int get_entity_total_light_radius_bonus(gamestate* const g, entityid id) {
//    int total_light_radius_bonus = 0;
//    // get the light radius bonus from the equipment
//    massert(g, "gamestate is NULL");
//    massert(id != ENTITYID_INVALID, "entity id is invalid");
//    // check each equipment slot
//    for (int i = 0; i < EQUIPMENT_SLOT_COUNT; i++) {
//        entityid equip_id = g_get_equipment(g, id, i);
//        if (equip_id == ENTITYID_INVALID) continue;
//        if (!g_has_light_radius_bonus(g, equip_id)) continue;
//        int light_radius_bonus = g_get_light_radius_bonus(g, equip_id);
//        total_light_radius_bonus += light_radius_bonus;
//    }
//    // only return the total light radius bonus
//    // it is fine if it is negative that might be fun for cursed items
//    return total_light_radius_bonus;
//}

//static void update_player_tiles_explored(gamestate* const g) {
/*
static void update_player_tiles_explored(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    entityid hero_id = g->hero_id;
    massert(hero_id != ENTITYID_INVALID, "hero id is invalid");
    dungeon_floor_t* df = d_get_floor(g->d, g->d->current_floor);
    massert(df, "failed to get current dungeon floor");
    vec3 loc = g_get_location(g, hero_id);
    // Get the player's light radius
    int light_radius = g_get_light_radius(g, hero_id);
    int light_radius_bonus = g_get_entity_total_light_radius_bonus(g, hero_id);
    light_radius += light_radius_bonus;
    minfo("hero light radius: %d", light_radius);
    massert(light_radius > 0, "light radius is negative");
    // Reveal tiles in a diamond pattern
    for (int i = -light_radius; i <= light_radius; i++) {
        for (int j = -light_radius; j <= light_radius; j++) {
            // Calculate Manhattan distance for diamond shape
            int dist = abs(i) + abs(j);
            // Only reveal tiles within the light radius
            if (dist <= light_radius) {
                vec3 loc2 = {loc.x + i, loc.y + j, loc.z};
                // Skip if out of bounds
                if (loc2.x < 0 || loc2.x >= df->width || loc2.y < 0 ||
                    loc2.y >= df->height)
                    continue;
                //tile_t* tile = df_tile_at(df, loc2);
                shared_ptr<tile_t> tile = df_tile_at(df, loc2);
                massert(tile, "failed to get tile at hero location");
                tile->explored = true;
                tile->visible = true;
            }
        }
    }
}
*/

/*
static entityid
player_create(gamestate* const g, race_t rt, int x, int y, int z, string name) {
    printf("begin player_create...\n");
    massert(g, "gamestate is NULL");
    massert(name != "", "name is empty string");
    // use the previously-written liblogic_npc_create function
    //const entitytype_t type = ENTITY_PLAYER;
    //const entityid id = npc_create(g, rt, x, y, fl, name);
    printf("calling npc_create...\n");
    const entityid id = npc_create(g, rt, (vec3){x, y, z}, name);
    massert(id != ENTITYID_INVALID, "failed to create player");
    printf("calling gamestate_set_hero_id...\n");
    gamestate_set_hero_id(g, id);
    printf("calling g_set_type...\n");
    g_set_type(g, id, ENTITY_PLAYER);
    //int str_roll = do_roll_best_of_3((roll){3, 6, 0});
    //int con_roll = do_roll_best_of_3((roll){3, 6, 0});
    printf("rolling stats...\n");
    int str_roll = do_roll_best_of_3((vec3){3, 6, 0});
    int con_roll = do_roll_best_of_3((vec3){3, 6, 0});
    int dex_roll = do_roll_best_of_3((vec3){3, 6, 0});
    printf("calling set stats...\n");
    g_set_stat(g, id, STATS_STR, str_roll);
    g_set_stat(g, id, STATS_CON, con_roll);
    g_set_stat(g, id, STATS_DEX, dex_roll);
    int hitdie = 8;
    g_set_stat(g, id, STATS_HITDIE, hitdie);
    int maxhp_roll =
        do_roll_best_of_3((vec3){1, hitdie, 0}) + bonus_calc(con_roll);
    while (maxhp_roll < 1) {
        maxhp_roll =
            do_roll_best_of_3((vec3){1, hitdie, 0}) + bonus_calc(con_roll);
    }
    g_set_stat(g, id, STATS_MAXHP, maxhp_roll);
    g_set_stat(g, id, STATS_HP, maxhp_roll);

    int default_vision_distance = 2;
    int default_light_radius = 2;
    printf("calling set light radius...\n");
    g_set_light_radius(g, id, default_light_radius);
    printf("calling set vision radius...\n");
    g_set_vision_distance(g, id, default_vision_distance);
    printf("calling update player tiles explored...\n");
    update_player_tiles_explored(g);
    return id;
}
*/

/*
static void init_player(gamestate* const g) {
    massert(g, "gamestate is NULL");
    minfo("begin init_player...");
    // setting it up so we can return a vec3 from a function
    // that can scan for an appropriate starting location
    minfo("calling df_get_upstairs...");
    //vec3 loc = df_get_upstairs(g->d->floors[g->d->current_floor]);
    minfo("calling player_create...");
    //g->entity_turn = player_create(g, RACE_HUMAN, loc.x, loc.y, 0, "darkmage");
    massert(g->entity_turn != ENTITYID_INVALID, "failed to init hero");
    massert(g->hero_id == g->entity_turn, "hero id mismatch");
    msuccess("hero id %d", g->hero_id);
    //printf("end init_player...\n");
}
*/

//static vec3* get_empty_non_wall_locs_in_area(dungeon_floor_t* const df, int* count, int x0, int y0, int w, int h) {
//    massert(df, "dungeon floor is NULL");
//    massert(count, "count is NULL");
//    int c = df_count_empty_non_walls_in_area(df, x0, y0, w, h);
//    vec3* locs = (vec3*)malloc(sizeof(vec3) * c);
//    massert(locs, "malloc failed");
//    int i = 0;
//    for (int y = 0; y < h && y + y0 < df->height; y++) {
//        for (int x = 0; x < w && x + x0 < df->width; x++) {
//            int newx = x + x0;
//            int newy = y + y0;
//            tile_t* t = df_tile_at(df, (vec3){newx, newy, -1});
//            tiletype_t type = t->type;
//            if (tile_entity_count(t) == 0 && tile_is_walkable(type)) locs[i++] = (vec3){newx, newy};
//            if (i >= c) break;
//        }
//    }
//    massert(i == c, "count mismatch: expected %d, got %d", c, i);
//    *count = c;
//    return locs;
//}

//static vec3* get_available_locs_in_area(gamestate* const g, dungeon_floor_t* const df, int* count, int x0, int y0, int w, int h) {
/*
static vec3* get_available_locs_in_area(shared_ptr<gamestate> g,
                                        dungeon_floor_t* const df,
                                        int* count,
                                        int x0,
                                        int y0,
                                        int w,
                                        int h) {
    massert(df, "dungeon floor is NULL");
    massert(count, "count is NULL");
    int c = df_count_non_walls_in_area(df, x0, y0, w, h);
    vec3* locs = (vec3*)malloc(sizeof(vec3) * c);
    massert(locs, "malloc failed");
    int i = 0;
    // given the list of non-walls...
    // c: count of non-wall tiles in area
    // i: index and count of locs that dont have living NPCs found
    // i should never exceed c, but might be less than or equal to c
    for (int y = 0; y < h && y + y0 < df->height; y++) {
        for (int x = 0; x < w && x + x0 < df->width; x++) {
            int newx = x + x0;
            int newy = y + y0;
            //tile_t* t = df_tile_at(df, (vec3){newx, newy, df->floor});
            shared_ptr<tile_t> t =
                df_tile_at(df, (vec3){newx, newy, df->floor});
            tiletype_t type = t->type;
            if (tile_live_npc_count(g, t) == 0 && !tile_has_player(g, t) &&
                tile_is_walkable(type)) {
                locs[i++] = (vec3){newx, newy, df->floor};
            }
            if (i >= c) {
                break;
            }
        }
    }
    // its possible there are no available locations
    massert(i <= c, "count mismatch: expected %d, got %d", c, i);
    *count = i;
    return locs;
}
*/

/*
static vec3 get_random_available_loc_in_area(gamestate* const g,
                                             int floor,
                                             int x0,
                                             int y0,
                                             int w,
                                             int h) {
    massert(g, "gamestate is NULL");
    massert(floor >= 0, "floor is out of bounds");
    massert(floor < g->d->num_floors, "floor is out of bounds");
    dungeon_floor_t* df = d_get_floor(g->d, floor);
    massert(df, "failed to get current dungeon floor");
    massert(x0 >= 0 && x0 < df->width, "x0 is out of bounds");
    massert(y0 >= 0 && y0 < df->height, "y0 is out of bounds");
    massert(w > 0 && w <= df->width - x0, "w is out of bounds");
    massert(h > 0 && h <= df->height - y0, "h is out of bounds");
    int c = 0;
    vec3* locs = get_available_locs_in_area(g, df, &c, x0, y0, w, h);
    massert(locs, "locs is NULL");
    massert(c >= 0, "locs count is less than 0");
    if (c == 0) {
        merror("no available locations found in area (%d, %d, %d, %d)",
               x0,
               y0,
               w,
               h);
        free(locs);
        return (vec3){-1, -1, -1}; // return an invalid location
    }
    // pick a random location
    int index = rand() % c;
    vec3 loc = locs[index];
    free(locs);
    return loc;
}
*/

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
//            vec3 loc = get_random_empty_non_wall_loc_in_area(g, g->dungeon->current_floor, hallway.x, hallway.y, hallway.w, hallway.h);
//            entityid doorid = door_create(g, loc.x, loc.y, loc.z, "door");
//            massert(doorid != ENTITYID_INVALID, "door create fail");
//        }
//    }
//    // one NPC per room past the first
//    const int max_npcs = 3;
//    for (int i = 0; i < max_npcs; i++) {
//        room_data_t room = rooms[2];
//        vec3 loc = get_random_empty_non_wall_loc_in_area(g, g->dungeon->current_floor, room.x, room.y, room.w, room.h);
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
//        //e->target = (vec3){player->x, player->y, player->floor};
//        e->target = g_get_location(g, player->id);
//    }
//}

/*
static const char* get_action_key(const inputstate* const is,
                                  gamestate* const g) {
    int key = inputstate_get_pressed_key(is);
    // can return early if key == -1
    if (key == -1) {
        return "none";
    }
    return get_action_for_key(&g->keybinding_list, key);
}
*/

/*
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
*/

/*
static inline void handle_camera_zoom(gamestate* const g,
                                      const inputstate* const is) {
    massert(g, "Game state is NULL!");
    massert(is, "Input state is NULL!");
    if (inputstate_is_held(is, KEY_Z)) {
        if (inputstate_is_shift_held(is))
            g->cam2d.zoom -= (g->cam2d.zoom > 1.0) ? DEFAULT_ZOOM_INCR : 0.0;
        else
            g->cam2d.zoom += DEFAULT_ZOOM_INCR;
    }
}
*/

/*
static void handle_input_camera(const inputstate* const is,
                                gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    handle_camera_move(g, is);
    handle_camera_zoom(g, is);
}
*/

/*
static void handle_input_gameplay_settings(const inputstate* const is,
                                           gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");

    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->controlmode = CONTROLMODE_PLAYER;
        g->display_gameplay_settings_menu = false;
        g->display_inventory_menu = false;
        return;
    }

    // Cycle between settings and inventory menus
    if (inputstate_is_pressed(is, KEY_LEFT) ||
        inputstate_is_pressed(is, KEY_RIGHT)) {
        g->controlmode = CONTROLMODE_INVENTORY;
        g->display_gameplay_settings_menu = false;
        g->display_inventory_menu = true;
        return;
    }

    // Handle menu navigation
    int max_selections = 2; // 0: Music Volume, 1: Back
    if (inputstate_is_pressed(is, KEY_UP)) {
        //g->gameplay_settings_menu_selection = (g->gameplay_settings_menu_selection - 1 + 3) % 3;
        g->gameplay_settings_menu_selection =
            (g->gameplay_settings_menu_selection - 1 + max_selections) %
            max_selections;
        g->frame_dirty = true;
    } else if (inputstate_is_pressed(is, KEY_DOWN)) {
        g->gameplay_settings_menu_selection =
            (g->gameplay_settings_menu_selection + 1) % max_selections;
        g->frame_dirty = true;
    }

    // Handle menu item selection
    if (g->gameplay_settings_menu_selection == 0) {
        // Music Volume
        if (inputstate_is_pressed(is, KEY_LEFT_BRACKET)) {
            g->music_volume =
                g->music_volume > 0 ? g->music_volume - 0.1f : 0.0f;
            g->music_volume_changed = true;
        } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
            g->music_volume =
                g->music_volume < 1 ? g->music_volume + 0.1f : 1.0f;
            g->music_volume_changed = true;
        }

    } else if (g->gameplay_settings_menu_selection == 0) {
        // Back
        if (inputstate_is_pressed(is, KEY_ENTER) ||
            inputstate_is_pressed(is, KEY_SPACE)) {
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_gameplay_settings_menu = false;
        }
    }
}
*/

/*
static void handle_sort_inventory(gamestate* const g) {
    g->display_sort_inventory_menu = false;
    // we need to sort the inventory based on the selected type
    inventory_sort sort_type = (inventory_sort)g->sort_inventory_menu_selection;
    size_t count = 0;
    entityid* inventory = g_get_inventory(g, g->hero_id, &count);
    massert(inventory, "inventory is NULL");
    massert(count >= 0, "inventory count is less than 0");
    // sort the inventory
    entityid* sorted_inventory =
        g_sort_inventory(g, inventory, count, sort_type);
    massert(sorted_inventory, "sorted inventory is NULL");

    // now we have our original inventory and our sorted inventory...
    g_update_inventory(g, g->hero_id, sorted_inventory, count);
}
*/

/*
static void handle_input_sort_inventory(const inputstate* const is,
                                        gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_is_pressed(is, KEY_UP)) {
        minfo("sort inv menu--");
        g->sort_inventory_menu_selection--;
        if (g->sort_inventory_menu_selection < 0) {
            g->sort_inventory_menu_selection =
                g->sort_inventory_menu_selection_max - 1;
        }
    } else if (inputstate_is_pressed(is, KEY_DOWN)) {
        minfo("sort inv menu++");
        g->sort_inventory_menu_selection++;
        if (g->sort_inventory_menu_selection >=
            g->sort_inventory_menu_selection_max) {
            g->sort_inventory_menu_selection = 0;
        }
    } else if (inputstate_is_pressed(is, KEY_S) ||
               inputstate_is_pressed(is, KEY_ENTER) ||
               inputstate_is_pressed(is, KEY_APOSTROPHE)) {
        handle_sort_inventory(g);
    }
}
*/

/*
static void handle_input_inventory(const inputstate* const is,
                                   gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->controlmode = CONTROLMODE_PLAYER;
        g->display_gameplay_settings_menu = false;
        g->display_inventory_menu = false;
        g->display_sort_inventory_menu = false;
        return;
    }

    // cycle menus
    if (inputstate_is_pressed(is, KEY_LEFT) ||
        inputstate_is_pressed(is, KEY_RIGHT)) {
        g->controlmode = CONTROLMODE_GAMEPLAY_SETTINGS;
        g->display_gameplay_settings_menu = true;
        g->display_inventory_menu = false;
        return;
    }

    if (inputstate_is_pressed(is, KEY_S)) {
        minfo("toggling sort inventory menu");
        g->display_sort_inventory_menu = !g->display_sort_inventory_menu;
        return;
    }

    size_t count = 0;
    entityid* inventory = g_get_inventory(g, g->hero_id, &count);

    if (g->display_inventory_menu && g->display_sort_inventory_menu) {
        handle_input_sort_inventory(is, g);
        return;
    }

    if (count == 0) return;

    // make sure we separate control of inv menu from its sort inv menu

    if (g->display_inventory_menu && !g->display_sort_inventory_menu) {
        if (inputstate_is_pressed(is, KEY_DOWN) ||
            inputstate_is_pressed(is, KEY_X)) {
            g->inventory_menu_selection =
                (size_t)g->inventory_menu_selection + 1 >= count
                    ? 0
                    : g->inventory_menu_selection + 1;
        } else if (inputstate_is_pressed(is, KEY_UP) ||
                   inputstate_is_pressed(is, KEY_W)) {
            g->inventory_menu_selection = g->inventory_menu_selection - 1 < 0
                                              ? count - 1
                                              : g->inventory_menu_selection - 1;
            // drop item
        } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
            // we need to grab the entityid of the selected item
            entityid item_id = inventory[g->inventory_menu_selection];
            g_remove_from_inventory(g, g->hero_id, item_id);
            //handle_sort_inventory(g);
            // add the item to the tile where the player is located at
            vec3 loc = g_get_location(g, g->hero_id);
            dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
            massert(df, "Dungeon floor is NULL!");
            vec3 loc_cast = {loc.x, loc.y, loc.z};
            //tile_t* const tile = df_tile_at(df, loc_cast);
            shared_ptr<tile_t> tile = df_tile_at(df, loc_cast);
            massert(tile, "Tile is NULL!");
            if (!tile_add(tile, item_id)) return;
            // we also have to update the location of the item
            g_update_location(g, item_id, loc);
            g->controlmode = CONTROLMODE_PLAYER;
            g->display_inventory_menu = false;
        } else if (inputstate_is_pressed(is, KEY_ENTER) ||
                   inputstate_is_pressed(is, KEY_APOSTROPHE)) {
            entityid item_id = inventory[g->inventory_menu_selection];
            // we will eventually adjust this to check which slot it needs to go into based on its various types
            entitytype_t type = g_get_type(g, item_id);
            if (type == ENTITY_ITEM) {
                itemtype item_type = g_get_itemtype(g, item_id);
                if (item_type == ITEM_POTION) {
                    potiontype potion_type = g_get_potiontype(g, item_id);
                    if (potion_type == POTION_HEALTH_SMALL) {
                        int hp = g_get_stat(g, g->hero_id, STATS_HP);
                        int maxhp = g_get_stat(g, g->hero_id, STATS_MAXHP);
                        if (hp < maxhp) {
                            int small_hp_health_roll = rand() % 4 + 1;
                            hp += small_hp_health_roll;
                            if (hp > maxhp) hp = maxhp;
                            g_set_stat(g, g->hero_id, STATS_HP, hp);
                            add_message_history(
                                g,
                                "%s drank a %s and recovered %d HP",
                                g_get_name(g, g->hero_id).c_str(),
                                g_get_name(g, item_id).c_str(),
                                small_hp_health_roll);
                            // remove the potion from the inventory
                            g_remove_from_inventory(g, g->hero_id, item_id);
                            g->controlmode = CONTROLMODE_PLAYER;
                            g->display_inventory_menu = false;
                            g->controlmode = CONTROLMODE_PLAYER;
                            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                        }
                    }
                } else if (item_type == ITEM_WEAPON) {
                    // check if the item is already equipped
                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON);
                    //if (equipped_item != ENTITYID_INVALID) {
                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON, item_id);
                    add_message_history(g,
                                        "%s equipped %s",
                                        g_get_name(g, g->hero_id).c_str(),
                                        g_get_name(g, item_id).c_str());
                    //}
                    g->controlmode = CONTROLMODE_PLAYER;
                    g->display_inventory_menu = false;
                    g->controlmode = CONTROLMODE_PLAYER;
                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                } else if (item_type == ITEM_SHIELD) {
                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD);
                    //if (equipped_item != ENTITYID_INVALID) {
                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD, item_id);
                    add_message_history(g,
                                        "%s equipped %s",
                                        g_get_name(g, g->hero_id).c_str(),
                                        g_get_name(g, item_id).c_str());
                    //}
                    g->controlmode = CONTROLMODE_PLAYER;
                    g->display_inventory_menu = false;
                    g->controlmode = CONTROLMODE_PLAYER;
                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                } else if (item_type == ITEM_WAND) {
                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
                    //if (equipped_item != ENTITYID_INVALID) {
                    // unequip the currently equipped item
                    //    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
                    //    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
                    //} else {
                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WAND, item_id);
                    add_message_history(g,
                                        "%s equipped %s",
                                        g_get_name(g, g->hero_id).c_str(),
                                        g_get_name(g, item_id).c_str());
                    //}
                    g->controlmode = CONTROLMODE_PLAYER;
                    g->display_inventory_menu = false;
                    g->controlmode = CONTROLMODE_PLAYER;
                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                } else if (item_type == ITEM_RING) {
                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_RING, item_id);
                    add_message_history(g,
                                        "%s equipped %s",
                                        g_get_name(g, g->hero_id).c_str(),
                                        g_get_name(g, item_id).c_str());

                    // update player tiles
                    update_player_tiles_explored(g);

                    g->controlmode = CONTROLMODE_PLAYER;
                    g->display_inventory_menu = false;
                    g->controlmode = CONTROLMODE_PLAYER;
                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                }
            }
        }
    }
}
*/

/*
static inline void change_player_dir(gamestate* const g, direction_t dir) {
    massert(g, "Game state is NULL!");
    if (g_is_dead(g, g->hero_id)) return;
    g_update_direction(g, g->hero_id, dir);
    g_set_update(g, g->hero_id, true);
    //update_equipped_shield_dir(g, g->hero_id);
}
*/

/*
static bool try_entity_pickup(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    g_set_update(g, id, true);
    // check if the player is on a tile with an item
    vec3 loc = g_get_location(g, id);
    vec3 loc_cast = {loc.x, loc.y, loc.z};
    dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    if (!df) {
        merror("Failed to get dungeon floor");
        return false;
    }
    //tile_t* const tile = df_tile_at(df, loc_cast);
    shared_ptr<tile_t> tile = df_tile_at(df, loc_cast);
    if (!tile) {
        merror("Failed to get tile");
        return false;
    }
    if (tile->entities->size() == 0) {
        merror("No items on tile");
        return false;
    }
    for (int i = 0; (size_t)i < tile->entities->size(); i++) {
        entityid itemid = tile->entities->at(i);
        entitytype_t type = g_get_type(g, itemid);
        //minfo("Item %s type: %d", g_get_name(g, itemid), type);
        if (type == ENTITY_ITEM) {
            add_message_history(g,
                                "%s picked up a %s",
                                g_get_name(g, id).c_str(),
                                g_get_name(g, itemid).c_str());
            bool result = g_add_to_inventory(g, id, itemid);
            if (!result) {
                merror("Failed to add item to inventory");
                return false;
            } else {
                //minfo("Item %s added to inventory", g_get_name(g, itemid));
                tile_remove(tile, itemid);
            }
            if (g_is_type(g, id, ENTITY_PLAYER)) {
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
            return true;
        }
    }
    add_message(g, "No items to pick up");
    return false;
}
*/

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

/*
static void handle_input_player(const inputstate* const is,
                                gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) {
        printf("handle_input_player: flag is not GAMESTATE_FLAG_PLAYER_INPUT, "
               "returning\n");
        return;
    }
    if (g->msg_system.is_active) {
        if (inputstate_any_pressed(is)) {
            g->msg_system.index++;
            if (g->msg_system.index >= g->msg_system.count) {
                // Reset when all messages read
                g->msg_system.index = 0;
                g->msg_system.count = 0;
                g->msg_system.is_active = false;
            }
            g->frame_dirty = true;
        }
        return;
    }
    if (inputstate_is_shift_held(is) && inputstate_is_pressed(is, KEY_SLASH)) {
        g->display_help_menu = true;
        g->controlmode = CONTROLMODE_HELP;
        g->frame_dirty = true;
        return;
    }

    // test enter key
    if (inputstate_is_pressed(is, KEY_ENTER)) {
        printf("handle_input_player: enter key pressed\n");
        return;
    }

    const char* action = get_action_key(is, g);
    if (!action) {
        return;
    }
    if (g_is_dead(g, g->hero_id)) {
        return;
    }
    if (action) {
        //printf("handle_input_player: action is %s\n", action);
        if (g->player_changing_direction) {
            if (strcmp(action, "wait") == 0) {
                execute_action(g, g->hero_id, ENTITY_ACTION_WAIT);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_w") == 0) {
                change_player_dir(g, DIR_LEFT);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_e") == 0) {
                change_player_dir(g, DIR_RIGHT);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_s") == 0) {
                change_player_dir(g, DIR_DOWN);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_n") == 0) {
                change_player_dir(g, DIR_UP);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_nw") == 0) {
                change_player_dir(g, DIR_UP_LEFT);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_ne") == 0) {
                change_player_dir(g, DIR_UP_RIGHT);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_sw") == 0) {
                change_player_dir(g, DIR_DOWN_LEFT);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            } else if (strcmp(action, "move_se") == 0) {
                change_player_dir(g, DIR_DOWN_RIGHT);
                g->player_changing_direction = false;
                g->frame_dirty = true;
            }
            // suggestion by patreon supporter hllcgn:
            // pressing 'attack' while in 'change direction' mode
            // should cause an attack right away so the player
            // does not have to get out of change-dir mode
            else if (strcmp(action, "attack") == 0) {
                g->player_changing_direction = false;
                vec3 loc = get_loc_from_dir(g_get_direction(g, g->hero_id));
                vec3 hloc = g_get_location(g, g->hero_id);
                try_entity_attack(
                    g, g->hero_id, hloc.x + loc.x, hloc.y + loc.y);
                g->frame_dirty = true;
            }
            return;
        }
        if (strcmp(action, "wait") == 0) {
            g->player_changing_direction = true;
            g->frame_dirty = true;
        } else if (strcmp(action, "inventory_menu") == 0) {
            //g->player_changing_direction = true;
            g->display_inventory_menu = true;
            g->frame_dirty = true;
            g->controlmode = CONTROLMODE_INVENTORY;
        } else if (strcmp(action, "move_w") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_LEFT);
            g->frame_dirty = true;
        } else if (strcmp(action, "move_e") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_RIGHT);
            g->frame_dirty = true;
        } else if (strcmp(action, "move_n") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_UP);
            g->frame_dirty = true;
        } else if (strcmp(action, "move_s") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_DOWN);
            g->frame_dirty = true;
        } else if (strcmp(action, "move_nw") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_UP_LEFT);
            g->frame_dirty = true;
        } else if (strcmp(action, "move_ne") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_UP_RIGHT);
            g->frame_dirty = true;
        } else if (strcmp(action, "move_sw") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_DOWN_LEFT);
            g->frame_dirty = true;
        } else if (strcmp(action, "move_se") == 0) {
            execute_action(g, g->hero_id, ENTITY_ACTION_MOVE_DOWN_RIGHT);
            g->frame_dirty = true;
        } else if (strcmp(action, "attack") == 0) {
            vec3 loc = get_loc_from_dir(g_get_direction(g, g->hero_id));
            vec3 hloc = g_get_location(g, g->hero_id);
            try_entity_attack(g, g->hero_id, hloc.x + loc.x, hloc.y + loc.y);
            g->frame_dirty = true;
        } else if (strcmp(action, "interact") == 0) {
            // we are hardcoding the flip switch interaction for now
            // but eventually this will be generalized
            // for instance u can talk to an NPC merchant using "interact"
            // or open a door, etc
            //msuccess("Space pressed!");
            //int tx = hero->x + get_x_from_dir(hero->direction);
            //direction_t dir = g_get_direction(g, g->hero_id);
            //vec3 hloc = g_get_location(g, g->hero_id);
            //int tx = hloc.x + get_x_from_dir(dir);
            //int ty = hloc.y + get_y_from_dir(dir);
            // check to see if there is a door
            //entity* door = get_door_from_tile(g, tx, ty, hloc.z);
            //if (door) {
            //    door->door_is_open = !door->door_is_open;
            //    //door->do_update = true;
            //    g_set_update(g, door->id, true);
            //}
            //g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            //try_flip_switch(g, hero, tx, ty, hero->floor);
            //g->frame_dirty = true;
        } else if (strcmp(action, "pickup") == 0) {
            //try_entity_pickup(g, hero);
            try_entity_pickup(g, g->hero_id);
            g->frame_dirty = true;
        } else if (strcmp(action, "toggle_camera") == 0) {
            g->controlmode = CONTROLMODE_CAMERA;
        } else if (strcmp(action, "traverse") == 0) {
            // we need to attempt to navigate either up or down a floor depending on the tile beneath the player
            try_entity_traverse_floors(g, g->hero_id);
            g->frame_dirty = true;
        }
    } else {
        merror("No action found for key");
    }
}
*/


/*
static void try_entity_traverse_floors(gamestate* const g, entityid id) {
    vec3 loc = g_get_location(g, id);
    vec3 loc_cast = {loc.x, loc.y, loc.z};
    dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    massert(df, "Dungeon floor is NULL!");
    //tile_t* const tile = df_tile_at(df, loc_cast);
    shared_ptr<tile_t> tile = df_tile_at(df, loc_cast);
    massert(tile, "Tile is NULL!");
    entitytype_t type = g_get_type(g, id);
    if (tile->type == TILE_UPSTAIRS) {
        // we need to check if the player is on the stairs
        // and if so, we need to move them up a floor
        //g_traverse_up(g, g->hero_id);
        // we need to check to see which floor we are currently on
        if (g->d->current_floor > 0) {
            if (id == g->hero_id) {
                add_message(g, "You ascend the stairs");
                if (!df_remove_at(df, id, loc.x, loc.y)) return;
                vec3 next_downstairs_loc =
                    df_get_downstairs(g->d->floors[g->d->current_floor - 1]);
                massert(next_downstairs_loc.x != -1 &&
                            next_downstairs_loc.y != -1,
                        "Failed to get next downstairs location");
                // we need to set the player's location to the corresponding TILE_downstairs
                next_downstairs_loc.z = g->d->current_floor - 1;
                vec3 next_downstairs_loc_vec3 = {next_downstairs_loc.x,
                                                 next_downstairs_loc.y,
                                                 next_downstairs_loc.z};
                g_update_location(g, id, next_downstairs_loc_vec3);
                // we need to set the player's floor to the next floor
                g->d->current_floor--;
                // get the next dungeon floor
                dungeon_floor_t* const next_floor =
                    d_get_floor(g->d, g->d->current_floor);
                int ex = next_downstairs_loc.x;
                int ey = next_downstairs_loc.y;
                if (!df_add_at(next_floor, id, ex, ey)) return;
                if (id == g->hero_id) {
                    update_player_tiles_explored(g);
                }
                if (type == ENTITY_PLAYER) {
                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                }
            }
        } else {
            if (id == g->hero_id) {
                add_message(g, "You are already at the top floor");
            }
        }
    } else if (tile->type == TILE_DOWNSTAIRS) {
        // we need to check if the player is on the stairs
        // and if so, we need to move them down a floor
        //g_traverse_down(g, g->hero_id);
        if (g->d->current_floor < g->d->num_floors - 1) {
            if (id == g->hero_id) {
                add_message(g, "You descend the stairs");
                if (!df_remove_at(df, id, loc.x, loc.y)) {
                    merror("Failed to remove entity from old tile");
                    return;
                }
                vec3 next_upstairs_loc =
                    df_get_upstairs(g->d->floors[g->d->current_floor + 1]);
                massert(next_upstairs_loc.x != -1 && next_upstairs_loc.y != -1,
                        "Failed to get next upstairs location");
                // we need to set the player's location to the corresponding TILE_UPSTAIRS
                next_upstairs_loc.z = g->d->current_floor + 1;
                vec3 next_upstairs_loc_vec3 = {next_upstairs_loc.x,
                                               next_upstairs_loc.y,
                                               next_upstairs_loc.z};
                g_update_location(g, id, next_upstairs_loc_vec3);
                // we need to set the player's floor to the next floor
                g->d->current_floor++;
                // get the next dungeon floor
                dungeon_floor_t* const next_floor =
                    d_get_floor(g->d, g->d->current_floor);
                int ex = next_upstairs_loc.x;
                int ey = next_upstairs_loc.y;
                if (!df_add_at(next_floor, id, ex, ey)) {
                    merror("Failed to add entity to new tile");
                    return;
                }
                if (id == g->hero_id) update_player_tiles_explored(g);
                if (type == ENTITY_PLAYER) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
        } else {
            if (id == g->hero_id) {
                add_message(g, "You are already at the bottom floor");
            }
        }
    }
}
*/

//static void handle_input(const inputstate* const is, gamestate* const g) {
static void handle_input(shared_ptr<inputstate> is, shared_ptr<gamestate> g) {
    massert(is, "inputstate is NULL");
    massert(g, "gamestate is NULL");
    // no matter which mode we are in, we can toggle the debug panel
    if (inputstate_is_pressed(is, KEY_P)) {
        g->debugpanelon = !g->debugpanelon;
        minfo("Toggling debug panel: %s", g->debugpanelon ? "ON" : "OFF");
        return;
    }

    if (g->current_scene == SCENE_TITLE) {
        //if (inputstate_any_pressed(is)) {
        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            minfo("Title screen input detected, switching to main menu");
            g->current_scene = SCENE_MAIN_MENU;
            g->frame_dirty = true;
        }
    } else if (g->current_scene == SCENE_MAIN_MENU) {
        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            if (g->title_screen_selection == 0) {
                minfo("Switching to character creation scene");
                g->current_scene = SCENE_CHARACTER_CREATION;
                g->frame_dirty = true;
            }
        } else if (inputstate_is_pressed(is, KEY_DOWN)) {
            minfo("Title screen selection++");
            g->title_screen_selection++;
            if (g->title_screen_selection >= g->max_title_screen_selections) {
                g->title_screen_selection = 0;
            }
        } else if (inputstate_is_pressed(is, KEY_UP)) {
            minfo("Title screen selection--");
            g->title_screen_selection--;
            if (g->title_screen_selection < 0) {
                g->title_screen_selection = g->max_title_screen_selections - 1;
            }
        } else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            g->do_quit = true;
        }
    } else if (g->current_scene == SCENE_CHARACTER_CREATION) {
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            minfo("Character creation confirmed");
            // we need to copy the character creation stats to the hero entity
            // hero has already been created, so its id is available
            //g_set_stat(g, g->hero_id, STATS_STR, g->chara_creation.strength);
            //g_set_stat(g, g->hero_id, STATS_DEX, g->chara_creation.dexterity);
            //g_set_stat(g, g->hero_id, STATS_CON, g->chara_creation.constitution);
            int hitdie = 8;
            int maxhp_roll = do_roll_best_of_3((vec3){1, hitdie, 0});
            //bonus_calc(g->chara_creation.constitution);
            while (maxhp_roll < 1) {
                maxhp_roll = do_roll_best_of_3((vec3){1, hitdie, 0});
                //bonus_calc(g->chara_creation.constitution);
            }
            //g_set_stat(g, g->hero_id, STATS_MAXHP, maxhp_roll);
            //g_set_stat(g, g->hero_id, STATS_HP, maxhp_roll);
            g->current_scene = SCENE_GAMEPLAY;
            g->frame_dirty = true;
        } else if (inputstate_is_pressed(is, KEY_SPACE)) {
            // re-roll character creation stats
            minfo("Re-rolling character creation stats");
            g->frame_dirty = true;
            g->chara_creation->strength = do_roll_best_of_3((vec3){3, 6, 0});
            g->chara_creation->dexterity = do_roll_best_of_3((vec3){3, 6, 0});
            g->chara_creation->constitution = do_roll_best_of_3((vec3){3, 6, 0});
        }
    }


    //if (g->display_quit_menu) {
    //    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
    //        g->display_quit_menu = false;
    //        return;
    //    }
    //if (inputstate_is_pressed(is, KEY_Q)) {
    // this forces the window to close but this is crashing on exit
    //CloseWindow();
    //    g->do_quit = true;
    //    return;
    //}
    //}
    //if (!g->display_quit_menu) {
    //if (inputstate_is_pressed(is, KEY_Q)) {
    //    g->display_quit_menu = true;
    //    return;
    //}

    /*
    if (g->current_scene == SCENE_GAMEPLAY) {
        if (g->gameover) {
            //if (inputstate_any_pressed(is)) liblogic_restart(g);
            //if (inputstate_any_pressed(is)) {
            if (inputstate_is_pressed(is, KEY_ENTER) ||
                inputstate_is_pressed(is, KEY_SPACE)) {
                liblogic_restart(g);
            }
            return;
        }
        //printf("Current control mode: %d\n", g->controlmode);

        switch (g->controlmode) {
        case CONTROLMODE_PLAYER: handle_input_player(is, g); break;
        case CONTROLMODE_CAMERA: handle_input_camera(is, g); break;
        case CONTROLMODE_INVENTORY: handle_input_inventory(is, g); break;
        case CONTROLMODE_GAMEPLAY_SETTINGS:
            handle_input_gameplay_settings(is, g);
            break;
        case CONTROLMODE_HELP: handle_input_help_menu(is, g); break;
        default: merror("Unknown control mode: %d", g->controlmode); break;
        }

    } else if (g->current_scene == SCENE_TITLE) {
        //if (inputstate_any_pressed(is)) {
        if (inputstate_is_pressed(is, KEY_ENTER) ||
            inputstate_is_pressed(is, KEY_SPACE)) {
            //minfo("Title screen input detected, switching to main menu");
            g->current_scene = SCENE_MAIN_MENU;
            g->frame_dirty = true;
        }
    } else if (g->current_scene == SCENE_MAIN_MENU) {
        if (inputstate_is_pressed(is, KEY_DOWN)) {
            g->frame_dirty = true;
            //if (g->title_screen_selection == g->max_title_screen_selections - 1) {
            //    g->title_screen_selection = 0;
            //} else {
            //    g->title_screen_selection++;
            //}
            //minfo("Title screen selection: %d", g->title_screen_selection);
        } else if (inputstate_is_pressed(is, KEY_UP)) {
            g->frame_dirty = true;
            //if (g->title_screen_selection == 0) {
            //    g->title_screen_selection = g->max_title_screen_selections - 1;
            //} else {
            //    g->title_screen_selection--;
            //}
            //minfo("Title screen selection: %d", g->title_screen_selection);

        } else if (inputstate_is_pressed(is, KEY_ENTER)) {
            if (g->title_screen_selection == 0) {
                g->current_scene = SCENE_CHARACTER_CREATION;
                g->frame_dirty = true;
            }
        }
        //else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        //    g->do_quit = true;
        //}

    } else if (g->current_scene == SCENE_CHARACTER_CREATION) {
        //if (inputstate_any_pressed(is)) {
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            minfo("Character creation");
            // we need to copy the character creation stats to the hero entity
            // hero has already been created, so its id is available
            g_set_stat(g, g->hero_id, STATS_STR, g->chara_creation.strength);
            g_set_stat(g, g->hero_id, STATS_DEX, g->chara_creation.dexterity);
            g_set_stat(
                g, g->hero_id, STATS_CON, g->chara_creation.constitution);
            int hitdie = 8;
            int maxhp_roll = do_roll_best_of_3((vec3){1, hitdie, 0}) +
                             bonus_calc(g->chara_creation.constitution);
            while (maxhp_roll < 1) {
                maxhp_roll = do_roll_best_of_3((vec3){1, hitdie, 0}) +
                             bonus_calc(g->chara_creation.constitution);
            }
            g_set_stat(g, g->hero_id, STATS_MAXHP, maxhp_roll);
            g_set_stat(g, g->hero_id, STATS_HP, maxhp_roll);
            g->current_scene = SCENE_GAMEPLAY;
            g->frame_dirty = true;
        } else if (inputstate_is_pressed(is, KEY_SPACE)) {
            // re-roll character creation stats
            g->frame_dirty = true;
            g->chara_creation.strength = do_roll_best_of_3((vec3){3, 6, 0});
            g->chara_creation.dexterity = do_roll_best_of_3((vec3){3, 6, 0});
            g->chara_creation.constitution = do_roll_best_of_3((vec3){3, 6, 0});
        } else if (inputstate_is_pressed(is, KEY_LEFT)) {
            // change race/class
            g->frame_dirty = true;
        } else if (inputstate_is_pressed(is, KEY_RIGHT)) {
            // change race/class
            g->frame_dirty = true;
        }
    } else {
    }
*/
}

/*
static void handle_input_help_menu(const inputstate* const is,
                                   gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_any_pressed(is)) {
        g->display_help_menu = false;
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }
}
*/

static void update_debug_panel_buffer(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    // Static buffers to avoid reallocating every frame
    //static const char* control_modes[] = {"Player", "Camera", "Unknown"};
    static const char* control_modes[] = {"Camera", "Player", "Unknown"};
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
    int z = -1;
    int inventory_count = -1;
    //entityid shield_id = -1;
    direction_t player_dir = DIR_NONE;
    direction_t shield_dir = DIR_NONE;
    bool is_b = false;
    bool test_guard = g->test_guard;
    //vec3 loc = g_get_location(g, g->hero_id);
    //x = loc.x;
    //y = loc.y;
    //z = loc.z;

    //int inventory_sort_menu_selection = g->sort_inventory_menu_selection;

    //if (e) {
    //    vec3 loc = g_get_location(g, e->id);
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
             "Frame : %d\n"
             "Update: %d\n"
             "Frame Dirty: %d\n"
             "Draw Time: %.1fms\n"
             "Is3D: %d\n"
             "Cam: (%.0f,%.0f) Zoom: %.1f\n"
             "Mode: %s | Floor: %d/%d\n"
             "Entities: %d | Flag: %s\n"
             "Turn: %d | Hero: (%d,%d,%d)\n"
             "Inventory: %d\n"
             "msg_history.count: %d\n"
             "shield_dir_str: %s\n"
             "player_dir_str: %s\n"
             "is_blocking: %d\n"
             "test_guard: %d\n"
             "inventory_sort_menu_selection: %d\n"
             "HELLO TWITCH AND YOUTUBE!\n"
             "66666\n",
             g->timebeganbuf,
             g->currenttimebuf,
             g->framecount,
             g->frame_updates,
             g->frame_dirty,
             g->last_frame_time * 1000,
             g->is3d,
             g->cam2d.offset.x,
             g->cam2d.offset.y,
             g->cam2d.zoom,
             control_mode,
             0, //g->d->current_floor + 1, // More user-friendly 1-based
             0, //g->d->num_floors,
             g->next_entityid,
             flag_name,
             g->entity_turn,
             x,
             y,
             z,
             inventory_count,
             0, //g->msg_history.count,
             get_dir_as_string(shield_dir),
             get_dir_as_string(player_dir),
             is_b,
             test_guard,
             0);
}
/*
*/

void liblogic_init(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    srand(time(NULL));

    init_dungeon(g);

    //g->msg_system.count = 0;
    //g->msg_system.index = 0;
    //g->msg_system.is_active = false;
    //
    //init_player(g);
    msuccess("liblogic_init: Game state initialized");
}

/*
static int get_hitdie_for_race(race_t race) {
    int hit_die = 4;
    switch (race) {
    case RACE_GREEN_SLIME: hit_die = 4; break;
    case RACE_BAT: hit_die = 4; break;
    case RACE_HALFLING: hit_die = 6; break;
    case RACE_GOBLIN: hit_die = 6; break;
    case RACE_WOLF: hit_die = 6; break;
    case RACE_HUMAN: hit_die = 8; break;
    case RACE_ELF: hit_die = 8; break;
    case RACE_DWARF: hit_die = 8; break;
    case RACE_ORC: hit_die = 10; break;
    case RACE_WARG: hit_die = 12; break;
    default: break;
    }
    return hit_die;
}
*/

/*
static vec3 get_base_attack_damage_for_race(race_t race) {
    vec3 r = {1, 4, 0}; // Default base attack damage
    switch (race) {
    case RACE_GREEN_SLIME: r = (vec3){1, 1, 0}; break;
    case RACE_BAT: r = (vec3){1, 2, 0}; break;
    case RACE_HALFLING: r = (vec3){1, 4, 0}; break;
    case RACE_GOBLIN: r = (vec3){1, 4, 0}; break;
    case RACE_WOLF: r = (vec3){1, 6, 0}; break;
    case RACE_HUMAN: r = (vec3){1, 4, 0}; break;
    case RACE_ELF: r = (vec3){1, 4, 0}; break;
    case RACE_DWARF: r = (vec3){1, 4, 0}; break;
    case RACE_ORC: r = (vec3){1, 6, 0}; break;
    case RACE_WARG: r = (vec3){1, 12, 0}; break;
    default: break;
    }
    return r;
}
*/

/*
static race_t get_random_race() {
    race_t race = RACE_NONE;
    int num_choices = 10;
    int choice = rand() % num_choices;
    switch (choice) {
    case 0: race = RACE_BAT; break;
    case 1: race = RACE_WOLF; break;
    case 2: race = RACE_HUMAN; break;
    case 3: race = RACE_ELF; break;
    case 4: race = RACE_DWARF; break;
    case 5: race = RACE_HALFLING; break;
    case 6: race = RACE_ORC; break;
    case 7: race = RACE_GOBLIN; break;
    case 8: race = RACE_GREEN_SLIME; break;
    case 9: race = RACE_WARG; break;
    default: break;
    }
    return race;
}
*/

/*
static race_t get_random_race_for_floor(int floor) {
    race_t race = RACE_NONE;
    if (floor == 0) {
        return RACE_GREEN_SLIME;
    } else if (floor >= 1 && floor <= 4) {
        int num_choices = 3;
        int choice = rand() % num_choices;
        switch (choice) {
        case 0: race = RACE_GREEN_SLIME; break;
        case 1: race = RACE_BAT; break;
        case 2: race = RACE_WOLF; break;
        default: break;
        }
        return race;
    }
    return get_random_race();
}
*/

/*
static bool npc_create_set_stats(gamestate* const g, vec3 loc, race_t race) {
    minfo("npc_create_set_stats: %d,%d,%d %d", loc.x, loc.y, loc.z, race);
    entityid id = ENTITYID_INVALID;
    bool success = false;
    //const char* race_name = get_race_str(race);
    string race_name = get_race_str(race);
    id = npc_create(g, race, loc, race_name);
    if (id != ENTITYID_INVALID) {
        int floor = loc.z + 1;
        //minfo("Spawning entity at %d, %d, %d on floor %d with HP %d", loc.x, loc.y, loc.z, floor, g_get_stat(g, id, STATS_HP));
        int hit_die = get_hitdie_for_race(race);
        vec3 r = {1, hit_die, 0};
        int max_hp = do_roll(r);
        g_set_stat(g, id, STATS_HITDIE, hit_die);
        g_set_stat(g, id, STATS_AC, 10);
        g_set_stat(g, id, STATS_XP, 0);
        g_set_stat(g, id, STATS_LEVEL, 1);
        vec3 base_attack_dmg = get_base_attack_damage_for_race(race);
        g_set_base_attack_damage(g, id, base_attack_dmg);
        g_set_stat(g, id, STATS_STR, do_roll_best_of_3((vec3){3, 6, 0}));
        g_set_stat(g, id, STATS_DEX, do_roll_best_of_3((vec3){3, 6, 0}));
        g_set_stat(g, id, STATS_CON, do_roll_best_of_3((vec3){3, 6, 0}));
        max_hp += bonus_calc(g_get_stat(g, id, STATS_CON));
        if (max_hp <= 0) max_hp = 1; // Ensure max HP is at least 1
        g_set_stat(g, id, STATS_MAXHP, max_hp);
        g_set_stat(g, id, STATS_HP, max_hp);
        g_set_default_action(g, id, ENTITY_ACTION_MOVE_A_STAR);
        // we will update this to do an appropriate difficulty-scaling
        // level-up is too powerful and results in imbalance
        // the goal is to make the spawns challenge rating approximate
        // and close either above or below the player's level and cr
        for (int i = 1; i < floor; i++) {
            handle_level_up(g, id);
        }
        //int new_level = g_get_stat(g, id, STATS_LEVEL);
        //massert(g_get_stat(g, id, STATS_LEVEL) == floor, "New level %d does not match floor %d", new_level, floor);
        msuccess("Spawned entity of Level %d with %d HP at %d, %d, %d",
                 g_get_stat(g, id, STATS_LEVEL),
                 max_hp,
                 loc.x,
                 loc.y,
                 loc.z);
        // update vision distance
        // this will be appropriately set on a per-npc basis but for now...
        // hard code 5
        //int vision_distance0 = g_get_vision_distance(g, id);
        g_set_vision_distance(g, id, 5);
        // verify vision distance
        //int vision_distance = g_get_vision_distance(g, id);
        //massert(g_get_vision_distance(g, id) == 5, "Vision distance %d does not match expected value 5", vision_distance);
        //int default_light_radius = 3;
        g_set_light_radius(g, id, 3);
        // verify light radius
        massert(g_get_light_radius(g, id) == 3,
                "Light radius %d does not match expected value 3",
                g_get_light_radius(g, id));
        success = true;
    }
    return success;
}
*/

/*
static void try_spawn_npc(gamestate* const g) {
    massert(g, "gamestate is NULL");
    static bool do_this_once = true;
    int every_nth_turn = 10;
    if (g->turn_count % every_nth_turn == 0) {
        bool success = false;
        if (do_this_once) {
            while (!success) {
                int current_floor = g->d->current_floor;
                vec3 loc = get_random_available_loc(g, current_floor);
                if (loc.x == -1 && loc.y == -1 && loc.z == -1) {
                    merror("No available location found for NPC spawn");
                    return; // No valid location found, exit early
                }
                race_t race = get_random_race_for_floor(current_floor);
                success = npc_create_set_stats(g, loc, race);
            }
            do_this_once = false;
        }
    } else {
        do_this_once = true;
    }
}
*/

/*
static void update_player_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    if (!g->gameover) {
        if (g_is_dead(g, g->hero_id)) {
            add_message_history(g, "You died!");
            g->gameover = true;
        }
        check_and_handle_level_up(g, g->hero_id);
    }
    if (g_is_dead(g, g->hero_id)) {
        return;
    }
}
*/

/*
static void handle_level_up(gamestate* const g, entityid id) {
    int level = g_get_stat(g, id, STATS_LEVEL);
    // Level up the entity
    level++;
    g_set_stat(g, id, STATS_LEVEL, level);
    //int test_level = g_get_stat(g, id, STATS_LEVEL);
    massert(g_get_stat(g, id, STATS_LEVEL) == level, "Failed to set level");
    // Increase attack bonus
    int old_attack_bonus = g_get_stat(g, id, STATS_ATTACK_BONUS);
    int new_attack_bonus = old_attack_bonus + 1; // Example: increase by 1
    g_set_stat(g, id, STATS_ATTACK_BONUS, new_attack_bonus);
    // Increase max HP based on Constitution bonus
    int con_bonus = bonus_calc(g_get_stat(g, id, STATS_CON));
    if (con_bonus < 0)
        con_bonus = 0; // Ensure Constitution bonus is not negative
    int hitdie = g_get_stat(g, id, STATS_HITDIE);
    // roll the hitdie
    vec3 r = {1, hitdie, 0}; // 1d(hitdie)
    int hp_gain = do_roll(r) + con_bonus; // Add Constitution bonus to HP gain
    int old_max_hp = g_get_stat(g, id, STATS_MAXHP);
    int new_max_hp =
        old_max_hp + hp_gain; // Increase max HP by the rolled amount
    if (new_max_hp <= old_max_hp)
        new_max_hp = old_max_hp + 1; // Ensure max HP increases
    int new_next_level_xp = calc_next_lvl_xp(g, id);
    g_set_stat(g, id, STATS_MAXHP, new_max_hp);
    g_set_stat(g, id, STATS_HP, new_max_hp); // Restore HP to max
    // Set next level XP requirement (example: 1000 XP for next level)
    g_set_stat(g, id, STATS_NEXT_LEVEL_XP, new_next_level_xp);
    if (id == g->hero_id) {
        add_message_and_history(g, "You leveled up!");
        add_message_and_history(g, "Level %d", level);
        add_message_and_history(g, "Max HP increased to %d", new_max_hp);
    } else {
        msuccess(
            "NPC %d leveled up to Level %d with %d HP", id, level, new_max_hp);
    }
}
*/

/*
static void check_and_handle_level_up(gamestate* const g, entityid id) {
    if (g_get_stat(g, id, STATS_XP) >= g_get_stat(g, id, STATS_NEXT_LEVEL_XP)) {
        handle_level_up(g, id);
    }
}
*/

/*
static inline void update_npc_state(gamestate* const g, entityid id) {
    if (g_is_dead(g, id)) return;
}
*/

/*
static void update_npcs_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    for (entityid id = 0; id < g->next_entityid; id++) {
        if (id == g->hero_id) continue;
        update_npc_state(g, id);
    }
}
*/

/*
static void handle_npc(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    if (id == g->hero_id) return;
    if (g_is_type(g, id, ENTITY_NPC) && !g_is_dead(g, id)) {
        minfo("Handling NPC %d", id);
        execute_action(g, id, g_get_default_action(g, id));
    }
}
*/

/*
static void handle_npcs(gamestate* const g) {
    massert(g, "Game state is NULL!");
    massert(g->flag == GAMESTATE_FLAG_NPC_TURN,
            "Game state is not in NPC turn!");
    // Process all NPCs
    //for (int i = 0; i < g->index_entityids; i++) handle_nth_npc(g, i);
    for (entityid id = 0; id < g->next_entityid; id++) {
        handle_npc(g, id);
    }
    // After processing all NPCs, set the flag to animate all movements together
    g->flag = GAMESTATE_FLAG_NPC_ANIM;
}
*/

/*
static inline void reset_player_blocking(gamestate* const g) {
    massert(g, "Game state is NULL!");
    g_set_blocking(g, g->hero_id, false);
    g->test_guard = false;
}
*/

/*
static inline void reset_player_block_success(gamestate* const g) {
    massert(g, "Game state is NULL!");
    g_set_block_success(g, g->hero_id, false);
}
*/

void liblogic_tick(shared_ptr<inputstate> is, shared_ptr<gamestate> g) {
    //minfo("liblogic_tick: is=%p, g=%p", is, g);
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    // Spawn NPCs periodically
    //try_spawn_npc(g);
    //update_player_state(g);
    //update_npcs_state(g);
    //if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
    //    reset_player_blocking(g);
    //    reset_player_block_success(g);
    //}
    handle_input(is, g);
    //if (g->flag == GAMESTATE_FLAG_NPC_TURN) {
    //    handle_npcs(g);
    //}
    update_debug_panel_buffer(g);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}

//void liblogic_close(gamestate* const g) {
void liblogic_close(shared_ptr<gamestate> g) {
    massert(g, "liblogic_close: gamestate is NULL");
    d_free(g->dungeon);
}

// Check if a location is traversable (walkable and unoccupied)
/*
static inline bool is_traversable(gamestate* const g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    // get the dungeon floor
    dungeon_floor_t* df = d_get_floor(g->d, z);
    massert(df, "floor is NULL");
    // Check map bounds
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        return false;
    }
    // Get the current tile
    //tile_t* tile = df_tile_at(df, (vec3){x, y, z});
    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, z});
    massert(tile, "tile is NULL");
    // Check if the tile type is walkable
    if (!tile_is_walkable(tile->type)) return false;
    // Check for blocking entities
    // Comment out the next line if entity blocking is interfering with pathfinding tests
    if (tile_has_live_npcs(g, tile)) return false;
    return true;
}
*/

/*
void liblogic_restart(gamestate* const g) {
    massert(g, "liblogic_restart: gamestate is NULL");
    g->do_restart = true;
}
*/
