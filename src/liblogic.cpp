#include "ComponentTable.h"
#include "ComponentTraits.h"
#include "controlmode.h"
#include "direction.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "inputstate.h"
#include "item.h"
#include "libgame_defines.h"
#include "liblogic.h"
#include "liblogic_add_message.h"
#include "liblogic_change_player_dir.h"
#include "liblogic_create_npc_set_stats.h"
#include "liblogic_create_player.h"
#include "liblogic_create_weapon.h"
#include "liblogic_drop_item.h"
#include "liblogic_handle_camera_move.h"
#include "liblogic_handle_npc.h"
#include "liblogic_init_dungeon.h"
#include "liblogic_try_entity_move.h"
#include "liblogic_try_entity_pickup.h"
#include "massert.h"
#include "roll.h"
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

// these have been moved to external source files
void handle_input_inventory(shared_ptr<inputstate> is, shared_ptr<gamestate> g);
void handle_attack_success(shared_ptr<gamestate> g, entityid atk_id, entityid tgt_id, bool* atk_successful);
bool handle_attack_helper_innerloop(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, int i, entityid attacker_id, bool* attack_successful);
void handle_attack_success_gamestate_flag(shared_ptr<gamestate> g, entitytype_t type, bool success);
void cycle_messages(shared_ptr<gamestate> g);
void handle_input_title_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is);
void handle_input_character_creation_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is);
void handle_input_main_menu_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is);
void update_debug_panel_buffer(shared_ptr<gamestate> g, shared_ptr<inputstate> is);
void handle_input_help_menu(shared_ptr<gamestate> g, shared_ptr<inputstate> is);
void try_entity_attack(shared_ptr<gamestate> g, entityid attacker_id, int target_x, int target_y);


void handle_input_title_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        minfo("Title screen input detected, switching to main menu");
        g->current_scene = SCENE_MAIN_MENU;
        g->frame_dirty = true;
    }
}


void handle_input_main_menu_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
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

    g->frame_dirty = true;
}


void handle_input_character_creation_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
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
        g->entity_turn = create_player(g, (vec3){0, 0, 0}, "darkmage");
        //g_set_stat(g, g->hero_id, STATS_MAXHP, maxhp_roll);
        //g_set_stat(g, g->hero_id, STATS_HP, maxhp_roll);
        g->current_scene = SCENE_GAMEPLAY;
    } else if (inputstate_is_pressed(is, KEY_SPACE)) {
        // re-roll character creation stats
        minfo("Re-rolling character creation stats");
        g->chara_creation->strength = do_roll_best_of_3((vec3){3, 6, 0});
        g->chara_creation->dexterity = do_roll_best_of_3((vec3){3, 6, 0});
        g->chara_creation->constitution = do_roll_best_of_3((vec3){3, 6, 0});
    }
    //else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
    //    minfo("Exiting character creation");
    //    g->current_scene = SCENE_TITLE;
    //}
    else if (inputstate_is_pressed(is, KEY_LEFT)) {
        int race = g->chara_creation->race;
        if (race > 1) {
            race--;
        } else {
            race = RACE_WARG;
        }
        g->chara_creation->race = (race_t)race;
        //g->chara_creation->race = (race_t)(((int)g->chara_creation->race)-1);
        //if (race == RACE_HUMAN) {
        //    g->chara_creation->race = RACE_ORC;
        //} else if (race == RACE_ORC) {
        //    g->chara_creation->race = RACE_HUMAN;
        //}
    } else if (inputstate_is_pressed(is, KEY_RIGHT)) {
        int race = g->chara_creation->race;
        if (race < RACE_COUNT - 1) {
            race++;
        } else {
            race = RACE_HALFLING;
        }
        g->chara_creation->race = (race_t)race;
        //race_t race = g->chara_creation->race;
        //if (race == RACE_HUMAN) {
        //    g->chara_creation->race = RACE_ORC;
        //} else if (race == RACE_ORC) {
        //    g->chara_creation->race = RACE_HUMAN;
        //}
    }
    g->frame_dirty = true;
}


static void handle_input_gameplay_controlmode_player(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
        return;
    }


    if (g->display_help_menu) {
        handle_input_help_menu(g, is);
    }

    if (g->msg_system_is_active) {
        // press enter to cycle thru message
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            cycle_messages(g);
        }
        return;
    }

    if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
        if (inputstate_is_pressed(is, KEY_LEFT_BRACKET)) {
            g->cam2d.zoom += DEFAULT_ZOOM_INCR;
            g->frame_dirty = true;
            //return;
        } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
            g->cam2d.zoom -= (g->cam2d.zoom > 1.0) ? DEFAULT_ZOOM_INCR : 0.0;
            g->frame_dirty = true;
        }

        if (g->player_changing_dir) {
            // double 's' is wait one turn
            if (inputstate_is_pressed(is, KEY_S)) {
                g->player_changing_dir = false;
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
                change_player_dir(g, DIR_UP);
            } else if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
                change_player_dir(g, DIR_DOWN);
            } else if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
                change_player_dir(g, DIR_LEFT);
            } else if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
                change_player_dir(g, DIR_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_Q)) {
                change_player_dir(g, DIR_UP_LEFT);
            } else if (inputstate_is_pressed(is, KEY_E)) {
                change_player_dir(g, DIR_UP_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_Z)) {
                change_player_dir(g, DIR_DOWN_LEFT);
            } else if (inputstate_is_pressed(is, KEY_C)) {
                change_player_dir(g, DIR_DOWN_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
                //            g_set_attacking(g, g->hero_id, true);
                g->ct.set<attacking>(g->hero_id, true);
                //            g_set_update(g, g->hero_id, true);
                g->ct.set<update>(g->hero_id, true);
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                g->player_changing_dir = false;
            }
            return;
        }

        if (inputstate_is_pressed(is, KEY_S)) {
            g->player_changing_dir = true;
            return;
        }


        if (g->hero_id != ENTITYID_INVALID) {
            //vec3 loc = g_get_loc(g, g->hero_id);
            if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
                try_entity_move(g, g->hero_id, (vec3){0, -1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
                try_entity_move(g, g->hero_id, (vec3){0, 1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
                try_entity_move(g, g->hero_id, (vec3){-1, 0, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
                try_entity_move(g, g->hero_id, (vec3){1, 0, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_Q)) {
                try_entity_move(g, g->hero_id, (vec3){-1, -1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_E)) {
                try_entity_move(g, g->hero_id, (vec3){1, -1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_Z)) {
                try_entity_move(g, g->hero_id, (vec3){-1, 1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_C)) {
                try_entity_move(g, g->hero_id, (vec3){1, 1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
                //g->ct.set<attacking>(g->hero_id, true);
                //g->ct.set<update>(g->hero_id, true);


                if (g->ct.get<location>(g->hero_id).has_value() && g->ct.get<direction>(g->hero_id).has_value()) {
                    vec3 loc = g->ct.get<location>(g->hero_id).value();
                    direction_t dir = g->ct.get<direction>(g->hero_id).value();
                    if (dir == DIR_UP) {
                        loc.y -= 1;
                    } else if (dir == DIR_DOWN) {
                        loc.y += 1;
                    } else if (dir == DIR_LEFT) {
                        loc.x -= 1;
                    } else if (dir == DIR_RIGHT) {
                        loc.x += 1;
                    } else if (dir == DIR_UP_LEFT) {
                        loc.x -= 1;
                        loc.y -= 1;
                    } else if (dir == DIR_UP_RIGHT) {
                        loc.x += 1;
                        loc.y -= 1;
                    } else if (dir == DIR_DOWN_LEFT) {
                        loc.x -= 1;
                        loc.y += 1;
                    } else if (dir == DIR_DOWN_RIGHT) {
                        loc.x += 1;
                        loc.y += 1;
                    }

                    try_entity_attack(g, g->hero_id, loc.x, loc.y);

                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                }

                //} else if (inputstate_is_pressed(is, KEY_SEMICOLON)) {
            } else if (inputstate_is_pressed(is, KEY_SLASH)) {
                //add_message(g, "pickup item (unimplemented)");
                try_entity_pickup(g, g->hero_id);
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
            //else if (inputstate_is_pressed(is, KEY_SLASH) && inputstate_is_shift_held(is)) {
            //    // open inventory
            //    //g->inventory_is_open = !g->inventory_is_open;
            //    g->display_help_menu = !g->display_help_menu;
            //    g->frame_dirty = true;
            //}
            else if (inputstate_is_pressed(is, KEY_I)) {
                g->display_inventory_menu = true;
                g->controlmode = CONTROLMODE_INVENTORY;
                g->frame_dirty = true;
            }
            return;
        }
    }
}


static void handle_input_gameplay_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(g, "Game state is NULL!");
    massert(is, "Input state is NULL!");

    if (inputstate_is_pressed(is, KEY_B)) {
        if (g->controlmode == CONTROLMODE_PLAYER) {
            g->controlmode = CONTROLMODE_CAMERA;
        } else if (g->controlmode == CONTROLMODE_CAMERA) {
            g->controlmode = CONTROLMODE_PLAYER;
        }
        g->frame_dirty = true;
        //    //return;
    }

    if (g->controlmode == CONTROLMODE_CAMERA) {
        handle_camera_move(g, is);
        g->frame_dirty = true;
        //return;
    }

    if (g->controlmode == CONTROLMODE_PLAYER) {
        handle_input_gameplay_controlmode_player(g, is);
        return;
    }

    if (g->controlmode == CONTROLMODE_INVENTORY) {
        handle_input_inventory(is, g);
        return;
    }
}


static void handle_input(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(is, "inputstate is NULL");
    massert(g, "gamestate is NULL");
    // no matter which mode we are in, we can toggle the debug panel
    if (inputstate_is_pressed(is, KEY_P)) {
        g->debugpanelon = !g->debugpanelon;
        minfo("Toggling debug panel: %s", g->debugpanelon ? "ON" : "OFF");
        return;
    }

    if (g->current_scene == SCENE_TITLE) {
        handle_input_title_scene(g, is);
    } else if (g->current_scene == SCENE_MAIN_MENU) {
        handle_input_main_menu_scene(g, is);
    } else if (g->current_scene == SCENE_CHARACTER_CREATION) {
        handle_input_character_creation_scene(g, is);
    } else if (g->current_scene == SCENE_GAMEPLAY) {
        handle_input_gameplay_scene(g, is);
    }
}


void update_debug_panel_buffer(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(g, "gamestate is NULL");
    // Static buffers to avoid reallocating every frame
    static const char* control_modes[] = {"Camera", "Player", "Unknown"};
    // Get hero position once
    //int x;
    //int y;
    //int z;
    int inventory_count;
    //direction_t player_dir;
    //shield_dir;
    //bool is_b, test_guard;
    vec3 loc;
    //x = -1;
    //y = -1;
    //z = -1;
    inventory_count = -1;
    //entityid shield_id = -1;
    //player_dir = DIR_NONE;
    //shield_dir = DIR_NONE;
    //is_b = false;
    //test_guard = g->test_guard;

    if (g->hero_id != ENTITYID_INVALID) {
        loc = g->ct.get<location>(g->hero_id).value();
    }
    // Determine control mode and flag strings
    const char* control_mode = control_modes[(g->controlmode >= 0 && g->controlmode < 2) ? g->controlmode : 2];
    // zero out the buffer
    memset(g->debugpanel.buffer, 0, sizeof(g->debugpanel.buffer));
    // Format the string in one pass
    snprintf(g->debugpanel.buffer,
             sizeof(g->debugpanel.buffer),
             "@evildojo666\n"
             "project.rpg\n"
             //"%s\n" // timebeganbuf
             //"%s\n" // currenttimebuf
             "Frame : %d\n"
             "Update: %d\n"
             "Mouse: %.01f, %.01f\n"
             "Last Clicked: %.01f, %.01f\n"
             "Frame Dirty: %d\n"
             "Draw Time: %.1fms\n"
             "Is3D: %d\n"
             "Cam: (%.0f,%.0f) Zoom: %.1f\n"
             "Mode: %s \n"
             "Floor: %d/%d \n"
             "Entities: %d\n"
             "Flag: %d\n"
             "Turn: %d\n"
             "Hero: (%d,%d,%d)\n"
             "Weapon: %d\n"
             "Inventory: %d\n",
             //g->timebeganbuf,
             //g->currenttimebuf,
             g->framecount,
             g->frame_updates,
             is->mouse_position.x,
             is->mouse_position.y,
             g->last_click_screen_pos.x,
             g->last_click_screen_pos.y,
             g->frame_dirty,
             g->last_frame_time * 1000,
             g->is3d,
             g->cam2d.offset.x,
             g->cam2d.offset.y,
             g->cam2d.zoom,
             control_mode,
             0,
             0,
             g->next_entityid,
             g->flag,
             g->entity_turn,
             loc.x,
             loc.y,
             loc.z,
             g->ct.get<equipped_weapon>(g->hero_id).value_or(ENTITYID_INVALID),
             inventory_count);
}


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

    //create_npc_set_stats(g, (vec3){5, 3, 0}, RACE_HUMAN);
    //create_npc_set_stats(g, (vec3){5, 4, 0}, RACE_ELF);
    //create_npc_set_stats(g, (vec3){5, 5, 0}, RACE_DWARF);
    //create_npc_set_stats(g, (vec3){5, 6, 0}, RACE_GOBLIN);
    //create_npc_set_stats(g, (vec3){5, 7, 0}, RACE_HALFLING);
    //create_npc_set_stats(g, (vec3){6, 3, 0}, RACE_GREEN_SLIME);

    int start_x = 5;

    int x = start_x;
    const int max_x = 16;
    int y = 5;
    const int max_y = 16;
    const int num = 25;
    for (int i = 0; i < num; i++) {
        create_npc_set_stats(g, (vec3){x, y, 0}, RACE_ORC);
        x++;
        if (x >= max_x) {
            x = start_x;
            y++;
        }
        if (y >= max_y) {
            break;
        }
    }

    //create_npc_set_stats(g, (vec3){6, 5, 0}, RACE_WOLF);
    //create_npc_set_stats(g, (vec3){6, 6, 0}, RACE_WARG);
    //create_npc_set_stats(g, (vec3){6, 7, 0}, RACE_BAT);

    //create_potion(g, (vec3){1, 1, 0}, POTION_HP_SMALL);
    //create_potion(g, (vec3){2, 1, 0}, POTION_MP_SMALL);
    //create_potion(g, (vec3){3, 1, 0}, POTION_HP_MEDIUM);
    //create_potion(g, (vec3){4, 1, 0}, POTION_MP_MEDIUM);
    //create_potion(g, (vec3){5, 1, 0}, POTION_HP_LARGE);
    //create_potion(g, (vec3){6, 1, 0}, POTION_MP_LARGE);

    //create_weapon(g, (vec3){1, 5, 0}, WEAPON_TWO_HANDED_SWORD);
    //create_weapon(g, (vec3){1, 6, 0}, WEAPON_BOW);
    //create_weapon(g, (vec3){1, 6, 0}, WEAPON_WARHAMMER);

    create_weapon(g, (vec3){1, 3, 0}, WEAPON_DAGGER);
    create_weapon(g, (vec3){1, 2, 0}, WEAPON_SWORD);
    create_weapon(g, (vec3){1, 4, 0}, WEAPON_AXE);

    //create_weapon(g, get_random_loc((rect){0, 0, 8, 8}, 0), WEAPON_AXE);
    //create_weapon(g, (vec3){1, 5, 0}, WEAPON_FLAIL);
    //create_weapon(g, (vec3){2, 3, 0}, WEAPON_DAGGER);
    //create_weapon(g, (vec3){2, 2, 0}, WEAPON_SWORD);
    //create_weapon(g, (vec3){2, 4, 0}, WEAPON_AXE);
    //create_weapon(g, (vec3){2, 5, 0}, WEAPON_FLAIL);

    add_message(g, "Welcome to the game! Press enter to cycle messages.");
    add_message(g, "To move around, press q w e a d z x c");
    add_message(g, "This is a test message to demonstrate the resizing of the message history box");
    msuccess("liblogic_init: Game state initialized");
}


void liblogic_tick(shared_ptr<inputstate> is, shared_ptr<gamestate> g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    minfo2("Begin tick");
    // Spawn NPCs periodically
    //try_spawn_npc(g);
    // update ALL entities
    // this was update player state
    if (g->hero_id != ENTITYID_INVALID) {
        if (!g->gameover) {
            unsigned char a = g->ct.get<txalpha>(g->hero_id).value_or(255);
            if (a < 255) {
                a++;
            }
            g->ct.set<txalpha>(g->hero_id, a);
            //if (g_is_dead(g, g->hero_id)) {
            if (g->ct.get<dead>(g->hero_id).value_or(true)) {
                //add_message_history(g, "You died!");
                g->gameover = true;
            }
            //check_and_handle_level_up(g, g->hero_id);
        }
        //if (g_is_dead(g, g->hero_id)) {
        if (g->ct.get<dead>(g->hero_id).value_or(true)) {
            return;
        }
    }
    // this was update_npcs_state
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
    }
    if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
        if (g->hero_id != ENTITYID_INVALID) {
            g->ct.set<blocking>(g->hero_id, false);
            //        g_set_block_success(g, g->hero_id, false);
            g->ct.set<blocksuccess>(g->hero_id, false);
        }
    }
    handle_input(g, is);
    if (g->flag == GAMESTATE_FLAG_NPC_TURN) {
        for (entityid id = 0; id < g->next_entityid; id++) {
            handle_npc(g, id);
        }
        // After processing all NPCs, set the flag to animate all movements together
        g->flag = GAMESTATE_FLAG_NPC_ANIM;
    }
    update_debug_panel_buffer(g, is);
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&g->currenttime);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    msuccess2("End tick");
}


void liblogic_close(shared_ptr<gamestate> g) {
    massert(g, "liblogic_close: gamestate is NULL");
    d_free(g->dungeon);
}


//static inline void reset_player_blocking(shared_ptr<gamestate> g) {
//    massert(g, "gamestate is NULL");
//    g_set_blocking(g, g->hero_id, false);
//g_set_block_success(g, g->hero_id, false);
//g_set_update(g, g->hero_id, true);
//}


//static void handle_attack_blocked(shared_ptr<gamestate> g,
//                                  entityid attacker_id,
//                                  entityid target_id,
//                                  bool* atk_successful);
//static void handle_input_help_menu(const inputstate* const is, gamestate* const g);
//static void handle_level_up(shared_ptr<gamestate> g, entityid id);
//static void handle_input_sort_inventory(const inputstate* const is, gamestate* const g);
//static void handle_input_sort_inventory(shared_ptr<inputstate> is,
//                                        shared_ptr<gamestate> g);
//static void init_dungeon(shared_ptr<gamestate> g);


//static void
//add_message_and_history(shared_ptr<gamestate> g, const char* fmt, ...);
//static void add_message(gamestate* g, const char* fmt, ...);
//static void add_message(shared_ptr<gamestate> g, const char* fmt, ...);
//static void try_entity_move_a_star(shared_ptr<gamestate> g, entityid id);


void cycle_messages(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (g->msg_system->size() > 0) {
        string msg = g->msg_system->front();
        int len = msg.length();
        // measure the length of the message as calculated by MeasureText

        if (len > g->msg_history_max_len_msg) {
            g->msg_history_max_len_msg = len;
            int font_size = 10;
            int measure = MeasureText(msg.c_str(), font_size);
            //if (measure > g->msg_history_max_len_msg_measure) {
            g->msg_history_max_len_msg_measure = measure;
        }

        g->msg_history->push_back(g->msg_system->front());
        g->msg_system->erase(g->msg_system->begin());
    }
    if (g->msg_system->size() == 0) {
        g->msg_system_is_active = false;
    }
}


void handle_attack_success_gamestate_flag(shared_ptr<gamestate> g, entitytype_t type, bool success) {
    //g->flag = success && type == ENTITY_PLAYER    ? GAMESTATE_FLAG_PLAYER_ANIM
    //          : !success && type == ENTITY_PLAYER ? GAMESTATE_FLAG_PLAYER_ANIM
    //          : !success && type == ENTITY_NPC    ? GAMESTATE_FLAG_NPC_ANIM
    //          : !success                          ? GAMESTATE_FLAG_NONE
    //                                              : g->flag;
    if (success) {
        if (type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
    } else {
        if (type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (type == ENTITY_NPC) {
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        } else {
            // this should NEVER happen!
            g->flag = GAMESTATE_FLAG_NONE;
        }
    }
}

void handle_attack_success(shared_ptr<gamestate> g, entityid atk_id, entityid tgt_id, bool* atk_successful) {
    massert(g, "gamestate is NULL");
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
    massert(atk_successful, "attack_successful is NULL");
    entitytype_t tgttype = g->ct.get<entitytype>(tgt_id).value_or(ENTITY_NONE);
    if (*atk_successful) {
        msuccess("Attack was SUCCESSFUL!");
        //entityid attacker_weapon_id = g_get_equipment(g, atk_id, EQUIP_SLOT_WEAPON);
        //entityid attacker_weapon_id = g_get_equipped_weapon(g, atk_id);
        int dmg = 1;
        //if (attacker_weapon_id == ENTITYID_INVALID) {
        // no weapon
        // get the entity's base attack damage
        //vec3 dmg_roll = g_get_base_attack_damage(g, atk_id);
        //dmg = do_roll(dmg_roll);
        //int atk_bonus = g_get_stat(g, atk_id, STATS_ATTACK_BONUS);
        //dmg += atk_bonus;
        //} else {
        // weapon
        // we will calculate damage based on weapon attributes
        //massert(g_has_damage(g, attacker_weapon_id),
        //        "attacker weapon does not have damage attached");
        //vec3 dmg_roll = g_get_damage(g, attacker_weapon_id);
        //dmg = do_roll(dmg_roll);
        //int atk_bonus = g_get_stat(g, atk_id, STATS_ATTACK_BONUS);
        //dmg += atk_bonus;
        //}
        //    g_set_damaged(g, tgt_id, true);
        g->ct.set<damaged>(tgt_id, true);
        //    g_set_update(g, tgt_id, true);
        g->ct.set<update>(tgt_id, true);
        //int hp = g_get_stat(g, tgt_id, STATS_HP);
        int hp = 1;
        if (hp <= 0) {
            merror("Target is already dead, hp was: %d", hp);
            //g_update_dead(g, tgt_id, true);
            g->ct.set<dead>(tgt_id, true);
            return;
        }
        hp -= dmg;
        //g_set_stat(g, tgt_id, STATS_HP, hp);
        if (tgttype == ENTITY_PLAYER) {
            //add_message_history(g,
            //                    "%s attacked you for %d damage!",
            //                    g_get_name(g, atk_id).c_str(),
            //                    dmg);
        } else if (tgttype == ENTITY_NPC) {
            //add_message_history(g,
            //                    "%s attacked %s for %d damage!",
            //                    g_get_name(g, atk_id).c_str(),
            //                    g_get_name(g, tgt_id).c_str(),
            //                    dmg);
        }
        if (hp <= 0) {
            //g_update_dead(g, tgt_id, true);
            g->ct.set<dead>(tgt_id, true);
            if (tgttype == ENTITY_NPC) {
                //add_message_history(g,
                //                    "%s killed %s!",
                //                    g_get_name(g, atk_id).c_str(),
                //                    g_get_name(g, tgt_id).c_str());
                // increment attacker's xp
                //int old_xp = g_get_stat(g, atk_id, STATS_XP);
                //massert(old_xp >= 0, "attacker's xp is negative");
                //int reward_xp = calc_reward_xp(g, atk_id, tgt_id);
                //int reward_xp = 1;
                //massert(reward_xp >= 0, "reward xp is negative");
                //int new_xp = old_xp + reward_xp;
                //massert(new_xp >= 0, "new xp is negative");
                //g_set_stat(g, atk_id, STATS_XP, new_xp);
                //vec3 loc = g_get_loc(g, tgt_id);
                //vec3 loc_cast = {loc.x, loc.y, loc.z};
                //entityid id = ENTITYID_INVALID;
                //while (id == ENTITYID_INVALID)
                //    id = potion_create(g,
                //                       loc,
                //                       POTION_HEALTH_SMALL,
                //                       "small health potion");
            }
        } else {
            //g_update_dead(g, tgt_id, false);
            g->ct.set<dead>(tgt_id, false);
        }
    } else {
        merror("Attack MISSED");
    }
    // handle attack miss
    //if (tgttype == ENTITY_PLAYER)
    //add_message_history(
    //    g, "%s's attack missed!", g_get_name(g, atk_id).c_str());
    //else if (tgttype == ENTITY_NPC)
    //add_message_history(g,
    //                    "%s missed %s!",
    //                    g_get_name(g, atk_id).c_str(),
    //                    g_get_name(g, tgt_id).c_str());
    //}
    //e_set_hp(target, e_get_hp(target) - dmg); // Reduce HP by 1
    //if (target->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked you for %d damage!", attacker->name, dmg);
    //if (attacker->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked %s for %d damage!", attacker->name, target->name, dmg);
    //if (e_get_hp(target) <= 0) {
    //target->dead = true;
    //g_update_dead(g, tgt_id, true);
    //}
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

bool handle_attack_helper_innerloop(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, int i, entityid attacker_id, bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(i >= 0, "i is out of bounds");
    //massert((size_t)i < tile->entity_max, "i is out of bounds");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(attack_successful, "attack_successful is NULL");
    entityid target_id = tile->entities->at(i);
    if (target_id == ENTITYID_INVALID) {
        return false;
    }

    entitytype_t type = g->ct.get<entitytype>(target_id).value_or(ENTITY_NONE);

    if (type != ENTITY_PLAYER && type != ENTITY_NPC) return false;
    if (g->ct.get<dead>(target_id).value_or(true)) {
        return false;
    }
    //    // lets try an experiment...
    //    // get the armor class of the target
    //int base_ac = g_get_stat(g, target_id, STATS_AC);
    //int base_str = g_get_stat(g, attacker_id, STATS_STR);
    //int str_bonus = bonus_calc(base_str);
    //int str_bonus = 0;
    //int atk_bonus = g_get_stat(g, attacker_id, STATS_ATTACK_BONUS);
    //int attack_roll = rand() % 20 + 1 + str_bonus + atk_bonus; // 1d20 + str bonus + attack bonus
    //*attack_successful = false;
    *attack_successful = true;
    //if (attack_roll >= base_ac) {
    //    return handle_shield_check(g, attacker_id, target_id, attack_roll, base_ac, attack_successful);
    //}
    // attack misses
    handle_attack_success(g, attacker_id, target_id, attack_successful);
    return false;
}


void handle_attack_helper(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, entityid attacker_id, bool* successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(successful, "attack_successful is NULL");
    //for (size_t i = 0; i < tile->entity_max; i++) {
    for (int i = 0; (size_t)i < tile->entities->size(); i++) {
        handle_attack_helper_innerloop(g, tile, i, attacker_id, successful);
    }
}


void try_entity_attack(shared_ptr<gamestate> g, entityid atk_id, int tgt_x, int tgt_y) {
    massert(g, "gamestate is NULL");
    massert(!g->ct.get<dead>(atk_id).value_or(false), "attacker entity is dead");

    minfo("Trying to attack...");

    vec3 loc = g->ct.get<location>(atk_id).value();
    shared_ptr<dungeon_floor_t> floor = d_get_floor(g->dungeon, loc.z);
    massert(floor, "failed to get dungeon floor");
    shared_ptr<tile_t> tile = df_tile_at(floor, (vec3){tgt_x, tgt_y, loc.z});
    if (!tile) {
        return;
    }
    // Calculate direction based on target position
    bool ok = false;
    //vec3 eloc = g_get_loc(g, atk_id);
    vec3 eloc = g->ct.get<location>(atk_id).value();
    int dx = tgt_x - eloc.x;
    int dy = tgt_y - eloc.y;
    g->ct.set<direction>(atk_id, get_dir_from_xy(dx, dy));
    g->ct.set<attacking>(atk_id, true);
    g->ct.set<update>(atk_id, true);
    handle_attack_helper(g, tile, atk_id, &ok);
    entitytype_t type0 = g->ct.get<entitytype>(atk_id).value_or(ENTITY_NONE);
    handle_attack_success_gamestate_flag(g, type0, ok);
}

//static bool
//entities_adjacent(shared_ptr<gamestate> g, entityid id0, entityid id1) {
//    massert(g, "gamestate is NULL");
//    massert(id0 != ENTITYID_INVALID, "id0 is invalid");
//    massert(id1 != ENTITYID_INVALID, "id1 is invalid");
//    massert(id0 != id1, "id0 and id1 are the same");
//    // use e0 and check the surrounding 8 tiles
//    for (int y = -1; y <= 1; y++) {
//        for (int x = -1; x <= 1; x++) {
//            if (x == 0 && y == 0) continue;
//            vec3 loc0 = g_get_location(g, id0);
//            vec3 loc1 = g_get_location(g, id1);
//            if (loc0.x + x == loc1.x && loc0.y + y == loc1.y) return true;
//        }
//    }
//    return false;
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

//static void update_player_tiles_explored(shared_ptr<gamestate> g) {
//    massert(g, "gamestate is NULL");
//    entityid hero_id = g->hero_id;
//    massert(hero_id != ENTITYID_INVALID, "hero id is invalid");
//    dungeon_floor_t* df = d_get_floor(g->d, g->d->current_floor);
//    massert(df, "failed to get current dungeon floor");
//    vec3 loc = g_get_location(g, hero_id);
//    // Get the player's light radius
//    int light_radius = g_get_light_radius(g, hero_id);
//    int light_radius_bonus = g_get_entity_total_light_radius_bonus(g, hero_id);
//    light_radius += light_radius_bonus;
//    minfo("hero light radius: %d", light_radius);
//    massert(light_radius > 0, "light radius is negative");
//    // Reveal tiles in a diamond pattern
//    for (int i = -light_radius; i <= light_radius; i++) {
//        for (int j = -light_radius; j <= light_radius; j++) {
//            // Calculate Manhattan distance for diamond shape
//            int dist = abs(i) + abs(j);
//            // Only reveal tiles within the light radius
//            if (dist <= light_radius) {
//                vec3 loc2 = {loc.x + i, loc.y + j, loc.z};
//                // Skip if out of bounds
//                if (loc2.x < 0 || loc2.x >= df->width || loc2.y < 0 ||
//                    loc2.y >= df->height)
//                    continue;
//                //tile_t* tile = df_tile_at(df, loc2);
//                shared_ptr<tile_t> tile = df_tile_at(df, loc2);
//                massert(tile, "failed to get tile at hero location");
//                tile->explored = true;
//                tile->visible = true;
//            }
//        }
//    }
//}


void handle_input_inventory(shared_ptr<inputstate> is, shared_ptr<gamestate> g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->controlmode != CONTROLMODE_INVENTORY) {
        //merror("wrong mode");
        return;
    }
    if (!g->display_inventory_menu) {
        return;
    }
    //if (inputstate_is_pressed(is, KEY_ESCAPE) || inputstate_is_pressed(is, KEY_I)) {
    if (inputstate_is_pressed(is, KEY_I)) {
        g->controlmode = CONTROLMODE_PLAYER;
        g->display_inventory_menu = false;
        //g->display_gameplay_settings_menu = false;
        //g->display_sort_inventory_menu = false;
        return;
    }
    if (inputstate_is_pressed(is, KEY_LEFT)) {
        if (g->inventory_cursor.x > 0) {
            g->inventory_cursor.x--;
        }
    }
    if (inputstate_is_pressed(is, KEY_RIGHT)) {
        g->inventory_cursor.x++;
    }
    if (inputstate_is_pressed(is, KEY_UP)) {
        if (g->inventory_cursor.y > 0) {
            g->inventory_cursor.y--;
        }
    }
    if (inputstate_is_pressed(is, KEY_DOWN)) {
        g->inventory_cursor.y++;
    }
    if (inputstate_is_pressed(is, KEY_E)) {
        // equip item
        // get the item id of the current selection
        size_t index = g->inventory_cursor.y * 7 + g->inventory_cursor.x;

        optional<shared_ptr<vector<entityid>>> my_inventory = g->ct.get<inventory>(g->hero_id);
        if (my_inventory) {
            if (my_inventory.has_value()) {
                shared_ptr<vector<entityid>> unpacked_inventory = my_inventory.value();
                if (index >= 0 && index < unpacked_inventory->size()) {
                    entityid item_id = unpacked_inventory->at(index);
                    entitytype_t type = g->ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
                    if (type == ENTITY_ITEM) {
                        itemtype_t item_type = g->ct.get<itemtype>(item_id).value_or(ITEM_NONE);
                        if (item_type != ITEM_NONE) {
                            if (item_type == ITEM_WEAPON) {
                                // try to equip it

                                g->ct.set<equipped_weapon>(g->hero_id, item_id);
                            }
                        }
                    }
                }
            }
        }
        //                minfo("equipped item %d", item_id);
        //            } else {
        //                minfo("failed to equip item %d", item_id);
        //            }
        //        }
        //    }
    }
    //}
    //if (inputstate_is_pressed(is, KEY_U)) {
    // unequip weapon/item
    // we will assume it is a weapon for now
    //entityid weapon_id = g_get_equipped_weapon(g, g->hero_id);
    //if (weapon_id != ENTITYID_INVALID) {
    //    if (g_unequip_weapon(g, g->hero_id, weapon_id)) {
    //        minfo("unequipped weapon %d", weapon_id);
    //    } else {
    //        minfo("failed to unequip weapon %d", weapon_id);
    //    }
    //}
    //}
    if (inputstate_is_pressed(is, KEY_D)) {
        // drop item
        drop_item_from_hero_inventory(g);
    }
    // cycle menus
    //if (inputstate_is_pressed(is, KEY_LEFT) ||
    //    inputstate_is_pressed(is, KEY_RIGHT)) {
    //    g->controlmode = CONTROLMODE_GAMEPLAY_SETTINGS;
    //    g->display_gameplay_settings_menu = true;
    //    g->display_inventory_menu = false;
    //    return;
    //}
    //if (inputstate_is_pressed(is, KEY_S)) {
    //    minfo("toggling sort inventory menu");
    //    g->display_sort_inventory_menu = !g->display_sort_inventory_menu;
    //    return;
    //}
    //size_t count = 0;
    //entityid* inventory = g_get_inventory(g, g->hero_id, &count);
    //if (g->display_inventory_menu && g->display_sort_inventory_menu) {
    //    handle_input_sort_inventory(is, g);
    //    return;
    //}
    //
    //    if (count == 0) return;
    // make sure we separate control of inv menu from its sort inv menu
    //    if (g->display_inventory_menu && !g->display_sort_inventory_menu) {
    //        if (inputstate_is_pressed(is, KEY_DOWN) ||
    //            inputstate_is_pressed(is, KEY_X)) {
    //            g->inventory_menu_selection =
    //                (size_t)g->inventory_menu_selection + 1 >= count
    //                    ? 0
    //                    : g->inventory_menu_selection + 1;
    //        } else if (inputstate_is_pressed(is, KEY_UP) ||
    //                   inputstate_is_pressed(is, KEY_W)) {
    //            g->inventory_menu_selection = g->inventory_menu_selection - 1 < 0
    //                                              ? count - 1
    //                                              : g->inventory_menu_selection - 1;
    //            // drop item
    //        } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
    //            // we need to grab the entityid of the selected item
    //            entityid item_id = inventory[g->inventory_menu_selection];
    //            g_remove_from_inventory(g, g->hero_id, item_id);
    //            //handle_sort_inventory(g);
    //            // add the item to the tile where the player is located at
    //            vec3 loc = g_get_location(g, g->hero_id);
    //            dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    //            massert(df, "Dungeon floor is NULL!");
    //            vec3 loc_cast = {loc.x, loc.y, loc.z};
    //            //tile_t* const tile = df_tile_at(df, loc_cast);
    //            shared_ptr<tile_t> tile = df_tile_at(df, loc_cast);
    //            massert(tile, "Tile is NULL!");
    //            if (!tile_add(tile, item_id)) return;
    //            // we also have to update the location of the item
    //            g_update_location(g, item_id, loc);
    //            g->controlmode = CONTROLMODE_PLAYER;
    //            g->display_inventory_menu = false;
    //        } else if (inputstate_is_pressed(is, KEY_ENTER) ||
    //                   inputstate_is_pressed(is, KEY_APOSTROPHE)) {
    //            entityid item_id = inventory[g->inventory_menu_selection];
    //            // we will eventually adjust this to check which slot it needs to go into based on its various types
    //            entitytype_t type = g_get_type(g, item_id);
    //            if (type == ENTITY_ITEM) {
    //                itemtype item_type = g_get_itemtype(g, item_id);
    //                if (item_type == ITEM_POTION) {
    //                    potiontype potion_type = g_get_potiontype(g, item_id);
    //                    if (potion_type == POTION_HEALTH_SMALL) {
    //                        int hp = g_get_stat(g, g->hero_id, STATS_HP);
    //                        int maxhp = g_get_stat(g, g->hero_id, STATS_MAXHP);
    //                        if (hp < maxhp) {
    //                            int small_hp_health_roll = rand() % 4 + 1;
    //                            hp += small_hp_health_roll;
    //                            if (hp > maxhp) hp = maxhp;
    //                            g_set_stat(g, g->hero_id, STATS_HP, hp);
    //                            add_message_history(
    //                                g,
    //                                "%s drank a %s and recovered %d HP",
    //                                g_get_name(g, g->hero_id).c_str(),
    //                                g_get_name(g, item_id).c_str(),
    //                                small_hp_health_roll);
    //                            // remove the potion from the inventory
    //                            g_remove_from_inventory(g, g->hero_id, item_id);
    //                            g->controlmode = CONTROLMODE_PLAYER;
    //                            g->display_inventory_menu = false;
    //                            g->controlmode = CONTROLMODE_PLAYER;
    //                            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                        }
    //                    }
    //                } else if (item_type == ITEM_WEAPON) {
    //                    // check if the item is already equipped
    //                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON);
    //                    //if (equipped_item != ENTITYID_INVALID) {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //                    //}
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                } else if (item_type == ITEM_SHIELD) {
    //                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD);
    //                    //if (equipped_item != ENTITYID_INVALID) {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //                    //}
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                } else if (item_type == ITEM_WAND) {
    //                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
    //                    //if (equipped_item != ENTITYID_INVALID) {
    //                    // unequip the currently equipped item
    //                    //    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
    //                    //    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                    //} else {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WAND, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //                    //}
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                } else if (item_type == ITEM_RING) {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_RING, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //
    //                    // update player tiles
    //                    update_player_tiles_explored(g);
    //
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                }
    //            }
    //        }
    //    }
}


void handle_input_help_menu(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_any_pressed(is)) {
        g->display_help_menu = false;
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }
}


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
