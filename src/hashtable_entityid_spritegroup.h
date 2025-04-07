#pragma once

#include "entityid.h"
#include "specifier.h"
#include "spritegroup.h"

typedef struct hashtable_entityid_spritegroup_node_t hashtable_entityid_spritegroup_node_t;
typedef struct hashtable_entityid_spritegroup_node_t {
    entityid key;
    spritegroup_t* value;
    hashtable_entityid_spritegroup_node_t* next;
} hashtable_entityid_spritegroup_node_t;

typedef struct hashtable_entityid_spritegroup_t {
    hashtable_entityid_spritegroup_node_t** table;
    int size;
    int num_entries;
} hashtable_entityid_spritegroup_t;

hashtable_entityid_spritegroup_t* hashtable_entityid_spritegroup_create(const int size);

const int hashtable_entityid_spritegroup_hash(hashtable_entityid_spritegroup_t* const ht, const entityid key);
const bool hashtable_entityid_spritegroup_has_specifier(hashtable_entityid_spritegroup_t* const ht,
                                                        const entityid key,
                                                        const specifier_t spec);
spritegroup_t* hashtable_entityid_spritegroup_get(hashtable_entityid_spritegroup_t* const ht, const entityid key);
spritegroup_t* hashtable_entityid_spritegroup_get_by_specifier(hashtable_entityid_spritegroup_t* const ht,
                                                               const entityid key,
                                                               const specifier_t spec);
void hashtable_entityid_spritegroup_print(hashtable_entityid_spritegroup_t* const ht);
void hashtable_entityid_spritegroup_delete(hashtable_entityid_spritegroup_t* const ht, const entityid key);
void hashtable_entityid_spritegroup_delete_all(hashtable_entityid_spritegroup_t* const ht, const entityid key);
void hashtable_entityid_spritegroup_insert(hashtable_entityid_spritegroup_t* const ht,
                                           const entityid key,
                                           spritegroup_t* value);

void hashtable_entityid_spritegroup_destroy(hashtable_entityid_spritegroup_t* ht);
