#pragma once

#include "entityid.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t size;
    size_t current_index;
    entityid* data;
} vectorentityid;

vectorentityid vectorentityid_new();
vectorentityid vectorentityid_create(size_t size);
void vectorentityid_destroy(vectorentityid* v);
void vectorentityid_set(vectorentityid* v, size_t index, int value);
int vectorentityid_get(vectorentityid* v, size_t index);
void vectorentityid_resize(vectorentityid* v, size_t new_size);
size_t vectorentityid_size(vectorentityid* v);
size_t vectorentityid_capacity(vectorentityid* v);
void vectorentityid_pushback(vectorentityid* v, entityid value);

bool vectorentityid_pushback_unique(vectorentityid* v, entityid value);

size_t vectorentityid_contains(vectorentityid* v, entityid value);
entityid vectorentityid_remove_by_index(vectorentityid* v, size_t index);
entityid vectorentityid_remove_by_value(vectorentityid* v, entityid value);
