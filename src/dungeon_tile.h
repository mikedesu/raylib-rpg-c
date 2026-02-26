#pragma once

#include "ComponentTraits.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "massert.h"
#include "mprint.h"
#include "tile_id.h"
//#include <algorithm>
//#include <memory>
//#include <vector>

//#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 8
//#define DUNGEON_TILE_MAX_ENTITIES_MAX 256
//#define DUNGEON_TILE_ENTITIES_RESERVE 4


//using std::find;
//using std::make_shared;
//using std::shared_ptr;
//using std::vector;


class tile_t {
private:
    bool can_have_door;
    tiletype_t type;
    tile_id id;
    bool visible;
    bool explored;
    bool cached_player_present;
    bool dirty_entities;
    //int cached_live_npcs;
    //int cached_item_count;

    entityid cached_live_npc;
    entityid cached_item;
    entityid cached_box;
    entityid cached_door;

    //shared_ptr<vector<entityid>> entities;



public:
    inline size_t entity_count() {
        size_t count = 0;
        count += cached_live_npc != INVALID ? 1 : 0;
        count += cached_item != INVALID ? 1 : 0;
        count += cached_box != INVALID ? 1 : 0;
        count += cached_door != INVALID ? 1 : 0;
        return count;
    }



    void set_cached_box(entityid id) {
        cached_box = id;
    }



    entityid get_cached_box() {
        return cached_box;
    }



    void set_cached_door(entityid id) {
        cached_door = id;
    }



    entityid get_cached_door() {
        return cached_door;
    }




    void set_cached_item(entityid id) {
        cached_item = id;
    }




    entityid get_cached_item() {
        return cached_item;
    }




    void set_cached_live_npc(entityid id) {
        cached_live_npc = id;
    }




    entityid get_cached_live_npc() {
        return cached_live_npc;
    }




    void set_dirty_entities(bool b) {
        dirty_entities = b;
    }




    bool get_dirty_entities() {
        return dirty_entities;
    }




    void set_cached_player_present(bool b) {
        cached_player_present = b;
    }




    bool get_cached_player_present() {
        return cached_player_present;
    }




    void set_explored(bool b) {
        explored = b;
    }




    bool get_explored() {
        return explored;
    }




    void set_visible(const bool b) {
        visible = b;
    }




    bool get_visible() {
        return visible;
    }




    tile_id get_id() {
        return id;
    }




    void set_id(const tile_id tid) {
        id = tid;
    }




    tiletype_t get_type() {
        return type;
    }




    void set_type(tiletype_t t) {
        type = t;
    }




    bool get_can_have_door() {
        return can_have_door;
    }




    void set_can_have_door(bool b) {
        can_have_door = b;
    }




    constexpr inline bool tile_is_wall() {
        return tiletype_is_wall(type);
    }




    inline void tile_reset_cache() {
        cached_player_present = false;
        cached_live_npc = ENTITYID_INVALID;
        cached_item = ENTITYID_INVALID;
        cached_box = ENTITYID_INVALID;
        cached_door = ENTITYID_INVALID;
    }




    inline void tile_init(tiletype_t t) {
        minfo2("tile_init(%d)", t);
        type = t;

        visible = true;
        explored = true;
        //visible = false;
        //explored = false;

        can_have_door = false;
        dirty_entities = true;

        tile_reset_cache();
    }




    inline entityid tile_add(entityid id, entitytype_t type) {
        // Check if the entity already exists
        //minfo("tile_add: %d", id);

        tile_reset_cache();

        if (type == ENTITY_PLAYER) {
            cached_player_present = true;
            cached_live_npc = id;
        }
        else if (type == ENTITY_NPC) {
            cached_live_npc = id;
        }
        else if (type == ENTITY_ITEM) {
            cached_item = id;
        }
        else if (type == ENTITY_BOX) {
            cached_box = id;
        }

        dirty_entities = true;
        return id;
    }




    inline entityid tile_remove(entityid id) {
        //massert(entities, "tile or tile entities is NULL");
        massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");

        dirty_entities = true;
        tile_reset_cache();

        if (id == cached_live_npc) {
            cached_live_npc = INVALID;
            return id;
        }
        else if (id == cached_box) {
            cached_box = INVALID;
            return id;
        }
        else if (id == cached_door) {
            cached_door = INVALID;
            return id;
        }
        else if (id == cached_item) {
            cached_item = INVALID;
            return id;
        }

        return INVALID;
    }




    inline void tile_create(tiletype_t type) {
        massert(type >= TILE_NONE && type < TILE_COUNT, "tile_create: type is out-of-bounds");
        tile_init(type);
    }




    //inline void tile_free() {
    //    entities->clear();
    //}




    tile_t(tiletype_t t, tile_id tid)
        : type(t)
        , id(tid) {
        minfo2("BEGIN tile_t(%d, %d)", tid, t);
        tile_init(t);
        minfo2("END tile_t(%d, %d)", tid, t);
    }




    tile_t() {
        minfo2("BEGIN tile_t()");
        tile_init(TILE_NONE);
        minfo2("END tile_t()");
    }




    ~tile_t() {
        minfo2("destroying tile id %d", id);
    }
};
