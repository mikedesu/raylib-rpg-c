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
vectorentityid_t vectorentityid_create(const size_t size);

void vectorentityid_destroy(vectorentityid_t* v);
void vectorentityid_set(vectorentityid_t* v, const size_t index, const int value);
void vectorentityid_resize(vectorentityid_t* v, const size_t new_size);

const entityid vectorentityid_get(const vectorentityid_t* v, const size_t index);
const size_t vectorentityid_size(const vectorentityid_t* v);
const size_t vectorentityid_capacity(const vectorentityid_t* v);
const size_t vectorentityid_contains(const vectorentityid_t* v, const entityid value);

bool vectorentityid_add(vectorentityid_t* v, const entityid value);

const entityid vectorentityid_remove_index(vectorentityid_t* v, const size_t index);
const entityid vectorentityid_remove_value(vectorentityid_t* v, const entityid value);
