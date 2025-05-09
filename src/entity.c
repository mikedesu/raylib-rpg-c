#include "entity.h"
#include "massert.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

entity_t* e_new(entityid id, entitytype_t type) {
    entity_t* e = malloc(sizeof(entity_t));
    massert(e, "Failed to allocate memory for entity");
    massert(id >= 0, "id is less than 0");
    massert(type >= 0, "type is less than 0");
    massert(type < ENTITY_TYPE_COUNT, "type is greater than ENTITY_TYPE_COUNT");
    e->id = id;
    //e->type = type;
    //e->race = RACE_NONE;
    //e->direction = DIR_RIGHT;
    e->default_action = ENTITY_ACTION_MOVE_RANDOM; // Default for NPCs, can be overridden
    //e->x = 0;
    //e->y = 0;
    //e->floor = 0;
    //e->sprite_move_x = 0;
    //e->sprite_move_y = 0;
    e->inventory_count = 0;
    e->weapon = -1;
    e->shield = -1;
    e->stats.hp = 1;
    e->stats.maxhp = 1;
    e->stats.level = 1;
    e->stats.mp = 0;
    e->stats.maxmp = 0;
    e->do_update = e->is_attacking = e->is_blocking = e->is_damaged = e->dead = e->door_is_open = false;
    e->potion_type = POTION_NONE;
    e->next = NULL;
    e->target_path = NULL;
    e->target_path_length = 0;
    e->target = (loc_t){-1, -1, -1};
    //strncpy(e->name, "NONAME", ENTITY_NAME_LEN_MAX);
    return e;
}

// one caveat to the way entities are handled:
// entities may contain a "next" for use with the entitymap
// you MUST acquire the next pointer externally before calling this function
// otherwise things will break badly!
void e_free(entity_t* e) {
    massert(e, "e is NULL");
    massert(e->next == NULL, "e->next is not NULL. You MUST acquire the next pointer externally and set it to NULL before calling this function");
    if (e->target_path) {
        free(e->target_path);
        e->target_path = NULL;
        e->target_path_length = 0;
    }
    free(e);
}

void e_free_target_path(entity_t* const e) {
    massert(e, "e is NULL");
    if (e->target_path) {
        free(e->target_path);
        e->target_path = NULL;
        e->target_path_length = 0;
    }
}

entity_t* e_new_at(entityid id, entitytype_t type, int x, int y, int floor) {
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    massert(floor >= 0, "floor is less than 0");
    entity_t* e = e_new(id, type);
    massert(e, "Failed to create entity");
    //e_set_xy(e, x, y);
    //e_set_floor(e, floor);
    return e;
}

bool e_add_item_to_inventory(entity_t* const e, entityid item_id) {
    massert(e, "e is NULL");
    massert(item_id >= 0, "item_id is less than 0");
    if (e->inventory_count >= ENTITY_INVENTORY_MAX_SIZE) {
        merror("inventory is full");
        return false;
    }
    if (e_item_is_in_inventory(e, item_id)) {
        merror("item is already in inventory");
        return false;
    }
    e->inventory[e->inventory_count++] = item_id;
    return true;
}

bool e_remove_item_from_inventory(entity_t* const e, entityid item_id) {
    massert(e, "e is NULL");
    massert(item_id >= 0, "item_id is less than 0");
    if (e->inventory_count <= 0) {
        merror("inventory is empty");
        return false;
    }
    if (!e_item_is_in_inventory(e, item_id)) {
        merror("item is not in inventory");
        return false;
    }
    for (int i = 0; i < e->inventory_count; i++) {
        if (e->inventory[i] == item_id) {
            e->inventory[i] = e->inventory[--e->inventory_count];
            return true;
        }
    }
    return false;
}
