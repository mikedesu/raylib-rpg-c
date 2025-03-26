#include "em.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

em_t* em_new() {
    em_t* em = malloc(sizeof(em_t));
    if (!em) {
        merror("em_new: em malloc failed");
        return NULL;
    }
    for (int i = 0; i < EM_MAX_SLOTS; i++) { em->entities[i] = NULL; }
    em->count = 0;
    em->max_slots = EM_MAX_SLOTS;
    return em;
}

void em_free(em_t* em) {
    if (!em) {
        merror("em_free: em is NULL");
        return;
    }
    minfo("Freeing entity map");
    for (int i = 0; i < EM_MAX_SLOTS; i++) {
        entity_t* current = em->entities[i];
        while (current != NULL) {
            entity_t* next = current->next;
            free(current);
            current = next;
        }
    }
    free(em);
    msuccess("Freed entity map");
}

// returns the first item in the set which will be the oldest
entity_t* const em_get(em_t* em, const entityid id) {
    if (!em) {
        merror("em_get: em is NULL");
        return NULL;
    }
    if (id < 0) {
        //merror("em_get: id is -1");
        return NULL;
    }
    const int hash = id % EM_MAX_SLOTS;
    if (hash < 0) {
        merrorint("em_get: hash is negative", hash);
        return NULL;
    }
    // this code is not needed because the hash is already checked
    //if (hash >= EM_MAX_SLOTS) {
    //    merrorint("em_get: hash is too large", hash);
    //    return NULL;
    //}
    return em->entities[hash];
}

entity_t* em_get_last(em_t* em, const entityid id) {
    if (!em) {
        merror("em_get_last: em is NULL");
        return NULL;
    }
    if (id == -1) {
        merror("em_get_last: id is -1");
        return NULL;
    }
    const int hash = id % EM_MAX_SLOTS;
    entity_t* current = em->entities[hash];
    if (current == NULL) { return NULL; }
    while (current->next != NULL) { current = current->next; }
    return current;
}

entity_t* em_add(em_t* em, entity_t* e) {
    if (!em) {
        merror("em_add: em is NULL");
        return NULL;
    }
    if (!e) {
        merror("em_add: entity is NULL");
        return NULL;
    }
    const int hash = e->id % EM_MAX_SLOTS;
    if (hash < 0) {
        merrorint("em_add: hash is negative", hash);
        return NULL;
    }
    if (hash >= EM_MAX_SLOTS) {
        merrorint("em_add: hash is too large", hash);
        return NULL;
    }
    if (em->entities[hash] != NULL) {
        entity_t* current = em->entities[hash];
        // find a new slot
        while (current->next != NULL) current = current->next;
        current->next = e;
    } else {
        em->entities[hash] = e;
    }
    em->count++;
    return e;
}

entity_t* em_remove_last(em_t* em, const entityid id) {
    if (!em) {
        merror("em_remove_last: em is NULL");
        return NULL;
    }
    if (id == -1) {
        merror("em_remove_last: id is -1");
        return NULL;
    }
    const int hash = id % EM_MAX_SLOTS;
    if (hash < 0) {
        merrorint("em_remove_last: hash is negative", hash);
        return NULL;
    }
    if (hash >= EM_MAX_SLOTS) {
        merrorint("em_remove_last: hash is too large", hash);
        return NULL;
    }
    entity_t* current = em->entities[hash];
    if (current == NULL) {
        merror("em_remove_last: current is NULL");
        return NULL;
    }
    if (current->next == NULL) {
        em->entities[hash] = NULL;
        return current;
    }
    while (current->next->next != NULL) current = current->next;
    entity_t* last = current->next;
    current->next = NULL;
    em->count--;
    return last;
}

const int em_count(const em_t* const em) {
    if (!em) {
        merror("em_count: em is NULL");
        return -1;
    }
    return em->count;
}
