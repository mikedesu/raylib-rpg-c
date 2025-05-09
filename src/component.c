#include "component.h"
#include "massert.h"
#include <string.h>

void init_name_component(name_component* comp, entityid id, const char* name) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    massert(name != NULL, "name is NULL");
    if (!name) { return; }
    massert(strlen(name) > 0, "name is empty");
    if (strlen(name) == 0) { return; }
    massert(strlen(name) < sizeof(comp->name), "name is too long");
    if (strlen(name) >= sizeof(comp->name)) { return; }
    comp->id = id;
    snprintf(comp->name, sizeof(comp->name), "%s", name);
}

void init_type_component(type_component* comp, entityid id, entitytype_t type) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    if (type <= ENTITY_NONE || type >= ENTITY_TYPE_COUNT) { return; }
    comp->id = id;
    comp->type = type;
}

void init_race_component(race_component* comp, entityid id, race_t race) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    massert(race > RACE_NONE && race < RACE_COUNT, "race is invalid: %d", race);
    if (race <= RACE_NONE || race >= RACE_COUNT) { return; }
    comp->id = id;
    comp->race = race;
}

void init_direction_component(direction_component* comp, entityid id, direction_t dir) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    massert(dir >= 0 && dir < DIR_COUNT, "dir is invalid: %d", dir);
    if (dir < 0 || dir >= DIR_COUNT) { return; }
    comp->id = id;
    comp->dir = dir;
}

void init_loc_component(loc_component* comp, entityid id, loc_t loc) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->loc = loc;
}

void init_sprite_move_component(sprite_move_component* comp, entityid id, loc_t loc) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->loc = loc;
}

void init_dead_component(dead_component* comp, entityid id, bool dead) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->dead = dead;
}

void init_update_component(update_component* comp, entityid id, bool update) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->update = update;
}

void init_attacking_component(attacking_component* comp, entityid id, bool attacking) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->attacking = attacking;
}

void init_blocking_component(blocking_component* comp, entityid id, bool blocking) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->blocking = blocking;
}

void init_block_success_component(block_success_component* comp, entityid id, bool block_success) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->block_success = block_success;
}

void init_damaged_component(damaged_component* comp, entityid id, bool damaged) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->damaged = damaged;
}

void init_inventory_component(inventory_component* comp, entityid id) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->count = 0;
    memset(comp->inventory, ENTITYID_INVALID, sizeof(comp->inventory));
}

void init_equipped_weapon_component(equipped_weapon_component* comp, entityid id, entityid weapon) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->weapon = weapon;
}

void init_equipped_shield_component(equipped_shield_component* comp, entityid id, entityid shield) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->shield = shield;
}

void init_target_component(target_component* comp, entityid id, loc_t target) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->target = target;
}

void init_target_path_component(target_path_component* comp, entityid id, loc_t* target_path, int target_path_length) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) { return; }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) { return; }
    comp->id = id;
    comp->target_path = target_path;
    comp->target_path_length = target_path_length;
}
