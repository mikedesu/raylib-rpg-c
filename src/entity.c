#include "entity.h"
#include "massert.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

entity_t* entity_new(entityid id, entitytype_t type) {
    entity_t* e = malloc(sizeof(entity_t));
    massert(e, "Failed to allocate memory for entity");
    massert(id >= 0, "id is less than 0");
    massert(type >= 0, "type is less than 0");
    massert(type < ENTITY_TYPE_COUNT, "type is greater than ENTITY_TYPE_COUNT");
    e->id = id;
    e->type = type;
    e->race = RACE_NONE;
    e->direction = DIR_RIGHT;
    e->x = -1;
    e->y = -1;
    e->weapon = -1;
    e->shield = -1;
    e->floor = -1;
    e->hp = 1;
    e->maxhp = 1;
    e->do_update = false;
    e->is_attacking = false;
    e->is_blocking = false;
    e->is_damaged = false;
    e->is_dead = false;
    e->sprite_move_x = 0;
    e->sprite_move_y = 0;

    //e->inventory = NULL;
    //e->inventory_max_size = 0;
    e->inventory_count = 0;

    strncpy(e->name, "NONAME", ENTITY_NAME_LEN_MAX);
    //e->next = NULL;
    e->default_action = ENTITY_ACTION_MOVE_RANDOM; // Default for NPCs, can be overridden
    return e;
}

entity_t* entity_new_at(entityid id, entitytype_t type, int x, int y, int floor) {
    massert(x >= 0, "entity_new_at: x is less than 0");
    massert(y >= 0, "entity_new_at: y is less than 0");
    massert(floor >= 0, "entity_new_at: floor is less than 0");
    entity_t* e = entity_new(id, type);
    massert(e, "Failed to create entity");
    entity_set_xy(e, x, y);
    entity_set_floor(e, floor);
    return e;
}

bool entity_item_is_already_in_inventory(entity_t* const e, entityid item_id) {
    massert(e, "entity_item_is_already_in_inventory: e is NULL");
    massert(item_id >= 0, "entity_item_is_already_in_inventory: item_id is less than 0");
    for (int i = 0; i < e->inventory_count; i++) {
        if (e->inventory[i] == item_id) return true;
    }
    return false;
}

bool entity_add_item_to_inventory(entity_t* const e, entityid item_id) {
    massert(e, "entity_add_item_to_inventory: e is NULL");
    massert(item_id >= 0, "entity_add_item_to_inventory: item_id is less than 0");
    if (e->inventory_count >= ENTITY_INVENTORY_MAX_SIZE) {
        merror("entity_add_item_to_inventory: inventory is full");
        return false;
    }
    if (entity_item_is_already_in_inventory(e, item_id)) {
        merror("entity_add_item_to_inventory: item is already in inventory");
        return false;
    }
    e->inventory[e->inventory_count++] = item_id;
    return true;
}

bool entity_remove_item_from_inventory(entity_t* const e, entityid item_id) {
    massert(e, "entity_remove_item_from_inventory: e is NULL");
    massert(item_id >= 0, "entity_remove_item_from_inventory: item_id is less than 0");

    if (e->inventory_count <= 0) {
        merror("entity_remove_item_from_inventory: inventory is empty");
        return false;
    }

    if (!entity_item_is_already_in_inventory(e, item_id)) {
        merror("entity_remove_item_from_inventory: item is not in inventory");
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

void entity_set_floor(entity_t* const e, int floor) {
    massert(e, "entity_set_floor: e is NULL");
    massert(floor >= 0, "entity_set_floor: floor is less than 0");
    e->floor = floor;
}

entity_t* entity_new_npc_at(entityid id, race_t r, int x, int y, int floor, const char* name) {
    entity_t* e = entity_new_at(id, ENTITY_NPC, x, y, floor);
    massert(e, "Failed to create entity");
    entity_set_race(e, r);
    entity_set_name(e, name);
    return e;
}

entity_t* entity_new_weapon_at(entityid id, int x, int y, int floor, const char* name) {
    entity_t* e = entity_new_at(id, ENTITY_WEAPON, x, y, floor);
    massert(e, "Failed to create entity");
    //entity_set_race(e, r);
    entity_set_name(e, name);
    return e;
}

entity_t* entity_new_shield_at(entityid id, int x, int y, int floor, const char* name) {
    entity_t* e = entity_new_at(id, ENTITY_SHIELD, x, y, floor);
    massert(e, "Failed to create entity");
    //entity_set_race(e, r);
    entity_set_name(e, name);
    return e;
}

// this frees the entity so you lose the next pointer!
// make sure you wire the map correctly before calling this!
void entity_free(entity_t* e) {
    if (!e) return;
    free(e);
}

void entity_set_name(entity_t* const e, const char* name) {
    massert(e, "entity_set_name: e is NULL");
    massert(name, "entity_set_name: name is NULL");
    massert(strlen(name) < ENTITY_NAME_LEN_MAX, "entity_set_name: name is too long");
    strncpy(e->name, name, ENTITY_NAME_LEN_MAX);
}

void entity_set_race(entity_t* const e, race_t race) {
    massert(e, "entity is null");
    massert(race >= 0, "race is less than 0");
    massert(race < RACE_COUNT, "race is greater than RACE_COUNT");
    e->race = race;
}

void entity_set_x(entity_t* const e, int x) {
    massert(e, "entity is null");
    massert(x >= 0, "x is less than 0");
    e->x = x;
}

void entity_set_y(entity_t* const e, int y) {
    massert(e, "entity is null");
    massert(y >= 0, "y is less than 0");
    e->y = y;
}

void entity_set_xy(entity_t* const e, int x, int y) {
    entity_set_x(e, x);
    entity_set_y(e, y);
}

void entity_incr_x(entity_t* const e) { entity_set_x(e, e->x + 1); }
void entity_incr_y(entity_t* const e) { entity_set_y(e, e->y + 1); }
void entity_decr_x(entity_t* const e) { entity_set_x(e, e->x - 1); }
void entity_decr_y(entity_t* const e) { entity_set_y(e, e->y - 1); }

void entity_set_dir(entity_t* const e, direction_t dir) {
    massert(e, "entity_set_dir: e is NULL");
    massert(dir >= 0, "entity_set_dir: dir is less than 0");
    massert(dir < DIR_COUNT, "entity_set_dir: dir is greater than DIR_COUNT");
    e->direction = dir;
}

direction_t entity_get_dir(entity_t* const e) {
    massert(e, "e is NULL");
    return e->direction;
}

void entity_set_type(entity_t* const e, entitytype_t type) {
    massert(e, "entity_set_type: e is NULL");
    massert(type >= 0, "entity_set_type: type is less than 0");
    massert(type < ENTITY_TYPE_COUNT, "entity_set_type: type is greater than ENTITY_COUNT");
    e->type = type;
}

entitytype_t entity_get_type(entity_t* const e) {
    massert(e, "entity_get_type: e is NULL");
    return e->type;
}

void entity_set_default_action(entity_t* const e, entity_action_t action) {
    massert(e, "entity_set_default_action: e is NULL");
    massert(action >= 0, "entity_set_default_action: action is less than 0");
    massert(action < ENTITY_ACTION_COUNT, "entity_set_default_action: action is greater than ENTITY_ACTION_COUNT");
    e->default_action = action;
}

entity_action_t entity_get_default_action(entity_t* const e) {
    massert(e, "entity_get_default_action: e is NULL");
    return e->default_action;
}

void entity_set_hp(entity_t* const e, int hp) {
    massert(e, "entity_set_hp: e is NULL");
    e->hp = hp < 0 ? 0 : hp > e->maxhp ? e->maxhp : hp;
}

int entity_get_hp(entity_t* const e) {
    massert(e, "entity_get_hp: e is NULL");
    return e->hp;
}

void entity_set_maxhp(entity_t* const e, int maxhp) {
    massert(e, "entity_set_maxhp: e is NULL");
    massert(maxhp > 0, "entity_set_maxhp: maxhp is less than 1");
    e->maxhp = maxhp;
}

int entity_get_maxhp(entity_t* const e) {
    massert(e, "entity_get_maxhp: e is NULL");
    return e->maxhp;
}

void entity_set_hpmaxhp(entity_t* const e, int hp, int maxhp) {
    entity_set_hp(e, hp);
    entity_set_maxhp(e, maxhp);
}

void entity_incr_hp(entity_t* const e, int hp) {
    massert(e, "entity_incr_hp: e is NULL");
    entity_set_hp(e, e->hp + hp);
}

void entity_decr_hp(entity_t* const e, int hp) {
    massert(e, "entity_decr_hp: e is NULL");
    entity_set_hp(e, e->hp - hp);
}

int entity_get_inventory_count(entity_t* const e) {
    massert(e, "entity_get_inventory_count: e is NULL");
    return e->inventory_count;
}
