#include "entityid.h"
#include "mprint.h"
#include "vectorentityid.h"
#include <stdlib.h>




vectorentityid_t vectorentityid_new() {
    return vectorentityid_create(VECTORENTITYID_DEFAULT_STARTING_SIZE);
}




vectorentityid_t vectorentityid_create(size_t size) {
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




void vectorentityid_set(vectorentityid_t* v, size_t index, int value) {
    if (index < v->size) {
        v->data[index] = value;
    }
}




bool vectorentityid_add(vectorentityid_t* v, int value) {
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




entityid vectorentityid_get(vectorentityid_t* v, size_t index) {
    entityid id;
    if (v) {
        if (index < v->size) {
            id = v->data[index];
        } else {
            id = 0;
        }
    } else {
        id = 0;
    }
    return id;
}




void vectorentityid_resize(vectorentityid_t* v, size_t new_size) {
    v->data = (entityid*)realloc(v->data, new_size * sizeof(int));
    v->size = new_size;
}




size_t vectorentityid_size(vectorentityid_t* v) {
    return v->size;
}




size_t vectorentityid_capacity(vectorentityid_t* v) {
    return v->current_index;
}




size_t vectorentityid_contains(vectorentityid_t* v, entityid value) {
    //minfo("vectorentityid_contains begin");
    for (size_t i = 0; i < v->current_index; i++) {
        if (v->data[i] == value) {
            return i;
        }
    }
    //minfo("vectorentityid_contains end");
    return -1;
}




entityid vectorentityid_remove_index(vectorentityid_t* v, size_t index) {
    //minfo("vectorentityid_remove_index");
    if (index >= v->current_index) {
        return -1;
    }
    entityid value = v->data[index];
    //minfo("vectorentityid_remove_index begin copying...");
    fprintf(stderr, "index: %zu\n", index);
    for (size_t i = index; i < v->current_index - 1; i++) {
        v->data[i] = v->data[i + 1];
    }
    //minfo("vectorentityid_remove_index end copying...");
    v->current_index--;
    return value;
}




entityid vectorentityid_remove_value(vectorentityid_t* v, entityid value) {
    //minfo("vectorentityid_remove_value");
    size_t index = vectorentityid_contains(v, value);
    if (index == (size_t)-1) {
        return -1;
    }
    return vectorentityid_remove_index(v, index);
}
