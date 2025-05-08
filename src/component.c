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
