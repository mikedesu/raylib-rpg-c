#pragma once

#include "entity.h"
#include "entityid.h"

typedef struct hashtable_entityid_entity_node_t hashtable_entityid_entity_node_t;
typedef struct hashtable_entityid_entity_node_t {
    entityid key;
    entity_t* value;
    hashtable_entityid_entity_node_t* next;
} hashtable_entityid_entity_node_t;


typedef struct hashtable_entityid_entity_t {
    hashtable_entityid_entity_node_t** table;
    int size;
} hashtable_entityid_entity_t;

const int hashtable_entityid_entity_hash(const hashtable_entityid_entity_t* ht, const entityid key);
hashtable_entityid_entity_t* hashtable_entityid_entity_create(const int size);
void hashtable_entityid_entity_destroy(hashtable_entityid_entity_t* ht);
void hashtable_entityid_entity_insert(hashtable_entityid_entity_t* ht, const entityid key, entity_t* value);
//entity_t* hashtable_entityid_entity_search(hashtable_entityid_entity_t* ht, entityid key);
entity_t* hashtable_entityid_entity_get(const hashtable_entityid_entity_t* ht, const entityid key);
void hashtable_entityid_entity_delete(hashtable_entityid_entity_t* ht, entityid key);
void hashtable_entityid_entity_print(const hashtable_entityid_entity_t* ht);
