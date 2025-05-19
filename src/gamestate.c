#include "component.h"
#include "controlmode.h"
#include "entityid.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "item.h"
#include "massert.h"
#include "mprint.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr() {
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    massert(g, "g is NULL");
    g->framecount = 0;
    g->turn_count = 0;
    g->debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
    g->debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
    g->debugpanel.w = 200;
    g->debugpanel.h = 200;
    g->debugpanel.fg_color = RAYWHITE;
    g->debugpanel.bg_color = RED;
    g->debugpanel.pad_top = 0;
    g->debugpanel.pad_left = 0;
    g->debugpanel.pad_right = 0;
    g->debugpanel.pad_bottom = 0;
    g->debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->lock = 0;
    g->targetwidth = -1;
    g->targetheight = -1;
    g->windowwidth = -1;
    g->windowheight = -1;
    g->timebegan = time(NULL);
    g->currenttime = time(NULL);
    g->timebegantm = localtime(&(g->timebegan));
    g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time: %Y-%m-%d %H:%M:%S", g->timebegantm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    g->debugpanelon = false;
    g->player_input_received = false;
    g->is_locked = false;
    g->gridon = false;
    g->display_inventory_menu = false;
    g->display_quit_menu = false;
    g->do_quit = false;
    g->processing_actions = false;
    g->cam_lockon = true;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){0, 0};
    g->cam2d.zoom = 10.0f;
    g->cam2d.rotation = 0.0;
    g->cam3d = (Camera3D){0};
    g->cam3d.position = (Vector3){0.0f, 20.0f, 20.0f};
    g->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    g->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};
    g->cam3d.fovy = 45.0f;
    g->cam3d.projection = CAMERA_PERSPECTIVE;
    g->camera_mode = CAMERA_FREE;
    g->is3d = false;
    g->fadealpha = 0.0f;
    g->controlmode = CONTROLMODE_PLAYER;
    g->fadestate = FADESTATENONE;
    g->d = NULL;
    //g->entityids = NULL;
    //g->index_entityids = 0;
    //g->max_entityids = -1;
    g->hero_id = -1;
    // current displayed dungeon floor
    g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
    g->entity_turn = -1;
    g->gameover = false;
    g->test_guard = false;
    g->font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->pad = 20;
    g->line_spacing = 1.0f;
    g->inventory_menu_selection = 0;

    g->components = ct_create();

    g->name_list_count = g->type_list_count = g->race_list_count = g->direction_list_count = 0;
    g->loc_list_count = g->sprite_move_list_count = g->dead_list_count = g->update_list_count = 0;
    g->attacking_list_count = g->blocking_list_count = g->block_success_list_count = g->damaged_list_count = 0;
    g->inventory_list_count = g->target_list_count = g->target_path_list_count = g->default_action_list_count = 0;
    g->equipment_list_count = g->stats_list_count = g->itemtype_list_count = g->weapontype_list_count = 0;
    g->shieldtype_list_count = g->potion_type_list_count = 0;
    g->damage_list_count = g->ac_list_count = 0;

    const size_t n = LIST_INIT_CAPACITY;

    g->name_list_capacity = g->type_list_capacity = g->race_list_capacity = g->direction_list_capacity = n;
    g->loc_list_capacity = g->sprite_move_list_capacity = g->dead_list_capacity = g->update_list_capacity = n;
    g->attacking_list_capacity = g->blocking_list_capacity = g->block_success_list_capacity = g->damaged_list_capacity = n;
    g->inventory_list_capacity = g->target_list_capacity = g->target_path_list_capacity = g->default_action_list_capacity = n;
    g->equipment_list_capacity = g->stats_list_capacity = g->itemtype_list_capacity = g->weapontype_list_capacity = n;
    g->shieldtype_list_capacity = g->potion_type_list_capacity = n;
    g->damage_list_capacity = g->ac_list_capacity = n;

    g->name_list = (name_component*)malloc(sizeof(name_component) * n);
    massert(g->name_list, "g->name_list is NULL");
    g->type_list = (type_component*)malloc(sizeof(type_component) * n);
    massert(g->type_list, "g->type_list is NULL");
    g->race_list = (race_component*)malloc(sizeof(race_component) * n);
    massert(g->race_list, "g->race_list is NULL");
    g->direction_list = (direction_component*)malloc(sizeof(direction_component) * n);
    massert(g->direction_list, "g->direction_list is NULL");
    g->loc_list = (loc_component*)malloc(sizeof(loc_component) * n);
    massert(g->loc_list, "g->loc_list is NULL");
    g->sprite_move_list = (sprite_move_component*)malloc(sizeof(sprite_move_component) * n);
    massert(g->sprite_move_list, "g->sprite_move_list is NULL");
    g->dead_list = (dead_component*)malloc(sizeof(dead_component) * n);
    massert(g->dead_list, "g->dead_list is NULL");
    g->update_list = (update_component*)malloc(sizeof(update_component) * n);
    massert(g->update_list, "g->update_list is NULL");
    g->attacking_list = (attacking_component*)malloc(sizeof(attacking_component) * n);
    massert(g->attacking_list, "g->attacking_list is NULL");
    g->blocking_list = (blocking_component*)malloc(sizeof(blocking_component) * n);
    massert(g->blocking_list, "g->blocking_list is NULL");
    g->block_success_list = (block_success_component*)malloc(sizeof(block_success_component) * n);
    massert(g->block_success_list, "g->block_success_list is NULL");
    g->damaged_list = (damaged_component*)malloc(sizeof(damaged_component) * n);
    massert(g->damaged_list, "g->damaged_list is NULL");
    g->inventory_list = (inventory_component*)malloc(sizeof(inventory_component) * n);
    massert(g->inventory_list, "g->inventory_list is NULL");
    g->target_list = (target_component*)malloc(sizeof(target_component) * n);
    massert(g->target_list, "g->target_list is NULL");
    g->target_path_list = (target_path_component*)malloc(sizeof(target_path_component) * n);
    massert(g->target_path_list, "g->target_path_list is NULL");
    g->default_action_list = (default_action_component*)malloc(sizeof(default_action_component) * n);
    massert(g->default_action_list, "g->default_action_list is NULL");
    g->equipment_list = (equipment_component*)malloc(sizeof(equipment_component) * n);
    massert(g->equipment_list, "g->equipment_list is NULL");
    g->stats_list = (stats_component*)malloc(sizeof(stats_component) * n);
    massert(g->stats_list, "g->stats_list is NULL");
    g->itemtype_list = (itemtype_component*)malloc(sizeof(itemtype_component) * n);
    massert(g->itemtype_list, "g->itemtype_list is NULL");
    g->weapontype_list = (weapontype_component*)malloc(sizeof(weapontype_component) * n);
    massert(g->weapontype_list, "g->weapontype_list is NULL");
    g->shieldtype_list = (shieldtype_component*)malloc(sizeof(shieldtype_component) * n);
    massert(g->shieldtype_list, "g->shieldtype_list is NULL");
    g->potion_type_list = (potiontype_component*)malloc(sizeof(potiontype_component) * n);
    massert(g->potion_type_list, "g->potion_list is NULL");
    g->damage_list = (damage_component*)malloc(sizeof(damage_component) * n);
    massert(g->damage_list, "g->damage_list is NULL");
    g->ac_list = (ac_component*)malloc(sizeof(ac_component) * n);
    massert(g->ac_list, "g->ac_list is NULL");

    g->next_entityid = 0;

    g->dirty_entities = false;
    g->new_entityid_begin = g->new_entityid_end = -1;

    gamestate_init_msg_history(g);
    return g;
}

bool gamestate_init_msg_history(gamestate* const g) {
    massert(g, "g is NULL");
    g->msg_history.count = 0;
    g->msg_history.max_count = DEFAULT_MAX_HISTORY_SIZE;
    g->msg_history.messages = (char**)malloc(sizeof(char*) * g->msg_history.max_count);
    if (g->msg_history.messages == NULL) {
        merror("g->msg_history.messages is NULL");
        free(g);
        return false;
    }
    for (int i = 0; i < g->msg_history.max_count; i++) {
        g->msg_history.messages[i] = (char*)malloc(sizeof(char) * MAX_MSG_LENGTH);
        if (g->msg_history.messages[i] == NULL) {
            merror("g->msg_history.messages[%d] is NULL", i);
            for (int j = 0; j < i; j++) {
                free(g->msg_history.messages[j]);
            }
            free(g->msg_history.messages);
            //free(g);
            return false;
        }
    }
    return true;
}

bool gamestate_free_msg_history(gamestate* const g) {
    massert(g, "g is NULL");
    for (int i = 0; i < g->msg_history.max_count; i++) {
        if (g->msg_history.messages[i]) {
            free(g->msg_history.messages[i]);
            g->msg_history.messages[i] = NULL;
        }
    }
    free(g->msg_history.messages);
    g->msg_history.messages = NULL;
    return true;
}

void gamestatefree(gamestate* g) {
    massert(g, "g is NULL");
    //minfo("Freeing dungeon");
    //dungeon_destroy(g->dungeon);
    // free message history
    gamestate_free_msg_history(g);
    ct_destroy(g->components);
    //free(g->entityids);
    free(g->name_list);
    free(g->type_list);
    free(g->race_list);
    free(g->direction_list);
    free(g->loc_list);
    free(g->sprite_move_list);
    free(g->dead_list);
    free(g->update_list);
    free(g->attacking_list);
    free(g->blocking_list);
    free(g->block_success_list);
    free(g->damaged_list);
    free(g->inventory_list);
    free(g->target_list);
    free(g->target_path_list);
    free(g->default_action_list);
    free(g->equipment_list);
    free(g->stats_list);
    free(g->itemtype_list);
    free(g->weapontype_list);
    free(g->shieldtype_list);
    free(g->potion_type_list);
    free(g->damage_list);
    free(g->ac_list);
    free(g);
    //msuccess("Freed gamestate");
}

//bool gs_add_entityid(gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != -1, "id is -1");
//    massert(g->index_entityids < g->max_entityids, "index_entityids >= max_entityids");
//    massert(g->entityids, "g->entityids is NULL");
//    massert(g->index_entityids == id, "index_entityids != id");
//    g->entityids[g->index_entityids] = id;
//    g->index_entityids++;
//    return true;
//}

void gamestate_set_hero_id(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    g->hero_id = id;
}

entityid gamestate_get_hero_id(const gamestate* const g) {
    massert(g, "g is NULL");
    return g->hero_id;
}

//void gamestate_init_entityids(gamestate* const g) {
//    massert(g, "g is NULL");
//    g->entityids = (entityid*)malloc(sizeof(entityid) * GAMESTATE_INIT_ENTITYIDS_MAX);
//    if (g->entityids == NULL) {
//        merror("g->entityids is NULL");
//        return;
//    }
//    for (int i = 0; i < g->max_entityids; i++) {
//        g->entityids[i] = -1;
//    }
//    g->index_entityids = 0;
//    g->max_entityids = GAMESTATE_INIT_ENTITYIDS_MAX;
//}

//entityid gamestate_get_entityid_unsafe(const gamestate* const g, int index) { return g->entityids[index]; }

//void gamestate_dungeon_destroy(gamestate* const g) {
//    if (!g) {
//        merror("g is NULL");
//        return;
//    }
//    dungeon_destroy(g->dungeon);
//    g->dungeon = NULL;
//}

//int gamestate_get_entityid_index(const gamestate* const g, entityid id) {
//    if (!g) {
//        merror("g is NULL");
//        return -1;
//    }
//    for (int i = 0; i < g->max_entityids; i++) {
//        if (g->entityids[i] == id) {
//            return i;
//        }
//    }
//    return -1;
//}

//int gamestate_get_next_npc_entityid_from_index(const gamestate* const g, int index) {
//    if (!g) {
//        merror("g is NULL");
//        return -1;
//    }
//    for (int i = index + 1; i < g->max_entityids; i++) {
//        if (g->entityids[i] != -1) {
//            return g->entityids[i];
//        }
//    }
//    return -1;
//}

//void gamestate_incr_entity_turn(gamestate* const g) {
//    if (!g) {
//        merror("g is NULL");
//        return;
//    }
//    if (g->entity_turn == -1) {
//        if (g->hero_id == -1) {
//            merror("both g->entity_turn and g->hero_id are -1");
//            return;
//        }
//        g->entity_turn = g->hero_id;
//    } else {
//        // given that entity_turn is an entityid, we need to find the next entity in our
//        // entityids array that belongs to an NPC
//        const int index = gamestate_get_entityid_index(g, g->entity_turn);
//        if (index == -1) {
//            merror("index is -1");
//            return;
//        }
//        g->entity_turn = gamestate_get_next_npc_entityid_from_index(g, index);
//    }
//}

void gamestate_load_keybindings(gamestate* const g) {
    if (!g) {
        merror("g is NULL");
        return;
    }
    const char* filename = "keybindings.ini";
    load_keybindings(filename, &g->keybinding_list);
}

bool gamestate_add_msg_history(gamestate* const g, const char* msg) {
    massert(g, "g is NULL");
    massert(msg, "msg is NULL");
    if (g->msg_history.count >= g->msg_history.max_count) {
        merror("msg history is full");
        return false;
    }
    strncpy(g->msg_history.messages[g->msg_history.count], msg, MAX_MSG_LENGTH);
    g->msg_history.count++;
    return true;
}

void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) {
        merror("windowwidth or windowheight is -1");
        return;
    }
    g->debugpanel.x = 0;
    g->debugpanel.y = g->windowheight - g->debugpanel.h;
}

void gamestate_set_debug_panel_pos_top_right(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) {
        merror("windowwidth or windowheight is -1");
        return;
    }
    g->debugpanel.x = g->windowwidth - g->debugpanel.w;
    g->debugpanel.y = g->debugpanel.pad_right;
}

bool g_register_comp(gamestate* const g, entityid id, component comp) {
    //minfo("g_register_comp: id: %d, comp: %s", id, component2str(comp));
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    //minfo("g_register_comp: id: %d, comp: %d", id, comp);
    if (g->components == NULL) {
        merror("g->components is NULL");
        return false;
    }
    ct_add_entity(g->components, id); // {
    //minfo("ct_add_entity failed, entity may already exist in table");
    //    return false;
    //}
    if (!ct_add_component(g->components, id, comp)) {
        merror("ct_add_component failed");
        return false;
    }
    return true;
}

bool g_register_comps(gamestate* const g, entityid id, ...) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->components == NULL) {
        merror("g->components is NULL");
        return false;
    }
    if (!ct_add_entity(g->components, id)) {
        merror("ct_add_entity failed");
        return false;
    }
    va_list args;
    va_start(args, id);
    component comp;
    //while ((comp = va_arg(args, component)) != C_COUNT) {
    while ((comp = va_arg(args, component)) != 0) {
        if (!ct_add_component(g->components, id, comp)) {
            merror("ct_add_component failed: %d %d", id, comp);
            va_end(args);
            return false;
        }
    }
    va_end(args);
    return true;
}

bool g_add_component(gamestate* const g, entityid id, component comp, void* data, size_t c_size, void** c_list, int* c_count, int* c_capacity) {
    //minfo("g_add_component: id: %d, comp: %s", id, component2str(comp));
    massert(g, "g is NULL");
    if (!data) {
        mwarning("data is NULL");
    }

    //massert(data, "data is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");

    massert(*c_list, "c_list is NULL");

    //massert(*c_count < *c_capacity, "c_count >= c_capacity: %d >= %d", *c_count, *c_capacity);
    massert(*c_capacity > 0, "c_capacity is 0");
    massert(*c_count >= 0, "c_count is negative");
    //massert(*c_count < g->max_entityids, "c_count >= g->max_entityids: %d >= %d", *c_count, g->max_entityids);
    massert(*c_count <= g->next_entityid, "c_count >= g->next_entityid: %d >= %d", *c_count, g->next_entityid);

    // Ensure entity exists in component table
    if (!ct_has_entity(g->components, id)) {
        if (!ct_add_entity(g->components, id)) {
            merror("Failed to add entity %d to component table", id);
            return false;
        }
    }

    // Automatically register component if not already registered
    if (!g_has_component(g, id, comp)) {
        if (!g_register_comp(g, id, comp)) {
            merror("Failed to register component %s for entity %d", component2str(comp), id);
            return false;
        }
    }

    if (*c_count >= *c_capacity) {
        *c_capacity *= 2;
        *c_list = realloc(*c_list, c_size * (*c_capacity));
        if (*c_list == NULL) {
            merror("c_list is NULL: realloc failed");
            merror("c_count: %d, c_capacity: %d", *c_count, *c_capacity);
            return false;
        }
    }

    // Initialize the component

    void* c_ptr = *c_list + (*c_count * c_size);

    switch (comp) {
    //case C_NAME: init_name_component((name_component*)(*c_list + (*c_count * c_size)), id, (const char*)data); break;
    case C_NAME: init_name_component((name_component*)c_ptr, id, (const char*)data); break;
    case C_TYPE: init_type_component((type_component*)c_ptr, id, *(entitytype_t*)data); break;
    case C_RACE: init_race_component((race_component*)c_ptr, id, *(race_t*)data); break;
    case C_DIRECTION: init_direction_component((direction_component*)c_ptr, id, *(direction_t*)data); break;
    case C_LOCATION: init_loc_component((loc_component*)c_ptr, id, *(loc_t*)data); break;
    case C_SPRITE_MOVE: init_sprite_move_component((sprite_move_component*)c_ptr, id, *(loc_t*)data); break;
    case C_DEAD: init_dead_component((dead_component*)c_ptr, id, *(bool*)data); break;
    case C_UPDATE: init_update_component((update_component*)c_ptr, id, *(bool*)data); break;
    case C_ATTACKING: init_attacking_component((attacking_component*)c_ptr, id, *(bool*)data); break;
    case C_BLOCKING: init_blocking_component((blocking_component*)c_ptr, id, *(bool*)data); break;
    case C_BLOCK_SUCCESS: init_block_success_component((block_success_component*)c_ptr, id, *(bool*)data); break;
    case C_DAMAGED: init_damaged_component((damaged_component*)c_ptr, id, *(bool*)data); break;
    case C_INVENTORY: init_inventory_component((inventory_component*)c_ptr, id); break;
    case C_TARGET: init_target_component((target_component*)c_ptr, id, *(loc_t*)data); break;
    case C_TARGET_PATH: init_target_path_component((target_path_component*)c_ptr, id, NULL, 0);
    case C_EQUIPMENT: init_equipment_component((equipment_component*)c_ptr, id); break;
    case C_DEFAULT_ACTION: init_default_action_component((default_action_component*)c_ptr, id, *(entity_action_t*)data); break;
    case C_STATS: init_stats_component((stats_component*)c_ptr, id); break;
    case C_ITEMTYPE: init_itemtype_component((itemtype_component*)c_ptr, id, *(itemtype*)data); break;
    case C_WEAPONTYPE: init_weapontype_component((weapontype_component*)c_ptr, id, *(weapontype*)data); break;
    case C_SHIELDTYPE: init_shieldtype_component((shieldtype_component*)c_ptr, id, *(shieldtype*)data); break;
    case C_POTIONTYPE: init_potiontype_component((potiontype_component*)c_ptr, id, *(potiontype*)data); break;
    case C_DAMAGE: init_damage_component((damage_component*)c_ptr, id, *(roll*)data); break;
    case C_AC: init_ac_component((ac_component*)c_ptr, id, *(int*)data); break;
    default: merror("Unsupported component type: %s", component2str(comp)); return false;
    }

    (*c_count)++;

    //msuccess("Added component %s to entity %d", component2str(comp), id);
    return true;
}

bool g_add_name(gamestate* const g, entityid id, const char* name) {
    massert(g, "g is NULL");
    massert(name, "name is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_add_component(g, id, C_NAME, (void*)name, sizeof(name_component), (void**)&g->name_list, &g->name_list_count, &g->name_list_capacity);
}

bool g_has_component(const gamestate* const g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    if (g->components == NULL) {
        merror("g->components is NULL");
        return false;
    }
    //if (!ct_has_entity(g->components, id)) { return false; }
    return ct_has_component(g->components, id, comp);
}

bool g_has_name(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_NAME);
}

const char* g_get_name(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_name(g, id), "id %d does not have a name component", id);
    for (int i = 0; i < g->name_list_count; i++) {
        if (g->name_list[i].id == id) {
            return g->name_list[i].name;
        }
    }
    return NULL;
}

bool g_add_type(gamestate* const g, entityid id, entitytype_t type) {
    massert(g, "g is NULL");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    return g_add_component(g, id, C_TYPE, (void*)&type, sizeof(type_component), (void**)&g->type_list, &g->type_list_count, &g->type_list_capacity);
}

entitytype_t g_get_type(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    if (id != ENTITYID_INVALID) {
        for (int i = 0; i < g->type_list_count; i++) {
            if (g->type_list[i].id == id) {
                return g->type_list[i].type;
            }
        }
    }
    return ENTITY_NONE;
}

bool g_set_type(gamestate* const g, entityid id, entitytype_t type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    massert(g->type_list, "g->type_list is NULL");
    if (g->type_list == NULL) {
        merror("g->type_list is NULL");
        return false;
    }
    for (int i = 0; i < g->type_list_count; i++) {
        if (g->type_list[i].id == id) {
            g->type_list[i].type = type;
            return true;
        }
    }
    return false;
}

bool g_has_type(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_TYPE);
}

bool g_is_type(const gamestate* const g, entityid id, entitytype_t type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    massert(g->type_list, "g->type_list is NULL");
    if (g->type_list == NULL) {
        merror("g->type_list is NULL");
        return false;
    }
    return type == g_get_type(g, id);
}

bool g_add_race(gamestate* const g, entityid id, race_t race) {
    massert(g, "g is NULL");
    //massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    // make sure the entity has the component
    return g_add_component(g, id, C_RACE, (void*)&race, sizeof(race_component), (void**)&g->race_list, &g->race_list_count, &g->race_list_capacity);
}

race_t g_get_race(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g_has_race(g, id)) {
        for (int i = 0; i < g->race_list_count; i++) {
            if (g->race_list[i].id == id) {
                return g->race_list[i].race;
            }
        }
    }
    return RACE_NONE;
}

bool g_is_race(gamestate* const g, entityid id, race_t race) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_get_race(g, id) == race;
}

bool g_has_race(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_RACE);
}

direction_t g_get_direction(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g_has_component(g, id, C_DIRECTION)) {
        merror("id %d does not have a direction component", id);
        return DIR_NONE;
    }
    for (int i = 0; i < g->direction_list_count; i++) {
        if (g->direction_list[i].id == id) {
            return g->direction_list[i].dir;
        }
    }
    return DIR_NONE;
}

bool g_has_direction(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DIRECTION);
}

bool g_add_direction(gamestate* const g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(dir >= 0 && dir < DIR_COUNT, "dir is invalid");
    // make sure the entity has the component
    return g_add_component(
        g, id, C_DIRECTION, (void*)&dir, sizeof(direction_component), (void**)&g->direction_list, &g->direction_list_count, &g->direction_list_capacity);
}

bool g_is_direction(gamestate* const g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_get_direction(g, id) == dir;
}

bool g_update_direction(gamestate* const g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->direction_list == NULL) {
        merror("g->direction_list is NULL");
        return false;
    }
    for (int i = 0; i < g->direction_list_count; i++) {
        if (g->direction_list[i].id == id) {
            g->direction_list[i].dir = dir;
            return true;
        }
    }
    return false;
}

bool g_has_location(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_LOCATION);
}

bool g_add_location(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the location component
    return g_add_component(g, id, C_LOCATION, (void*)&loc, sizeof(loc_component), (void**)&g->loc_list, &g->loc_list_count, &g->loc_list_capacity);
}

bool g_update_location(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) {
        merror("g->loc_list is NULL");
        return false;
    }
    for (int i = 0; i < g->loc_list_count; i++) {
        if (g->loc_list[i].id == id) {
            g->loc_list[i].loc = loc;
            return true;
        }
    }
    return false;
}

loc_t g_get_location(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) {
        merror("g->loc_list is NULL");
        return (loc_t){-1, -1};
    }
    for (int i = 0; i < g->loc_list_count; i++) {
        if (g->loc_list[i].id == id) {
            return g->loc_list[i].loc;
        }
    }
    merror("id %d not found in loc_list", id);
    return (loc_t){-1, -1};
}

bool g_is_location(const gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) {
        merror("g->loc_list is NULL");
        return false;
    }
    loc_t loc2 = g_get_location(g, id);
    return (loc.x == loc2.x && loc.y == loc2.y);
}

bool g_has_sprite_move(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_SPRITE_MOVE);
}

bool g_add_sprite_move(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the sprite move component
    return g_add_component(g,
                           id,
                           C_SPRITE_MOVE,
                           (void*)&loc,
                           sizeof(sprite_move_component),
                           (void**)&g->sprite_move_list,
                           &g->sprite_move_list_count,
                           &g->sprite_move_list_capacity);
}

bool g_update_sprite_move(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_component(g, id, C_SPRITE_MOVE), "id %d does not have a sprite move component", id);
    if (g_has_sprite_move(g, id)) {
        if (g->sprite_move_list == NULL) {
            merror("g->sprite_move_list is NULL");
            return false;
        }
        for (int i = 0; i < g->sprite_move_list_count; i++) {
            if (g->sprite_move_list[i].id == id) {
                g->sprite_move_list[i].loc = loc;
                return true;
            }
        }
    }
    return false;
}

loc_t g_get_sprite_move(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_sprite_move(g, id), "id %d does not have a sprite move component", id);
    if (g_has_sprite_move(g, id)) {
        for (int i = 0; i < g->sprite_move_list_count; i++) {
            if (g->sprite_move_list[i].id == id) {
                return g->sprite_move_list[i].loc;
            }
        }
    }
    merror("id %d not found in sprite_move_list", id);
    return (loc_t){-1, -1};
}

bool g_has_dead(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DEAD);
}

bool g_add_dead(gamestate* const g, entityid id, bool dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the dead component
    return g_add_component(g, id, C_DEAD, (void*)&dead, sizeof(dead_component), (void**)&g->dead_list, &g->dead_list_count, &g->dead_list_capacity);
}

bool g_update_dead(gamestate* const g, entityid id, bool dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_component(g, id, C_DEAD), "id %d does not have a dead component", id);
    if (g->dead_list == NULL) {
        merror("g->dead_list is NULL");
        return false;
    }
    for (int i = 0; i < g->dead_list_count; i++) {
        if (g->dead_list[i].id == id) {
            g->dead_list[i].dead = dead;
            return true;
        }
    }
    return false;
}

bool g_is_dead(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    if (id != ENTITYID_INVALID) {
        if (g->dead_list == NULL) {
            merror("g->dead_list is NULL");
            return false;
        }
        for (int i = 0; i < g->dead_list_count; i++) {
            if (g->dead_list[i].id == id) {
                return g->dead_list[i].dead;
            }
        }
    }
    return false;
}

bool g_has_update(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_UPDATE);
}

bool g_add_update(gamestate* const g, entityid id, bool update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the update component
    return g_add_component(g, id, C_UPDATE, (void*)&update, sizeof(update_component), (void**)&g->update_list, &g->update_list_count, &g->update_list_capacity);
}

bool g_get_update(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->update_list == NULL) {
        merror("g->update_list is NULL");
        return false;
    }
    for (int i = 0; i < g->update_list_count; i++) {
        if (g->update_list[i].id == id) {
            return g->update_list[i].update;
        }
    }
    merror("id %d not found in update_list", id);
    return false;
}

bool g_set_update(gamestate* const g, entityid id, bool update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->update_list == NULL) {
        merror("g->update_list is NULL");
        return false;
    }
    for (int i = 0; i < g->update_list_count; i++) {
        if (g->update_list[i].id == id) {
            g->update_list[i].update = update;
            return true;
        }
    }
    return false;
}

bool g_has_attacking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_ATTACKING);
}

bool g_add_attacking(gamestate* const g, entityid id, bool attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the attacking component
    return g_add_component(
        g, id, C_ATTACKING, (void*)&attacking, sizeof(attacking_component), (void**)&g->attacking_list, &g->attacking_list_count, &g->attacking_list_capacity);
}

bool g_set_attacking(gamestate* const g, entityid id, bool attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->attacking_list == NULL) {
        merror("g->attacking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->attacking_list_count; i++) {
        if (g->attacking_list[i].id == id) {
            g->attacking_list[i].attacking = attacking;
            return true;
        }
    }
    return false;
}

bool g_get_attacking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->attacking_list == NULL) {
        merror("g->attacking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->attacking_list_count; i++) {
        if (g->attacking_list[i].id == id) {
            return g->attacking_list[i].attacking;
        }
    }
    //merror("id %d not found in attacking_list", id);
    return false;
}

bool g_has_blocking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_BLOCKING);
}

bool g_add_blocking(gamestate* const g, entityid id, bool blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the blocking component
    massert(g_has_component(g, id, C_BLOCKING), "id %d does not have a blocking component", id);
    return g_add_component(
        g, id, C_BLOCKING, (void*)&blocking, sizeof(blocking_component), (void**)&g->blocking_list, &g->blocking_list_count, &g->blocking_list_capacity);
}

bool g_set_blocking(gamestate* const g, entityid id, bool blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->blocking_list == NULL) {
        merror("g->blocking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->blocking_list_count; i++) {
        if (g->blocking_list[i].id == id) {
            g->blocking_list[i].blocking = blocking;
            return true;
        }
    }
    return false;
}

bool g_get_blocking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->blocking_list == NULL) {
        merror("g->blocking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->blocking_list_count; i++) {
        if (g->blocking_list[i].id == id) {
            return g->blocking_list[i].blocking;
        }
    }
    merror("id %d not found in blocking_list", id);
    return false;
}

bool g_has_block_success(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_BLOCK_SUCCESS);
}

bool g_add_block_success(gamestate* const g, entityid id, bool block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the block success component
    return g_add_component(g,
                           id,
                           C_BLOCK_SUCCESS,
                           (void*)&block_success,
                           sizeof(block_success_component),
                           (void**)&g->block_success_list,
                           &g->block_success_list_count,
                           &g->block_success_list_capacity);
}

bool g_set_block_success(gamestate* const g, entityid id, bool block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->block_success_list == NULL) {
        merror("g->block_success_list is NULL");
        return false;
    }
    for (int i = 0; i < g->block_success_list_count; i++) {
        if (g->block_success_list[i].id == id) {
            g->block_success_list[i].block_success = block_success;
            return true;
        }
    }
    return false;
}

bool g_get_block_success(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->block_success_list == NULL) {
        merror("g->block_success_list is NULL");
        return false;
    }
    for (int i = 0; i < g->block_success_list_count; i++) {
        if (g->block_success_list[i].id == id) {
            return g->block_success_list[i].block_success;
        }
    }
    return false;
}

bool g_has_damaged(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DAMAGED);
}

bool g_add_damaged(gamestate* const g, entityid id, bool damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the damaged component
    return g_add_component(
        g, id, C_DAMAGED, (void*)&damaged, sizeof(damaged_component), (void**)&g->damaged_list, &g->damaged_list_count, &g->damaged_list_capacity);
}

bool g_set_damaged(gamestate* const g, entityid id, bool damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damaged_list == NULL) {
        merror("g->damaged_list is NULL");
        return false;
    }
    for (int i = 0; i < g->damaged_list_count; i++) {
        if (g->damaged_list[i].id == id) {
            g->damaged_list[i].damaged = damaged;
            return true;
        }
    }
    return false;
}

bool g_get_damaged(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damaged_list == NULL) {
        merror("g->damaged_list is NULL");
        return false;
    }
    for (int i = 0; i < g->damaged_list_count; i++) {
        if (g->damaged_list[i].id == id) {
            return g->damaged_list[i].damaged;
        }
    }
    return false;
}

bool g_has_default_action(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DEFAULT_ACTION);
}

bool g_add_default_action(gamestate* const g, entityid id, entity_action_t action) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the default action component
    return g_add_component(g,
                           id,
                           C_DEFAULT_ACTION,
                           (void*)&action,
                           sizeof(default_action_component),
                           (void**)&g->default_action_list,
                           &g->default_action_list_count,
                           &g->default_action_list_capacity);
}

bool g_set_default_action(gamestate* const g, entityid id, entity_action_t action) {
    massert(g, "g is NULL");
    //massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) {
        merror("id is invalid");
        return false;
    }
    if (g->default_action_list == NULL) {
        merror("g->default_action_list is NULL");
        return false;
    }
    for (int i = 0; i < g->default_action_list_count; i++) {
        if (g->default_action_list[i].id == id) {
            g->default_action_list[i].action = action;
            return true;
        }
    }
    return false;
}

entity_action_t g_get_default_action(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->default_action_list == NULL) {
        merror("g->default_action_list is NULL");
        return ENTITY_ACTION_NONE;
    }
    for (int i = 0; i < g->default_action_list_count; i++) {
        if (g->default_action_list[i].id == id) {
            return g->default_action_list[i].action;
        }
    }
    return ENTITY_ACTION_NONE;
}

bool g_has_inventory(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_INVENTORY);
}

bool g_add_inventory(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the inventory component
    return g_add_component(
        g, id, C_INVENTORY, (void*)NULL, sizeof(inventory_component), (void**)&g->inventory_list, &g->inventory_list_count, &g->inventory_list_capacity);
    return true;
}

bool g_add_to_inventory(gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) {
        merror("g->inventory_list is NULL");
        return false;
    }
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            if (g->inventory_list[i].count >= MAX_INVENTORY_SIZE) {
                merror("inventory is full");
                return false;
            }
            g->inventory_list[i].inventory[g->inventory_list[i].count++] = itemid;
            msuccess("added item %d to inventory %d", itemid, id);
            return true;
        }
    }
    return false;
}

bool g_remove_from_inventory(gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) {
        merror("g->inventory_list is NULL");
        return false;
    }
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            for (int j = 0; j < g->inventory_list[i].count; j++) {
                if (g->inventory_list[i].inventory[j] == itemid) {
                    g->inventory_list[i].inventory[j] = g->inventory_list[i].inventory[g->inventory_list[i].count - 1];
                    g->inventory_list[i].count--;
                    msuccess("removed item %d from inventory %d", itemid, id);
                    return true;
                }
            }
        }
    }
    return false;
}

entityid* g_get_inventory(const gamestate* const g, entityid id, int* count) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(count, "count is NULL");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) {
        merror("g->inventory_list is NULL");
        return NULL;
    }
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            *count = g->inventory_list[i].count;
            return g->inventory_list[i].inventory;
        }
    }
    *count = 0;
    return NULL;
}

size_t g_get_inventory_count(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) {
        merror("g->inventory_list is NULL");
        return 0;
    }
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            return g->inventory_list[i].count;
        }
    }
    return 0;
}

bool g_has_item_in_inventory(const gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) {
        merror("g->inventory_list is NULL");
        return false;
    }
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            for (int j = 0; j < g->inventory_list[i].count; j++) {
                if (g->inventory_list[i].inventory[j] == itemid) {
                    return true;
                }
            }
        }
    }
    return false; //
}

bool g_has_target(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_TARGET);
}

bool g_add_target(gamestate* const g, entityid id, loc_t target) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the target component
    //massert(g_has_component(g, id, C_TARGET), "id %d does not have a target component", id);
    return g_add_component(g, id, C_TARGET, (void*)&target, sizeof(target_component), (void**)&g->target_list, &g->target_list_count, &g->target_list_capacity);
}

bool g_set_target(gamestate* const g, entityid id, loc_t target) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->target_list == NULL) {
        merror("g->target_list is NULL");
        return false;
    }
    for (int i = 0; i < g->target_list_count; i++) {
        if (g->target_list[i].id == id) {
            g->target_list[i].target = target;
            return true;
        }
    }
    return false;
}

bool g_get_target(const gamestate* const g, entityid id, loc_t* target) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(target, "target is NULL");
    if (g->target_list == NULL) {
        merror("g->target_list is NULL");
        return false;
    }
    for (int i = 0; i < g->target_list_count; i++) {
        if (g->target_list[i].id == id) {
            *target = g->target_list[i].target;
            return true;
        }
    }
    return false;
}

bool g_has_target_path(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_TARGET_PATH);
}

bool g_add_target_path(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the target path component
    //massert(g_has_component(g, id, C_TARGET_PATH), "id %d does not have a target path component", id);
    return g_add_component(g,
                           id,
                           C_TARGET_PATH,
                           (void*)NULL,
                           sizeof(target_path_component),
                           (void**)&g->target_path_list,
                           &g->target_path_list_count,
                           &g->target_path_list_capacity);
    return true;
}

bool g_set_target_path(gamestate* const g, entityid id, loc_t* target_path, int target_path_length) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->target_path_list == NULL) {
        merror("g->target_path_list is NULL");
        return false;
    }
    for (int i = 0; i < g->target_path_list_count; i++) {
        if (g->target_path_list[i].id == id) {
            g->target_path_list[i].target_path = target_path;
            g->target_path_list[i].target_path_length = target_path_length;
            return true;
        }
    }
    return false;
}

bool g_get_target_path(const gamestate* const g, entityid id, loc_t** target_path, int* target_path_length) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(target_path, "target_path is NULL");
    massert(target_path_length, "target_path_length is NULL");
    if (g->target_path_list == NULL) {
        merror("g->target_path_list is NULL");
        return false;
    }
    for (int i = 0; i < g->target_path_list_count; i++) {
        if (g->target_path_list[i].id == id) {
            *target_path = g->target_path_list[i].target_path;
            *target_path_length = g->target_path_list[i].target_path_length;
            return true;
        }
    }
    return false;
}

bool g_get_target_path_length(const gamestate* const g, entityid id, int* target_path_length) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(target_path_length, "target_path_length is NULL");
    if (g->target_path_list == NULL) {
        merror("g->target_path_list is NULL");
        return false;
    }
    for (int i = 0; i < g->target_path_list_count; i++) {
        if (g->target_path_list[i].id == id) {
            *target_path_length = g->target_path_list[i].target_path_length;
            return true;
        }
    }
    return false;
}

bool g_add_equipment(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the equipment component
    //massert(g_has_component(g, id, C_EQUIPMENT), "id %d does not have an equipment component", id);
    return g_add_component(
        g, id, C_EQUIPMENT, (void*)NULL, sizeof(equipment_component), (void**)&g->equipment_list, &g->equipment_list_count, &g->equipment_list_capacity);
}

bool g_has_equipment(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_EQUIPMENT);
}

bool g_set_equipment(gamestate* const g, entityid id, equipment_slot slot, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    if (g->equipment_list != NULL) {
        for (int i = 0; i < g->equipment_list_count; i++) {
            if (g->equipment_list[i].id == id) {
                g->equipment_list[i].equipment[slot] = itemid;
                return true;
            }
        }
    }
    return false;
}

bool g_unset_equipment(gamestate* const g, entityid id, equipment_slot slot) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->equipment_list != NULL) {
        for (int i = 0; i < g->equipment_list_count; i++) {
            if (g->equipment_list[i].id == id) {
                g->equipment_list[i].equipment[slot] = ENTITYID_INVALID;
                return true;
            }
        }
    }
    return false;
}

entityid g_get_equipment(const gamestate* const g, entityid id, equipment_slot slot) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->equipment_list != NULL) {
        for (int i = 0; i < g->equipment_list_count; i++) {
            if (g->equipment_list[i].id == id) {
                return g->equipment_list[i].equipment[slot];
            }
        }
    }
    return ENTITYID_INVALID;
}

bool g_is_equipped(const gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    if (g->equipment_list != NULL) {
        for (int i = 0; i < g->equipment_list_count; i++) {
            if (g->equipment_list[i].id == id) {
                for (int j = 0; j < EQUIPMENT_SLOT_COUNT; j++) {
                    if (g->equipment_list[i].equipment[j] == itemid) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

dungeon_t* g_get_dungeon(gamestate* const g) {
    massert(g, "g is NULL");
    return g->d;
}

bool g_add_stats(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the stats component
    //massert(g_has_component(g, id, C_STATS), "id %d does not have a stats component", id);
    return g_add_component(g, id, C_STATS, (void*)NULL, sizeof(stats_component), (void**)&g->stats_list, &g->stats_list_count, &g->stats_list_capacity);
}

bool g_has_stats(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_STATS);
}

bool g_set_stat(gamestate* const g, entityid id, stats_slot stats_slot, int value) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g->stats_list, "g->stats_list is NULL");
    if (g->stats_list == NULL) {
        merror("g->stats_list is NULL");
        return false;
    }
    for (int i = 0; i < g->stats_list_count; i++) {
        if (g->stats_list[i].id == id) {
            g->stats_list[i].stats[stats_slot] = value;
            return true;
        }
    }
    return false;
}

int* g_get_stats(const gamestate* const g, entityid id, int* count) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(count, "count is NULL");
    massert(g->stats_list, "g->stats_list is NULL");
    if (g->stats_list == NULL) {
        merror("g->stats_list is NULL");
        return NULL;
    }
    for (int i = 0; i < g->stats_list_count; i++) {
        if (g->stats_list[i].id == id) {
            *count = STATS_COUNT;
            return g->stats_list[i].stats;
        }
    }
    *count = 0;
    return NULL;
}

int g_get_stat(const gamestate* const g, entityid id, stats_slot stats_slot) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g->stats_list, "g->stats_list is NULL");
    if (g->stats_list == NULL) {
        merror("g->stats_list is NULL");
        return 0;
    }
    for (int i = 0; i < g->stats_list_count; i++) {
        if (g->stats_list[i].id == id) {
            return g->stats_list[i].stats[stats_slot];
        }
    }
    return 0;
}

bool g_add_itemtype(gamestate* const g, entityid id, itemtype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the itemtype component
    //massert(g_has_component(g, id, C_ITEMTYPE), "id %d does not have an itemtype component", id);
    return g_add_component(
        g, id, C_ITEMTYPE, (void*)&type, sizeof(itemtype_component), (void**)&g->itemtype_list, &g->itemtype_list_count, &g->itemtype_list_capacity);
}

bool g_has_itemtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_ITEMTYPE);
}

bool g_set_itemtype(gamestate* const g, entityid id, itemtype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->itemtype_list == NULL) {
        merror("g->itemtype_list is NULL");
        return false;
    }
    for (int i = 0; i < g->itemtype_list_count; i++) {
        if (g->itemtype_list[i].id == id) {
            g->itemtype_list[i].type = type;
            return true;
        }
    }
    return false;
}

itemtype g_get_itemtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->itemtype_list == NULL) {
        merror("g->itemtype_list is NULL");
        return ITEM_NONE;
    }
    for (int i = 0; i < g->itemtype_list_count; i++) {
        if (g->itemtype_list[i].id == id) {
            return g->itemtype_list[i].type;
        }
    }
    return ITEM_NONE;
}

bool g_add_weapontype(gamestate* const g, entityid id, weapontype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the weapontype component
    return g_add_component(
        g, id, C_WEAPONTYPE, (void*)&type, sizeof(weapontype_component), (void**)&g->weapontype_list, &g->weapontype_list_count, &g->weapontype_list_capacity);
}

bool g_has_weapontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_WEAPONTYPE);
}

bool g_set_weapontype(gamestate* const g, entityid id, weapontype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->weapontype_list == NULL) {
        merror("g->weapontype_list is NULL");
        return false;
    }
    for (int i = 0; i < g->weapontype_list_count; i++) {
        if (g->weapontype_list[i].id == id) {
            g->weapontype_list[i].type = type;
            return true;
        }
    }
    return false;
}

weapontype g_get_weapontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    //massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) {
        merror("id is invalid");
        return WEAPON_NONE;
    }
    if (g->weapontype_list == NULL) {
        merror("g->weapontype_list is NULL");
        return WEAPON_NONE;
    }
    for (int i = 0; i < g->weapontype_list_count; i++) {
        if (g->weapontype_list[i].id == id) {
            return g->weapontype_list[i].type;
        }
    }
    return WEAPON_NONE;
}

bool g_is_weapontype(const gamestate* const g, entityid id, weapontype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->weapontype_list == NULL) {
        merror("g->weapontype_list is NULL");
        return false;
    }
    for (int i = 0; i < g->weapontype_list_count; i++) {
        if (g->weapontype_list[i].id == id) {
            return g->weapontype_list[i].type == type;
        }
    }
    return false;
}

bool g_add_shieldtype(gamestate* const g, entityid id, shieldtype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the shieldtype component
    return g_add_component(
        g, id, C_SHIELDTYPE, (void*)&type, sizeof(shieldtype_component), (void**)&g->shieldtype_list, &g->shieldtype_list_count, &g->shieldtype_list_capacity);
}

bool g_has_shieldtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_SHIELDTYPE);
}

bool g_set_shieldtype(gamestate* const g, entityid id, shieldtype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->shieldtype_list == NULL) {
        merror("g->shieldtype_list is NULL");
        return false;
    }
    for (int i = 0; i < g->shieldtype_list_count; i++) {
        if (g->shieldtype_list[i].id == id) {
            g->shieldtype_list[i].type = type;
            return true;
        }
    }
    return false;
}

shieldtype g_get_shieldtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->shieldtype_list == NULL) {
        merror("g->shieldtype_list is NULL");
        return SHIELD_NONE;
    }
    for (int i = 0; i < g->shieldtype_list_count; i++) {
        if (g->shieldtype_list[i].id == id) {
            return g->shieldtype_list[i].type;
        }
    }
    return SHIELD_NONE;
}

bool g_is_shieldtype(const gamestate* const g, entityid id, shieldtype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->shieldtype_list == NULL) {
        merror("g->shieldtype_list is NULL");
        return false;
    }
    for (int i = 0; i < g->shieldtype_list_count; i++) {
        if (g->shieldtype_list[i].id == id) {
            return g->shieldtype_list[i].type == type;
        }
    }
    return false;
}

bool g_add_potiontype(gamestate* const g, entityid id, potiontype type) {
    minfo("g_add_potiontype");
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the potiontype component
    return g_add_component(g,
                           id,
                           C_POTIONTYPE,
                           (void*)&type,
                           sizeof(potiontype_component),
                           (void**)&g->potion_type_list,
                           &g->potion_type_list_count,
                           &g->potion_type_list_capacity);
}

bool g_has_potiontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_POTIONTYPE);
}

bool g_set_potiontype(gamestate* const g, entityid id, potiontype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->potion_type_list == NULL) {
        merror("g->potion_type_list is NULL");
        return false;
    }
    for (int i = 0; i < g->potion_type_list_count; i++) {
        if (g->potion_type_list[i].id == id) {
            g->potion_type_list[i].type = type;
            return true;
        }
    }
    return false;
}

bool g_is_potion(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->potion_type_list == NULL) {
        merror("g->potion_type_list is NULL");
        return false;
    }
    return g_has_potiontype(g, id);
    //for (int i = 0; i < g->potion_type_list_count; i++) {
    //    if (g->potion_type_list[i].id == id) {
    //        return g->potion_type_list[i].type == POTION_NONE;
    //    }
    //}
    //return false;
}

potiontype g_get_potiontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->potion_type_list == NULL) {
        merror("g->potion_type_list is NULL");
        return POTION_NONE;
    }
    for (int i = 0; i < g->potion_type_list_count; i++) {
        if (g->potion_type_list[i].id == id) {
            return g->potion_type_list[i].type;
        }
    }
    return POTION_NONE;
}

bool g_add_damage(gamestate* const g, entityid id, roll r) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the damage component
    //massert(g_has_component(g, id, C_DAMAGE), "id %d does not have a damage component", id);
    return g_add_component(g, id, C_DAMAGE, (void*)&r, sizeof(damage_component), (void**)&g->damage_list, &g->damage_list_count, &g->damage_list_capacity);
}

bool g_has_damage(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DAMAGE);
}

bool g_set_damage(gamestate* const g, entityid id, roll r) {
    massert(g, "g is NULL");
    //massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) {
        merror("id is invalid");
        return false;
    }
    if (g->damage_list == NULL) {
        merror("g->damage_list is NULL");
        return false;
    }
    for (int i = 0; i < g->damage_list_count; i++) {
        if (g->damage_list[i].id == id) {
            g->damage_list[i].r = r;
            return true;
        }
    }
    merror("Could not find id %d in damage list", id);
    return false;
}

roll g_get_damage(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    //massert(damage, "damage is NULL");
    if (g->damage_list == NULL) {
        merror("g->damage_list is NULL");
        return (roll){0};
    }
    for (int i = 0; i < g->damage_list_count; i++) {
        if (g->damage_list[i].id == id) {
            return g->damage_list[i].r;
        }
    }
    return (roll){0};
}

bool g_add_ac(gamestate* const g, entityid id, int ac) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the ac component
    return g_add_component(g, id, C_AC, (void*)&ac, sizeof(ac_component), (void**)&g->ac_list, &g->ac_list_count, &g->ac_list_capacity);
}

bool g_has_ac(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_AC);
}

bool g_set_ac(gamestate* const g, entityid id, int ac) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->ac_list == NULL) {
        merror("g->ac_list is NULL");
        return false;
    }
    for (int i = 0; i < g->ac_list_count; i++) {
        if (g->ac_list[i].id == id) {
            g->ac_list[i].ac = ac;
            return true;
        }
    }
    return false;
}

int g_get_ac(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->ac_list == NULL) {
        merror("g->ac_list is NULL");
        return 0;
    }
    for (int i = 0; i < g->ac_list_count; i++) {
        if (g->ac_list[i].id == id) {
            return g->ac_list[i].ac;
        }
    }
    return 0;
}

entityid g_add_entity(gamestate* const g) {
    entityid id = g->next_entityid;
    if (!g->dirty_entities) {
        g->dirty_entities = true;
        g->new_entityid_begin = id;
        g->new_entityid_end = id + 1;
    } else {
        //g->dirty_entities = true;
        g->new_entityid_end = id + 1;
    }
    g->next_entityid++;
    return id;
}
