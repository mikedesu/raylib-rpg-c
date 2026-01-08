#pragma once

#include "ComponentTraits.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "massert.h"
#include "mprint.h"
#include "tile_id.h"
#include <algorithm>
#include <memory>
#include <vector>

//#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 8
//#define DUNGEON_TILE_MAX_ENTITIES_MAX 256
#define DUNGEON_TILE_ENTITIES_RESERVE 4


using std::find;
using std::make_shared;
using std::shared_ptr;
using std::vector;


class tile_t
{
private:
    bool can_have_door;
    tiletype_t type;
    tile_id id;
    bool visible;
    bool explored;
    bool cached_player_present;
    bool dirty_entities;
    int cached_live_npcs;
    int cached_item_count;
    entityid cached_npc;
    entityid cached_item;
    shared_ptr<vector<entityid>> entities;



public:
    shared_ptr<vector<entityid>> get_entities()
    {
        return entities;
    }




    const size_t get_entity_count()
    {
        return entities->size();
    }




    entityid get_entity_at(size_t i)
    {
        if (entities->size() == 0 || i >= entities->size())
        {
            return ENTITYID_INVALID;
        }
        return entities->at(i);
    }




    void set_cached_item(entityid id)
    {
        cached_item = id;
    }




    entityid get_cached_item()
    {
        return cached_item;
    }




    void set_cached_npc(entityid id)
    {
        cached_npc = id;
    }




    entityid get_cached_npc()
    {
        return cached_npc;
    }




    void set_cached_item_count(int n)
    {
        cached_item_count = n;
    }




    int get_cached_item_count()
    {
        return cached_item_count;
    }




    void set_cached_live_npcs(int n)
    {
        cached_live_npcs = n;
    }




    int get_cached_live_npcs()
    {
        return cached_live_npcs;
    }




    void set_dirty_entities(bool b)
    {
        dirty_entities = b;
    }




    bool get_dirty_entities()
    {
        return dirty_entities;
    }




    void set_cached_player_present(bool b)
    {
        cached_player_present = b;
    }




    bool get_cached_player_present()
    {
        return cached_player_present;
    }




    void set_explored(bool b)
    {
        explored = b;
    }




    bool get_explored()
    {
        return explored;
    }




    void set_visible(const bool b)
    {
        visible = b;
    }




    bool get_visible()
    {
        return visible;
    }




    tile_id get_id()
    {
        return id;
    }




    void set_id(const tile_id tid)
    {
        id = tid;
    }




    tiletype_t get_type()
    {
        return type;
    }




    void set_type(tiletype_t t)
    {
        type = t;
    }




    bool get_can_have_door()
    {
        return can_have_door;
    }




    void set_can_have_door(bool b)
    {
        can_have_door = b;
    }




    inline size_t tile_entity_count()
    {
        return entities->size();
    }




    constexpr inline entityid tile_get_entity(size_t i)
    {
        return i >= 0 && i < entities->size() ? entities->at(i) : ENTITYID_INVALID;
    }




    constexpr inline bool tile_is_wall()
    {
        return tiletype_is_wall(type);
    }




    inline void tile_init(tiletype_t t)
    {
        minfo2("tile_init(%d)", t);
        type = t;
        visible = false;
        explored = false;
        cached_player_present = false;
        can_have_door = false;
        dirty_entities = true;
        cached_live_npcs = 0;
        cached_item_count = 0;
        cached_npc = ENTITYID_INVALID;
        cached_item = ENTITYID_INVALID;

        entities = make_shared<vector<entityid>>();
        entities->reserve(DUNGEON_TILE_ENTITIES_RESERVE);
    }




    inline entityid tile_add(entityid id)
    {
        // Check if the entity already exists
        //minfo("tile_add: %d", id);
        if (find(entities->begin(), entities->end(), id) != entities->end())
        {
            merror("tile_add: entity already exists on tile");
            return ENTITYID_INVALID;
        }
        entities->push_back(id);
        dirty_entities = true;
        return id;
    }




    inline entityid tile_remove(entityid id)
    {
        massert(entities, "tile or tile entities is NULL");
        massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");
        auto it = find(entities->begin(), entities->end(), id);
        if (it == entities->end())
        {
            merror("tile_remove: entity not found on tile");
            return ENTITYID_INVALID;
        }
        entities->erase(it);
        dirty_entities = true;
        return id;
    }




    inline void tile_create(tiletype_t type)
    {
        massert(type >= TILE_NONE && type < TILE_COUNT, "tile_create: type is out-of-bounds");
        tile_init(type);
    }




    inline void tile_free()
    {
        entities->clear();
    }




    tile_t(tiletype_t t, tile_id tid)
        : type(t)
        , id(tid)
    {
        minfo2("BEGIN tile_t(%d, %d)", tid, t);
        tile_init(t);
        minfo2("END tile_t(%d, %d)", tid, t);
    }




    tile_t()
    {
        minfo2("BEGIN tile_t()");
        tile_init(TILE_NONE);
        minfo2("END tile_t()");
    }




    ~tile_t()
    {
        minfo2("destroying tile id %d", id);
        //tile_free();
    }
};
