#include "em.h"
#include "massert.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

em_t* em_new() {
    em_t* em = malloc(sizeof(em_t));
    massert(em, "em_new: em malloc failed");
    for (int i = 0; i < EM_MAX_SLOTS; i++) em->entities[i] = NULL;
    em->count = 0;
    em->max_slots = EM_MAX_SLOTS;
    return em;
}

bool em_free(em_t* em) {
    massert(em, "em_free: em is NULL");
    //if (!em) {
    //    merror("em_free: em is NULL");
    //    return false;
    //}
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
    return true;
}

// returns the first item in the set which will be the oldest
entity_t* const em_get(const em_t* const em, entityid id) {
    massert(em, "em_get: em is NULL");
    //massert(id >= 0, "em_get: id is less than 0");
    if (id < 0) { return NULL; }
    const int hash = id % EM_MAX_SLOTS;
    massert(hash >= 0, "em_get: hash is negative");
    return em->entities[hash];
}

entity_t* em_get_last(const em_t* const em, entityid id) {
    massert(em, "em_get_last: em is NULL");
    massert(id >= 0, "em_get_last: id is less than 0");
    const int hash = id % EM_MAX_SLOTS;
    massert(hash >= 0, "em_get_last: hash is negative");
    entity_t* current = em->entities[hash];
    if (current == NULL) return NULL;
    while (current->next != NULL) { current = current->next; }
    return current;
}

entity_t* em_add(em_t* const em, entity_t* const e) {
    massert(em, "em_add: em is NULL");
    massert(e, "em_add: entity is NULL");
    const int hash = e->id % EM_MAX_SLOTS;
    massert(hash >= 0, "em_add: hash is negative");
    massert(hash < EM_MAX_SLOTS, "em_add: hash is too large");
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

entity_t* em_remove_last(em_t* const em, entityid id) {
    massert(em, "em_remove_last: em is NULL");
    massert(id >= 0, "em_remove_last: id is less than 0");
    const int hash = id % EM_MAX_SLOTS;
    massert(hash >= 0, "em_remove_last: hash is negative");
    massert(hash < EM_MAX_SLOTS, "em_remove_last: hash is too large");
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

int em_count(const em_t* const em) {
    massert(em, "em_count: em is NULL");
    return em->count;
}
