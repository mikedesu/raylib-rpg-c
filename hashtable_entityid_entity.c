#include "hashtable_entityid_entity.h"
#include <stdlib.h>


hashtable_entityid_entity_t* hashtable_entityid_entity_create(int size) {
    hashtable_entityid_entity_t* ht =
        (hashtable_entityid_entity_t*)malloc(sizeof(hashtable_entityid_entity_t));
    ht->size = size;
    ht->table = (hashtable_entityid_entity_node_t**)malloc(
        sizeof(hashtable_entityid_entity_node_t*) * size);
    for (int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}


void hashtable_entityid_entity_destroy(hashtable_entityid_entity_t* ht) {

    for (int i = 0; i < ht->size; i++) {
        hashtable_entityid_entity_node_t* node = ht->table[i];
        while (node != NULL) {
            hashtable_entityid_entity_node_t* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}


int hashtable_entityid_entity_hash(hashtable_entityid_entity_t* ht, entityid key) {
    return key % ht->size;
}


void hashtable_entityid_entity_insert(hashtable_entityid_entity_t* ht,
                                      entityid key,
                                      entity_t* value) {

    int index = hashtable_entityid_entity_hash(ht, key);
    hashtable_entityid_entity_node_t* node = ht->table[index];
    while (node != NULL) {
        if (node->key == key) {
            node->value = value;
            return;
        }
        node = node->next;
    }
    hashtable_entityid_entity_node_t* new_node =
        (hashtable_entityid_entity_node_t*)malloc(sizeof(hashtable_entityid_entity_node_t));
    new_node->key = key;
    new_node->value = value;
    new_node->next = ht->table[index]; // insert at head
    ht->table[index] = new_node;
}


entity_t* hashtable_entityid_entity_get(hashtable_entityid_entity_t* ht, entityid key) {
    //entity_t* hashtable_entityid_entity_search(hashtable_entityid_entity_t* ht, entityid key) {
    int index = hashtable_entityid_entity_hash(ht, key);

    hashtable_entityid_entity_node_t* node = ht->table[index];
    entity_t* result = NULL;

    while (node != NULL) {
        if (node->key == key) {
            //return node->value;
            result = node->value;
            break;
        }
        node = node->next;
    }

    return result;
}


void hashtable_entityid_entity_delete(hashtable_entityid_entity_t* ht, entityid key) {
    int index = hashtable_entityid_entity_hash(ht, key);
    hashtable_entityid_entity_node_t* node = ht->table[index];
    if (node != NULL) {
        if (node->key == key) {
            ht->table[index] = node->next;
            free(node);
        } else {
            while (node->next != NULL) {
                if (node->next->key == key) {
                    hashtable_entityid_entity_node_t* temp = node->next;
                    node->next = node->next->next;
                    free(temp);
                    break;
                }
                node = node->next;
            }
        }
    }
}


//void hashtable_entityid_entity_print(hashtable_entityid_entity_t* ht) {
//}
