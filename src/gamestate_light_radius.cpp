#include "gamestate_light_radius.h"

bool g_add_light_radius(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the light radius component
    return g_add_component(
        g, id, C_LIGHT_RADIUS, (void*)&radius, sizeof(int_component), (void**)&g->light_radius_list, &g->light_radius_list_count, &g->light_radius_list_capacity);
}

bool g_has_light_radius(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_LIGHT_RADIUS);
}

bool g_set_light_radius(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_list == NULL) return false;
    for (int i = 0; i < g->light_radius_list_count; i++) {
        if (g->light_radius_list[i].id == id) {
            g->light_radius_list[i].data = radius;
            return true;
        }
    }
    return false;
}

int g_get_light_radius(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_list == NULL) return 0;
    for (int i = 0; i < g->light_radius_list_count; i++) {
        if (g->light_radius_list[i].id == id) return g->light_radius_list[i].data;
    }
    return 0;
}

bool g_add_light_radius_bonus(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the light radius bonus component
    return g_add_component(g,
                           id,
                           C_LIGHT_RADIUS_BONUS,
                           (void*)&radius,
                           sizeof(int_component),
                           (void**)&g->light_radius_bonus_list,
                           &g->light_radius_bonus_list_count,
                           &g->light_radius_bonus_list_capacity);
}

bool g_has_light_radius_bonus(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_LIGHT_RADIUS_BONUS);
}

bool g_set_light_radius_bonus(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_bonus_list == NULL) return false;
    for (int i = 0; i < g->light_radius_bonus_list_count; i++) {
        if (g->light_radius_bonus_list[i].id == id) {
            g->light_radius_bonus_list[i].data = radius;
            return true;
        }
    }
    return false;
}

int g_get_light_radius_bonus(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_bonus_list == NULL) return 0;
    for (int i = 0; i < g->light_radius_bonus_list_count; i++) {
        if (g->light_radius_bonus_list[i].id == id) {
            return g->light_radius_bonus_list[i].data;
        }
    }
    return 0;
}

int g_get_entity_total_light_radius_bonus(const gamestate* const g, entityid id) {
    int total_light_radius_bonus = 0;
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // check each equipment slot
    //for (int i = 0; i < EQUIPMENT_SLOT_COUNT; i++) {
    for (int i = 0; i < EQUIPMENT_SLOT_COUNT; i++) {
        equipment_slot ei = (equipment_slot)i;
        entityid equip_id = g_get_equipment(g, id, ei);
        if (equip_id == ENTITYID_INVALID) continue;
        if (!g_has_light_radius_bonus(g, equip_id)) continue;
        total_light_radius_bonus += g_get_light_radius_bonus(g, equip_id);
    }
    // only return the total light radius bonus
    // it is fine if it is negative that might be fun for cursed items
    return total_light_radius_bonus;
}
