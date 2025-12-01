#include "ComponentTable.h"
#include "ComponentTraits.h"
#include "create_door.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "entityid.h"
#include "gamestate.h"
#include "inputstate.h"
#include "liblogic.h"
#include "liblogic_add_message.h"
#include "liblogic_create_npc.h"
#include "liblogic_create_shield.h"
#include "liblogic_create_weapon.h"
#include "liblogic_handle_input.h"
#include "liblogic_handle_npcs.h"
#include "liblogic_init_dungeon.h"
#include "liblogic_update_debug_panel_buffer.h"
#include "liblogic_update_player_state.h"
#include "manage_inventory.h"
#include "massert.h"
#include "shield.h"
#include "update_player_tiles_explored.h"
#include "weapon.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <raylib.h>


using std::shared_ptr;
using std::string;


int liblogic_restart_count = 0;

void update_npcs_state(shared_ptr<gamestate> g);


void liblogic_init(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    srand(time(NULL));
    SetRandomSeed(time(NULL));
    minfo("liblogic_init");
    init_dungeon(g);
    //g->entity_turn = create_player(g, (vec3){0, 0, 0}, "darkmage");
    //create_wooden_box(g, (vec3){2, 2, 0});
    //create_wooden_box(g, (vec3){3, 2, 0});
    //create_wooden_box(g, (vec3){4, 2, 0});
    //create_wooden_box(g, (vec3){5, 2, 0});
    //create_wooden_box(g, (vec3){6, 2, 0});
    //create_wooden_box(g, (vec3){7, 2, 0});
    //create_wooden_box(g, (vec3){7, 3, 0});
    //create_wooden_box(g, (vec3){7, 4, 0});
    //create_wooden_box(g, (vec3){7, 5, 0});
    //create_wooden_box(g, (vec3){7, 6, 0});
    //create_wooden_box(g, (vec3){7, 7, 0});

    create_weapon_at(g, (vec3){10, 9, 0}, WEAPON_DAGGER);
    create_shield_at(g, (vec3){11, 9, 0}, SHIELD_BUCKLER);

    create_door_at(g, (vec3){12, 9, 0});
    create_door_at(g, (vec3){12, 9, 1});
    create_door_at(g, (vec3){14, 12, 0});
    //create_weapon_at(g, (vec3){3, 1, 0}, WEAPON_AXE);
    //create_weapon_at(g, (vec3){4, 1, 0}, WEAPON_SWORD);

    //entityid orc1 = create_npc(g, RACE_ORC, (vec3){13, 9, 0}, "orc");
    //entityid orc1_weapon = create_weapon(g, WEAPON_AXE);
    //add_to_inventory(g, orc1, orc1_weapon);
    //g->ct.set<equipped_weapon>(orc1, orc1_weapon);

    //entityid orc2 = create_npc(g, RACE_ORC, (vec3){2, 3, 0}, "orc");
    //entityid orc2_weapon = create_weapon(g, WEAPON_AXE);
    //add_to_inventory(g, orc2, orc2_weapon);
    //g->ct.set<equipped_weapon>(orc2, orc2_weapon);

    //entityid orc3 = create_npc(g, RACE_ORC, (vec3){3, 3, 0}, "orc");
    //entityid orc3_weapon = create_weapon(g, WEAPON_AXE);
    //add_to_inventory(g, orc3, orc3_weapon);
    //g->ct.set<equipped_weapon>(orc3, orc3_weapon);

    //entityid orc4 = create_npc(g, RACE_ORC, (vec3){1, 4, 0}, "orc");
    //entityid orc4_weapon = create_weapon(g, WEAPON_AXE);
    //add_to_inventory(g, orc4, orc4_weapon);
    //g->ct.set<equipped_weapon>(orc4, orc4_weapon);

    //entityid orc5 = create_npc(g, RACE_ORC, (vec3){2, 4, 0}, "orc");
    //entityid orc5_weapon = create_weapon(g, WEAPON_AXE);
    //add_to_inventory(g, orc5, orc5_weapon);
    //g->ct.set<equipped_weapon>(orc5, orc5_weapon);

    //entityid orc6 = create_npc(g, RACE_ORC, (vec3){3, 4, 0}, "orc");
    //entityid orc6_weapon = create_weapon(g, WEAPON_AXE);
    //add_to_inventory(g, orc6, orc6_weapon);
    //g->ct.set<equipped_weapon>(orc6, orc6_weapon);


    //create_potion(g, (vec3){1, 1, 0}, POTION_HP_SMALL);
    //create_potion(g, (vec3){3, 1, 0}, POTION_HP_MEDIUM);
    //create_potion(g, (vec3){5, 1, 0}, POTION_HP_LARGE);
    //create_weapon(g, (vec3){1, 5, 0}, WEAPON_TWO_HANDED_SWORD);
    //create_weapon(g, (vec3){1, 6, 0}, WEAPON_BOW);
    //create_weapon(g, (vec3){1, 6, 0}, WEAPON_WARHAMMER);
    //create_weapon(g, (vec3){1, 5, 0}, WEAPON_FLAIL);

    add_message(g, "Welcome to the game! Press enter to cycle messages.");
    add_message(g, "To move around, press [q w e a d z x c]");
    add_message(g, "To pick up items, press / ");
    add_message(g, "To attack, press ' ");
    msuccess("liblogic_init: Game state initialized");
}


//void update_player_state(shared_ptr<gamestate> g) {
//    if (g->hero_id != ENTITYID_INVALID) {
//        if (!g->gameover) {
//            unsigned char a = g->ct.get<txalpha>(g->hero_id).value_or(255);
//            if (a < 255) {
//                a++;
//            }
//            g->ct.set<txalpha>(g->hero_id, a);
//            if (g->ct.get<dead>(g->hero_id).value_or(true)) {
//                //add_message_history(g, "You died!");
//                g->gameover = true;
//            }
//            //check_and_handle_level_up(g, g->hero_id);
//        }
//        if (g->ct.get<dead>(g->hero_id).value_or(true)) {
//            return;
//        }
//
//        if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
//            g->ct.set<blocking>(g->hero_id, false);
//            g->ct.set<blocksuccess>(g->hero_id, false);
//            g->ct.set<damaged>(g->hero_id, false);
//        }
//    }
//}


void update_npcs_state(shared_ptr<gamestate> g) {
    for (entityid id = 0; id < g->next_entityid; id++) {
        if (id == g->hero_id) {
            continue;
        }
        //g_incr_tx_alpha(g, id, 4);
        unsigned char a = g->ct.get<txalpha>(id).value_or(255);
        if (a < 255) {
            a++;
        }
        g->ct.set<txalpha>(id, a);
        g->ct.set<damaged>(id, false);
    }
}


void liblogic_tick(shared_ptr<inputstate> is, shared_ptr<gamestate> g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    //minfo("Begin tick");
    // Spawn NPCs periodically
    //try_spawn_npc(g);
    // update ALL entities
    update_player_tiles_explored(g);

    update_player_state(g);
    update_npcs_state(g);
    handle_input(g, is);
    handle_npcs(g);
    update_debug_panel_buffer(g, is);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    //msuccess("End tick");
}


void liblogic_close(shared_ptr<gamestate> g) {
    massert(g, "liblogic_close: gamestate is NULL");
    d_free(g->dungeon);
}


//static void handle_attack_blocked(shared_ptr<gamestate> g,
//                                  entityid attacker_id,
//                                  entityid target_id,
//                                  bool* atk_successful) {
//    massert(g, "gamestate is NULL");
//    massert(attacker_id != ENTITYID_INVALID, "attacker entity id is invalid");
//    massert(target_id != ENTITYID_INVALID, "target entity id is invalid");
//    massert(atk_successful, "attack_successful is NULL");
//    *atk_successful = false;
//    g_set_damaged(g, target_id, false);
//    g_set_block_success(g, target_id, true);
//    g_set_update(g, target_id, true);
//    //entitytype_t tgttype = g_get_type(g, target_id);
//    //if (tgttype == ENTITY_PLAYER) {
//    //} else if (tgttype == ENTITY_NPC) {
//    add_message_history(g,
//                        "%s blocked %s's attack!",
//                        g_get_name(g, target_id).c_str(),
//                        g_get_name(g, attacker_id).c_str());
//    //}
//    //if (target->type == ENTITY_PLAYER) { add_message_and_history(g, "%s blocked %s's attack!", target->name, attacker->name); }
//}

//static bool handle_shield_check(shared_ptr<gamestate> g,
//                                entityid attacker_id,
//                                entityid target_id,
//                                int attack_roll,
//                                int base_ac,
//                                bool* attack_successful) {
//    // if you have a shield at all, the attack will get auto-blocked
//    entityid shield_id = g_get_equipment(g, target_id, EQUIP_SLOT_SHIELD);
//    if (shield_id != ENTITYID_INVALID) {
//        int shield_ac = g_get_ac(g, shield_id);
//        int total_ac = base_ac + shield_ac;
//        if (attack_roll < total_ac) {
//            *attack_successful = false;
//            handle_attack_blocked(g, attacker_id, target_id, attack_successful);
//            return false;
//        }
//    }
//    msuccess("Attack successful");
//    *attack_successful = true;
//    handle_attack_success(g, attacker_id, target_id, attack_successful);
//    return true;
//}


//static void try_entity_wait(shared_ptr<gamestate> g, entityid id) {
//    massert(g, "Game state is NULL!");
//    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
//    if (g_is_type(g, id, ENTITY_PLAYER)) g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
//    g_set_update(g, id, true);
//}

//static void
//execute_action(shared_ptr<gamestate> g, entityid id, entity_action_t action) {
//    massert(g, "gamestate is NULL");
//    massert(id != ENTITYID_INVALID, "entity id is invalid");
//    switch (action) {
//    case ENTITY_ACTION_MOVE_LEFT: try_entity_move(g, id, -1, 0); break;
//    case ENTITY_ACTION_MOVE_RIGHT: try_entity_move(g, id, 1, 0); break;
//    case ENTITY_ACTION_MOVE_UP: try_entity_move(g, id, 0, -1); break;
//    case ENTITY_ACTION_MOVE_DOWN: try_entity_move(g, id, 0, 1); break;
//    case ENTITY_ACTION_MOVE_UP_LEFT: try_entity_move(g, id, -1, -1); break;
//    case ENTITY_ACTION_MOVE_UP_RIGHT: try_entity_move(g, id, 1, -1); break;
//    case ENTITY_ACTION_MOVE_DOWN_LEFT: try_entity_move(g, id, -1, 1); break;
//    case ENTITY_ACTION_MOVE_DOWN_RIGHT: try_entity_move(g, id, 1, 1); break;
//    //case ENTITY_ACTION_ATTACK_LEFT: try_entity_attack(g, e->id, loc.x - 1, loc.y); break;
//    //case ENTITY_ACTION_ATTACK_RIGHT: try_entity_attack(g, e->id, loc.x + 1, loc.y); break;
//    //case ENTITY_ACTION_ATTACK_UP: try_entity_attack(g, e->id, loc.x, loc.y - 1); break;
//    //case ENTITY_ACTION_ATTACK_DOWN: try_entity_attack(g, e->id, loc.x, loc.y + 1); break;
//    //case ENTITY_ACTION_ATTACK_UP_LEFT: try_entity_attack(g, e->id, loc.x - 1, loc.y - 1); break;
//    //case ENTITY_ACTION_ATTACK_UP_RIGHT: try_entity_attack(g, e->id, loc.x + 1, loc.y - 1); break;
//    //case ENTITY_ACTION_ATTACK_DOWN_LEFT: try_entity_attack(g, e->id, loc.x - 1, loc.y + 1); break;
//    //case ENTITY_ACTION_ATTACK_DOWN_RIGHT: try_entity_attack(g, e->id, loc.x + 1, loc.y + 1); break;
//    case ENTITY_ACTION_MOVE_RANDOM: try_entity_move_random(g, id); break;
//    case ENTITY_ACTION_WAIT: try_entity_wait(g, id); break;
//    //case ENTITY_ACTION_ATTACK_RANDOM: try_entity_attack_random(g, e); break;
//    //case ENTITY_ACTION_MOVE_PLAYER:
//    //    try_entity_move_player(g, e);
//    //    break;
//    //case ENTITY_ACTION_ATTACK_PLAYER: try_entity_attack_player(g, e); break;
//    //case ENTITY_ACTION_MOVE_ATTACK_PLAYER: try_entity_move_attack_player(g, e); break;
//    case ENTITY_ACTION_MOVE_A_STAR:
//        try_entity_move_a_star(g, id);
//        break;
//        //case ENTITY_ACTION_INTERACT_DOWN_LEFT:
//        //case ENTITY_ACTION_INTERACT_DOWN_RIGHT:
//        //case ENTITY_ACTION_INTERACT_UP_LEFT:
//        //case ENTITY_ACTION_INTERACT_UP_RIGHT:
//        //case ENTITY_ACTION_INTERACT_LEFT:
//        //case ENTITY_ACTION_INTERACT_RIGHT:
//        //case ENTITY_ACTION_INTERACT_UP:
//        //case ENTITY_ACTION_INTERACT_DOWN:
//    case ENTITY_ACTION_NONE:
//        // do nothing
//        break;
//    default: merror("Unknown entity action: %d", action); break;
//    }
//}

//static vec3* get_locs_around_entity(shared_ptr<gamestate> g, entityid id) {
//    massert(g, "gamestate is NULL");
//    //vec3* locs = malloc(sizeof(vec3) * 8);
//    vec3* locs = (vec3*)malloc(sizeof(vec3) * 8);
//    massert(locs, "failed to allocate memory for locs");
//    int index = 0;
//    vec3 oldloc = g_get_location(g, id);
//    for (int i = -1; i <= 1; i++) {
//        for (int j = -1; j <= 1; j++) {
//            if (i == 0 && j == 0) continue;
//            locs[index] = (vec3){oldloc.x + i, oldloc.y + j, oldloc.z};
//            index++;
//        }
//    }
//    return locs;
//}

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

//static vec3 get_random_available_loc(shared_ptr<gamestate> g, int floor) {
//    massert(g, "gamestate is NULL");
//    massert(floor >= 0, "floor is out of bounds");
//    massert(floor < g->d->num_floors, "floor is out of bounds");
//
//    int w = g->dungeon->floors->at(floor)->width;
//    int h = g->dungeon->floors->at(floor)->height;
//
//    return get_random_available_loc_in_area(g, floor, 0, 0, w, h);
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


//static void try_spawn_npc(shared_ptr<gamestate> const g) {
//    massert(g, "gamestate is NULL");
//    static bool do_this_once = true;
//    int every_nth_turn = 2;
//    if (g->turn_count % every_nth_turn == 0) {
//        entityid success = ENTITYID_INVALID;
//        if (do_this_once) {
//            while (success == ENTITYID_INVALID) {
//                int current_floor = g->dungeon->current_floor;
//                int x = rand() % g->dungeon->floors->at(current_floor)->width + 1;
//                int y = rand() % g->dungeon->floors->at(current_floor)->height + 1;
//                vec3 loc = {x, y, current_floor};
//                shared_ptr<tile_t> tile = df_tile_at(g->dungeon->floors->at(current_floor), loc);
//                if (!tile || tile_is_walkable(tile->type) == false) {
//                    merror("Tile at %d, %d, %d is not walkable", x, y, current_floor);
//                    continue; // Tile is not walkable, try again
//                }
//                // if the tile contains entities, try again
//                if (tile_has_live_npcs(g, tile)) {
//                    merror("Tile is not empty");
//                    continue;
//                }
//                race_t race = get_random_race();
//                success = create_npc_set_stats(g, loc, race);
//            }
//            do_this_once = false;
//        }
//    } else {
//        do_this_once = true;
//    }
//}
