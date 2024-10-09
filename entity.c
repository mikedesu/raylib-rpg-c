#include "entity.h"
//#include "mprint.h"
#include <raymath.h>
#include <stdlib.h>
#include <string.h>




static entityid next_entity_id = 0;




entity_t* entity_create(const char* n) {
    //minfo("entity_create");
    entity_t* e = malloc(sizeof(entity_t));
    if (!e) {
        //merror("entity_create: malloc failed");
        return NULL;
    }
    e->id = next_entity_id++;
    e->type = ENTITY_NONE;
    e->itemtype = ITEM_NONE;
    entity_set_x(e, -1);
    entity_set_y(e, -1);
    entity_set_name(e, n);

    e->race.primary = RACETYPE_NONE;
    e->race.secondary = RACETYPE_NONE;

    e->weapontype = WEAPON_NONE;
    e->damagetype = DAMAGE_NONE;
    e->shieldtype = SHIELD_NONE;

    e->hp = 0;
    e->maxhp = 0;

    e->pos = (Vector2){-1, -1};
    e->x = -1;
    e->y = -1;

    return e;
}




void entity_destroy_unsafe(entity_t* e) {
    // empty and destroy the vector
    vectorentityid_destroy(&e->inventory);
    free(e);
}




void entity_destroy(entity_t* e) {
    //minfo("entity_destroy");
    if (e) {
        entity_destroy_unsafe(e);
    }
}




void entity_set_name_unsafe(entity_t* e, const char* n) {
    //minfo("entity_set_name_unsafe");
    strncpy(e->name, n, ENTITY_NAME_MAX);
}




void entity_set_name(entity_t* e, const char* n) {
    //minfo("entity_set_name");
    if (e) {
        entity_set_name_unsafe(e, n);
    }
}




void entity_set_x_unsafe(entity_t* e, const int x) {
    //minfo("entity_set_x_unsafe");
    e->pos.x = x;
}




void entity_set_x(entity_t* e, const int x) {
    //minfo("entity_set_x");
    if (e) {
        entity_set_x_unsafe(e, x);
    }
}




void entity_set_y_unsafe(entity_t* e, const int y) {
    //minfo("entity_set_y_unsafe");
    //e->y = y;
    e->pos.y = y;
}




void entity_set_y(entity_t* e, const int y) {
    //minfo("entity_set_y");
    if (e) {
        entity_set_y_unsafe(e, y);
    }
}




void entity_set_xy_unsafe(entity_t* e, const int x, const int y) {
    //minfo("entity_set_xy_unsafe");
    entity_set_x_unsafe(e, x);
    entity_set_y_unsafe(e, y);
}




void entity_set_xy(entity_t* e, const int x, const int y) {
    //minfo("entity_set_xy");
    if (e) {
        entity_set_xy_unsafe(e, x, y);
    }
}




void entity_set_pos(entity_t* e, const Vector2 pos) {
    //minfo("entity_set_pos");
    if (e) {
        entity_set_x(e, pos.x);
        entity_set_y(e, pos.y);
    }
}



void entity_set_pos_unsafe(entity_t* e, const Vector2 pos) {
    //minfo("entity_set_pos_unsafe");
    entity_set_x_unsafe(e, pos.x);
    entity_set_y_unsafe(e, pos.y);
}




void entity_move(entity_t* e, const Vector2 dir) {
    //minfo("entity_move");
    if (e) {
        entity_move_unsafe(e, dir);
    }
}




void entity_move_unsafe(entity_t* e, const Vector2 dir) {
    //minfo("entity_move_unsafe");
    entity_set_pos_unsafe(e, Vector2Add(e->pos, dir));
}




void entity_set_weapontype(entity_t* e, const weapontype_t wt) {
    if (e) {
        entity_set_weapontype_unsafe(e, wt);
    }
}




void entity_set_weapontype_unsafe(entity_t* e, const weapontype_t wt) {
    e->weapontype = wt;
}




void entity_set_damagetype(entity_t* e, const damagetype_t dt) {
    if (e) {
        entity_set_damagetype_unsafe(e, dt);
    }
}




void entity_set_damagetype_unsafe(entity_t* e, const damagetype_t dt) {
    e->damagetype = dt;
}




void entity_set_hp(entity_t* e, const int hp) {
    if (e) {
        entity_set_hp_unsafe(e, hp);
    }
}




void entity_set_hp_unsafe(entity_t* e, const int hp) {
    e->hp = hp;
}




void entity_set_maxhp(entity_t* e, const int maxhp) {
    if (e) {
        entity_set_maxhp_unsafe(e, maxhp);
    }
}




void entity_set_maxhp_unsafe(entity_t* e, const int maxhp) {
    e->maxhp = maxhp;
}




void entity_incr_hp(entity_t* e, const int hp) {
    if (e) {
        entity_incr_hp_unsafe(e, hp);
    }
}




void entity_incr_hp_unsafe(entity_t* e, const int hp) {
    e->hp += hp;
    if (e->hp > e->maxhp) {
        e->hp = e->maxhp;
    }
}




void entity_decr_hp(entity_t* e, const int hp) {
    if (e) {
        entity_decr_hp_unsafe(e, hp);
    }
}




void entity_decr_hp_unsafe(entity_t* e, const int hp) {
    e->hp -= hp;
    if (e->hp < -10) {
        e->hp = -10;
    }
}
