#pragma once

#include "ComponentTraits.h"
#include "create_weapon.h"
#include "entityid.h"
#include "gamestate.h"
#include "manage_inventory.h"
#include "weapon.h"

// Component Setters
static inline void set_name(shared_ptr<gamestate> g, entityid id, string n) {
    g->ct.set<name>(id, n);
}

static inline void set_description(shared_ptr<gamestate> g, entityid id, string d) {
    g->ct.set<description>(id, d);
}


static inline void set_weapontype(shared_ptr<gamestate> g, entityid id, weapontype_t t) {
    g->ct.set<weapontype>(id, t);
}

static inline void set_damage(shared_ptr<gamestate> g, entityid id, vec3 dmg) {
    g->ct.set<damage>(id, dmg);
}

static inline void set_durability(shared_ptr<gamestate> g, entityid id, int d) {
    g->ct.set<durability>(id, d);
}


static inline void set_max_durability(shared_ptr<gamestate> g, entityid id, int d) {
    g->ct.set<max_durability>(id, d);
}


static inline void set_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id) {
    g->ct.set<equipped_weapon>(id, wpn_id);
}


static inline void set_shieldtype(shared_ptr<gamestate> g, entityid id, shieldtype_t t) {
    g->ct.set<shieldtype>(id, t);
}


static inline void set_block_chance(shared_ptr<gamestate> g, entityid id, int chance) {
    g->ct.set<block_chance>(id, chance);
}


static inline void set_location(shared_ptr<gamestate> g, entityid id, vec3 loc) {
    g->ct.set<location>(id, loc);
}


static inline void set_door_open(shared_ptr<gamestate> g, entityid id, bool o) {
    g->ct.set<door_open>(id, o);
}


static inline void set_update(shared_ptr<gamestate> g, entityid id, bool o) {
    g->ct.set<update>(id, o);
}


static inline void axe_init_test(shared_ptr<gamestate> g, entityid id) {
    set_name(g, id, "Axe");
    set_description(g, id, "We choppin' trees");
    set_weapontype(g, id, WEAPON_AXE);
    set_damage(g, id, (vec3){1, 8, 0});
    set_durability(g, id, 100);
    set_max_durability(g, id, 100);
}


static inline void dagger_init_test(shared_ptr<gamestate> g, entityid id) {
    set_name(g, id, "Dagger");
    set_description(g, id, "Stabby stabby.");
    set_weapontype(g, id, WEAPON_DAGGER);
    set_damage(g, id, (vec3){1, 4, 0});
    set_durability(g, id, 2);
    set_max_durability(g, id, 2);
}


static inline void sword_init_test(shared_ptr<gamestate> g, entityid id) {
    set_name(g, id, "Sword");
    set_description(g, id, "Standard sword");
    set_weapontype(g, id, WEAPON_SWORD);
    set_damage(g, id, (vec3){1, 6, 0});
    set_durability(g, id, 100);
    set_max_durability(g, id, 100);
}


static inline void buckler_init_test(shared_ptr<gamestate> g, entityid id) {
    set_name(g, id, "Buckler");
    set_description(g, id, "The basic buckler");
    set_shieldtype(g, id, SHIELD_BUCKLER);
    set_block_chance(g, id, 50);
}


static inline void tower_shield_init_test(shared_ptr<gamestate> g, entityid id) {
    set_name(g, id, "Tower Shield");
    set_description(g, id, "The tower towers");
    set_shieldtype(g, id, SHIELD_TOWER);
    set_block_chance(g, id, 100);
}


static inline void kite_shield_init_test(shared_ptr<gamestate> g, entityid id) {
    set_name(g, id, "Kite");
    set_description(g, id, "Standard knight's shield");
    set_shieldtype(g, id, SHIELD_KITE);
    set_block_chance(g, id, 90);
}


static inline void random_weapon_init_test(shared_ptr<gamestate> g, entityid id) {
    weapontype_t t = (weapontype_t)GetRandomValue(WEAPON_NONE + 1, WEAPON_TYPE_COUNT - 1);
    //set_weapontype(g, id, t);

    switch (t) {
    case WEAPON_SWORD: sword_init_test(g, id); break;
    case WEAPON_DAGGER: dagger_init_test(g, id); break;
    case WEAPON_AXE: axe_init_test(g, id); break;
    default: break;
    }
}


static inline void orc_init_test(shared_ptr<gamestate> g, entityid id) {
    //const entityid wpn_id = create_weapon_with(g, random_weapon_init_test);
    const entityid wpn_id = create_weapon_with(g, axe_init_test);

    add_to_inventory(g, id, wpn_id);
    set_equipped_weapon(g, id, wpn_id);
    set_name(g, id, "Zook");
}
