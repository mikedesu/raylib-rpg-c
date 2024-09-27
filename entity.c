#include "entity.h"
#include "mprint.h"

#include <stdlib.h>
#include <string.h>

static entityid next_entity_id = 0;

entity_t* entity_create(const char* n) {
    minfo("entity_create");
    entity_t* e = malloc(sizeof(entity_t));
    if (!e) {
        merror("entity_create: malloc failed");
        return NULL;
    }
    e->id = next_entity_id++;
    entity_set_x(e, -1);
    entity_set_y(e, -1);
    entity_set_name(e, n);
    return e;
}


void entity_destroy_unsafe(entity_t* e) {
    minfo("entity_destroy_unsafe");
    free(e);
}


void entity_destroy(entity_t* e) {
    minfo("entity_destroy");
    if (e) {
        entity_destroy_unsafe(e);
    }
}


void entity_set_name_unsafe(entity_t* e, const char* n) {
    minfo("entity_set_name_unsafe");
    strncpy(e->name, n, ENTITY_NAME_MAX);
}


void entity_set_name(entity_t* e, const char* n) {
    minfo("entity_set_name");
    if (e) {
        entity_set_name_unsafe(e, n);
    }
}


void entity_set_x_unsafe(entity_t* e, const int x) {
    minfo("entity_set_x_unsafe");
    e->x = x;
}


void entity_set_x(entity_t* e, const int x) {
    minfo("entity_set_x");
    if (e) {
        entity_set_x_unsafe(e, x);
    }
}


void entity_set_y_unsafe(entity_t* e, const int y) {
    minfo("entity_set_y_unsafe");
    e->y = y;
}

void entity_set_y(entity_t* e, const int y) {
    minfo("entity_set_y");
    if (e) {
        entity_set_y_unsafe(e, y);
    }
}


void entity_set_xy_unsafe(entity_t* e, const int x, const int y) {
    minfo("entity_set_xy_unsafe");
    entity_set_x_unsafe(e, x);
    entity_set_y_unsafe(e, y);
}

void entity_set_xy(entity_t* e, const int x, const int y) {
    minfo("entity_set_xy");
    if (e) {
        entity_set_xy_unsafe(e, x, y);
    }
}
