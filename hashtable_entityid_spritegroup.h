#pragma once

#include "entityid.h"
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
} hashtable_entityid_spritegroup_t;

int hashtable_entityid_spritegroup_hash(hashtable_entityid_spritegroup_t* ht, entityid key);
hashtable_entityid_spritegroup_t* hashtable_entityid_spritegroup_create(int size);
void hashtable_entityid_spritegroup_destroy(hashtable_entityid_spritegroup_t* ht);
void hashtable_entityid_spritegroup_insert(hashtable_entityid_spritegroup_t* ht,
                                           entityid key,
                                           spritegroup_t* value);
//spritegroup_t* hashtable_entityid_spritegroup_search(hashtable_entityid_spritegroup_t* ht,
spritegroup_t* hashtable_entityid_spritegroup_get(hashtable_entityid_spritegroup_t* ht,
                                                  entityid key);
void hashtable_entityid_spritegroup_delete(hashtable_entityid_spritegroup_t* ht, entityid key);
void hashtable_entityid_spritegroup_print(hashtable_entityid_spritegroup_t* ht);
