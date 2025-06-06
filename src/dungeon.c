// dungeon.c
#include "dungeon.h"
#include "dungeon_tile_type.h"
//#include "massert.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_DUNGEON_CAPACITY 4

dungeon_t* d_create() {
    dungeon_t* dungeon = malloc(sizeof(dungeon_t));
    if (!dungeon) {
        merror("Failed to allocate memory for dungeon");
        return NULL;
    }

    dungeon->floors = malloc(sizeof(dungeon_floor_t*) * INITIAL_DUNGEON_CAPACITY);
    if (!dungeon->floors) {
        merror("Failed to allocate memory for dungeon floors");
        free(dungeon);
        return NULL;
    }

    dungeon->current_floor = 0;
    dungeon->num_floors = 0;
    dungeon->capacity_floors = INITIAL_DUNGEON_CAPACITY;
    dungeon->is_locked = false;
    msuccess("Created new dungeon with capacity %d", INITIAL_DUNGEON_CAPACITY);
    return dungeon;
}

void d_destroy(dungeon_t* const d) {
    if (!d) {
        return;
    }

    if (d->floors) {
        for (int i = 0; i < d->num_floors; i++) {
            if (d->floors[i]) {
                df_free(d->floors[i]);
            }
        }
        free(d->floors);
    }
    free(d);
}

void d_free(dungeon_t* const dungeon) {
    if (!dungeon) {
        merror("Cannot free - dungeon is NULL");
        return;
    }
    minfo("Freeing dungeon with %d floors", dungeon->num_floors);
    d_destroy(dungeon);
}

size_t d_serialized_size(const dungeon_t* d) {
    massert(d, "dungeon is NULL");

    // Calculate size by exactly matching what's written in d_serialize
    size_t size = 0;

    // Basic fields
    size += sizeof(int) * 3; // num_floors, capacity_floors, current_floor
    size += sizeof(bool); // is_locked

    // Floors
    for (int i = 0; i < d->num_floors; i++) {
        size += sizeof(size_t); // Size of the serialized floor
        size += df_serialized_size(d->floors[i]);
    }

    return size;
}

size_t d_serialize(const dungeon_t* d, char* buffer, size_t buffer_size) {
    massert(d, "dungeon is NULL");
    massert(buffer, "buffer is NULL");

    size_t required_size = d_serialized_size(d);
    if (buffer_size < required_size) {
        merror("Buffer too small for dungeon serialization: %zu < %zu", buffer_size, required_size);
        return 0;
    }

    char* ptr = buffer;

    // Serialize basic fields
    memcpy(ptr, &d->num_floors, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &d->capacity_floors, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &d->current_floor, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &d->is_locked, sizeof(bool));
    ptr += sizeof(bool);

    // Serialize each floor
    for (int i = 0; i < d->num_floors; i++) {
        // First write the size of the serialized floor
        size_t floor_size = df_serialized_size(d->floors[i]);
        memcpy(ptr, &floor_size, sizeof(size_t));
        ptr += sizeof(size_t);

        // Then serialize the floor
        size_t written = df_serialize(d->floors[i], ptr, floor_size);
        if (written == 0) {
            merror("Failed to serialize floor %d", i);
            return 0;
        }
        ptr += written;
    }

    return ptr - buffer;
}

bool d_deserialize(dungeon_t* d, const char* buffer, size_t buffer_size) {
    massert(d, "dungeon is NULL");
    massert(buffer, "buffer is NULL");

    // Initialize the dungeon with default values
    d->floors = NULL;
    d->num_floors = 0;
    d->capacity_floors = 0;
    d->current_floor = 0;
    d->is_locked = false;

    const char* ptr = buffer;
    size_t remaining = buffer_size;

    // Deserialize basic fields
    if (remaining < sizeof(int) * 3 + sizeof(bool)) {
        merror("Buffer too small for dungeon basic fields");
        return false;
    }

    memcpy(&d->num_floors, ptr, sizeof(int));
    ptr += sizeof(int);
    remaining -= sizeof(int);

    memcpy(&d->capacity_floors, ptr, sizeof(int));
    ptr += sizeof(int);
    remaining -= sizeof(int);

    memcpy(&d->current_floor, ptr, sizeof(int));
    ptr += sizeof(int);
    remaining -= sizeof(int);

    memcpy(&d->is_locked, ptr, sizeof(bool));
    ptr += sizeof(bool);
    remaining -= sizeof(bool);
    // Validate fields
    if (d->num_floors < 0 || d->capacity_floors < d->num_floors || (d->num_floors > 0 && (d->current_floor < 0 || d->current_floor >= d->num_floors))) {
        merror("Invalid dungeon fields after deserialization");
        return false;
    }
    // Allocate memory for floors
    d->floors = malloc(d->capacity_floors * sizeof(dungeon_floor_t*));
    if (!d->floors) {
        merror("Failed to allocate memory for floors during deserialization");
        return false;
    }
    // Initialize all floor pointers to NULL
    for (int i = 0; i < d->capacity_floors; i++) {
        d->floors[i] = NULL;
    }
    // Deserialize each floor
    for (int i = 0; i < d->num_floors; i++) {
        // First read the size of the serialized floor
        if (remaining < sizeof(size_t)) {
            merror("Buffer too small for floor size");
            d_destroy(d);
            return false;
        }
        size_t floor_size;
        memcpy(&floor_size, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        remaining -= sizeof(size_t);
        if (remaining < floor_size) {
            merror("Buffer too small for floor data");
            d_destroy(d);
            return false;
        }
        // Create a new floor
        d->floors[i] = malloc(sizeof(dungeon_floor_t));
        if (!d->floors[i]) {
            merror("Failed to allocate memory for floor %d", i);
            d_destroy(d);
            return false;
        }
        // Deserialize the floor
        if (!df_deserialize(d->floors[i], ptr, floor_size)) {
            merror("Failed to deserialize floor %d", i);
            d_destroy(d);
            return false;
        }
        ptr += floor_size;
        remaining -= floor_size;
    }
    return true;
}

size_t d_memory_size(const dungeon_t* d) {
    massert(d, "dungeon is NULL");
    // Calculate the memory size of a dungeon
    // Size of the dungeon_t struct itself
    size_t size = sizeof(dungeon_t);
    // Size of floors array
    if (d->floors) {
        size += d->capacity_floors * sizeof(dungeon_floor_t*);
        // Size of each floor
        for (int i = 0; i < d->num_floors; i++)
            if (d->floors[i]) size += df_memory_size(d->floors[i]);
    }
    return size;
}

bool d_add_floor(dungeon_t* const dungeon, int width, int height) {
    if (!dungeon) return false;
    if (width <= 0 || height <= 0) return false;
    if (dungeon->is_locked) return false;
    if (dungeon->num_floors >= dungeon->capacity_floors) {
        int new_capacity = dungeon->capacity_floors * 2;
        dungeon_floor_t** new_floors = realloc(dungeon->floors, sizeof(dungeon_floor_t*) * new_capacity);
        if (!new_floors) return false;
        dungeon->floors = new_floors;
        dungeon->capacity_floors = new_capacity;
    }
    int current_floor = dungeon->num_floors;
    dungeon_floor_t* new_floor = df_create(current_floor, width, height);
    if (!new_floor) return false;
    // Initialize the new floor
    df_init(new_floor);
    // Add a room
    int w = 20;
    int h = 20;
    // Get center of the floor
    int cx = width / 2;
    int cy = height / 2;

    bool res = df_add_room(new_floor, cx, cy, w, h, TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, "TestRoom");
    if (!res) {
        //merror("Failed to add room to new floor");
        df_free(new_floor);
        return false;
    }
    // Assign upstairs and downstairs locations
    res = df_assign_upstairs_in_area(new_floor, 0, 0, width, height);
    df_assign_downstairs_in_area(new_floor, 0, 0, width, height);
    // Add the new floor to the dungeon
    dungeon->floors[dungeon->num_floors++] = new_floor;
    return true;
}
