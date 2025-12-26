#pragma once

#include "ComponentTraits.h"
#include "entityid.h"
#include "gamestate.h"
#include "orc_names.h"
#include "weapon.h"


static inline void axe_init_test(gamestate& g, entityid id) {
    g.ct.set<name>(id, "Axe");
    g.ct.set<description>(id, "We choppin' trees");
    g.ct.set<weapontype>(id, WEAPON_AXE);
    g.ct.set<damage>(id, (vec3){1, 8, 0});
    g.ct.set<durability>(id, 100);
    g.ct.set<max_durability>(id, 100);
    g.ct.set<rarity>(id, RARITY_COMMON);
}


static inline void dagger_init_test(gamestate& g, entityid id) {
    g.ct.set<name>(id, "Dagger");
    g.ct.set<description>(id, "Stabby stabby.");
    g.ct.set<weapontype>(id, WEAPON_DAGGER);
    g.ct.set<damage>(id, (vec3){1, 4, 0});
    g.ct.set<durability>(id, 100);
    g.ct.set<max_durability>(id, 100);
    g.ct.set<rarity>(id, RARITY_COMMON);
}


static inline void sword_init_test(gamestate& g, entityid id) {
    g.ct.set<name>(id, "Sword");
    g.ct.set<description>(id, "Standard sword");
    g.ct.set<weapontype>(id, WEAPON_SWORD);
    g.ct.set<damage>(id, (vec3){1, 6, 0});
    g.ct.set<durability>(id, 100);
    g.ct.set<max_durability>(id, 100);
    g.ct.set<rarity>(id, RARITY_COMMON);
}


static inline void buckler_init_test(gamestate& g, entityid id) {
    g.ct.set<name>(id, "Buckler");
    g.ct.set<description>(id, "The basic buckler");
    g.ct.set<shieldtype>(id, SHIELD_BUCKLER);
    g.ct.set<block_chance>(id, 50);
}


static inline void tower_shield_init_test(gamestate& g, entityid id) {
    g.ct.set<name>(id, "Tower Shield");
    g.ct.set<description>(id, "The tower towers");
    g.ct.set<shieldtype>(id, SHIELD_TOWER);
    g.ct.set<block_chance>(id, 100);
}


static inline void kite_shield_init_test(gamestate& g, entityid id) {
    g.ct.set<name>(id, "Kite Shield");
    g.ct.set<description>(id, "Standard knight's shield");
    g.ct.set<shieldtype>(id, SHIELD_KITE);
    g.ct.set<block_chance>(id, 90);
}


static inline void random_weapon_init_test(gamestate& g, entityid id) {
    const weapontype_t t = (weapontype_t)GetRandomValue(WEAPON_NONE + 1, WEAPON_TYPE_COUNT - 1);

    switch (t) {
    case WEAPON_SWORD: sword_init_test(g, id); break;
    case WEAPON_DAGGER: dagger_init_test(g, id); break;
    case WEAPON_AXE: axe_init_test(g, id); break;
    default: break;
    }
}
