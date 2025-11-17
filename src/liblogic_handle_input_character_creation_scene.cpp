#include "liblogic_create_player.h"
#include "liblogic_handle_input_character_creation_scene.h"
#include "roll.h"
#include "sfx.h"
#include <raylib.h>

void handle_input_character_creation_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_is_pressed(is, KEY_ENTER)) {
        minfo("Character creation confirmed");
        PlaySound(g->sfx->at(SFX_CONFIRM_01));
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
        PlaySound(g->sfx->at(SFX_CONFIRM_01));
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
