#include "gamestate_flag.h"
#include "libdraw_handle_dirty_entities.h"
#include "libdraw_handle_gamestate_flag.h"
#include "libdraw_update_sprite.h"
#include "libdraw_update_sprites.h"
#include "libgame_defines.h"

extern Music music;
extern unordered_map<entityid, spritegroup_t*> spritegroups;

extern int ANIM_SPEED;

void libdraw_update_sprites_pre(shared_ptr<gamestate> g) {
    //minfo("Begin update sprites pre");
    massert(g, "gamestate is NULL");

    UpdateMusicStream(music);
    //if (g->music_volume_changed) {
    //    SetMusicVolume(music, g->music_volume);
    //    g->music_volume_changed = false;
    //}

    if (g->current_scene == SCENE_GAMEPLAY) {
        if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT || g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            ANIM_SPEED = DEFAULT_ANIM_SPEED / 2;
        } else if (g->flag == GAMESTATE_FLAG_NPC_TURN || g->flag == GAMESTATE_FLAG_NPC_ANIM) {
            ANIM_SPEED = DEFAULT_ANIM_SPEED / 4;
        }


        libdraw_handle_dirty_entities(g);
        //minfo("Begin update sprites pre loop");
        for (entityid id = 0; id < g->next_entityid; id++) {
            libdraw_update_sprite_pre(g, id);
        }
        //minfo("End update sprites pre loop");
    }
    //minfo("End update sprites pre");
}


void libdraw_update_sprites_post(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    //if (g->music_volume_changed) {
    //    SetMusicVolume(music, g->music_volume);
    //    g->music_volume_changed = false;
    //}
    //UpdateMusicStream(music);
    if (g->current_scene != SCENE_GAMEPLAY) {
        g->frame_dirty = false;
        return;
    }
    // for the gameplay scene...
    if (g->framecount % ANIM_SPEED == 0) {
        libdraw_handle_dirty_entities(g);
        g->frame_dirty = true;


        // For every entity...
        for (entityid id = 0; id < g->next_entityid; id++) {
            // verify it has an entity type
            entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
            //spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);

            // grab the sprite group for that entity
            spritegroup_t* sg = spritegroups[id];
            if (sg) {
                //int num_sprites = sg->sprites2->size();
                //for (int i = 0; i < num_sprites; i++) {
                //shared_ptr<sprite> s = sg->sprites2->at(i);
                shared_ptr<sprite> s = sg->sprites2->at(sg->current);
                if (s) {
                    sprite_incrframe2(s);
                    g->frame_dirty = true;
                    // this condition for the animation reset seems incorrect
                    // certain cases are causing animations to drop-off mid-sequence
                    if (s->num_loops >= 1) {
                        sg->current = sg->default_anim;
                        s->num_loops = 0;
                    }

                    // lets try something
                    if (type == ENTITY_ITEM) {
                        itemtype_t itype = g->ct.get<itemtype>(id).value_or(ITEM_NONE);
                        if (itype == ITEM_WEAPON) {
                            if (sg->current > 0) {
                                shared_ptr<sprite> s2 = sg->sprites2->at(sg->current + 1);
                                if (s2) {
                                    sprite_incrframe2(s2);
                                    g->frame_dirty = true;
                                    // this condition for the animation reset seems incorrect
                                    // certain cases are causing animations to drop-off mid-sequence
                                    if (s2->num_loops >= 1) {
                                        sg->current = sg->default_anim;
                                        s2->num_loops = 0;
                                    }
                                }
                            }
                        }
                    }
                }


                //}
            }
            //}
            //if (s_shadow) {
            //    sprite_incrframe(s_shadow);
            //    if (s_shadow->num_loops >= 1) {
            //        sg->current = sg->default_anim;
            //        s_shadow->num_loops = 0;
            //    }
            //}
        }
    }
    libdraw_handle_gamestate_flag(g);
}
