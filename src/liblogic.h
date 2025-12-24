#pragma once

#include "add_message.h"
#include "create_box.h"
#include "create_monster.h"
#include "create_npc.h"
#include "create_potion.h"
#include "create_shield.h"
#include "entity_templates.h"
#include "gamestate.h"
#include "handle_input.h"
#include "handle_npcs.h"
#include "init_dungeon.h"
#include "inputstate.h"
#include "place_doors.h"
#include "potion.h"
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


static inline void liblogic_init(gamestate& g) {
    srand(time(NULL));
    SetRandomSeed(time(NULL));
    minfo("liblogic_init");
    init_dungeon(g);

    place_doors(g);

    create_box_at_with(g, (vec3){11, 9, 0}, [](gamestate& g, entityid id) {});
    create_box_at_with(g, (vec3){10, 9, 0}, [](gamestate& g, entityid id) {});
    create_box_at_with(g, (vec3){9, 9, 0}, [](gamestate& g, entityid id) {});
    //create_weapon_at_with(g, df_get_random_loc(d_get_floor(g.dungeon, 0)), dagger_init_test);
    create_weapon_at_with(g, df_get_random_loc(g.dungeon.floors[0]), dagger_init_test);
    create_shield_at_with(g, df_get_random_loc(g.dungeon.floors[0]), kite_shield_init_test);
    create_potion_at_with(g, df_get_random_loc(g.dungeon.floors[0]), [](gamestate& g, entityid id) {
        // set healing
        g.ct.set<name>(id, "small healing potion");
        g.ct.set<description>(id, "a small healing potion");
        g.ct.set<potiontype>(id, POTION_HP_SMALL);
        g.ct.set<healing>(id, (vec3){1, 6, 0});
    });
    //#ifdef SPAWN_MONSTERS
    for (int i = 0; i < (int)g.dungeon.floors.size(); i++) {
        for (int j = 1; j <= i + 1; j++) {
            //create_npc_at_with(g, RACE_ORC, df_get_random_loc(d_get_floor(g.dungeon, i)), orc_init_test);

            //const race_t r = RACE_SKELETON;
            //create_npc_at_with(
            //    g,
            //    r,
            //    df_get_random_loc(d_get_floor(g.dungeon, i)),
            //    [](gamestate& g, entityid id) { g.ct.set<name>(id, race2str(r)); });

            const vec3 random_loc = df_get_random_loc(d_get_floor(g.dungeon, i));
            auto init_function = [](gamestate& g, entityid id) {};
            create_random_monster_at_with(g, random_loc, init_function);
        }
    }

    //create_npc_at_with(g, RACE_ORC, df_get_random_loc(g->dungeon->floors->at(i)), orc_init_test);
    //create_npc_at_with(g, RACE_ORC, (vec3){10, 10, i}, orc_init_test);
    //create_npc_at_with(g, RACE_ORC, (vec3){10, 9, i}, orc_init_test);
    //create_npc_at_with(g, RACE_ORC, (vec3){11, 9, i}, orc_init_test);
    //create_npc_at_with(g, RACE_ORC, (vec3){13, 11, i}, orc_init_test);
    //create_npc_at_with(g, RACE_ORC, (vec3){14, 11, i}, orc_init_test);
    //}
    //#endif
    //create_potion(g, (vec3){3, 1, 0}, POTION_HP_MEDIUM);
    //create_potion(g, (vec3){5, 1, 0}, POTION_HP_LARGE);
    add_message(g, "Welcome to the game! Press enter to cycle messages.");
#ifdef START_MESSAGES
    add_message(g, "To move around, press [q w e a d z x c]");
    add_message(g, "To pick up items, press / ");
    add_message(g, "To manage inventory, press i ");
    add_message(g, "To equip/unequip an item, highlight and press e ");
    add_message(g, "To drop an item, highlight and press q ");
    add_message(g, "To attack, press ' ");
    add_message(g, "To go up/down a floor, press . ");
    add_message(g, "To wait a turn, press s s ");
    add_message(g, "To change direction, press s and then [q w e a d z x c] ");
    add_message(g, "To open a door, face it and press o ");
#endif
    msuccess("liblogic_init: Game state initialized");
}

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
