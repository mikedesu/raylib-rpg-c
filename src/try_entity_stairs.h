#pragma once

#include "add_message.h"
#include "gamestate.h"
#include "play_sound.h"
#include "sfx.h"


static inline bool try_entity_stairs(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");

    g->ct.set<update>(id, true);
    const vec3 loc = g->ct.get<location>(id).value();

    // first, we prob want to get the tile at this location
    const int current_floor = g->dungeon->current_floor;
    auto df = g->dungeon->floors->at(current_floor);
    auto t = df_tile_at(df, loc);
    massert(t, "tile was NULL");

    // check the tile type
    if (t->type == TILE_UPSTAIRS) {
        // can't go up on the top floor
        // otherwise...
        if (current_floor == 0) {
            add_message(g, "You are already on the top floor!");
        } else {
            // go upstairs
            // we have to remove the player from the old tile
            df_remove_at(df, g->hero_id, loc.x, loc.y);

            g->dungeon->current_floor--;

            const int new_floor = g->dungeon->current_floor;
            auto df2 = g->dungeon->floors->at(new_floor);
            const vec3 uloc = df2->downstairs_loc;
            auto t2 = df_tile_at(df2, uloc);

            df_add_at(df2, g->hero_id, uloc.x, uloc.y);
            g->ct.set<location>(g->hero_id, uloc);
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            //PlaySound(g->sfx->at(SFX_STEP_STONE_1));
            play_sound(SFX_STEP_STONE_1);
            return true;
        }

    } else if (t->type == TILE_DOWNSTAIRS) {
        // can't go down on the bottom floor
        // otherwise...

        if ((size_t)current_floor < g->dungeon->floors->size() - 1) {
            // go downstairs
            // we have to remove the player from the old tile
            df_remove_at(df, g->hero_id, loc.x, loc.y);

            g->dungeon->current_floor++;

            const int new_floor = g->dungeon->current_floor;
            auto df2 = g->dungeon->floors->at(new_floor);
            const vec3 uloc = df2->upstairs_loc;
            auto t2 = df_tile_at(df2, uloc);

            df_add_at(df2, g->hero_id, uloc.x, uloc.y);
            g->ct.set<location>(g->hero_id, uloc);
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            //PlaySound(g->sfx->at(SFX_STEP_STONE_1));
            play_sound(SFX_STEP_STONE_1);
            return true;

        } else {
            // bottom floor
            add_message(g, "You can't go downstairs anymore!");
        }
    }

    return false;
}
