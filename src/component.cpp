#include "component.h"
#include "massert.h"
#include "vec3.h"
#include "vec3_component.h"
#include <cstring>
//#include "mprint.h"

void init_name_component(name_component* comp, entityid id, string name) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) {
        return;
    }
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) {
        return;
    }
    if (name == "") {
        return;
    }
    if (name.length() == 0) {
        return;
    }
    comp->id = id;
    comp->name = name;
}

void init_int_component(int_component* comp, entityid id, int data) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->data = data;
}

void init_vec3_component(vec3_component* comp, entityid id, vec3 v) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->data = v;
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

void init_target_path_component(target_path_component* comp, entityid id, vec3* target_path, int target_path_length) {
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

void init_vec3_list_component(vec3_list_component* comp, entityid id, vec3* list, int list_length) {
    massert(comp != NULL, "comp is NULL");
    if (!comp) return;
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) return;
    comp->id = id;
    comp->list = list;
    comp->count = list_length;
    comp->capacity = list_length; // assuming the list is already allocated with the correct capacity
}
