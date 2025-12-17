#pragma once

#include "create_player.h"
#include "gamestate.h"
#include "get_racial_hd.h"
#include "inputstate.h"
#include "roll.h"
#include "sfx.h"


static inline void handle_input_character_creation_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");

    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
        return;
    }

    if (inputstate_is_pressed(is, KEY_ENTER)) {
        minfo("Character creation confirmed");
        PlaySound(g->sfx->at(SFX_CONFIRM_01));
        // we need to copy the character creation stats to the hero entity
        // hero has already been created, so its id is available
        const int myhd = g->chara_creation->hitdie;
        int maxhp_roll = -1;
        while (maxhp_roll < 1)
            maxhp_roll = do_roll_best_of_3((vec3){1, myhd, 0}) + get_stat_bonus(g->chara_creation->constitution);
        const vec3 start_loc = g->dungeon->floors->at(g->dungeon->current_floor)->upstairs_loc;
        g->entity_turn = create_player(g, start_loc, "darkmage");

        massert(g->hero_id != ENTITYID_INVALID, "heroid is invalid");

        // set stats from char_creation
        g->ct.set<strength>(g->hero_id, g->chara_creation->strength);
        g->ct.set<dexterity>(g->hero_id, g->chara_creation->dexterity);
        g->ct.set<constitution>(g->hero_id, g->chara_creation->constitution);
        g->ct.set<intelligence>(g->hero_id, g->chara_creation->intelligence);
        g->ct.set<wisdom>(g->hero_id, g->chara_creation->wisdom);
        g->ct.set<charisma>(g->hero_id, g->chara_creation->charisma);

        g->ct.set<hd>(g->hero_id, (vec3){1, g->chara_creation->hitdie, 0});
        g->ct.set<hp>(g->hero_id, maxhp_roll);
        g->ct.set<maxhp>(g->hero_id, maxhp_roll);

        // temporary wedge-in code
        // set all the NPCs to target the hero
        minfo("BEGIN Temporary wedge-in code");
        for (entityid id = 0; id < g->next_entityid; id++) {
            minfo("Getting type for id %d", id);
            entitytype_t t = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (t == ENTITY_NPC) {
                minfo("Setting target for id %d", id);
                g->ct.set<target_id>(id, g->hero_id);
            }
        }
        minfo("END Temporary wedge-in code");
        g->current_scene = SCENE_GAMEPLAY;

    } else if (inputstate_is_pressed(is, KEY_SPACE)) {
        // re-roll character creation stats
        minfo("Re-rolling character creation stats");
        PlaySound(g->sfx->at(SFX_CONFIRM_01));
        g->chara_creation->strength = do_roll_best_of_3((vec3){3, 6, 0});
        g->chara_creation->dexterity = do_roll_best_of_3((vec3){3, 6, 0});
        g->chara_creation->intelligence = do_roll_best_of_3((vec3){3, 6, 0});
        g->chara_creation->wisdom = do_roll_best_of_3((vec3){3, 6, 0});
        g->chara_creation->constitution = do_roll_best_of_3((vec3){3, 6, 0});
        g->chara_creation->charisma = do_roll_best_of_3((vec3){3, 6, 0});
    } else if (inputstate_is_pressed(is, KEY_LEFT)) {
        PlaySound(g->sfx->at(SFX_CONFIRM_01));
        int race = g->chara_creation->race;
        if (g->chara_creation->race > 1)
            race--;
        else
            race = RACE_COUNT - 1;
        g->chara_creation->race = (race_t)race;
        g->chara_creation->hitdie = get_racial_hd(g->chara_creation->race);
    } else if (inputstate_is_pressed(is, KEY_RIGHT)) {
        PlaySound(g->sfx->at(SFX_CONFIRM_01));
        int race = g->chara_creation->race;
        if (race < RACE_COUNT - 1)
            race++;
        else
            race = RACE_NONE + 1;
        g->chara_creation->race = (race_t)race;
        g->chara_creation->hitdie = get_racial_hd(g->chara_creation->race);
    }
    g->frame_dirty = true;
}
