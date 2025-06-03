#include "component.h"
#include "massert.h"
//#include "mprint.h"
#include <string.h>

void init_name_component(name_component* comp, entityid id, const char* name) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    massert(name != NULL, "name is NULL");
    if (!name) return;
    massert(strlen(name) > 0, "name is empty");
    if (strlen(name) == 0) return;
    massert(strlen(name) < sizeof(comp->name), "name is too long");
    if (strlen(name) >= sizeof(comp->name)) return;
    comp->id = id;
    snprintf(comp->name, sizeof(comp->name), "%s", name);
}

void init_int_component(int_component* comp, entityid id, int data) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->data = data;
}

void init_loc_component(loc_component* comp, entityid id, loc_t loc) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->loc = loc;
}

void init_sprite_move_component(sprite_move_component* comp, entityid id, loc_t loc) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->loc = loc;
}

//void init_dead_component(dead_component* comp, entityid id, bool dead) {
//    massert(comp != NULL, "comp is NULL");
//    if (!comp) return;
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (id == ENTITYID_INVALID) return;
//    comp->id = id;
//    comp->dead = dead;
//}

void init_update_component(update_component* comp, entityid id, bool update) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->update = update;
}

void init_attacking_component(attacking_component* comp, entityid id, bool attacking) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->attacking = attacking;
}

void init_blocking_component(blocking_component* comp, entityid id, bool blocking) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->blocking = blocking;
}

void init_block_success_component(block_success_component* comp, entityid id, bool block_success) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->block_success = block_success;
}

void init_damaged_component(damaged_component* comp, entityid id, bool damaged) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->damaged = damaged;
}

void init_inventory_component(inventory_component* comp, entityid id) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->count = 0;
    memset(comp->inventory, ENTITYID_INVALID, sizeof(comp->inventory));
}

void init_target_component(target_component* comp, entityid id, loc_t target) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->target = target;
}

void init_target_path_component(target_path_component* comp, entityid id, loc_t* target_path, int target_path_length) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->target_path = target_path;
    comp->target_path_length = target_path_length;
}

void init_equipment_component(equipment_component* comp, entityid id) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    memset(comp->equipment, ENTITYID_INVALID, sizeof(comp->equipment));
}

void init_stats_component(stats_component* comp, entityid id) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    memset(comp->stats, 0, sizeof(comp->stats));
}

void init_arrowtype_component(arrowtype_component* comp, entityid id, arrowtype type) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    massert(type > ARROW_NONE && type < ARROW_TYPE_COUNT, "type is invalid: %d", type);
    if (type <= ARROW_NONE || type >= ARROW_TYPE_COUNT) return;
    comp->id = id;
    comp->type = type;
}

void init_quiver_component(quiver_component* comp, entityid id, int capacity) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    massert(capacity > 0, "capacity is invalid: %d", capacity);
    if (capacity <= 0) return;
    comp->id = id;
    comp->capacity = capacity;
}

void init_damage_component(damage_component* comp, entityid id, roll r) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->r = r;
}

void init_zapping_component(zapping_component* comp, entityid id, bool zapping) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->zapping = zapping;
}

void init_spell_effect_component(spell_effect_component* comp, entityid id, spell_effect effect) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->effect = effect;
}

void init_base_attack_damage_component(base_attack_damage_component* comp, entityid id, roll r) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->damage = r;
}
