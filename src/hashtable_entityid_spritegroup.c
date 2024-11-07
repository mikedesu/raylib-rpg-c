#include "hashtable_entityid_spritegroup.h"
#include "mprint.h"
#include <stdlib.h>




hashtable_entityid_spritegroup_t* hashtable_entityid_spritegroup_create(const int size) {
    hashtable_entityid_spritegroup_t* ht = (hashtable_entityid_spritegroup_t*)malloc(sizeof(hashtable_entityid_spritegroup_t));
    ht->size = size;
    ht->table = (hashtable_entityid_spritegroup_node_t**)malloc(sizeof(hashtable_entityid_spritegroup_node_t*) * size);
    for (int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}




void hashtable_entityid_spritegroup_destroy(hashtable_entityid_spritegroup_t* ht) {
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




const int hashtable_entityid_spritegroup_hash(hashtable_entityid_spritegroup_t* ht, const entityid key) {
    return key % ht->size;
}



const bool hashtable_entityid_spritegroup_has_specifier(hashtable_entityid_spritegroup_t* ht, const entityid key, const specifier_t spec) {
    if (ht == NULL) {
        return false;
    }
    if (key < 0) {
        return false;
    }
    if (spec < SPECIFIER_NONE) {
        return false;
    }
    if (spec >= SPECIFIER_COUNT) {
        return false;
    }
    int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    while (node != NULL) {
        if (node->key == key && node->value->specifier == spec) {
            return true;
        }
        node = node->next;
    }
    return false;
}




void hashtable_entityid_spritegroup_insert(hashtable_entityid_spritegroup_t* ht, const entityid key, spritegroup_t* value) {
    if (ht == NULL) {
        merror("hashtable_entityid_spritegroup_insert: ht is NULL");
        return;
    }
    if (key < 0) {
        merror("hashtable_entityid_spritegroup_insert: key is negative");
        return;
    }
    if (value == NULL) {
        merror("hashtable_entityid_spritegroup_insert: value is NULL");
        return;
    }

    // we are cool if there are multiple NONE specifiers in a bucket
    // but we are not cool if there are multiple of the same specifier in a bucket
    if (value->specifier != SPECIFIER_NONE && hashtable_entityid_spritegroup_has_specifier(ht, key, value->specifier)) {
        char buf[255];
        snprintf(buf, 255, "hashtable_entityid_spritegroup_insert: id %d already has a spritegroup with specifier: %s", key, specifier_get_str(value->specifier));
        merror(buf);
        return;
    }

    int index = hashtable_entityid_spritegroup_hash(ht, key);
    //hashtable_entityid_spritegroup_node_t* node = ht->table[index];

    //while (node != NULL) {
    //    if (node->key == key) {
    //        minfo("hashtable_entityid_spritegroup key already exists, updating value");
    //        node->value = value;
    //        return;
    //    }
    //    node = node->next;
    //}

    hashtable_entityid_spritegroup_node_t* new_node = (hashtable_entityid_spritegroup_node_t*)malloc(sizeof(hashtable_entityid_spritegroup_node_t));
    new_node->key = key;
    new_node->value = value;
    new_node->next = ht->table[index]; // insert at head

    ht->table[index] = new_node;
}




spritegroup_t* hashtable_entityid_spritegroup_get(hashtable_entityid_spritegroup_t* ht, const entityid key) {
    if (ht == NULL) {
        return NULL;
    }
    if (key < 0) {
        return NULL;
    }
    int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    spritegroup_t* result = NULL;
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




spritegroup_t* hashtable_entityid_spritegroup_get_by_specifier(hashtable_entityid_spritegroup_t* ht, const entityid key, const specifier_t spec) {
    if (ht == NULL) {
        merror("hashtable_entityid_spritegroup_get_by_specifier: ht is NULL");
        return NULL;
    }
    if (key < 0) {
        //merror("hashtable_entityid_spritegroup_get_by_specifier: key is negative");
        return NULL;
    }
    if (spec < SPECIFIER_NONE) {
        merror("hashtable_entityid_spritegroup_get_by_specifier: spec is less than SPECIFIER_NONE");
        return NULL;
    }
    if (spec >= SPECIFIER_COUNT) {
        merror("hashtable_entityid_spritegroup_get_by_specifier: spec is greater than or equal to SPECIFIER_COUNT");
        return NULL;
    }
    int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    spritegroup_t* result = NULL;
    while (node != NULL) {
        if (node->key == key && node->value->specifier == spec) {
            //return node->value;
            result = node->value;
            break;
        }

        // print the node specifier
        //char buf[255];
        //snprintf(buf, 255, "hashtable_entityid_spritegroup node specifier: %s", specifier_get_str(node->value->specifier));
        //minfo(buf);

        node = node->next;
    }

    if (!result) {
        //merror("hashtable_entityid_spritegroup_get_by_specifier: no spritegroup found");
    }

    return result;
}



// this only deletes the first instance of the key
void hashtable_entityid_spritegroup_delete(hashtable_entityid_spritegroup_t* ht, const entityid key) {
    int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    if (node != NULL) {
        if (node->key == key) {
            ht->table[index] = node->next;
            free(node);
        } else {
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
    }
}




// this deletes all instances of the key
void hashtable_entityid_spritegroup_delete_all(hashtable_entityid_spritegroup_t* ht, const entityid key) {
    int index = hashtable_entityid_spritegroup_hash(ht, key);
    hashtable_entityid_spritegroup_node_t* node = ht->table[index];
    if (node != NULL) {
        while (node != NULL) {
            if (node->key == key) {
                ht->table[index] = node->next;
                free(node);
                node = ht->table[index];
            } else {
                while (node->next != NULL) {
                    if (node->next->key == key) {
                        hashtable_entityid_spritegroup_node_t* temp = node->next;
                        node->next = node->next->next;
                        free(temp);
                    } else {
                        node = node->next;
                    }
                }
            }
        }
    }
}
