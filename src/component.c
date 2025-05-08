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
