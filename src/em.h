#pragma once

#include "entity.h"
#include "entityid.h"
//#include "entitytype.h"
#include <stddef.h>


#define EM_MAX_SLOTS 1024

typedef struct {
    entity_t* entities[EM_MAX_SLOTS];
    int count;
} em_t;

em_t* em_new();

void em_free(em_t* em);

entity_t* const em_get(em_t* em, const entityid id);

entity_t* em_get_last(em_t* em, const entityid id);
entity_t* em_add(em_t* em, entity_t* e);
entity_t* em_remove_last(em_t* em, const entityid id);

const int em_count(const em_t* const em);
