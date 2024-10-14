#include "hashtable_entityid_entity.h"
#include "mprint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {

    // create hash table
    hashtable_entityid_entity_t* ht = hashtable_entityid_entity_create(100);

    // create entities
    entity_t* entities[10];
    for (int i = 0; i < 10; i++) {
        minfo("malloc entity");
        entities[i] = (entity_t*)malloc(sizeof(entity_t));
        entities[i]->id = i;
        snprintf(entities[i]->name, 128, "entity%d", i);
    }

    // insert entities
    for (int i = 0; i < 10; i++) {
        minfo("insert entity");
        hashtable_entityid_entity_insert(ht, entities[i]->id, entities[i]);
    }

    // search entities
    for (int i = 0; i < 10; i++) {
        minfo("search entity");
        entity_t* entity = hashtable_entityid_entity_search(ht, i);
        if (entity != NULL) {
            printf("entity id: %d, name: %s\n", entity->id, entity->name);
        }
    }

    // destroy hash table
    minfo("destroy hash table");
    hashtable_entityid_entity_destroy(ht);

    return 0;
}
