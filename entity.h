#pragma once

#include "damagetype.h"
#include "entityid.h"
#include "entitytype.h"
#include "itemtype.h"
#include "race.h"
#include "vectorentityid.h"
#include "weapontype.h"
#include <raylib.h>

#define ENTITY_NAME_MAX 128

typedef struct entity_t {
    char name[ENTITY_NAME_MAX];
    entityid id;
    entitytype_t type;
    itemtype_t itemtype; // not be used if not an item
    Vector2 pos;
    vectorentityid_t inventory;
    race_t race;
    weapontype_t weapontype; // not be used if not a weapon
    damagetype_t damagetype; // not be used if not a weapon
    int hp;
    int maxhp;
} entity_t;


entity_t* entity_create(const char* name);


void entity_set_name(entity_t* e, const char* n);
void entity_set_name_unsafe(entity_t* e, const char* n);


void entity_set_x(entity_t* e, const int x);
void entity_set_x_unsafe(entity_t* e, const int x);


void entity_set_y(entity_t* e, const int y);
void entity_set_y_unsafe(entity_t* e, const int y);


void entity_set_xy(entity_t* e, const int x, const int y);
void entity_set_xy_unsafe(entity_t* e, const int x, const int y);


void entity_set_pos(entity_t* e, const Vector2 pos);
void entity_set_pos_unsafe(entity_t* e, const Vector2 pos);


void entity_move(entity_t* e, const Vector2 dir);
void entity_move_unsafe(entity_t* e, const Vector2 dir);


void entity_destroy(entity_t* entity);
void entity_destroy_unsafe(entity_t* entity);



void entity_set_weapontype(entity_t* e, const weapontype_t wt);
void entity_set_weapontype_unsafe(entity_t* e, const weapontype_t wt);
void entity_set_damagetype(entity_t* e, const damagetype_t dt);
void entity_set_damagetype_unsafe(entity_t* e, const damagetype_t dt);
void entity_set_hp(entity_t* e, const int hp);
void entity_set_hp_unsafe(entity_t* e, const int hp);
void entity_set_maxhp(entity_t* e, const int maxhp);
void entity_set_maxhp_unsafe(entity_t* e, const int maxhp);
void entity_incr_hp(entity_t* e, const int hp);
void entity_incr_hp_unsafe(entity_t* e, const int hp);
void entity_decr_hp(entity_t* e, const int hp);
void entity_decr_hp_unsafe(entity_t* e, const int hp);
