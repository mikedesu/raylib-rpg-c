#pragma once

#include "entity.h"
#include "entityid.h"
#include <stddef.h>

#define EM_MAX_SLOTS (1024 * 64)

typedef struct {
    entity_t* entities[EM_MAX_SLOTS];
    int count;
    int max_slots;
} em_t;

em_t* em_new();

entity_t* const em_get(em_t* em, const entityid id);
entity_t* em_get_last(em_t* em, const entityid id);
entity_t* em_add(em_t* em, entity_t* e);
entity_t* em_remove_last(em_t* em, const entityid id);

const int em_count(const em_t* const em);

void em_free(em_t* em);
