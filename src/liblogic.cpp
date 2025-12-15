#include "ComponentTable.h"
#include "ComponentTraits.h"
#include "add_message.h"
//#include "create_door.h"
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
#include "place_doors.h"
#include "update_debug_panel_buffer.h"
#include "update_player_state.h"
#include "update_player_tiles_explored.h"
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
    //create_wooden_box(g, (vec3){7, 7, 0});
    create_weapon_at_with(g, (vec3){11, 9, 0}, dagger_init_test);
    create_shield_at_with(g, (vec3){11, 10, 0}, kite_shield_init_test);
    place_doors(g);

    //#ifdef SPAWN_MONSTERS
    for (int i = 0; i < (int)g->dungeon->floors->size(); i++) {
        create_npc_at_with(g, RACE_ORC, (vec3){10, 10, i}, orc_init_test);
        create_npc_at_with(g, RACE_ORC, (vec3){10, 9, i}, orc_init_test);
        create_npc_at_with(g, RACE_ORC, (vec3){11, 9, i}, orc_init_test);
        create_npc_at_with(g, RACE_ORC, (vec3){13, 11, i}, orc_init_test);
        create_npc_at_with(g, RACE_ORC, (vec3){14, 11, i}, orc_init_test);
    }
    //#endif

    //create_potion(g, (vec3){1, 1, 0}, POTION_HP_SMALL);
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


void liblogic_tick(shared_ptr<inputstate> is, shared_ptr<gamestate> g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    // Spawn NPCs periodically
    //try_spawn_npc(g);
    update_player_tiles_explored(g);
    update_player_state(g);
    update_npcs_state(g);
    handle_input(g, is);
    handle_npcs(g);
    update_debug_panel_buffer(g, is);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}
