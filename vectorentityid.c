#include "entityid.h"
#include "vectorentityid.h"

#include <stdlib.h>

vectorentityid vectorentityid_new() {
    return vectorentityid_create(4);
}

vectorentityid vectorentityid_create(size_t size) {
    vectorentityid v;
    v.size = size;
    v.current_index = 0;
    const int mem_size = sizeof(int) * 2 + sizeof(entityid) * size;
    v.data = (entityid*)malloc(mem_size);
    return v;
}

void vectorentityid_destroy(vectorentityid* v) {
    free(v->data);
    v->data = NULL;
    v->size = 0;
}

void vectorentityid_set(vectorentityid* v, size_t index, int value) {
    if(index < v->size) {
        v->data[index] = value;
    }
}

void vectorentityid_pushback(vectorentityid* v, int value) {
    if(v->current_index >= v->size) {
        vectorentityid_resize(v, v->size * 2);
    }
    v->data[v->current_index] = value;
    v->current_index++;
}

int vectorentityid_get(vectorentityid* v, size_t index) {
    if(index < v->size) {
        return v->data[index];
    }
    return 0;
}

void vectorentityid_resize(vectorentityid* v, size_t new_size) {
    v->data = (entityid*)realloc(v->data, new_size * sizeof(int));
    v->size = new_size;
}

size_t vectorentityid_size(vectorentityid* v) {
    return v->size;
}

size_t vectorentityid_capacity(vectorentityid* v) {
    return v->current_index;
}

bool vectorentityid_pushback_unique(vectorentityid* v, entityid value) {
    if(vectorentityid_contains(v, value) != (size_t)-1) {
        return false;
    }

    vectorentityid_pushback(v, value);
    return true;
}

size_t vectorentityid_contains(vectorentityid* v, entityid value) {
    for(size_t i = 0; i < v->current_index; i++) {
        if(v->data[i] == value) {
            return i;
        }
    }
    return -1;
}

entityid vectorentityid_remove_by_index(vectorentityid* v, size_t index) {
    if(index >= v->current_index) {
        return -1;
    }
    entityid value = v->data[index];
    for(size_t i = index; i < v->current_index - 1; i++) {
        v->data[i] = v->data[i + 1];
    }
    v->current_index--;
    return value;
}

entityid vectorentityid_remove_by_value(vectorentityid* v, entityid value) {
    size_t index = vectorentityid_contains(v, value);
    if(index == (size_t)-1) {
        return -1;
    }
    return vectorentityid_remove_by_index(v, index);
}
