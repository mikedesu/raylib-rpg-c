#include "controlmode.h"
#include "gamestate_flag.h"
#include "liblogic_update_debug_panel_buffer.h"

void update_debug_panel_buffer(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(g, "gamestate is NULL");
    // Static buffers to avoid reallocating every frame
    //static const char* control_modes[] = {"Camera", "Player", "Unknown"};

    int message_count = g->msg_history->size();
    int inventory_count;
    vec3 loc = {0, 0, 0};
    inventory_count = -1;
    if (g->hero_id != ENTITYID_INVALID) {
        loc = g->ct.get<location>(g->hero_id).value_or((vec3){-1, -1, -1});
    }
    // Determine control mode and flag strings
    const char* control_mode = g->controlmode == CONTROLMODE_CAMERA ? "Camera" : g->controlmode == CONTROLMODE_PLAYER ? "Player" : "Unknown";
    // zero out the buffer
    memset(g->debugpanel.buffer, 0, sizeof(g->debugpanel.buffer));
    // Format the string in one pass
    snprintf(g->debugpanel.buffer,
             sizeof(g->debugpanel.buffer),
             "@evildojo666\n"
             "project.rpg\n"
             //"%s\n" // timebeganbuf
             //"%s\n" // currenttimebuf
             "frame : %d\n"
             "update: %d\n"
             "mouse: %.01f, %.01f\n"
             "last clicked: %.01f, %.01f\n"
             "frame dirty: %d\n"
             "draw time: %.1fms\n"
             "cam: (%.0f,%.0f) Zoom: %.1f\n"
             "controlmode: %s \n"
             "floor: %d/%d \n"
             "next_entity_id: %d\n"
             "hero_id: %d\n"
             "flag: %s\n"
             "entity_turn: %d\n"
             "hero: (%d,%d,%d)\n"
             "weapon: %d\n"
             "inventory: %d\n"
             "message count: %d\n",
             g->framecount,
             g->frame_updates,
             is->mouse_position.x,
             is->mouse_position.y,
             g->last_click_screen_pos.x,
             g->last_click_screen_pos.y,
             g->frame_dirty,
             g->last_frame_time * 1000,
             g->cam2d.offset.x,
             g->cam2d.offset.y,
             g->cam2d.zoom,
             control_mode,
             0,
             0,
             g->next_entityid,
             g->hero_id,
             g->flag == GAMESTATE_FLAG_NONE           ? "None"
             : g->flag == GAMESTATE_FLAG_PLAYER_INPUT ? "Player Input"
             : g->flag == GAMESTATE_FLAG_PLAYER_ANIM  ? "Player anim"
             : g->flag == GAMESTATE_FLAG_NPC_TURN     ? "NPC Turn"
             : g->flag == GAMESTATE_FLAG_NPC_ANIM     ? "NPC anim"
                                                      : "Unknown",

             g->entity_turn,
             loc.x,
             loc.y,
             loc.z,
             g->ct.get<equipped_weapon>(g->hero_id).value_or(ENTITYID_INVALID),
             inventory_count,
             message_count);
}
