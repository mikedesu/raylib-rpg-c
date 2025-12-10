#pragma once

#include "create_weapon.h"
#include "gamestate.h"
#include "manage_inventory.h"

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


static inline void orc_init_test(shared_ptr<gamestate> g, entityid id) {
    const entityid wpn_id = create_weapon_with(g, axe_init_test);
    add_to_inventory(g, id, wpn_id);
    set_equipped_weapon(g, id, wpn_id);
    set_name(g, id, "Zook");
}


static inline void orc_init_test2(shared_ptr<gamestate> g, entityid id) {
    const entityid wpn_id = create_weapon_with(g, dagger_init_test);
    add_to_inventory(g, id, wpn_id);
    set_equipped_weapon(g, id, wpn_id);
    set_name(g, id, "Zook");
}


static inline void orc_init_test3(shared_ptr<gamestate> g, entityid id) {
    const entityid weaponid = create_weapon_with(g, sword_init_test);
    add_to_inventory(g, id, weaponid);
    set_equipped_weapon(g, id, weaponid);
    set_name(g, id, "Gorp");
}
