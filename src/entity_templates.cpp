#include "ComponentTraits.h"
#include "entity_templates.h"
#include "weapon.h"

void set_name(shared_ptr<gamestate> g, entityid id, string n) {
    g->ct.set<name>(id, n);
}


void set_description(shared_ptr<gamestate> g, entityid id, string d) {
    g->ct.set<description>(id, d);
}

void set_weapontype(shared_ptr<gamestate> g, entityid id, weapontype_t t) {
    g->ct.set<weapontype>(id, t);
}


void set_damage(shared_ptr<gamestate> g, entityid id, vec3 dmg) {
    g->ct.set<damage>(id, dmg);
}

void set_durability(shared_ptr<gamestate> g, entityid id, int d) {
    g->ct.set<durability>(id, d);
}

void set_max_durability(shared_ptr<gamestate> g, entityid id, int d) {
    g->ct.set<max_durability>(id, d);
}


void set_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id) {
    g->ct.set<equipped_weapon>(id, wpn_id);
}

void axe_init_test(shared_ptr<gamestate> g, entityid id) {
    set_name(g, id, "Axe");
    set_description(g, id, "We choppin' trees");
    set_weapontype(g, id, WEAPON_AXE);
    set_damage(g, id, (vec3){1, 8, 0});
    set_durability(g, id, 100);
    set_max_durability(g, id, 100);
}
