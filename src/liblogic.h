#pragma once

//#include "add_message.h"
//#include "create_box.h"
//#include "create_monster.h"
//#include "create_npc.h"
//#include "create_potion.h"
//#include "create_prop.h"
//#include "create_shield.h"
//#include "entity_templates.h"
#include "gamestate.h"
#include "handle_input.h"
#include "handle_npcs.h"
//#include "init_dungeon.h"
#include "inputstate.h"
//#include "place_doors.h"
//#include "place_props.h"
//#include "potion.h"
//#include "proptype.h"
#include "update_debug_panel_buffer.h"
#include "update_player_state.h"
#include "update_player_tiles_explored.h"
#include <memory>

#define TILE_COUNT_ERROR -999

using std::shared_ptr;

static inline void liblogic_close(gamestate& g) {
    d_destroy(g.dungeon);
}

static inline void update_npcs_state(gamestate& g) {
    for (entityid id = 0; id < g.next_entityid; id++) {
        if (id == g.hero_id || g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC)
            continue;
        unsigned char a = g.ct.get<txalpha>(id).value_or(255);
        if (a < 255)
            a++;
        g.ct.set<txalpha>(id, a);
        g.ct.set<damaged>(id, false);
    }
}


static inline void update_spells_state(gamestate& g) {
    for (entityid id = 0; id < g.next_entityid; id++) {
        if (id == g.hero_id || g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_SPELL)
            continue;
        unsigned char a = g.ct.get<txalpha>(id).value_or(255);
        if (a < 255)
            a++;
        g.ct.set<txalpha>(id, a);
        const bool is_complete = g.ct.get<spell_complete>(id).value_or(false);
        const bool is_destroyed = g.ct.get<destroyed>(id).value_or(false);
        if (is_complete && is_destroyed) {
            // remove it from the tile
            auto df = d_get_current_floor(g.dungeon);
            auto loc = g.ct.get<location>(id).value_or((vec3){-1, -1, -1});
            df_remove_at(df, id, loc.x, loc.y);
        }
    }
}


//static inline void liblogic_init(gamestate& g) {
//    srand(time(NULL));
//    SetRandomSeed(time(NULL));
//    minfo("liblogic_init");
//init_dungeon(g, 10);
//    g.init_dungeon(10);
// create doors
//    g.place_doors();
//place_props(g);
//    g.place_props();
//    create_box_at_with(g, (vec3){11, 9, 0}, [](gamestate& g, entityid id) {});
//    create_box_at_with(g, (vec3){10, 9, 0}, [](gamestate& g, entityid id) {});
//create_box_at_with(g, (vec3){9, 9, 0}, [](gamestate& g, entityid id) {});
//create_weapon_at_with(g, df_get_random_loc(d_get_floor(g.dungeon, 0)), dagger_init_test);


//create_weapon_at_with(g, df_get_random_loc(g.dungeon.floors[0]), dagger_init_test);
//    const entityid dagger_id = g.create_weapon_at_with(df_get_random_loc(g.dungeon.floors[0]));
//    g.ct.set<name>(dagger_id, "Dagger");
//    g.ct.set<description>(dagger_id, "Stabby stabby.");
//    g.ct.set<weapontype>(dagger_id, WEAPON_DAGGER);
//    g.ct.set<damage>(dagger_id, (vec3){1, 4, 0});
//    g.ct.set<durability>(dagger_id, 100);
//    g.ct.set<max_durability>(dagger_id, 100);
//    g.ct.set<rarity>(dagger_id, RARITY_COMMON);


//create_shield_at_with(g, df_get_random_loc(g.dungeon.floors[0]), kite_shield_init_test);
//    const entityid shield_id = g.create_shield_at_with(df_get_random_loc(g.dungeon.floors[0]));
//    g.ct.set<name>(shield_id, "Kite Shield");
//    g.ct.set<description>(shield_id, "Standard knight's shield");
//    g.ct.set<shieldtype>(shield_id, SHIELD_KITE);
//    g.ct.set<block_chance>(shield_id, 90);


//create_potion_at_with(g, df_get_random_loc(g.dungeon.floors[0]), [](gamestate& g, entityid id) {
//    g.ct.set<name>(id, "small healing potion");
//    g.ct.set<description>(id, "a small healing potion");
//    g.ct.set<potiontype>(id, POTION_HP_SMALL);
//    g.ct.set<healing>(id, (vec3){1, 6, 0});
//});
//    const entityid potion_id = g.create_potion_at_with(df_get_random_loc(g.dungeon.floors[0]));
//    g.ct.set<name>(potion_id, "small healing potion");
//    g.ct.set<description>(potion_id, "a small healing potion");
//    g.ct.set<potiontype>(potion_id, POTION_HP_SMALL);
//    g.ct.set<healing>(potion_id, (vec3){1, 6, 0});

//#ifdef SPAWN_MONSTERS
//for (int i = 0; i < (int)g.dungeon.floors.size(); i++) {
//for (int j = 1; j <= i + 1; j++) {
//const vec3 random_loc = df_get_random_loc(d_get_floor(g.dungeon, i));
//auto init_function = [](gamestate& g, entityid id) {};
//create_random_monster_at_with(g, random_loc, init_function);
//}
//}

//    for (int i = 0; i < (int)g.dungeon.floors.size(); i++) {
//        for (int j = 1; j <= i + 1; j++) {
//            const vec3 random_loc = df_get_random_loc(d_get_floor(g.dungeon, i));
//            g.create_random_monster_at_with(random_loc);
//        }
//    }

//#endif
//create_potion(g, (vec3){3, 1, 0}, POTION_HP_MEDIUM);
//create_potion(g, (vec3){5, 1, 0}, POTION_HP_LARGE);
//    g.add_message("Welcome to the game! Press enter to cycle messages.");
//#ifdef START_MESSAGES
//    g.add_message("To move around, press [q w e a d z x c]");
//    g.add_message("To pick up items, press / ");
//    g.add_message("To manage inventory, press i ");
//    g.add_message("To equip/unequip an item, highlight and press e ");
//    g.add_message("To drop an item, highlight and press q ");
//    g.add_message("To attack, press ' ");
//    g.add_message("To go up/down a floor, press . ");
//    g.add_message("To wait a turn, press s s ");
//    g.add_message("To change direction, press s and then [q w e a d z x c] ");
//    g.add_message("To open a door, face it and press o ");
//#endif
//    msuccess("liblogic_init: Game state initialized");
//}

static inline void liblogic_tick(gamestate& g, inputstate& is) {
    // Spawn NPCs periodically
    //try_spawn_npc(g);
    update_player_tiles_explored(g);
    update_player_state(g);
    update_npcs_state(g);
    update_spells_state(g);
    handle_input(g, is);
    handle_npcs(g);
    update_debug_panel_buffer(g, is);
    g.currenttime = time(NULL);
    g.currenttimetm = localtime(&g.currenttime);
    strftime(g.currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g.currenttimetm);
}
