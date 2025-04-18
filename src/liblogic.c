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

static inline bool liblogic_entity_has_shield(gamestate* const g, entityid id);
static inline void liblogic_reset_player_block_success(gamestate* const g);

static void liblogic_add_message_history(gamestate* const g, const char* msg) {
    massert(g, "liblogic_add_message_history: gamestate is NULL");
    massert(msg, "liblogic_add_message_history: msg is NULL");
    if (g->msg_history.count >= g->msg_history.max_count) {
        mwarning("Message history full!");
        return;
    }
    strncpy(g->msg_history.messages[g->msg_history.count], msg, MAX_MSG_LENGTH - 1);
    g->msg_history.messages[g->msg_history.count][MAX_MSG_LENGTH - 1] = '\0'; // Ensure null-termination
    g->msg_history.count++;
}

static void liblogic_add_message(gamestate* g, const char* fmt, ...) {
    massert(g, "liblogic_add_message: gamestate is NULL");
    massert(fmt, "liblogic_add_message: format string is NULL");
    if (g->msg_system.count >= MAX_MESSAGES) {
        mwarning("Message queue full!");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(g->msg_system.messages[g->msg_system.count], MAX_MSG_LENGTH - 1, fmt, args);
    va_end(args);
    liblogic_add_message_history(g, g->msg_system.messages[g->msg_system.count]);
    g->msg_system.count++;
    g->msg_system.is_active = true;
}

static void liblogic_update_equipped_shield_dir(gamestate* g, entity* e) {
    minfo("liblogic_update_equipped_shield_dir: e->id: %d, e->shield: %d", e->id, e->shield);
    if (e->shield != -1) {
        msuccess("liblogic_update_equipped_shield_dir: e->shield is not -1");
        entity* shield = em_get(g->entitymap, e->shield);
        if (shield) {
            msuccess("liblogic_update_equipped_shield_dir: shield found");
            shield->direction = e->direction;
            shield->do_update = true;
        }
    }
}

static bool liblogic_player_on_tile(const gamestate* const g, int x, int y, int floor) {
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

static inline int liblogic_tile_npc_living_count(const gamestate* const g, int x, int y, int fl) {
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
        if (eid == -1) {
            continue;
        }
        const entity* const e = em_get(g->entitymap, eid);
        if (!e) {
            continue;
        }
        if (e->type == ENTITY_NPC && !e->is_dead) {
            count++;
        }
    }
    return count;
}

static void liblogic_try_entity_move(gamestate* const g, entity* const e, int x, int y) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    e->do_update = true;
    e->direction = get_dir_from_xy(x, y);
    const int ex = e->x + x;
    const int ey = e->y + y;
    const int floor = e->floor;
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, floor);
    if (!df) {
        merror("Failed to get dungeon floor");
        return;
    }
    // i feel like this might be something we can set elsewhere...like after the player input phase?
    if (e->type == ENTITY_PLAYER) {
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
    tile_t* const tile = dungeon_floor_tile_at(df, ex, ey);
    if (!tile) {
        merror("Cannot move, tile is NULL");
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
    if (!dungeon_floor_remove_at(df, e->id, e->x, e->y)) {
        merror("Failed to remove entity from old tile");
        return;
    }
    if (!dungeon_floor_add_at(df, e->id, ex, ey)) {
        merror("Failed to add entity to new tile");
        return;
    }
    e->x = ex;
    e->y = ey;
    e->sprite_move_x = x * DEFAULT_TILE_SIZE;
    e->sprite_move_y = y * DEFAULT_TILE_SIZE;
    // at this point the move is 'successful'
    liblogic_update_equipped_shield_dir(g, e);
    // get the entity's new tile
    tile_t* const new_tile = dungeon_floor_tile_at(df, ex, ey);
    if (!new_tile) {
        merror("Failed to get new tile");
        return;
    }
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

static void liblogic_try_entity_move_player(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_move_player: gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "liblogic_try_entity_move_player: hero is NULL");
    int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
    int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
    if (dx != 0 || dy != 0) {
        liblogic_try_entity_move(g, e, dx, dy);
    }
}

static void liblogic_try_entity_move_random(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_move_random: gamestate is NULL");
    massert(e, "liblogic_try_entity_move_random: entity is NULL");
    int x = rand() % 3;
    int y = 0;
    if (x == 0) {
        x = -1;
    } else if (x == 1) {
        x = 0;
    } else {
        x = 1;
    }
    // if x is 0, y cannot also be 0
    if (x == 0) {
        y = rand() % 2;
        y = y == 0 ? -1 : 1;
    } else {
        y = rand() % 3;
        if (y == 0) {
            y = -1;
        } else if (y == 1) {
            y = 0;
        } else {
            y = 1;
        }
    }
    liblogic_try_entity_move(g, e, x, y);
}

static inline void liblogic_handle_attack_success_gamestate_flag(gamestate* const g, entitytype_t type, bool success) {
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

static void
liblogic_handle_attack_success(gamestate* const g, entity* attacker, entity* target, bool* attack_successful) {
    massert(g, "liblogic_handle_attack_success: gamestate is NULL");
    massert(attacker, "liblogic_handle_attack_success: attacker entity is NULL");
    massert(target, "liblogic_handle_attack_success: target entity is NULL");
    massert(attack_successful, "liblogic_handle_attack_success: attack_successful is NULL");
    msuccess("Successful Attack on target: %s", target->name);
    *attack_successful = true;
    target->is_damaged = true;
    target->do_update = true;
    int dmg = 1;
    entity_set_hp(target, entity_get_hp(target) - dmg); // Reduce HP by 1
    if (target->type == ENTITY_PLAYER) {
        liblogic_add_message(g, "You took %d damage!", dmg);
    }
    if (entity_get_hp(target) <= 0) {
        target->is_dead = true;
    }
}

static void
liblogic_handle_attack_blocked(gamestate* const g, entity* attacker, entity* target, bool* attack_successful) {
    massert(g, "liblogic_handle_attack_blocked: gamestate is NULL");
    massert(attacker, "liblogic_handle_attack_blocked: attacker entity is NULL");
    massert(target, "liblogic_handle_attack_blocked: target entity is NULL");
    massert(attack_successful, "liblogic_handle_attack_blocked: attack_successful is NULL");
    msuccess("Successful Block from target: %s", target->name);
    *attack_successful = false;
    target->is_damaged = false;
    target->block_success = true;
    target->do_update = true;
    if (target->type == ENTITY_PLAYER) {
        //liblogic_add_message(g, "You blocked the attack!");
        liblogic_add_message_history(g, "You blocked the attack!");
    }
}

static inline bool liblogic_handle_attack_helper_innerloop(gamestate* const g,
                                                           tile_t* tile,
                                                           int i,
                                                           entity* attacker,
                                                           bool* attack_successful) {
    massert(g, "liblogic_handle_attack_helper_innerloop: gamestate is NULL");
    massert(tile, "liblogic_handle_attack_helper_innerloop: tile is NULL");
    massert(i >= 0, "liblogic_handle_attack_helper_innerloop: i is out of bounds");
    massert(i < tile->entity_max, "liblogic_handle_attack_helper_innerloop: i is out of bounds");
    massert(attacker, "liblogic_handle_attack_helper_innerloop: attacker is NULL");
    massert(attack_successful, "liblogic_handle_attack_helper_innerloop: attack_successful is NULL");

    entityid id = tile->entities[i];
    if (id != -1) {
        entity* const target = em_get(g->entitymap, id);
        if (target) {
            entitytype_t type = target->type;
            if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
                msuccess("Attacking target: %s", target->name);
                if (!target->is_dead) {
                    if (target->is_blocking) {
                        msuccess("Block successful");
                        liblogic_handle_attack_blocked(g, attacker, target, attack_successful);
                        return false;
                    }

                    msuccess("Attack successful");
                    liblogic_handle_attack_success(g, attacker, target, attack_successful);
                    return true;
                } else {
                    merror("Target is dead, cannot attack");
                    return false;
                }
            } else {
                merror("Target is not a valid target: %s", entitytype_to_string(type));
                return false;
            }
        } else {
            merror("liblogic_handle_attack_helper: target entity is NULL");
            return false;
        }
    } else {
        //merror("liblogic_handle_attack_helper: target entity id is -1");
        return false;
    }

    merror("liblogic_handle_attack_helper_innerloop: no valid target found at the specified location");
    return false;
}

static void liblogic_handle_attack_helper(gamestate* const g, tile_t* tile, entity* attacker, bool* attack_successful) {
    massert(g, "liblogic_handle_attack_helper: gamestate is NULL");
    massert(tile, "liblogic_handle_attack_helper: tile is NULL");
    massert(attacker, "liblogic_handle_attack_helper: attacker is NULL");
    massert(attack_successful, "liblogic_handle_attack_helper: attack_successful is NULL");

    for (int i = 0; i < tile->entity_max; i++) {
        liblogic_handle_attack_helper_innerloop(g, tile, i, attacker, attack_successful);
    }
}

static void liblogic_try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y) {
    massert(g, "liblogic_try_entity_attack: gamestate is NULL");
    entity* const attacker = em_get(g->entitymap, attacker_id);
    massert(attacker, "liblogic_try_entity_attack: attacker entity is NULL");
    if (attacker->is_dead) {
        merror("liblogic_try_entity_attack: attacker entity is dead");
        return;
    }
    dungeon_floor_t* const floor = dungeon_get_floor(g->dungeon, attacker->floor);
    massert(floor, "liblogic_try_entity_attack: failed to get dungeon floor");
    tile_t* const tile = dungeon_floor_tile_at(floor, target_x, target_y);
    if (!tile) {
        merror("liblogic_try_entity_attack: target tile not found");
        return;
    }
    // Calculate direction based on target position
    bool attack_successful = false;
    int dx = target_x - attacker->x;
    int dy = target_y - attacker->y;
    attacker->direction = get_dir_from_xy(dx, dy);
    attacker->is_attacking = true;
    attacker->do_update = true;
    liblogic_handle_attack_helper(g, tile, attacker, &attack_successful);
    liblogic_handle_attack_success_gamestate_flag(g, attacker->type, attack_successful);
}

static void liblogic_try_entity_attack_random(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_attack_random: gamestate is NULL");
    massert(e, "liblogic_try_entity_attack_random: entity is NULL");
    int x = rand() % 3;
    int y = 0;
    if (x == 0) {
        x = -1;
    } else if (x == 1) {
        x = 0;
    } else {
        x = 1;
    }
    // if x is 0, y cannot also be 0
    if (x == 0) {
        y = rand() % 2;
        if (y == 0) {
            y = -1;
        } else {
            y = 1;
        }
    } else {
        y = rand() % 3;
        if (y == 0) {
            y = -1;
        } else if (y == 1) {
            y = 0;
        } else {
            y = 1;
        }
    }
    liblogic_try_entity_attack(g, e->id, e->x + x, e->y + y);
}

static void liblogic_try_entity_attack_player(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_attack_player: gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "liblogic_try_entity_attack_player: hero is NULL");
    int dx = (h->x > e->x) ? 1 : (h->x < e->x) ? -1 : 0;
    int dy = (h->y > e->y) ? 1 : (h->y < e->y) ? -1 : 0;
    if (dx != 0 || dy != 0) {
        liblogic_try_entity_attack(g, e->id, h->x, h->y);
    }
}

static bool liblogic_entities_adjacent(gamestate* const g, entityid id0, entityid id1) {
    massert(g, "liblogic_entities_adjacent: gamestate is NULL");
    entity* const e0 = em_get(g->entitymap, id0);
    if (!e0) {
        return false;
    }
    entity* const e1 = em_get(g->entitymap, id1);
    if (!e1) {
        return false;
    }
    // use e0 and check the surrounding 8 tiles
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0) {
                continue;
            }
            if (e0->x + x == e1->x && e0->y + y == e1->y) {
                return true;
            }
        }
    }
    return false;
}

static void liblogic_try_entity_move_attack_player(gamestate* const g, entity* const e) {
    massert(g, "liblogic_try_entity_move_attack_player: gamestate is NULL");
    entity* h = em_get(g->entitymap, g->hero_id);
    massert(h, "liblogic_try_entity_move_attack_player: hero is NULL");
    //if (h) {
    // check if we are adjacent to the player
    if (liblogic_entities_adjacent(g, e->id, h->id)) {
        liblogic_try_entity_attack(g, e->id, h->x, h->y);
    } else {
        liblogic_try_entity_move_player(g, e);
    }
    //}
}

static void liblogic_try_entity_wait(gamestate* const g, entity* const e) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    e->do_update = true;
    // waiting increments hp for player and NPCs
    if (e->type == ENTITY_PLAYER) {
        entity_incr_hp(e, 1);
        msuccess("Player HP: %d", e->hp);
    } else if (e->type == ENTITY_NPC) {
        entity_incr_hp(e, 1);
        msuccess("NPC HP: %d", e->hp);
    }
    // handle flag update
    if (e->type == ENTITY_PLAYER) {
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
}

static void liblogic_execute_action(gamestate* const g, entity* const e, entity_action_t action) {
    massert(g, "liblogic_execute_action: gamestate is NULL");
    massert(e, "liblogic_execute_action: entity is NULL");
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
    case ENTITY_ACTION_MOVE_RANDOM:
        liblogic_try_entity_move_random(g, e);
        break;
    case ENTITY_ACTION_WAIT: {
        liblogic_try_entity_wait(g, e);
        break;
    }
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
    case ENTITY_ACTION_INTERACT_DOWN_LEFT:
    case ENTITY_ACTION_INTERACT_DOWN_RIGHT:
    case ENTITY_ACTION_INTERACT_UP_LEFT:
    case ENTITY_ACTION_INTERACT_UP_RIGHT:
    case ENTITY_ACTION_INTERACT_LEFT:
    case ENTITY_ACTION_INTERACT_RIGHT:
    case ENTITY_ACTION_INTERACT_UP:
    case ENTITY_ACTION_INTERACT_DOWN:
    default:
        merror("Unknown entity action: %d", action);
        break;
    }
}

static entityid liblogic_weapon_create(gamestate* const g, int x, int y, int fl, const char* name) {
    // ... (existing validation checks) ...
    massert(g, "liblogic_weapon_create: gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "liblogic_weapon_create: entitymap is NULL");
    massert(name && name[0], "liblogic_weapon_create: name is NULL or empty");
    //massert(rt >= 0, "liblogic_weapon_create: race_type is out of bounds");
    //massert(rt < RACE_COUNT, "liblogic_weapon_create: race_type is out of bounds");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "liblogic_weapon_create: failed to get current dungeon floor");
    massert(x >= 0, "liblogic_weapon_create: x is out of bounds");
    massert(x < df->width, "liblogic_weapon_create: x is out of bounds");
    massert(y >= 0, "liblogic_weapon_create: y is out of bounds");
    massert(y < df->height, "liblogic_weapon_create: y is out of bounds");
    tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    massert(tile, "liblogic_weapon_create: failed to get tile");
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("liblogic_weapon_create: cannot create entity on wall");
        return -1;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("liblogic_weapon_create: cannot create entity on tile with NPC");
        return -1;
    }
    entity* const e = entity_new_weapon_at(next_entityid++, x, y, fl, name);
    if (!e) {
        merror("liblogic_weapon_create: failed to create entity");
        return -1;
    }

    // FIRST try to add to dungeon floor
    if (!dungeon_floor_add_at(df, e->id, x, y)) {
        merror("liblogic_weapon_create: failed to add entity to dungeon floor");
        entity_free(e); // Free immediately since EM doesn't own it yet
        return -1;
    }

    // ONLY add to EM after dungeon placement succeeds
    em_add(gamestate_get_entitymap(g), e);
    gamestate_add_entityid(g, e->id);

    return e->id;
}

//static entityid liblogic_weapon_create(gamestate* const g, int x, int y, int fl, const char* name) {
//    massert(g, "liblogic_weapon_create: gamestate is NULL");
//    em_t* em = gamestate_get_entitymap(g);
//    massert(em, "liblogic_weapon_create: entitymap is NULL");
//    massert(name && name[0], "liblogic_weapon_create: name is NULL or empty");
//    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
//    massert(df, "liblogic_weapon_create: failed to get current dungeon floor");
//    massert(x >= 0, "liblogic_weapon_create: x is out of bounds");
//    massert(x < df->width, "liblogic_weapon_create: x is out of bounds");
//    massert(y >= 0, "liblogic_weapon_create: y is out of bounds");
//    massert(y < df->height, "liblogic_weapon_create: y is out of bounds");
//    tile_t* const tile = dungeon_floor_tile_at(df, x, y);
//    massert(tile, "liblogic_weapon_create: failed to get tile");
//    if (!dungeon_tile_is_walkable(tile->type)) {
//        merror("liblogic_weapon_create: cannot create entity on wall");
//        return -1;
//    }
//    if (tile_has_live_npcs(tile, em)) {
//        merror("liblogic_weapon_create: cannot create entity on tile with NPC");
//        return -1;
//    }
//    entity* const e = entity_new_weapon_at(next_entityid++, x, y, fl, name);
//    if (!e) {
//        merror("liblogic_weapon_create: failed to create entity");
//        return -1;
//    }
//    em_add(em, e);
//    gamestate_add_entityid(g, e->id);
//    //dungeon_floor_add_at(df, e->id, x, y);
//    if (!dungeon_floor_add_at(df, e->id, x, y)) {
//        merror("liblogic_weapon_create: failed to add entity to dungeon floor");
//        entity_free(e);
//        return -1;
//    }
//    return e->id;
//}

static entityid liblogic_shield_create(gamestate* const g, int x, int y, int fl, const char* name) {
    massert(g, "liblogic_shield_create: gamestate is NULL");
    em_t* em = gamestate_get_entitymap(g);
    massert(em, "liblogic_shield_create: entitymap is NULL");
    massert(name && name[0], "liblogic_shield_create: name is NULL or empty");
    //massert(rt >= 0, "liblogic_shield_create: race_type is out of bounds");
    //massert(rt < RACE_COUNT, "liblogic_shield_create: race_type is out of bounds");
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, fl);
    massert(df, "liblogic_shield_create: failed to get current dungeon floor");
    massert(x >= 0, "liblogic_shield_create: x is out of bounds");
    massert(x < df->width, "liblogic_shield_create: x is out of bounds");
    massert(y >= 0, "liblogic_shield_create: y is out of bounds");
    massert(y < df->height, "liblogic_shield_create: y is out of bounds");
    // can we create an entity at this location? no entities can be made on wall-types etc
    tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    massert(tile, "liblogic_shield_create: failed to get tile");
    if (!dungeon_tile_is_walkable(tile->type)) {
        merror("liblogic_shield_create: cannot create entity on wall");
        return -1;
    }
    if (tile_has_live_npcs(tile, em)) {
        merror("liblogic_shield_create: cannot create entity on tile with NPC");
        return -1;
    }
    entity* const e = entity_new_shield_at(next_entityid++, x, y, fl, name);
    if (!e) {
        merror("liblogic_shield_create: failed to create entity");
        return -1;
    }
    em_add(em, e);
    gamestate_add_entityid(g, e->id);
    //dungeon_floor_add_at(df, e->id, x, y);
    if (!dungeon_floor_add_at(df, e->id, x, y)) {
        merror("liblogic_shield_create: failed to add entity to dungeon floor");
        entity_free(e);
        return -1;
    }
    return e->id;
}

static void liblogic_init_weapon_test(gamestate* const g) {
    massert(g, "liblogic_init_weapon_test: gamestate is NULL");
    dungeon_t* const d = g->dungeon;
    massert(d, "liblogic_init_weapon_test: dungeon is NULL");
    dungeon_floor_t* const df = dungeon_get_floor(d, 0);
    massert(df, "liblogic_init_weapon_test: dungeon floor is NULL");
    // get player position
    entity* const player = em_get(g->entitymap, g->hero_id);
    massert(player, "liblogic_init_weapon_test: player is NULL");
    int x = player->x;
    int y = player->y;
    bool found = false;

    // place the shield somewhere around the player
    found = false;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (found) {
                break;
            }
            if (i == 0 && j == 0) {
                continue;
            }
            int new_x = x + i;
            int new_y = y + j;
            tile_t* const tile = dungeon_floor_tile_at(df, new_x, new_y);
            if (tile_entity_count(tile) > 0) {
                continue;
            }
            // check if the tile is walkable
            if (!dungeon_tile_is_walkable(tile->type)) {
                continue;
            }
            // create the shield
            entityid shield_id = liblogic_shield_create(g, new_x, new_y, 0, "shield");
            massert(shield_id != -1, "liblogic_init_weapon_test: failed to create shield");
            entity* const shield = em_get(g->entitymap, shield_id);
            massert(shield, "liblogic_init_weapon_test: shield is NULL");
            // set the shield direction to the player direction
            found = true;
        }
        if (found) {
            break;
        }
    }

    // place the sword somewhere around the player
    //found = false;
    //for (int i = -1; i <= 1; i++) {
    //    for (int j = -1; j <= 1; j++) {
    //        if (found) {
    //            break;
    //        }
    //        if (i == 0 && j == 0) {
    //            continue;
    //        }
    //        int new_x = x + i;
    //        int new_y = y + j;
    //        tile_t* const tile = dungeon_floor_tile_at(df, new_x, new_y);
    //        if (tile_entity_count(tile) > 0) {
    //            continue;
    //        }
    //        // check if the tile is walkable
    //        if (!dungeon_tile_is_walkable(tile->type)) {
    //            continue;
    //        }
    //        // create the shield
    //        //        entityid shield_id = liblogic_shield_create(g, x - 1, y, 0, "shield");
    //        entityid sword_id = liblogic_weapon_create(g, new_x, new_y, 0, "sword");
    //        massert(sword_id != -1, "liblogic_init_weapon_test: failed to create weapon");
    //        //        massert(shield_id != -1, "liblogic_init_weapon_test: failed to create shield");
    //        entity* const sword = em_get(g->entitymap, sword_id);
    //        massert(sword, "liblogic_init_weapon_test: sword is NULL");
    //        found = true;
    //    }
    //    if (found) {
    //        break;
    //    }
    //}
}

static void liblogic_init_dungeon(gamestate* const g) {
    massert(g, "liblogic_init_dungeon: gamestate is NULL");
    g->dungeon = dungeon_create();
    massert(g->dungeon, "liblogic_init_dungeon: failed to init dungeon");
    dungeon_add_floor(g->dungeon, DEFAULT_DUNGEON_FLOOR_WIDTH, DEFAULT_DUNGEON_FLOOR_HEIGHT);
}

static void liblogic_init_em(gamestate* const g) {
    massert(g, "liblogic_init: gamestate is NULL");
    g->entitymap = em_new();
}

static entityid liblogic_npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
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

static entityid liblogic_player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
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

static void liblogic_init_player(gamestate* const g) {
    minfo("liblogic_init: initializing player");
    massert(g, "liblogic_init: gamestate is NULL");
    // setting it up so we can return a loc_t from a function
    // that can scan for an appropriate starting location
    loc_t loc = df_get_upstairs(g->dungeon->floors[g->dungeon->current_floor]);
    minfo("liblogic_init: creating player...");
    const int id = liblogic_player_create(g, RACE_HUMAN, loc.x, loc.y, 0, "hero");
    msuccess("liblogic_init: player id: %d", id);
    massert(id != -1, "liblogic_init: failed to init hero");
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "liblogic_init: hero is NULL");
    entity_set_maxhp(hero, 3);
    entity_set_hp(hero, 3);
    g->entity_turn = g->hero_id;
    msuccess("liblogic_init: hero id %d", g->hero_id);
}

//static void liblogic_init_orcs_test_naive_loop(gamestate* const g) {
//    massert(g, "liblogic_init: gamestate is NULL");
//    dungeon_t* const d = g->dungeon;
//    massert(d, "liblogic_init: dungeon is NULL");
//    //dungeon_floor_t* const df = d->floors[0];
//    dungeon_floor_t* const df = dungeon_get_floor(d, 0);
//    massert(df, "liblogic_init: dungeon floor is NULL");
//    // this works, but is naive and slow
//    int count = 0;
//    const int total_orcs_to_make = 100;
//    while (count < total_orcs_to_make) {
//        int x = rand() % df->width;
//        int y = rand() % df->height;
//        tile_t* const tile = dungeon_floor_tile_at(df, x, y);
//        if (dungeon_tile_is_wall(tile->type)) {
//            continue;
//        }
//        // check if there is already an entity at this location
//        if (tile_entity_count(tile) > 0) {
//            continue;
//        }
//        entity* const orc = liblogic_npc_create_ptr(g, RACE_ORC, x, y, 0, "orc");
//        if (!orc) {
//            merror("liblogic_init: failed to init orc");
//            continue;
//        }
//        entity_action_t action = ENTITY_ACTION_MOVE_ATTACK_PLAYER;
//        entity_set_default_action(orc, action);
//        entity_set_maxhp(orc, 1);
//        entity_set_hp(orc, 1);
//        count++;
//    }
//}

static entity_t* const liblogic_npc_create_ptr(gamestate* const g, race_t rt, int x, int y, int fl, const char* name) {
    massert(g, "liblogic_npc_create_ptr: gamestate is NULL");
    entityid id = liblogic_npc_create(g, rt, x, y, fl, name);
    if (id == -1) {
        merror("liblogic_npc_create_ptr: failed to create NPC");
        return NULL;
    }
    return em_get(g->entitymap, id);
}

static int liblogic_tile_npc_count(gamestate* const g, dungeon_floor_t* const df, tile_t* const tile) {
    massert(g, "liblogic_tile_npc_count: gamestate is NULL");
    massert(df, "liblogic_tile_npc_count: dungeon floor is NULL");
    //massert(x >= 0, "liblogic_tile_npc_count: x is out of bounds");
    //massert(x < df->width, "liblogic_tile_npc_count: x is out of bounds");
    //massert(y >= 0, "liblogic_tile_npc_count: y is out of bounds");
    //massert(y < df->height, "liblogic_tile_npc_count: y is out of bounds");
    //tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    massert(tile, "liblogic_tile_npc_count: failed to get tile");

    const int t_entity_count = tile_entity_count(tile);
    int tile_npc_count = 0;

    for (int i = 0; i < t_entity_count; i++) {
        entityid id = tile_get_entity(tile, i);
        massert(id != ENTITYID_INVALID, "liblogic_tile_npc_count: entity id is invalid");
        entity* const e = em_get(g->entitymap, id);
        massert(e, "liblogic_tile_npc_count: entity is NULL");
        if (e->type == ENTITY_NPC) {
            tile_npc_count++;
        }
    }
    return tile_npc_count;
}

static int liblogic_tile_npc_count_xy(gamestate* const g, dungeon_floor_t* const df, int x, int y) {
    massert(g, "liblogic_tile_npc_count: gamestate is NULL");
    massert(df, "liblogic_tile_npc_count: dungeon floor is NULL");
    massert(x >= 0, "liblogic_tile_npc_count: x is out of bounds");
    massert(x < df->width, "liblogic_tile_npc_count: x is out of bounds");
    massert(y >= 0, "liblogic_tile_npc_count: y is out of bounds");
    massert(y < df->height, "liblogic_tile_npc_count: y is out of bounds");

    tile_t* const tile = dungeon_floor_tile_at(df, x, y);
    massert(tile, "liblogic_tile_npc_count: failed to get tile");

    return liblogic_tile_npc_count(g, df, tile);
}

static void liblogic_init_orcs_test_intermediate(gamestate* const g) {
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
    const int count = df_count_walkable(df);
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
    int max_orcs = 1;
    //int max_orcs = count2;

    int created_orcs = 0;
    int i = 0;
    massert(max_orcs < count2, "liblogic_init: max_orcs is greater than count2");

    //for (int i = 0; i < max_orcs && i < count2; i++) {
    while (created_orcs < max_orcs && i < count2) {
        //tile_t* const tile = dungeon_floor_tile_at(df, locations[i].x, locations[i].y);
        //if (dungeon_tile_is_wall(tile->type)) {
        //    continue;
        //}
        // check if there is already an entity at this location
        //if (tile_entity_count(tile) > 0) {
        //if (liblogic_tile_npc_count(g, df, tile) > 0) {
        //    continue;
        //}

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
        entity_set_default_action(orc, action);
        entity_set_maxhp(orc, 1);
        entity_set_hp(orc, 1);
        i++;
        created_orcs++;
    }
    // we need to free the locations array
    free(locations);
}

static void liblogic_init_orcs_test(gamestate* const g) {
    //liblogic_init_orcs_test_naive_loop(g);
    liblogic_init_orcs_test_intermediate(g);
}

static const char* liblogic_get_action_key(const inputstate* const is, gamestate* const g) {
    const int key = inputstate_get_pressed_key(is);
    if (key != -1) {
        minfo("Key pressed: %d", key);
    }
    // can return early if key == -1
    if (key == -1) {
        return "none";
    }
    return get_action_for_key(&g->keybinding_list, key);
}

static void liblogic_handle_camera_move(gamestate* const g, const inputstate* const is) {
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
        //if (inputstate_is_pressed(is, KEY_C)) {
        //msuccess("C pressed!");
        g->cam2d.zoom = roundf(g->cam2d.zoom);
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }
}

static void liblogic_handle_camera_zoom(gamestate* const g, const inputstate* const is) {
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

static void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    liblogic_handle_camera_move(g, is);
    liblogic_handle_camera_zoom(g, is);
}

static inline void liblogic_change_player_dir(gamestate* const g, direction_t dir) {
    massert(g, "Game state is NULL!");
    //if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) {
    //    return;
    //}
    entity* const hero = em_get(g->entitymap, g->hero_id);
    massert(hero, "liblogic_change_player_dir: hero is NULL");
    if (hero->is_dead) {
        return;
    }
    hero->direction = dir;
    hero->do_update = true;
    liblogic_update_equipped_shield_dir(g, hero);
}

static void liblogic_try_entity_block(gamestate* const g, entity* const e) {
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");

    if (!liblogic_entity_has_shield(g, e->id)) {
        liblogic_add_message(g, "You have no shield to block with!");
        return;
    }

    e->do_update = true;
    e->is_blocking = true;
    g->test_guard = true;
    liblogic_update_equipped_shield_dir(g, e);
    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
}

static bool liblogic_try_entity_pickup(gamestate* const g, entity* const e) {
    minfo("liblogic_try_entity_pickup: trying to pick up item");
    massert(g, "Game state is NULL!");
    massert(e, "Entity is NULL!");
    e->do_update = true;
    // check if the player is on a tile with an item
    dungeon_floor_t* const df = dungeon_get_floor(g->dungeon, e->floor);
    if (!df) {
        merror("Failed to get dungeon floor");
        return false;
    }
    tile_t* const tile = dungeon_floor_tile_at(df, e->x, e->y);
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
            //liblogic_add_message(g, "Picked up weapon");
            liblogic_add_message(g, "Picked up %s", it->name);
            tile_remove(tile, id);
            // add the item to the entity inventory
            entity_add_item_to_inventory(e, id);
            e->weapon = id;
            msuccess("Picked up item: %s", it->name);
            if (e->type == ENTITY_PLAYER) {
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
            return true;
        } else if (it->type == ENTITY_SHIELD) {
            // pick up the item
            // picking up an item requires:
            // 1. removing its id from the tile
            // 2. adding its id to the entity inventory
            // remove the item from the tile
            liblogic_add_message(g, "Picked up %s", it->name);
            tile_remove(tile, id);
            // add the item to the entity inventory
            entity_add_item_to_inventory(e, id);
            liblogic_update_equipped_shield_dir(g, e);
            e->shield = id;
            msuccess("Picked up item: %s", it->name);
            if (e->type == ENTITY_PLAYER) {
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
            return true;
        }
    }
    liblogic_add_message(g, "No items to pick up");
    return false;
}

static void liblogic_try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl) {
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

static inline bool liblogic_entity_has_weapon(gamestate* const g, entityid id) {
    massert(g, "liblogic_entity_has_weapon: gamestate is NULL");
    massert(id != ENTITYID_INVALID, "liblogic_entity_has_weapon: entity ID is invalid");
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merror("liblogic_entity_has_weapon: entity not found");
        return false;
    }
    entityid id0 = e->weapon;
    if (id0 == ENTITYID_INVALID) {
        merror("liblogic_entity_has_weapon: entity has no weapon");
        return false;
    }
    return true;
}

static inline bool liblogic_entity_has_shield(gamestate* const g, entityid id) {
    massert(g, "liblogic_entity_has_shield: gamestate is NULL");
    massert(id != ENTITYID_INVALID, "liblogic_entity_has_shield: entity ID is invalid");
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merror("liblogic_entity_has_shield: entity not found");
        return false;
    }
    entityid id0 = e->shield;
    if (id0 == ENTITYID_INVALID) {
        merror("liblogic_entity_has_shield: entity has no shield");
        return false;
    }
    return true;
}

static void liblogic_handle_input_player(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT) {
        return;
    }
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
    if (hero->is_dead) {
        return;
    }
    if (action) {
        if (g->player_changing_direction) {
            if (strcmp(action, "wait") == 0) {
                liblogic_execute_action(g, hero, ENTITY_ACTION_WAIT);
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
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_LEFT);
        } else if (strcmp(action, "move_e") == 0) {
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_RIGHT);
        } else if (strcmp(action, "move_n") == 0) {
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_UP);
        } else if (strcmp(action, "move_s") == 0) {
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_DOWN);
        } else if (strcmp(action, "move_nw") == 0) {
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_UP_LEFT);
        } else if (strcmp(action, "move_ne") == 0) {
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_UP_RIGHT);
        } else if (strcmp(action, "move_sw") == 0) {
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_DOWN_LEFT);
        } else if (strcmp(action, "move_se") == 0) {
            liblogic_execute_action(g, hero, ENTITY_ACTION_MOVE_DOWN_RIGHT);
        } else if (strcmp(action, "attack") == 0) {
            msuccess("attack pressed!");
            if (liblogic_entity_has_weapon(g, hero->id)) {
                msuccess("Entity has weapon");
                int tx = hero->x + get_x_from_dir(hero->direction);
                int ty = hero->y + get_y_from_dir(hero->direction);
                liblogic_try_entity_attack(g, hero->id, tx, ty);
            } else {
                merror("Entity has no weapon");
                liblogic_add_message(g, "You have no weapon to attack with!");
                // add a message to the message system
            }
        } else if (strcmp(action, "block") == 0) {
            msuccess("Block pressed!");
            liblogic_try_entity_block(g, hero);
        } else if (strcmp(action, "interact") == 0) {
            // we are hardcoding the flip switch interaction for now
            // but eventually this will be generalized
            // for instance u can talk to an NPC merchant using "interact"
            // or open a door, etc
            msuccess("Space pressed!");
            int tx = hero->x + get_x_from_dir(hero->direction);
            int ty = hero->y + get_y_from_dir(hero->direction);
            liblogic_try_flip_switch(g, hero, tx, ty, hero->floor);
        } else if (strcmp(action, "pickup") == 0) {
            // add a message to the message system
            liblogic_try_entity_pickup(g, hero);
        } else if (strcmp(action, "toggle_camera") == 0) {
            g->controlmode = CONTROLMODE_CAMERA;
        }
    } else {
        merror("No action found for key");
    }
}

static void liblogic_handle_input(const inputstate* const is, gamestate* const g) {
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

static void liblogic_update_debug_panel_buffer(gamestate* const g) {
    massert(g, "liblogic_update_debug_panel_buffer: gamestate is NULL");
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
    int hero_x = -1;
    int hero_y = -1;
    int inventory_count = -1;
    entityid shield_id = -1;
    direction_t player_dir = DIR_NONE;
    direction_t shield_dir = DIR_NONE;
    bool is_blocking = false;
    bool test_guard = g->test_guard;
    if (e) {
        hero_x = e->x;
        hero_y = e->y;
        inventory_count = e->inventory_count;
        player_dir = e->direction;
        shield_id = e->shield;
        if (shield_id != -1) {
            entity* const shield = em_get(g->entitymap, shield_id);
            massert(shield, "liblogic_update_debug_panel_buffer: shield is NULL");
            shield_dir = shield->direction;
        }
        is_blocking = e->is_blocking;
    }
    // Determine control mode and flag strings
    const char* control_mode = control_modes[(g->controlmode >= 0 && g->controlmode < 2) ? g->controlmode : 2];
    const char* flag_name =
        flag_names[(g->flag >= GAMESTATE_FLAG_NONE && g->flag < GAMESTATE_FLAG_COUNT) ? g->flag : GAMESTATE_FLAG_COUNT];
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
             "block_success: %d\n",
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
             hero_y,
             inventory_count,
             g->msg_history.count,
             get_dir_as_string(shield_dir),
             get_dir_as_string(player_dir),
             is_blocking,
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
    liblogic_init_player(g);
    // test to create a weapon
    liblogic_init_weapon_test(g);
    // temporarily disabling
    liblogic_init_orcs_test(g);
    liblogic_update_debug_panel_buffer(g);
}

static void liblogic_update_player_state(gamestate* const g) {
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
        return;
    }
    if (e->hp <= 0) {
        e->is_dead = true;
        e->do_update = true;
        merror("Hero is dead!");
        return;
    }
}

static void liblogic_update_npc_state(gamestate* const g, entityid id) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merror("Failed to get entity");
        return;
    }
    if (e->is_dead) {
        return;
    }
    if (e->hp <= 0) {
        e->is_dead = true;
        e->do_update = true;
        merror("NPC is dead!");
        return;
    }
}

static void liblogic_update_npcs_state(gamestate* const g) {
    massert(g, "Game state is NULL!");
    for (int i = 0; i < g->index_entityids; i++) {
        entityid id = g->entityids[i];
        liblogic_update_npc_state(g, id);
    }
}

static void liblogic_handle_nth_npc(gamestate* const g, int i) {
    massert(g, "Game state is NULL!");
    massert(i >= 0, "Index is out of bounds!");
    massert(i < g->index_entityids, "Index is out of bounds!");
    entity* e = em_get(g->entitymap, g->entityids[i]);
    if (e) {
        if (e->type == ENTITY_NPC) {
            if (!e->is_dead) {
                liblogic_execute_action(g, e, e->default_action);
            }
        }
    }
}

static void liblogic_handle_npcs(gamestate* const g) {
    massert(g, "Game state is NULL!");
    massert(g->flag == GAMESTATE_FLAG_NPC_TURN, "Game state is not in NPC turn!");
    //if (g->flag != GAMESTATE_FLAG_NPC_TURN) return;
    // Process all NPCs
    for (int i = 0; i < g->index_entityids; i++) {
        liblogic_handle_nth_npc(g, i);
    }
    // After processing all NPCs, set the flag to animate all movements together
    g->flag = GAMESTATE_FLAG_NPC_ANIM;
}

static void liblogic_reset_player_blocking(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        merror("Failed to get hero entity");
        return;
    }
    e->is_blocking = false;
    //e->block_success = false;
    g->test_guard = false;
}

static inline void liblogic_reset_player_block_success(gamestate* const g) {
    massert(g, "Game state is NULL!");
    entity* const e = em_get(g->entitymap, g->hero_id);
    if (!e) {
        merror("Failed to get hero entity");
        return;
    }
    e->block_success = false;
}

void liblogic_tick(const inputstate* const is, gamestate* const g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    liblogic_update_player_state(g);
    liblogic_update_npcs_state(g);
    liblogic_handle_input(is, g);

    if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
        //liblogic_handle_input_player(is, g);
        liblogic_reset_player_block_success(g);
    }

    if (g->flag == GAMESTATE_FLAG_NPC_TURN) {
        liblogic_handle_npcs(g);
        liblogic_reset_player_blocking(g);
    }

    liblogic_update_debug_panel_buffer(g);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}

void liblogic_close(gamestate* const g) {
    massert(g, "liblogic_close: gamestate is NULL");
    //massert(g->entitymap, "liblogic_close: entitymap is NULL");

    //minfo("liblogic_close: calling em_free...");
    //em_free(g->entitymap);
    //msuccess("liblogic_close: em_free done");

    //g->entitymap = NULL;

    //if (!g->dungeon) {
    //    merror("Dungeon is NULL!");
    //    return;
    //}
    //dungeon_destroy(g->dungeon);
    //g->dungeon = NULL;
}
