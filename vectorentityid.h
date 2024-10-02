#pragma once

#include "entityid.h"
#include <stdbool.h>
#include <stddef.h>


#define VECTORENTITYID_DEFAULT_STARTING_SIZE 8


typedef struct {
    size_t size;
    size_t current_index;
    entityid* data;
} vectorentityid_t;

vectorentityid_t vectorentityid_new();
vectorentityid_t vectorentityid_create(size_t size);

void vectorentityid_destroy(vectorentityid_t* v);
void vectorentityid_set(vectorentityid_t* v, size_t index, int value);
void vectorentityid_resize(vectorentityid_t* v, size_t new_size);

entityid vectorentityid_get(vectorentityid_t* v, size_t index);

size_t vectorentityid_size(vectorentityid_t* v);
size_t vectorentityid_capacity(vectorentityid_t* v);
size_t vectorentityid_contains(vectorentityid_t* v, entityid value);

bool vectorentityid_add(vectorentityid_t* v, entityid value);

entityid vectorentityid_remove_index(vectorentityid_t* v, size_t index);
entityid vectorentityid_remove_value(vectorentityid_t* v, entityid value);
