//#include "component.h"
#include "bonus_table.h"
#include "controlmode.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "dungeon_tile_type.h"
#include "elemental.h"
#include "entity_actions.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "inputstate.h"
#include "keybinding.h"
#include "libgame_defines.h"
#include "liblogic.h"
#include "location.h"
#include "massert.h"
#include "mprint.h"
#include "path_node.h"
#include "potion.h"
#include "race.h"
#include "roll.h"
#include "shield.h"
#include "stats_slot.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

static inline tile_t* get_first_empty_tile_around_entity(gamestate* const g, entityid id);

static inline bool is_traversable(gamestate* const g, int x, int y, int z);

static void handle_attack_blocked(gamestate* const g, entityid attacker_id, entityid target_id, bool* atk_successful);
static inline void reset_player_blocking(gamestate* const g);
static inline void reset_player_block_success(gamestate* const g);
static inline void update_npc_state(gamestate* const g, entityid id);
static inline void handle_camera_zoom(gamestate* const g, const inputstate* const is);
//static inline void try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl);

static int get_hitdie_for_race(race_t race);
static loc_t* get_empty_non_wall_locs_in_area(dungeon_floor_t* const df, int* count, int x0, int y0, int w, int h);
static loc_t* get_locs_around_entity(gamestate* const g, entityid id);

static void init_npc_test(gamestate* g);
static void init_sword_test(gamestate* g);
static void init_dagger_test(gamestate* g);
static void init_axe_test(gamestate* g);
static void init_bow_test(gamestate* g);
static void init_shield_test(gamestate* g);
static void init_potion_test(gamestate* g);
static void init_wand_test(gamestate* g);
static void init_dungeon(gamestate* const g);
static void update_player_state(gamestate* const g);
static void update_debug_panel_buffer(gamestate* const g);
static void handle_camera_move(gamestate* const g, const inputstate* const is);
static void handle_input(const inputstate* const is, gamestate* const g);
static void handle_input_camera(const inputstate* const is, gamestate* const g);
static void handle_input_player(const inputstate* const is, gamestate* const g);
static void handle_input_inventory(const inputstate* const is, gamestate* const g);
static void add_message_history(gamestate* const g, const char* fmt, ...);
static void add_message_and_history(gamestate* g, const char* fmt, ...);
static void add_message(gamestate* g, const char* fmt, ...);
static void try_entity_move_a_star(gamestate* const g, entityid id);
static void try_entity_move(gamestate* const g, entityid id, int x, int y);
static void try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y);
static void try_entity_zap(gamestate* const g, entityid attacker_id, loc_t target_loc);

static const char* get_action_key(const inputstate* const is, gamestate* const g);

static entityid player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);
static entityid npc_create(gamestate* const g, race_t rt, loc_t loc, const char* name);
static entityid item_create(gamestate* const g, itemtype type, loc_t loc, const char* name);
static entityid weapon_create(gamestate* const g, weapontype type, loc_t loc, const char* name);
static entityid arrow_create(gamestate* const g, loc_t loc, const char* name);
static entityid shield_create(gamestate* const g, shieldtype type, loc_t loc, const char* name);
static entityid potion_create(gamestate* const g, loc_t loc, potiontype type, const char* name);
static entityid wand_create(gamestate* const g, loc_t loc, const char* name);

static loc_t get_random_empty_non_wall_loc_in_area(gamestate* const g, int floor, int x, int y, int w, int h);
static loc_t get_random_empty_non_wall_loc(gamestate* const g, int floor);

static bool entities_adjacent(gamestate* const g, entityid id0, entityid id1);
static bool player_on_tile(gamestate* g, int x, int y, int z);
static void try_entity_traverse_floors(gamestate* const g, entityid id);
//static bool tile_has_closed_door(const gamestate* const g, int x, int y, int fl);
//static bool tile_has_door(const gamestate* const g, int x, int y, int fl);
static int calc_next_lvl_xp(gamestate* const g, entityid id);

static int calc_next_lvl_xp(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // get the current level
    int lvl = g_get_stat(g, id, STATS_LEVEL);
    massert(lvl >= 0, "level is negative");
    // calculate the next level's xp
    const int base_xp = 10;
    int next_lvl_xp = base_xp * (powl(2, lvl) - 1);
    massert(next_lvl_xp >= 0, "next level xp is negative");
    // set the next level's xp
    return next_lvl_xp;
}

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
        entityid id = tile->entities[i];
        if (g_is_type(g, id, ENTITY_PLAYER)) {
            return true;
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
        if (g_is_type(g, eid, ENTITY_PLAYER) && !g_is_dead(g, eid)) count++;
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
    //if (player_on_tile(g, ex, ey, z)) {
    //    merror("Cannot move, player on tile");
    //    return;
    //}
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
    } else {
        if (type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
    }
}

static void handle_attack_success(gamestate* const g, entityid atk_id, entityid tgt_id, bool* atk_successful) {
    massert(g, "gamestate is NULL");
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
    massert(atk_successful, "attack_successful is NULL");

    entitytype_t tgttype = g_get_type(g, tgt_id);
    entitytype_t atktype = g_get_type(g, atk_id);

    if (*atk_successful) {
        entityid attacker_weapon_id = g_get_equipment(g, atk_id, EQUIP_SLOT_WEAPON);
        int dmg = 1;
        if (attacker_weapon_id == ENTITYID_INVALID) {
            // no weapon
            dmg = 1;
        } else {
            // weapon
            // we will calculate damage based on weapon attributes
            massert(g_has_damage(g, attacker_weapon_id), "attacker weapon does not have damage attached");
            roll dmg_roll = g_get_damage(g, attacker_weapon_id);
            dmg = do_roll(dmg_roll);
            const int atk_bonus = g_get_stat(g, atk_id, STATS_ATTACK_BONUS);
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
            add_message_history(g, "%s attacked you for %d damage!", g_get_name(g, atk_id), dmg);
        } else if (tgttype == ENTITY_NPC) {
            add_message_history(g, "%s attacked %s for %d damage!", g_get_name(g, atk_id), g_get_name(g, tgt_id), dmg);
        }

        if (hp <= 0) {
            g_update_dead(g, tgt_id, true);
            if (tgttype == ENTITY_NPC) {
                add_message_history(g, "%s killed %s!", g_get_name(g, atk_id), g_get_name(g, tgt_id));
                // increment attacker's xp
                //int xp = g_get_stat(g, atk_id, STATS_XP);
                g_set_stat(g, atk_id, STATS_XP, g_get_stat(g, atk_id, STATS_XP) + 1);

                loc_t loc = g_get_location(g, tgt_id);
                entityid id = ENTITYID_INVALID;
                while (id == ENTITYID_INVALID) {
                    id = potion_create(g, loc, POTION_HEALTH_SMALL, "small health potion");
                }
                msuccess("Potion created at %d %d %d", loc.x, loc.y, loc.z);
            }
            //else if (tgttype == ENTITY_PLAYER) {
            //    add_message_and_history(g, "You are dead!");
            //}
        } else {
            g_update_dead(g, tgt_id, false);
        }
    } else {
        // handle attack miss
        if (tgttype == ENTITY_PLAYER) {
            add_message_history(g, "%s's attack missed!", g_get_name(g, atk_id));
        } else if (tgttype == ENTITY_NPC) {
            add_message_history(g, "%s missed %s!", g_get_name(g, atk_id), g_get_name(g, tgt_id));
        }
    }

    //e_set_hp(target, e_get_hp(target) - dmg); // Reduce HP by 1
    //if (target->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked you for %d damage!", attacker->name, dmg);
    //if (attacker->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked %s for %d damage!", attacker->name, target->name, dmg);
    //if (e_get_hp(target) <= 0) {
    //target->dead = true;
    //g_update_dead(g, tgt_id, true);
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
    entitytype_t tgttype = g_get_type(g, target_id);
    //if (tgttype == ENTITY_PLAYER) {
    //} else if (tgttype == ENTITY_NPC) {
    add_message_history(g, "%s blocked %s's attack!", g_get_name(g, target_id), g_get_name(g, attacker_id));
    //}

    //if (target->type == ENTITY_PLAYER) { add_message_and_history(g, "%s blocked %s's attack!", target->name, attacker->name); }
}

static bool handle_shield_check(gamestate* const g, entityid attacker_id, entityid target_id, int attack_roll, int base_ac, bool* attack_successful) {
    // if you have a shield at all, the attack will get auto-blocked
    entityid shield_id = g_get_equipment(g, target_id, EQUIP_SLOT_SHIELD);
    if (shield_id != ENTITYID_INVALID) {
        const int shield_ac = g_get_ac(g, shield_id);
        const int total_ac = base_ac + shield_ac;
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
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) return false;
    if (g_is_dead(g, target_id)) return false;
    // lets try an experiment...

    // get the armor class of the target
    const int base_ac = g_get_stat(g, target_id, STATS_AC);
    const int base_str = g_get_stat(g, attacker_id, STATS_STR);
    const int str_bonus = bonus_calc(base_str);
    const int atk_bonus = g_get_stat(g, attacker_id, STATS_ATTACK_BONUS);
    const int attack_roll = rand() % 20 + 1 + str_bonus + atk_bonus; // 1d20 + str bonus + attack bonus
    *attack_successful = false;
    if (attack_roll >= base_ac) {
        return handle_shield_check(g, attacker_id, target_id, attack_roll, base_ac, attack_successful);
    }
    // attack misses
    merror("Attack missed");
    //*attack_successful = false;
    handle_attack_success(g, attacker_id, target_id, attack_successful);
    return false;
}

static void handle_attack_helper(gamestate* const g, tile_t* tile, entityid attacker_id, bool* successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(successful, "attack_successful is NULL");
    for (int i = 0; i < tile->entity_max; i++) handle_attack_helper_innerloop(g, tile, i, attacker_id, successful);
}

static void try_entity_zap(gamestate* const g, entityid atkid, loc_t targetloc) {
    massert(g, "gamestate is NULL");
    massert(!g_is_dead(g, atkid), "zapper entity is dead");
    loc_t loc = g_get_location(g, atkid);
    dungeon_floor_t* const floor = d_get_floor(g->d, loc.z);
    massert(floor, "failed to get dungeon floor");
    tile_t* const tile = df_tile_at(floor, targetloc);
    if (!tile) {
        merror("target tile not found");
        return;
    }
    bool ok = false;
    loc_t eloc = g_get_location(g, atkid);
    int dx = targetloc.x - eloc.x;
    int dy = targetloc.y - eloc.y;
    g_update_direction(g, atkid, get_dir_from_xy(dx, dy));
    //g_set_zapping(g, atkid, true);
    g_set_update(g, atkid, true);
    //handle_zap_helper(g, tile, atkid, &ok);
    //handle_zap_success_gamestate_flag(g, g_get_type(g, atkid), ok);
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
//}

//static void init_weapon_test2(gamestate* g) {
//    massert(g, "gamestate is NULL");
//    e_add_item_to_inventory(e, sword->id);
//    // equip the sword
//    e->weapon = sword->id;
//}

static void init_dungeon(gamestate* const g) {
    massert(g, "gamestate is NULL");
    g->d = d_create();
    massert(g->d, "failed to init dungeon");

    const int df_count = 20;
    for (int i = 0; i < df_count; i++) {
        d_add_floor(g->d, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
    }
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

    entityid id = g_add_entity(g);
    //entityid id = g->next_entityid;
    //if (!g->dirty_entities) {
    //    g->dirty_entities = true;
    //    g->new_entityid_begin = id;
    //    g->new_entityid_end = id + 1;
    //} else {
    //    //g->dirty_entities = true;
    //    g->new_entityid_end = id + 1;
    //}
    //g->next_entityid++;

    g_add_name(g, id, name);
    g_add_type(g, id, ENTITY_NPC);
    g_add_race(g, id, rt);
    g_add_direction(g, id, DIR_RIGHT);
    g_add_location(g, id, loc);
    g_add_sprite_move(g, id, (loc_t){0, 0}); // default
    g_add_dead(g, id, false);
    g_add_update(g, id, false);
    g_add_attacking(g, id, false);
    g_add_zapping(g, id, false);
    g_add_block_success(g, id, false);
    g_add_damaged(g, id, false);
    g_add_default_action(g, id, ENTITY_ACTION_WAIT);
    g_add_inventory(g, id);
    g_add_target(g, id, (loc_t){-1, -1, -1});
    g_add_target_path(g, id);
    g_add_equipment(g, id);

    g_add_stats(g, id);
    g_set_stat(g, id, STATS_LEVEL, 1);
    g_set_stat(g, id, STATS_XP, 0);

    g_set_stat(g, id, STATS_NEXT_LEVEL_XP, calc_next_lvl_xp(g, id));
    g_set_stat(g, id, STATS_MAXHP, 1);
    g_set_stat(g, id, STATS_HP, 1);
    g_set_stat(g, id, STATS_HITDIE, 1);
    g_set_stat(g, id, STATS_STR, 10);
    g_set_stat(g, id, STATS_DEX, 10);
    g_set_stat(g, id, STATS_CON, 10);
    g_set_stat(g, id, STATS_ATTACK_BONUS, 0);

    g_set_stat(g, id, STATS_AC, 10);

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
    entityid id = g_add_entity(g);

    g_add_name(g, id, name);
    g_add_type(g, id, ENTITY_ITEM);
    g_add_direction(g, id, DIR_RIGHT);
    g_add_location(g, id, loc);
    g_add_sprite_move(g, id, (loc_t){0, 0});
    g_add_update(g, id, false);
    g_add_itemtype(g, id, type);

    if (!df_add_at(df, id, loc.x, loc.y)) {
        merror("failed to add entity to dungeon floor");
        return ENTITYID_INVALID;
    }
    return id;
}

static entityid weapon_create(gamestate* const g, weapontype type, loc_t loc, const char* name) {
    massert(g, "gamestate is NULL");
    entityid id = item_create(g, ITEM_WEAPON, loc, name);
    if (id == ENTITYID_INVALID) {
        merror("failed to create weapon");
        return ENTITYID_INVALID;
    }
    g_add_weapontype(g, id, type);
    g_add_damage(g, id, (roll){0, 0, 0});
    return id;
}

static entityid arrow_create(gamestate* const g, loc_t loc, const char* name) {
    massert(g, "gamestate is NULL");
    entityid id = item_create(g, ITEM_ARROW, loc, name);
    if (id == ENTITYID_INVALID) {
        merror("failed to create arrow");
        return ENTITYID_INVALID;
    }
    //massert(id != ENTITYID_INVALID, "failed to create weapon");
    //g_register_comp(g, id, C_WEAPONTYPE);
    //g_add_weapontype(g, id, type);
    return id;
}

static entityid potion_create(gamestate* const g, loc_t loc, potiontype type, const char* name) {
    minfo("potion create...");
    massert(g, "gamestate is NULL");
    entityid id = item_create(g, ITEM_POTION, loc, name);
    if (id == ENTITYID_INVALID) {
        merror("failed to create potion");
        return ENTITYID_INVALID;
    }
    g_add_potiontype(g, id, type);
    msuccess("potion created!");
    return id;
}

static entityid wand_create(gamestate* const g, loc_t loc, const char* name) {
    minfo("wand create...");
    massert(g, "gamestate is NULL");
    entityid id = item_create(g, ITEM_WAND, loc, name);
    if (id == ENTITYID_INVALID) {
        merror("failed to create wand");
        return ENTITYID_INVALID;
    }
    //g_add_potiontype(g, id, type);
    spell_effect effect = {0, ELEMENTAL_FIRE, 1, {1, 6, 0}};
    g_add_spell_effect(g, id, effect);
    msuccess("wand created!");
    return id;
}

static entityid shield_create(gamestate* const g, shieldtype type, loc_t loc, const char* name) {
    massert(g, "gamestate is NULL");

    int random_ac = do_roll((roll){1, 4, 0});

    char name_buffer[128];
    if (name && name[0]) {
        snprintf(name_buffer, sizeof(name_buffer), "%s + %d", name, random_ac);
    }

    entityid id = item_create(g, ITEM_SHIELD, loc, name_buffer);
    if (id == ENTITYID_INVALID) {
        merror("failed to create shield");
        return ENTITYID_INVALID;
    }
    //massert(id != ENTITYID_INVALID, "failed to create weapon");
    g_add_shieldtype(g, id, type);

    g_add_ac(g, id, random_ac);

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

    //int str_roll = do_roll_best_of_3((roll){3, 6, 0});
    //int con_roll = do_roll_best_of_3((roll){3, 6, 0});
    int str_roll = do_roll_best_of_3((roll){3, 6, 0});
    int con_roll = do_roll_best_of_3((roll){3, 6, 0});
    int dex_roll = do_roll_best_of_3((roll){3, 6, 0});

    g_set_stat(g, id, STATS_STR, str_roll);
    g_set_stat(g, id, STATS_CON, con_roll);
    g_set_stat(g, id, STATS_DEX, dex_roll);

    int hitdie = 8;
    g_set_stat(g, id, STATS_HITDIE, hitdie);

    int maxhp_roll = do_roll_best_of_3((roll){1, hitdie, 0}) + bonus_calc(con_roll);
    while (maxhp_roll < 1) {
        maxhp_roll = do_roll_best_of_3((roll){1, hitdie, 0}) + bonus_calc(con_roll);
    }

    g_set_stat(g, id, STATS_MAXHP, maxhp_roll);
    g_set_stat(g, id, STATS_HP, maxhp_roll);

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
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->controlmode = CONTROLMODE_PLAYER;
        g->display_inventory_menu = false;
        return;
    }
    int count = 0;
    entityid* inventory = g_get_inventory(g, g->hero_id, &count);
    if (count == 0) {
        return;
    }
    if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
        g->inventory_menu_selection = g->inventory_menu_selection + 1 >= count ? 0 : g->inventory_menu_selection + 1;
        //} else if (inputstate_is_pressed(is, KEY_UP)) {
    } else if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
        g->inventory_menu_selection = g->inventory_menu_selection - 1 < 0 ? count - 1 : g->inventory_menu_selection - 1;
        // drop item
    } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
        // we need to grab the entityid of the selected item
        entityid item_id = inventory[g->inventory_menu_selection];
        g_remove_from_inventory(g, g->hero_id, item_id);
        // add the item to the tile where the player is located at
        loc_t loc = g_get_location(g, g->hero_id);
        dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
        massert(df, "Dungeon floor is NULL!");
        minfo("Dropping item %d at %d, %d, %d", item_id, loc.x, loc.y, loc.z);
        tile_t* const tile = df_tile_at(df, loc);
        massert(tile, "Tile is NULL!");
        if (!tile_add(tile, item_id)) {
            merror("Failed to add item to tile");
            return;
        }
        // we also have to update the location of the item
        g_update_location(g, item_id, loc);
        g->controlmode = CONTROLMODE_PLAYER;
        g->display_inventory_menu = false;
        //}
    }
    //else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
    //        entityid item_id = inventory[g->inventory_menu_selection];
    // we will eventually adjust this to check which slot it needs to go into based on its various types
    //        entitytype_t type = g_get_type(g, item_id);
    //        if (type == ENTITY_ITEM) {
    //            itemtype item_type = g_get_itemtype(g, item_id);
    //            if (item_type == ITEM_WEAPON) {
    //                // check if the item is already equipped
    //                entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON);
    //                if (equipped_item != ENTITYID_INVALID) {
    //                    // unequip the currently equipped item
    //                    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON);
    //                    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                } else {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON, item_id);
    //                    add_message_history(g, "%s equipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                }
    //            } else if (item_type == ITEM_SHIELD) {
    //                entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD);
    //                if (equipped_item != ENTITYID_INVALID) {
    //                    // unequip the currently equipped item
    //                    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD);
    //                    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                } else {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD, item_id);
    //                    add_message_history(g, "%s equipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                }
    //            } else if (item_type == ITEM_WAND) {
    //                entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
    //                if (equipped_item != ENTITYID_INVALID) {
    //                    // unequip the currently equipped item
    //                    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
    //                    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                } else {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WAND, item_id);
    //                    add_message_history(g, "%s equipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                }
    //            }
    //        }
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->display_inventory_menu = false;
    //        g->controlmode = CONTROLMODE_PLAYER;
    //        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    // }
    else if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_APOSTROPHE)) {
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
                        const int small_hp_health_roll = rand() % 4 + 1;
                        hp += small_hp_health_roll;
                        if (hp > maxhp) {
                            hp = maxhp;
                        }

                        g_set_stat(g, g->hero_id, STATS_HP, hp);
                        add_message_history(g, "%s drank a %s and recovered %d HP", g_get_name(g, g->hero_id), g_get_name(g, item_id), small_hp_health_roll);
                        // remove the potion from the inventory
                        g_remove_from_inventory(g, g->hero_id, item_id);
                        // add the potion to the tile where the player is located at
                        //loc_t loc = g_get_location(g, g->hero_id);
                        //dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
                        //massert(df, "Dungeon floor is NULL!");
                        //tile_t* const tile = df_tile_at(df, loc);
                        //massert(tile, "Tile is NULL!");
                        //if (!tile_add(tile, item_id)) {
                        //    merror("Failed to add item to tile");
                        //    return;
                        //}
                        g->controlmode = CONTROLMODE_PLAYER;
                        g->display_inventory_menu = false;
                        g->controlmode = CONTROLMODE_PLAYER;
                        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                    }
                }
            } else if (item_type == ITEM_WEAPON) {
                // check if the item is already equipped
                entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON);
                if (equipped_item != ENTITYID_INVALID) {
                    // unequip the currently equipped item
                    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON);
                    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
                } else {
                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON, item_id);
                    add_message_history(g, "%s equipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
                }
                g->controlmode = CONTROLMODE_PLAYER;
                g->display_inventory_menu = false;
                g->controlmode = CONTROLMODE_PLAYER;
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;

            } else if (item_type == ITEM_SHIELD) {
                entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD);
                if (equipped_item != ENTITYID_INVALID) {
                    // unequip the currently equipped item
                    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD);
                    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
                } else {
                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD, item_id);
                    add_message_history(g, "%s equipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
                }

                g->controlmode = CONTROLMODE_PLAYER;
                g->display_inventory_menu = false;
                g->controlmode = CONTROLMODE_PLAYER;
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;

            } else if (item_type == ITEM_WAND) {
                entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
                if (equipped_item != ENTITYID_INVALID) {
                    // unequip the currently equipped item
                    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
                    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
                } else {
                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WAND, item_id);
                    add_message_history(g, "%s equipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
                }

                g->controlmode = CONTROLMODE_PLAYER;
                g->display_inventory_menu = false;
                g->controlmode = CONTROLMODE_PLAYER;
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
        }
    }
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
        entityid itemid = tile->entities[i];
        entitytype_t type = g_get_type(g, itemid);
        minfo("Item %s type: %d", g_get_name(g, itemid), type);
        if (type == ENTITY_ITEM) {
            add_message_history(g, "%s picked up a %s", g_get_name(g, id), g_get_name(g, itemid));
            //add_message_history(g, "%s picked up a %s", g_get_name(g, id), g_get_name(g, itemid));
            tile_remove(tile, itemid);
            g_add_to_inventory(g, id, itemid);
            if (g_is_type(g, id, ENTITY_PLAYER)) {
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
            return true;
        }
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
        if (inputstate_any_pressed(is)) {
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
    //if (strcmp(action, "none") != 0) {
    //    minfo("action: %s", action);
    //}
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
        } else if (strcmp(action, "traverse") == 0) {
            // we need to attempt to navigate either up or down a floor depending on the tile beneath the player
            try_entity_traverse_floors(g, g->hero_id);
        }
    } else {
        merror("No action found for key");
    }
}

static void try_entity_traverse_floors(gamestate* const g, entityid id) {
    loc_t loc = g_get_location(g, id);
    dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    massert(df, "Dungeon floor is NULL!");
    tile_t* const tile = df_tile_at(df, loc);
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
                if (!df_remove_at(df, id, loc.x, loc.y)) {
                    merror("Failed to remove entity from old tile");
                    return;
                }
                loc_t next_downstairs_loc = df_get_downstairs(g->d->floors[g->d->current_floor - 1]);
                massert(next_downstairs_loc.x != -1 && next_downstairs_loc.y != -1, "Failed to get next downstairs location");
                // we need to set the player's location to the corresponding TILE_downstairs
                next_downstairs_loc.z = g->d->current_floor - 1;
                g_update_location(g, id, next_downstairs_loc);
                // we need to set the player's floor to the next floor
                g->d->current_floor--;
                // get the next dungeon floor
                dungeon_floor_t* const next_floor = d_get_floor(g->d, g->d->current_floor);
                int ex = next_downstairs_loc.x;
                int ey = next_downstairs_loc.y;

                if (!df_add_at(next_floor, id, ex, ey)) {
                    merror("Failed to add entity to new tile");
                    return;
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
                loc_t next_upstairs_loc = df_get_upstairs(g->d->floors[g->d->current_floor + 1]);
                massert(next_upstairs_loc.x != -1 && next_upstairs_loc.y != -1, "Failed to get next upstairs location");
                // we need to set the player's location to the corresponding TILE_UPSTAIRS
                next_upstairs_loc.z = g->d->current_floor + 1;
                g_update_location(g, id, next_upstairs_loc);
                // we need to set the player's floor to the next floor
                g->d->current_floor++;
                // get the next dungeon floor
                dungeon_floor_t* const next_floor = d_get_floor(g->d, g->d->current_floor);
                int ex = next_upstairs_loc.x;
                int ey = next_upstairs_loc.y;
                if (!df_add_at(next_floor, id, ex, ey)) {
                    merror("Failed to add entity to new tile");
                    return;
                }

                if (type == ENTITY_PLAYER) {
                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                }
            }
        } else {
            if (id == g->hero_id) {
                add_message(g, "You are already at the bottom floor");
            }
        }
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
    //if (inputstate_is_pressed(is, KEY_D)) {
    //    msuccess("D pressed!");
    //    g->debugpanelon = !g->debugpanelon;
    //    return;
    //}

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

    if (g->controlmode == CONTROLMODE_PLAYER) {
        handle_input_player(is, g);
    } else if (g->controlmode == CONTROLMODE_CAMERA) {
        handle_input_camera(is, g);
    } else if (g->controlmode == CONTROLMODE_INVENTORY) {
        handle_input_inventory(is, g);
    } else {
        merror("Unknown control mode");
    }
    //}
}

static void update_debug_panel_buffer(gamestate* const g) {
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
    entityid shield_id = -1;
    direction_t player_dir = DIR_NONE;
    direction_t shield_dir = DIR_NONE;
    bool is_b = false;
    bool test_guard = g->test_guard;
    loc_t loc = g_get_location(g, g->hero_id);
    x = loc.x;
    y = loc.y;
    z = loc.z;
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
             "Turn: %d | Hero: (%d,%d,%d)\n"
             "Inventory: %d\n"
             "msg_history.count: %d\n"
             "shield_dir_str: %s\n"
             "player_dir_str: %s\n"
             "is_blocking: %d\n"
             "test_guard: %d\n"
             "HELLO TWITCH AND YOUTUBE!\n"
             //"TEST 12345\n"
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
             g->next_entityid,
             flag_name,
             g->entity_turn,
             x,
             y,
             z,
             inventory_count,
             g->msg_history.count,
             get_dir_as_string(shield_dir),
             get_dir_as_string(player_dir),
             is_b,
             test_guard);
    //e->block_success);
}

void liblogic_init(gamestate* const g) {
    massert(g, "gamestate is NULL");
    srand(time(NULL));
    init_dungeon(g);
    //gamestate_init_entityids(g);
    g->msg_system.count = 0;
    g->msg_system.index = 0;
    g->msg_system.is_active = false;
    gamestate_load_keybindings(g);
    init_player(g);
    //minfo("player initialized");
    // test to create a weapon
    //init_weapon_test(g);
    //init_weapon_test2(g);
    //init_shield_test2(g);
    //init_potion_test(g, POTION_HP_SMALL, "small healing potion");
    //init_potion_test(g, POTION_HP_MEDIUM, "medium healing potion");
    //init_potion_test(g, POTION_HP_LARGE, "large healing potion");
    //init_npcs_test_by_room(g);
    //init_npc_test(g);
    //init_sword_test(g);
    init_dagger_test(g);
    //init_axe_test(g);
    //init_bow_test(g);
    init_shield_test(g);
    //init_wand_test(g);
    //init_potion_test(g);
    update_debug_panel_buffer(g);
}

static void init_npc_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    //loc_t loc = g_get_location(g, g->hero_id);
    //loc_t* locs = get_locs_around_entity(g, g->hero_id);
    int count = 0;
    int max = 1;
    //for (int i = 0; i < 8; i++) {
    while (count < max) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        entityid id = npc_create(g, RACE_ORC, loc, "orc");
        //entityid id = npc_create(g, RACE_ORC, locs[i], "orc");
        if (id != ENTITYID_INVALID) {
            g_set_default_action(g, id, ENTITY_ACTION_MOVE_A_STAR);
            count++;
        }
    }
}

static void init_sword_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = ENTITYID_INVALID;
    while (id == ENTITYID_INVALID) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        id = weapon_create(g, WEAPON_SWORD, loc, "dummy sword");
        g_set_damage(g, id, (roll){1, 6, 0});
    }
}

static void init_dagger_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = ENTITYID_INVALID;
    while (id == ENTITYID_INVALID) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        id = weapon_create(g, WEAPON_DAGGER, loc, "dagger");
        //if (id != ENTITYID_INVALID) {
        //massert(g_set_damage(g, id, (roll){1, 4, 0}), "Failed to set damage");
        g_set_damage(g, id, (roll){1, 4, 0});
        //}
    }
}

static void init_axe_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = ENTITYID_INVALID;
    while (id == ENTITYID_INVALID) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        id = weapon_create(g, WEAPON_AXE, loc, "dummy axe");
        g_set_damage(g, id, (roll){1, 8, 0});
    }
}

static void init_bow_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = ENTITYID_INVALID;
    while (id == ENTITYID_INVALID) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        id = weapon_create(g, WEAPON_BOW, loc, "dummy bow");
        g_set_damage(g, id, (roll){1, 6, 0});
    }
}

static void init_shield_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = ENTITYID_INVALID;
    while (id == ENTITYID_INVALID) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        id = shield_create(g, SHIELD_BUCKLER, loc, "dummy buckler");
    }
}

static void init_wand_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = ENTITYID_INVALID;
    while (id == ENTITYID_INVALID) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        id = wand_create(g, loc, "dummy wand");
    }
}

static int get_hitdie_for_race(race_t race) {
    int hit_die = 4;
    switch (race) {
    case RACE_HUMAN: hit_die = 6; break;
    case RACE_ELF: hit_die = 6; break;
    case RACE_DWARF: hit_die = 6; break;
    case RACE_ORC: hit_die = 8; break;
    case RACE_GOBLIN: hit_die = 4; break;
    case RACE_HALFLING: hit_die = 4; break;
    case RACE_WOLF: hit_die = 4; break;
    case RACE_BAT: hit_die = 2; break;
    case RACE_WARG: hit_die = 8; break;
    case RACE_GREEN_SLIME: hit_die = 4; break;
    default: break;
    }
    return hit_die;
}

static void try_spawn_npc(gamestate* const g) {
    massert(g, "gamestate is NULL");
    static bool do_this_once = true;
    const int every_nth_turn = 10;
    if (g->turn_count % every_nth_turn == 0) {
        bool success = false;
        if (do_this_once) {
            while (!success) {
                int current_floor = g->d->current_floor;
                loc_t loc = get_random_empty_non_wall_loc(g, current_floor);
                entityid id = ENTITYID_INVALID;
                race_t race = RACE_GREEN_SLIME;
                int choice = rand() % 8;
                switch (choice) {
                case 0: race = RACE_BAT; break;
                case 1: race = RACE_WOLF; break;
                case 2: race = RACE_HUMAN; break;
                case 3: race = RACE_ELF; break;
                case 4: race = RACE_DWARF; break;
                case 5: race = RACE_HALFLING; break;
                case 6: race = RACE_ORC; break;
                case 7: race = RACE_GOBLIN; break;
                default: break;
                }
                //race = RACE_BAT;
                id = npc_create(g, race, loc, "NPC");
                if (id != ENTITYID_INVALID) {
                    int hit_die = get_hitdie_for_race(race);
                    roll r = {1, hit_die, 0};
                    int max_hp = do_roll(r);
                    g_set_stat(g, id, STATS_AC, 10);
                    g_set_stat(g, id, STATS_XP, 0);
                    g_set_stat(g, id, STATS_LEVEL, 1);
                    g_set_stat(g, id, STATS_STR, do_roll((roll){3, 6, 0}));
                    g_set_stat(g, id, STATS_DEX, do_roll((roll){3, 6, 0}));
                    g_set_stat(g, id, STATS_CON, do_roll((roll){3, 6, 0}));
                    max_hp += bonus_calc(g_get_stat(g, id, STATS_CON));
                    g_set_stat(g, id, STATS_MAXHP, max_hp);
                    g_set_stat(g, id, STATS_HP, max_hp);
                    g_set_default_action(g, id, ENTITY_ACTION_MOVE_A_STAR);
                    success = true;
                }
            }
            do_this_once = false;
        }
    } else {
        do_this_once = true;
    }
}

static void init_potion_test(gamestate* g) {
    massert(g, "gamestate is NULL");
    entityid id = ENTITYID_INVALID;
    while (id == ENTITYID_INVALID) {
        loc_t loc = get_random_empty_non_wall_loc(g, 0);
        id = potion_create(g, loc, POTION_HEALTH_SMALL, "small health potion");
    }
}

static void update_player_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    //massert(e, "liblogic_update_player_state: hero is NULL");
    if (!g->gameover) {
        //if (e->dead) {
        //minfo("calling g_is_dead 0");
        if (g_is_dead(g, g->hero_id)) {
            add_message_history(g, "You died!");
            g->gameover = true;
        }
        //if (e->dead) return;
        //minfo("calling g_is_dead 1");
        //if (e_get_hp(e) <= 0) {
        //    g_update_dead(g, e->id, true);
        //    e->do_update = true;
        //}

        // check for player level up
        int xp = g_get_stat(g, g->hero_id, STATS_XP);
        int level = g_get_stat(g, g->hero_id, STATS_LEVEL);
        int next_level_xp = g_get_stat(g, g->hero_id, STATS_NEXT_LEVEL_XP);

        if (xp >= next_level_xp) {
            // Level up the player
            level++;
            g_set_stat(g, g->hero_id, STATS_LEVEL, level);
            // Increase attack bonus
            int old_attack_bonus = g_get_stat(g, g->hero_id, STATS_ATTACK_BONUS);
            int new_attack_bonus = old_attack_bonus + 1; // Example: increase by 1
            g_set_stat(g, g->hero_id, STATS_ATTACK_BONUS, new_attack_bonus);

            // Increase max HP based on Constitution bonus
            int con_bonus = bonus_calc(g_get_stat(g, g->hero_id, STATS_CON));

            int old_max_hp = g_get_stat(g, g->hero_id, STATS_MAXHP);
            int new_max_hp = old_max_hp + con_bonus;
            if (new_max_hp <= old_max_hp) new_max_hp = old_max_hp + 1; // Ensure max HP increases

            int new_next_level_xp = calc_next_lvl_xp(g, g->hero_id);
            g_set_stat(g, g->hero_id, STATS_MAXHP, new_max_hp);
            g_set_stat(g, g->hero_id, STATS_HP, new_max_hp); // Restore HP to max
            // Set next level XP requirement (example: 1000 XP for next level)
            g_set_stat(g, g->hero_id, STATS_NEXT_LEVEL_XP, new_next_level_xp);

            add_message_and_history(g, "You leveled up!");
            add_message_and_history(g, "Level %d", level);
            add_message_and_history(g, "Max HP increased to %d", new_max_hp);
        }
    }

    if (g_is_dead(g, g->hero_id)) return;
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
    //for (int i = 0; i < g->index_entityids; i++) {
    for (entityid id = 0; id < g->next_entityid; id++) {
        if (id == g->hero_id) continue;
        update_npc_state(g, id);
    }
}

//static void handle_nth_npc(gamestate* const g, int i) {
static void handle_npc(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    if (id == g->hero_id) {
        return;
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
    //for (int i = 0; i < g->index_entityids; i++) handle_nth_npc(g, i);
    for (entityid id = 0; id < g->next_entityid; id++) handle_npc(g, id);
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

    // Spawn NPCs periodically
    try_spawn_npc(g);

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
