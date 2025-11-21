#include "gamestate_flag.h"
#include "liblogic_update_debug_panel_buffer.h"

void update_debug_panel_buffer(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(g, "gamestate is NULL");
    // Static buffers to avoid reallocating every frame
    static const char* control_modes[] = {"Camera", "Player", "Unknown"};
    // Get hero position once
    //int x;
    //int y;
    //int z;
    int message_count = g->msg_history->size();


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
             "Flag: %s\n"
             "Turn: %d\n"
             "Hero: (%d,%d,%d)\n"
             "Weapon: %d\n"
             "Inventory: %d\n"
             "Message count: %d\n",
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
