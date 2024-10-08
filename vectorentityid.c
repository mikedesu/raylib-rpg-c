#include "entityid.h"
//#include "mprint.h"
#include "vectorentityid.h"
#include <stdlib.h>




vectorentityid_t vectorentityid_new() {
    return vectorentityid_create(VECTORENTITYID_DEFAULT_STARTING_SIZE);
}




vectorentityid_t vectorentityid_create(const size_t size) {
    vectorentityid_t v;
    v.size = size;
    v.current_index = 0;
    const int mem_size = sizeof(int) * 2 + sizeof(entityid) * size;
    v.data = (entityid*)malloc(mem_size);
    return v;
}




void vectorentityid_destroy(vectorentityid_t* v) {
    free(v->data);
    v->data = NULL;
    v->size = 0;
}




void vectorentityid_set(vectorentityid_t* v, const size_t index, const int value) {
    if (v) {
        if (index < v->size) {
            v->data[index] = value;
        }
    }
}




bool vectorentityid_add(vectorentityid_t* v, const int value) {
    if (vectorentityid_contains(v, value) != (size_t)-1) {
        return false;
    }
    if (v->current_index >= v->size) {
        vectorentityid_resize(v, v->size * 2);
    }
    v->data[v->current_index] = value;
    v->current_index++;
    return true;
}




const entityid vectorentityid_get(const vectorentityid_t* v, const size_t index) {
    if (v) {
        if (index < v->size) {
            return v->data[index];
        }
    }
    return -1;
}




void vectorentityid_resize(vectorentityid_t* v, const size_t new_size) {
    v->data = (entityid*)realloc(v->data, new_size * sizeof(int));
    v->size = new_size;
}




const size_t vectorentityid_size(const vectorentityid_t* v) {
    return v->size;
}




const size_t vectorentityid_capacity(const vectorentityid_t* v) {
    return v->current_index;
}




const size_t vectorentityid_contains(const vectorentityid_t* v, const entityid value) {
    if (v) {
        for (size_t i = 0; i < v->current_index; i++) {
            if (v->data[i] == value) {
                return i;
            }
        }
    }
    return -1;
}




const entityid vectorentityid_remove_index(vectorentityid_t* v, const size_t index) {
    if (v) {
        if (index >= v->current_index) {
            return -1;
        }
        entityid value = v->data[index];
        for (size_t i = index; i < v->current_index - 1; i++) {
            v->data[i] = v->data[i + 1];
        }
        v->current_index--;
        return value;
    }
    return -1;
}




const entityid vectorentityid_remove_value(vectorentityid_t* v, const entityid value) {
    if (v) {
        size_t index = vectorentityid_contains(v, value);
        if (index == (size_t)-1) {
            return -1;
        }
        return vectorentityid_remove_index(v, index);
    }
    return -1;
}
