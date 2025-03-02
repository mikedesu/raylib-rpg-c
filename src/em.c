#include "em.h"

#include "mprint.h"

#include <stdlib.h>
#include <string.h>



em_t* em_new() {
    em_t* em = malloc(sizeof(em_t));
    if (em == NULL) {
        merror("em_new: malloc failed");
        return NULL;
    }
    for (int i = 0; i < EM_MAX_SLOTS; i++) {
        em->entities[i] = NULL;
    }
    em->count = 0;
    return em;
}




void em_free(em_t* em) {
    for (int i = 0; i < EM_MAX_SLOTS; i++) {
        entity_t* current = em->entities[i];
        while (current != NULL) {
            entity_t* next = current->next;
            free(current);
            current = next;
        }
    }
    free(em);
}




// returns the first item in the set which will be the oldest
entity_t* em_get(em_t* em, const entityid id) {
    if (id == -1) return NULL;
    const int hash = id % EM_MAX_SLOTS;
    return em->entities[hash];
}




entity_t* em_get_last(em_t* em, const entityid id) {
    const int hash = id % EM_MAX_SLOTS;
    entity_t* current = em->entities[hash];
    if (current == NULL) {
        return NULL;
    }
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}



entity_t* em_add(em_t* em, entity_t* e) {
    if (!em) {
        merror("em_add: em is NULL");
        return NULL;
    } else if (!e) {
        merror("em_add: entity is NULL");
        return NULL;
    }

    const int hash = e->id % EM_MAX_SLOTS;
    if (em->entities[hash] != NULL) {
        entity_t* current = em->entities[hash];
        // find a new slot
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = e;
    } else {
        em->entities[hash] = e;
    }
    em->count++;
    return e;
}




entity_t* em_remove_last(em_t* em, const entityid id) {
    const int hash = id % EM_MAX_SLOTS;
    entity_t* current = em->entities[hash];
    if (current == NULL) {
        return NULL;
    }
    if (current->next == NULL) {
        em->entities[hash] = NULL;
        return current;
    }
    while (current->next->next != NULL) {
        current = current->next;
    }
    entity_t* last = current->next;
    current->next = NULL;
    em->count--;
    return last;
}




int em_count(em_t* em) {
    //size_t count = 0;
    //for (int i = 0; i < EM_MAX_SLOTS; i++) {
    //    entity_t* current = em->entities[i];
    //    while (current != NULL) {
    //        count++;
    //        current = current->next;
    //    }
    //}
    if (!em) {
        merror("em_count: em is NULL");
        return -1;
    }
    return em->count;
}



// remember to free the indices when you are done using them
int* em_get_indices(em_t* em) {
    int* indices = malloc(sizeof(int) * EM_MAX_SLOTS);
    memset(indices, 0, sizeof(int) * EM_MAX_SLOTS);
    for (int i = 0; i < EM_MAX_SLOTS; i++) {
        entity_t* current = em->entities[i];
        while (current != NULL) {
            indices[i]++;
            current = current->next;
        }
    }
    return indices;
}
