#pragma once

#include "direction.h"
#include "entity_actions.h" // Include the new enum
#include "entityid.h"
#include "entitytype.h"
#include "location.h"
#include "massert.h"
#include "potiontype.h"
#include "race.h"
#include <stdbool.h>
#include <string.h>

#define ENTITY_NAME_LEN_MAX 64
#define ENTITY_INVENTORY_MAX_SIZE 32

// forward declaration
typedef struct entity_t entity_t;

//typedef struct {
//    int level;
//    int hp;
//    int maxhp;
//    int mp;
//    int maxmp;
//} entity_stats_t;

typedef struct entity_t {
    entityid id;
    entityid weapon;
    entityid shield;
    // pathfinding
    entity_t* next;
} entity;

entity_t* e_new(entityid id, entitytype_t type);
entity_t* e_new_at(entityid id, entitytype_t type, int x, int y, int floor);

void e_free(entity_t* e);
//void e_free_target_path(entity_t* const e);

//static inline int e_get_inventory_count(entity_t* const e) {
//    massert(e, "e is NULL");
//    return e ? e->inventory_count : 0;
//}

static inline entity_t* e_new_npc_at(entityid id, race_t r, int x, int y, int floor, const char* name) {
    entity_t* e = e_new_at(id, ENTITY_NPC, x, y, floor);
    massert(e, "Failed to create entity");
    return e;
}

//static inline entity_t* e_new_door_at(entityid id, int x, int y, int floor, const char* name) {
//    entity_t* e = e_new_at(id, ENTITY_DOOR, x, y, floor);
//    massert(e, "Failed to create entity");
//    return e;
//}

//static inline entity_t* e_new_key_at(entityid id, int x, int y, int floor, const char* name) {
//    entity_t* e = e_new_at(id, ENTITY_KEY, x, y, floor);
//    massert(e, "Failed to create entity");
//    return e;
//}

//static inline entity_t* e_new_weapon(entityid id, const char* name) {
//    entity_t* e = e_new(id, ENTITY_WEAPON);
//    massert(e, "Failed to create weapon entity");
//    return e;
//}

//static inline entity_t* e_new_potion(entityid id, potiontype_t potion_type, const char* name) {
//    massert(id >= 0, "id is less than 0");
//    massert(potion_type >= 0, "potion_type is less than 0");
//    massert(potion_type < POTION_COUNT, "potion_type is greater than POTION_COUNT");
//    massert(name, "name is NULL");
//    massert(strlen(name) > 0, "name is empty");
//    massert(strlen(name) < ENTITY_NAME_LEN_MAX, "name is too long");
//    entity_t* e = e_new(id, ENTITY_POTION);
//    massert(e, "Failed to create potion entity");
//    e->potion_type = potion_type;
//    return e;
//}

//static inline void e_equip_weapon(entity* e, entityid weapon_id) {
//    massert(e, "entity is NULL");
//    e->weapon = weapon_id;
//}

//static inline bool e_item_is_in_inventory(entity_t* const e, entityid item_id) {
//    massert(e, "e is NULL");
//    massert(item_id >= 0, "item_id is less than 0");
//    for (int i = 0; i < e->inventory_count; i++) {
//        if (e->inventory[i] == item_id) return true;
//    }
//    return false;
//}

//static inline bool e_has_weapon(entity_t* const e) {
//    massert(e, "e is NULL");
//    return e->weapon != ENTITYID_INVALID;
//}

//static inline bool e_has_shield(entity_t* const e) {
//    massert(e, "e is NULL");
//    return e->shield != ENTITYID_INVALID;
//}
