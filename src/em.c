#include "em.h"
#include "massert.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

em_t* em_new() {
    em_t* em = malloc(sizeof(em_t));
    massert(em, "em malloc failed");
    for (int i = 0; i < EM_MAX_SLOTS; i++) em->entities[i] = NULL;
    em->count = 0;
    em->max_slots = EM_MAX_SLOTS;
    return em;
}

bool em_free(em_t* em) {
    if (!em) {
        merror("em is NULL");
        return false;
    }
    minfo("Freeing entity map");
    for (int i = 0; i < EM_MAX_SLOTS; i++) {
        entity_t* current = em->entities[i];
        while (current != NULL) {
            entity_t* next = current->next;
            current->next = NULL; // Clear the next pointer
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
    massert(em, "em is NULL");
    if (id < 0) { return NULL; }
    const int hash = id % EM_MAX_SLOTS;
    massert(hash >= 0, "hash is negative");
    return em->entities[hash];
}

entity_t* em_get_last(const em_t* const em, entityid id) {
    massert(em, "em is NULL");
    massert(id >= 0, "id is less than 0");
    const int hash = id % EM_MAX_SLOTS;
    massert(hash >= 0, "hash is negative");
    entity_t* current = em->entities[hash];
    if (current == NULL) return NULL;
    while (current->next != NULL) current = current->next;
    return current;
}

entity_t* em_add(em_t* const em, entity_t* const e) {
    massert(em, "em is NULL");
    massert(e, "entity is NULL");
    const int hash = e->id % EM_MAX_SLOTS;
    massert(hash >= 0, "hash is negative");
    massert(hash < EM_MAX_SLOTS, "hash is too large");
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
    massert(em, "em is NULL");
    massert(id >= 0, "id is less than 0");
    const int hash = id % EM_MAX_SLOTS;
    massert(hash >= 0, "hash is negative");
    massert(hash < EM_MAX_SLOTS, "hash is too large");
    entity_t* current = em->entities[hash];
    if (current == NULL) {
        merror("current is NULL");
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
    massert(em, "em is NULL");
    return em->count;
}
