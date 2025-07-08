#include "hashtable_entityid_spritegroup.h"
#include "massert.h"
#include "mprint.h"
#include <stdlib.h>

hashtable_entityid_spritegroup_t* hashtable_entityid_spritegroup_create(int size) {
    if (size <= 0) {
        merror("hashtable_entityid_spritegroup_create: size must be greater than 0");
        return NULL;
    }
    hashtable_entityid_spritegroup_t* ht = (hashtable_entityid_spritegroup_t*)malloc(sizeof(hashtable_entityid_spritegroup_t));
    if (ht == NULL) {
        merror("hashtable_entityid_spritegroup_create: failed to allocate memory for hashtable_entityid_spritegroup");
        return NULL;
    }
    ht->size = size;
    ht->num_entries = 0;
    ht->table = (hashtable_entityid_spritegroup_node_t**)malloc(sizeof(hashtable_entityid_spritegroup_node_t*) * size);
    if (ht->table == NULL) {
        free(ht);
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

void hashtable_entityid_spritegroup_destroy(hashtable_entityid_spritegroup_t* ht) {
    if (ht) {
        minfo("Freeing hashtable");
        for (int i = 0; i < ht->size; i++) {
            hashtable_entityid_spritegroup_node_t* node = ht->table[i];
            while (node != NULL) {
                hashtable_entityid_spritegroup_node_t* temp = node;
                node = node->next;
                free(temp);
            }
        }
        free(ht->table);
        free(ht);
    }
}

const int hashtable_entityid_spritegroup_hash(hashtable_entityid_spritegroup_t* const ht, entityid key) { return key % ht->size; }

//const bool hashtable_entityid_spritegroup_has_specifier(hashtable_entityid_spritegroup_t* const ht,
//                                                        entityid key,
//                                                        specifier_t spec) {
//    if (ht == NULL || key < 0 || spec < SPECIFIER_NONE || spec >= SPECIFIER_COUNT) return false;
//    hashtable_entityid_spritegroup_node_t* node = ht->table[hashtable_entityid_spritegroup_hash(ht, key)];
//    while (node != NULL) {
//        if (node->key == key && node->value->specifier == spec) return true;
//        node = node->next;
//    }
//    return false;
//}

void hashtable_entityid_spritegroup_insert(hashtable_entityid_spritegroup_t* const ht, entityid key, spritegroup_t* value) {
    if (ht == NULL || key < 0 || value == NULL) return;
    //(value->specifier != SPECIFIER_NONE && hashtable_entityid_spritegroup_has_specifier(ht, key, value->specifier)))
    const int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* new_node = (hashtable_entityid_spritegroup_node_t*)malloc(sizeof(hashtable_entityid_spritegroup_node_t));
    new_node->key = key;
    new_node->value = value;
    new_node->next = ht->table[index]; // insert at head
    ht->table[index] = new_node;
    ht->num_entries++;
}

spritegroup_t* hashtable_entityid_spritegroup_get(hashtable_entityid_spritegroup_t* const ht, entityid key) {
    massert(ht, "hashtable is NULL");
    massert(key >= 0, "key is less than zero");
    //if (ht == NULL || key < 0) {
    //    return NULL;
    //}
    const int index = hashtable_entityid_spritegroup_hash(ht, key);
    massert(index >= 0 && index < ht->size, "index is out of bounds");
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    spritegroup_t* result = NULL;
    while (node != NULL) {
        if (node->key == key) {
            result = node->value;
            break;
        }
        node = node->next;
    }
    return result;
}

spritegroup_t* hashtable_entityid_spritegroup_get_by_index(hashtable_entityid_spritegroup_t* const ht, entityid key, int index) {
    if (ht == NULL || key < 0) {
        return NULL;
    }
    if (index < 0) {
        return NULL;
    }
    const int hash = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[hash];
    //spritegroup_t* result = NULL;
    int current_index = 0;
    while (node != NULL) {
        if (node->key == key) {
            //result = node->value;
            if (current_index == index) {
                return node->value;
            }
            current_index++;
        }
        node = node->next;
    }
    return NULL;
    //if (current_index == index) {
    //        break;
    //    }
    //    node = node->next;
    //}
    //return result;
}

//spritegroup_t* hashtable_entityid_spritegroup_get_by_specifier(hashtable_entityid_spritegroup_t* const ht, entityid key, specifier_t spec) {
//    if (ht == NULL || key < 0 || spec < SPECIFIER_NONE || spec >= SPECIFIER_COUNT) return NULL;
//    const int index = hashtable_entityid_spritegroup_hash(ht, key);
//    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
//    spritegroup_t* result = NULL;
//    while (node != NULL) {
//        if (node->key == key && node->value->specifier == spec) {
//            result = node->value;
//            break;
//        }
//        node = node->next;
//    }
//    return result;
//}

// this only deletes the first instance of the key
void hashtable_entityid_spritegroup_delete(hashtable_entityid_spritegroup_t* const ht, entityid key) {
    const int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    if (!node) return;
    if (node->key == key) {
        ht->table[hashtable_entityid_spritegroup_hash(ht, key)] = node->next;
        free(node);
        return;
    }
    while (node->next != NULL) {
        if (node->next->key == key) {
            hashtable_entityid_spritegroup_node_t* temp = node->next;
            node->next = node->next->next;
            free(temp);
            break;
        }
        node = node->next;
    }
}

// this deletes all instances of the key
void hashtable_entityid_spritegroup_delete_all(hashtable_entityid_spritegroup_t* const ht, entityid key) {
    const int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    if (node == NULL) return;
    while (node != NULL) {
        if (node->key != key) {
            while (node->next != NULL) {
                if (node->next->key == key) {
                    hashtable_entityid_spritegroup_node_t* temp = node->next;
                    node->next = node->next->next;
                    free(temp);
                } else
                    node = node->next;
            }
        } else {
            ht->table[index] = node->next;
            free(node);
            node = ht->table[index];
        }
    }
}

//int ht_entityid_sg_get_num_entries_for_key(hashtable_entityid_spritegroup_t* const ht, entityid key) {
//    spritegroup_t* sg = hashtable_entityid_spritegroup_get(ht, key);
//    if (!sg) {
//        return 0;
//    }
//    int count = 1;
//    int hash = hashtable_entityid_spritegroup_hash(ht, key);
//    hashtable_entityid_spritegroup_node_t* node = ht->table[hash];
//    while (node != NULL) {
//        if (node->key == key) {
//            count++;
//        }
//        node = node->next;
//    }
//    return count;
//}

int ht_entityid_sg_get_num_entries_for_key(hashtable_entityid_spritegroup_t* const ht, entityid key) {
    if (!ht || key < 0) {
        return 0;
    }
    int count = 0;
    int hash = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[hash];
    while (node != NULL) {
        if (node->key == key) {
            count++;
        }
        node = node->next;
    }
    return count;
}
