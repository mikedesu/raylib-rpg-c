#include "ComponentTable.h"
#include "ComponentTraits.h"
#include "add_message.h"
#include "create_door.h"
#include "create_shield.h"
#include "create_weapon.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "entity_templates.h"
#include "entityid.h"
#include "gamestate.h"
#include "handle_input.h"
#include "handle_npcs.h"
#include "init_dungeon.h"
#include "inputstate.h"
#include "liblogic.h"
#include "massert.h"
#include "update_debug_panel_buffer.h"
#include "update_player_state.h"
#include "update_player_tiles_explored.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <raylib.h>
//#include "create_npc.h"


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

    create_weapon_at_with(g, (vec3){11, 9, 0}, dagger_init_test);
    //create_shield_at_with(g, (vec3){11, 9, 0}, buckler_init_test);
    create_shield_at_with(g, (vec3){11, 10, 0}, tower_shield_init_test);
    //create_shield_at_with(g, (vec3){10, 9, 0}, kite_shield_init_test);


    //auto loc = (vec3){12, 9, 0};
    //auto tile = df_tile_at(d_get_floor(g->dungeon, 0), loc);
    //if (tile) {
    //    create_door_at_with(g, loc, [](shared_ptr<gamestate> g, entityid id) {});
    //}


    //create_door_at_with(g, (vec3){14, 12, 0}, [](shared_ptr<gamestate> g, entityid id) {});
    //create_door_at_with(g, (vec3){12, 9, 1}, [](shared_ptr<gamestate> g, entityid id) {});

    //minfo("BEGIN CREATING DOORS");
    for (int z = 0; z < (int)g->dungeon->floors->size(); z++) {
        auto df = d_get_floor(g->dungeon, z);
        for (int x = 0; x < df->width; x++) {
            for (int y = 0; y < df->height; y++) {
                const vec3 loc = {x, y, z};
                auto tile = df_tile_at(df, loc);
                if (!tile || !tile->can_have_door)
                    continue;
                create_door_at_with(g, loc, [](shared_ptr<gamestate> g, entityid id) {});
            }
        }
    }
    //minfo("END CREATING DOORS");


    //create_door_at(g, (vec3){14, 12, 0});
    //create_door_at(g, (vec3){12, 9, 1});

    //#ifdef SPAWN_MONSTERS

    create_npc_at_with(g, RACE_ORC, (vec3){10, 10, 0}, orc_init_test);
    create_npc_at_with(g, RACE_ORC, (vec3){14, 9, 0}, orc_init_test);
    create_npc_at_with(g, RACE_ORC, (vec3){14, 10, 0}, orc_init_test);

    //#endif


    //create_potion(g, (vec3){1, 1, 0}, POTION_HP_SMALL);
    //create_potion(g, (vec3){3, 1, 0}, POTION_HP_MEDIUM);
    //create_potion(g, (vec3){5, 1, 0}, POTION_HP_LARGE);
    //create_weapon(g, (vec3){1, 5, 0}, WEAPON_TWO_HANDED_SWORD);
    //create_weapon(g, (vec3){1, 6, 0}, WEAPON_BOW);
    //create_weapon(g, (vec3){1, 6, 0}, WEAPON_WARHAMMER);
    //create_weapon(g, (vec3){1, 5, 0}, WEAPON_FLAIL);

    add_message(g, "Welcome to the game! Press enter to cycle messages.");
    //add_message_history(g, "To move around, press [q w e a d z x c]");
    //add_message_history(g, "To pick up items, press / ");
    //add_message_history(g, "To manage inventory, press i ");
    //add_message_history(g, "To equip/unequip an item, highlight and press e ");
    //add_message_history(g, "To drop an item, highlight and press q ");
    //add_message_history(g, "To attack, press ' ");

    msuccess("liblogic_init: Game state initialized");
}


void update_npcs_state(shared_ptr<gamestate> g) {
    for (entityid id = 0; id < g->next_entityid; id++) {
        if (id == g->hero_id)
            continue;
        //g_incr_tx_alpha(g, id, 4);
        unsigned char a = g->ct.get<txalpha>(id).value_or(255);
        if (a < 255)
            a++;
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
    //minfo("Update player tiles explored");
    update_player_tiles_explored(g);

    //minfo("Update player state");
    update_player_state(g);
    //minfo("Update NPCs state");
    update_npcs_state(g);
    //minfo("Handle input");
    handle_input(g, is);
    //minfo("Handle NPCs");
    handle_npcs(g);
    //minfo("Update debug panel buffer");
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
