#pragma once

#include "gamestate.h"


// Component Setters
void set_name(shared_ptr<gamestate> g, entityid id, string n);
void set_description(shared_ptr<gamestate> g, entityid id, string d);
void set_weapontype(shared_ptr<gamestate> g, entityid id, weapontype_t t);
void set_damage(shared_ptr<gamestate> g, entityid id, vec3 dmg);
void set_durability(shared_ptr<gamestate> g, entityid id, int d);
void set_max_durability(shared_ptr<gamestate> g, entityid id, int d);
void set_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id);


void axe_init_test(shared_ptr<gamestate> g, entityid id);
