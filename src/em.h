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

entity_t* const em_get(const em_t* const em, entityid id);
entity_t* em_get_last(const em_t* const em, entityid id);
entity_t* em_add(em_t* const em, entity_t* const e);
entity_t* em_remove_last(em_t* const em, entityid id);

int em_count(const em_t* const em);

bool em_free(em_t* em);
