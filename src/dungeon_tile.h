#pragma once

/** @file dungeon_tile.h
 *  @brief Tile-level storage for floor geometry, visibility, and cached entities.
 */

#include "ComponentTraits.h"
#include "dead_npc_cache.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "item_cache.h"
#include "massert.h"
#include "mprint.h"
#include "tile_id.h"

/**
 * @brief Runtime storage for a single dungeon tile.
 *
 * A tile owns its tile type, fog-of-war state, door candidacy, and a compact
 * cache of entities currently occupying the tile. The cache is optimized for
 * the gameplay renderer and movement/interaction checks, so it stores
 * different entity categories in different ways.
 */
class tile_t {
private:
    bool can_have_door;
    bool visible;
    bool explored;
    bool cached_player_present;
    bool dirty_entities;
    tiletype_t type;
    entityid cached_live_npc;
    entityid cached_prop;
    entityid cached_box;
    entityid cached_door;
    dead_npc_cache dead_npcs;
    item_cache items;

public:
    /**
     * @brief Return the number of cached entities currently represented on this tile.
     *
     * @return Count of cached live entities, dead bodies, and item stack entries.
     */
    inline size_t entity_count() {
        size_t count = 0;
        count += cached_live_npc != INVALID ? 1 : 0;
        count += cached_prop != INVALID ? 1 : 0;
        count += cached_box != INVALID ? 1 : 0;
        count += cached_door != INVALID ? 1 : 0;

        count += dead_npcs.get_count();
        count += items.get_count();

        return count;
    }

    void set_cached_box(entityid id) {
        cached_box = id;
    }

    void set_cached_prop(entityid id) {
        cached_prop = id;
    }

    entityid get_cached_prop() {
        return cached_prop;
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
        massert(items.add_id(id), "item cache is full");
    }

    /** @brief Return the top visible item from the tile item cache. */
    entityid get_cached_item() {
        return items.top();
    }

    entityid get_cached_item_at(size_t index) {
        return items.at(index);
    }

    size_t get_item_count() {
        return items.get_count();
    }

    void set_cached_live_npc(entityid id) {
        cached_live_npc = id;
    }

    /** @brief Return the primary live NPC cached on this tile, if any. */
    entityid get_cached_live_npc() {
        return cached_live_npc;
    }

    void set_cached_dead_npc(entityid id) {
        dead_npcs.add_id(id);
        dirty_entities = true;
    }

    size_t get_dead_npc_count() {
        return dead_npcs.get_count();
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

    /** @brief Return whether this tile is currently visible to the player. */
    bool get_visible() {
        return visible;
    }

    tiletype_t get_type() {
        return type;
    }

    void set_type(tiletype_t t) {
        type = t;
    }

    /** @brief Return whether this tile may currently host a generated door. */
    bool get_can_have_door() {
        return can_have_door;
    }

    void set_can_have_door(bool b) {
        can_have_door = b;
    }

    constexpr inline bool tile_is_wall() {
        return tiletype_is_wall(type);
    }

    /**
     * @brief Clear all cached entity occupancy from the tile.
     *
     * This does not change the tile type or fog-of-war state.
     */
    inline void tile_reset_cache() {
        cached_player_present = false;
        cached_live_npc = ENTITYID_INVALID;
        cached_prop = ENTITYID_INVALID;
        dead_npcs = dead_npc_cache(); // Reset cache
        items = item_cache();
        cached_box = ENTITYID_INVALID;
        cached_door = ENTITYID_INVALID;
    }

    inline bool add_dead_npc(entityid id) {
        set_cached_dead_npc(id);
        return true;
    }

    /**
     * @brief Initialize the tile to a fresh runtime state with the given tile type.
     *
     * @param t Tile type to assign during initialization.
     */
    inline void tile_init(tiletype_t t) {
        minfo2("tile_init(%d)", t);
        type = t;
        visible = false;
        explored = false;
        //visible = true;
        //explored = true;
        can_have_door = false;
        dirty_entities = true;
        tile_reset_cache();
    }

    /**
     * @brief Add an entity to the tile's category-specific cache.
     *
     * @param id Entity id to cache on the tile.
     * @param type Logical entity type that controls which cache slot is used.
     * @return The entity id on success, or `INVALID` when the cache cannot accept it.
     */
    inline entityid tile_add(entityid id, entitytype_t type) {
        if (type == ENTITY_PLAYER) {
            cached_player_present = true;
            cached_live_npc = id;
        }
        else if (type == ENTITY_NPC) {
            cached_live_npc = id;
            // Dead NPCs are now handled through dead_npc_cache
        }
        else if (type == ENTITY_ITEM) {
            if (!items.add_id(id)) {
                merror("tile_add: item cache is full");
                return INVALID;
            }
        }
        else if (type == ENTITY_PROP) {
            cached_prop = id;
        }
        else if (type == ENTITY_BOX) {
            cached_box = id;
        }
        else if (type == ENTITY_DOOR) {
            if (cached_door != INVALID) {
                merror("tile_add: door cache already occupied");
                return INVALID;
            }
            cached_door = id;
        }
        dirty_entities = true;
        return id;
    }

    /**
     * @brief Remove an entity from whichever tile cache currently owns it.
     *
     * @param id Entity id to remove.
     * @return The removed entity id, or `INVALID` if the tile did not contain it.
     */
    inline entityid tile_remove(entityid id) {
        massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");
        if (id == cached_live_npc) {
            cached_live_npc = INVALID;
            if (cached_player_present) {
                cached_player_present = false;
            }
            return id;
        }
        else if (dead_npcs.remove_id(id)) {
            return id;
        }
        else if (id == cached_prop) {
            cached_prop = INVALID;
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
        else if (items.remove_id(id)) {
            return id;
        }
        return INVALID;
    }

    inline void tile_create(tiletype_t type) {
        massert(type >= TILE_NONE && type < TILE_COUNT, "tile_create: type is out-of-bounds");
        tile_init(type);
    }

    tile_t(tiletype_t t)
        : type(t) {
        //minfo2("BEGIN tile_t(%d, %d)", tid, t);
        tile_init(t);
        //minfo2("END tile_t(%d, %d)", tid, t);
    }

    tile_t() {
        minfo2("BEGIN tile_t()");
        tile_init(TILE_NONE);
        minfo2("END tile_t()");
    }

    ~tile_t() {
        minfo2("destroying tile");
    }

    /** @brief Return the top cached dead NPC on this tile, if any. */
    entityid get_cached_dead_npc() {
        return dead_npcs.top();
    }
};
