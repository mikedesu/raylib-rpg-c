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
    e->type = type;
    e->race = RACE_NONE;
    e->direction = DIR_RIGHT;
    e->default_action = ENTITY_ACTION_MOVE_RANDOM; // Default for NPCs, can be overridden

    e->x = e->y = e->sprite_move_x = e->sprite_move_y = e->inventory_count = 0;
    e->weapon = e->shield = e->floor = -1;
    e->stats.hp = e->stats.maxhp = 1;
    e->do_update = e->is_attacking = e->is_blocking = e->is_damaged = e->dead = false;

    e->next = NULL;
    strncpy(e->name, "NONAME", ENTITY_NAME_LEN_MAX);
    return e;
}

entity_t* e_new_at(entityid id, entitytype_t type, int x, int y, int floor) {
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    massert(floor >= 0, "floor is less than 0");
    entity_t* e = e_new(id, type);
    massert(e, "Failed to create entity");
    e_set_xy(e, x, y);
    e_set_floor(e, floor);
    return e;
}

bool e_item_is_already_in_inventory(entity_t* const e, entityid item_id) {
    massert(e, "e is NULL");
    massert(item_id >= 0, "item_id is less than 0");
    for (int i = 0; i < e->inventory_count; i++) {
        if (e->inventory[i] == item_id) return true;
    }
    return false;
}

bool e_add_item_to_inventory(entity_t* const e, entityid item_id) {
    massert(e, "e is NULL");
    massert(item_id >= 0, "item_id is less than 0");
    if (e->inventory_count >= ENTITY_INVENTORY_MAX_SIZE) {
        merror("inventory is full");
        return false;
    }
    if (e_item_is_already_in_inventory(e, item_id)) {
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

    if (!e_item_is_already_in_inventory(e, item_id)) {
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

void e_set_floor(entity_t* const e, int floor) {
    massert(e, "e is NULL");
    massert(floor >= 0, "floor is less than 0");
    e->floor = floor;
}

entity_t* e_new_npc_at(entityid id, race_t r, int x, int y, int floor, const char* name) {
    entity_t* e = e_new_at(id, ENTITY_NPC, x, y, floor);
    massert(e, "Failed to create entity");
    e_set_race(e, r);
    e_set_name(e, name);
    return e;
}

entity_t* e_new_weapon(entityid id, const char* name) {
    entity_t* e = e_new(id, ENTITY_WEAPON);
    massert(e, "Failed to create weapon entity");
    e_set_name(e, name);
    return e;
}

entity_t* e_new_weapon_at(entityid id, int x, int y, int floor, const char* name) {
    entity_t* e = e_new_weapon(id, name);
    massert(e, "Failed to create weapon entity");
    e_set_xy(e, x, y);
    e_set_floor(e, floor);
    return e;
}

entity_t* e_new_shield(entityid id, const char* name) {
    entity_t* e = e_new(id, ENTITY_SHIELD);
    massert(e, "Failed to create shield entity");
    e_set_name(e, name);
    return e;
}

entity_t* e_new_shield_at(entityid id, int x, int y, int floor, const char* name) {
    entity_t* e = e_new_shield(id, name);
    massert(e, "Failed to create shield entity");
    e_set_xy(e, x, y);
    e_set_floor(e, floor);
    return e;
}

void e_set_name(entity_t* const e, const char* name) {
    massert(e, "e is NULL");
    massert(name, "name is NULL");
    massert(strlen(name) < ENTITY_NAME_LEN_MAX, "name is too long");
    strncpy(e->name, name, ENTITY_NAME_LEN_MAX);
}

void e_set_race(entity_t* const e, race_t race) {
    massert(e, "entity is null");
    massert(race >= 0, "race is less than 0");
    massert(race < RACE_COUNT, "race is greater than RACE_COUNT");
    e->race = race;
}

void e_set_x(entity_t* const e, int x) {
    massert(e, "entity is null");
    massert(x >= 0, "x is less than 0");
    e->x = x;
}

void e_set_y(entity_t* const e, int y) {
    massert(e, "entity is null");
    massert(y >= 0, "y is less than 0");
    e->y = y;
}

void e_set_xy(entity_t* const e, int x, int y) {
    e_set_x(e, x);
    e_set_y(e, y);
}

void e_incr_x(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_x(e, e->x + 1);
}

void e_incr_y(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_y(e, e->y + 1);
}

void e_decr_x(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_x(e, e->x - 1);
}

void e_decr_y(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_y(e, e->y - 1);
}

void e_set_dir(entity_t* const e, direction_t dir) {
    massert(e, "e is NULL");
    massert(dir >= 0, "dir is less than 0");
    massert(dir < DIR_COUNT, "dir is greater than DIR_COUNT");
    e->direction = dir;
}

direction_t e_get_dir(entity_t* const e) {
    massert(e, "e is NULL");
    return e->direction;
}

void e_set_type(entity_t* const e, entitytype_t type) {
    massert(e, "e is NULL");
    massert(type >= 0, "type is less than 0");
    massert(type < ENTITY_TYPE_COUNT, "type is greater than ENTITY_COUNT");
    e->type = type;
}

void e_set_default_action(entity_t* const e, entity_action_t action) {
    massert(e, "e is NULL");
    massert(action >= 0, "action is less than 0");
    massert(action < ENTITY_ACTION_COUNT, "action is greater than ENTITY_ACTION_COUNT");
    e->default_action = action;
}

entity_action_t e_get_default_action(entity_t* const e) {
    massert(e, "e is NULL");
    return e->default_action;
}

void e_set_hp(entity_t* const e, int hp) {
    massert(e, "e is NULL");
    e->stats.hp = hp < 0 ? 0 : hp > e->stats.maxhp ? e->stats.maxhp : hp;
}

int e_get_hp(entity_t* const e) {
    massert(e, "e is NULL");
    return e->stats.hp;
}

void e_set_maxhp(entity_t* const e, int maxhp) {
    massert(e, "e is NULL");
    massert(maxhp > 0, "maxhp is less than 1");
    e->stats.maxhp = maxhp;
}

int e_get_maxhp(entity_t* const e) {
    massert(e, "e is NULL");
    return e->stats.maxhp;
}

void e_set_hpmaxhp(entity_t* const e, int hp, int maxhp) {
    massert(e, "e is NULL");
    e_set_hp(e, hp);
    e_set_maxhp(e, maxhp);
}

void e_incr_hp(entity_t* const e, int hp) {
    massert(e, "e is NULL");
    e_set_hp(e, e->stats.hp + hp);
}

void e_decr_hp(entity_t* const e, int hp) {
    massert(e, "e is NULL");
    e_set_hp(e, e->stats.hp - hp);
}

int e_get_inventory_count(entity_t* const e) {
    massert(e, "e is NULL");
    return e->inventory_count;
}

void e_set_is_dead(entity_t* const e, bool dead) {
    massert(e, "e is NULL");
    e->dead = dead;
}

void e_set_do_update(entity_t* const e, bool do_update) {
    massert(e, "e is NULL");
    e->do_update = do_update;
}

bool e_is_alive(entity_t* const e) {
    massert(e, "e is NULL");
    return !e->dead;
}
