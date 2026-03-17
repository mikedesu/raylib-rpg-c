#pragma once

/** @file dungeon_floor.h
 *  @brief Floor-level geometry, painting, placement, and tile access helpers.
 */

#include "biome.h"
#include "direction.h"
#include "dungeon_tile.h"
#include <functional>

using std::function;
using std::make_shared;

static constexpr int DEFAULT_DUNGEON_FLOOR_WIDTH = 8;
static constexpr int DEFAULT_DUNGEON_FLOOR_HEIGHT = 8;

/**
 * @brief Mutable runtime representation of a single dungeon floor.
 *
 * A dungeon floor owns the tile grid, floor-local metadata such as stairs and
 * lighting, and helper routines used by procedural generation and entity
 * placement.
 */
class dungeon_floor {
private:
    int floor; // the floor number, starting from 0
    int width;
    int height;
    biome_t biome;
    vec3 downstairs_loc;
    vec3 upstairs_loc;
    bool full_light;
    vector<tile_t> grid;

    inline size_t tile_index(vec3 loc) const {
        massert(loc.x >= 0 && loc.x < width, "x is out of bounds");
        massert(loc.y >= 0 && loc.y < height, "y is out of bounds");
        return static_cast<size_t>(loc.y) * static_cast<size_t>(width) + static_cast<size_t>(loc.x);
    }

public:
    /** @brief Return the upstairs location for this floor, or an invalid vec3 if unset. */
    vec3 get_upstairs_loc() {
        return upstairs_loc;
    }

    /** @brief Return the downstairs location for this floor, or an invalid vec3 if unset. */
    vec3 get_downstairs_loc() {
        return downstairs_loc;
    }

    biome_t get_biome() {
        return biome;
    }

    int get_height() {
        return height;
    }

    int get_width() {
        return width;
    }

    int get_floor() {
        return floor;
    }

    /** @brief Return whether debug full-light mode is enabled for this floor. */
    bool get_full_light() {
        return full_light;
    }

    void set_full_light(bool enabled) {
        full_light = enabled;
    }

    void toggle_full_light() {
        full_light = !full_light;
    }

    /**
     * @brief Assign an upstairs tile on this floor.
     *
     * @param loc Floor-local location; `z` will be normalized to this floor id.
     * @return `true` on success, otherwise `false`.
     */
    bool df_set_upstairs_loc(vec3 loc) {
        massert(loc.x >= 0 && loc.x < width, "x is out of bounds");
        massert(loc.y >= 0 && loc.y < height, "y is out of bounds");
        loc.z = floor;
        tile_t& tile = tile_at(loc);
        if (!tile_is_possible_upstairs(tile.get_type())) {
            return false;
        }
        if (tile.get_can_have_door() || tile.get_cached_door() != ENTITYID_INVALID) {
            return false;
        }
        upstairs_loc = loc;
        tile.set_type(TILE_UPSTAIRS);
        tile.set_can_have_door(false);
        return true;
    }

    /**
     * @brief Assign a downstairs tile on this floor.
     *
     * @param loc Floor-local location; `z` will be normalized to this floor id.
     * @return `true` on success, otherwise `false`.
     */
    bool df_set_downstairs_loc(vec3 loc) {
        massert(loc.x >= 0 && loc.x < width, "x is out of bounds");
        massert(loc.y >= 0 && loc.y < height, "y is out of bounds");
        loc.z = floor;
        tile_t& tile = tile_at(loc);
        if (!tile_is_possible_downstairs(tile.get_type())) {
            return false;
        }
        if (tile.get_can_have_door() || tile.get_cached_door() != ENTITYID_INVALID) {
            return false;
        }
        downstairs_loc = loc;
        tile.set_type(TILE_DOWNSTAIRS);
        tile.set_can_have_door(false);
        return true;
    }

    /**
     * @brief Return mutable access to a tile on this floor.
     *
     * @param loc Tile location on this floor.
     * @return Reference to the addressed tile.
     */
    tile_t& tile_at(vec3 loc) {
        minfo3("tile_at: (%d, %d, %d)", loc.x, loc.y, loc.z);
        return grid[tile_index(loc)];
    }

    void df_set_can_have_door(vec3 loc) {
        tile_t& tile = tile_at(loc);
        tile.set_can_have_door(true);
    }

    int df_count_walkable_cardinal_neighbors(vec3 loc) {
        static constexpr vec3 offsets[] = {
            vec3{0, -1, 0},
            vec3{-1, 0, 0},
            vec3{1, 0, 0},
            vec3{0, 1, 0},
        };

        int count = 0;
        for (const vec3 offset : offsets) {
            const vec3 neighbor_loc = {loc.x + offset.x, loc.y + offset.y, loc.z};
            if (neighbor_loc.x < 0 || neighbor_loc.x >= width || neighbor_loc.y < 0 || neighbor_loc.y >= height) {
                continue;
            }
            tile_t& neighbor = tile_at(neighbor_loc);
            if (tile_is_walkable(neighbor.get_type())) {
                count++;
            }
        }

        return count;
    }

    /**
     * @brief Return whether a walkable tile forms a valid geometric door candidate.
     *
     * @param loc Tile location to evaluate.
     * @return `true` when the tile matches the local doorway topology rules.
     */
    bool df_is_good_door_loc(vec3 loc) {
        tile_t& tile = tile_at(loc);
        if (!tile_is_walkable(tile.get_type())) {
            return false;
        }
        if (loc.x < 1 || loc.y < 1 || loc.x >= width - 1 || loc.y >= height - 1) {
            return false;
        }

        const vec3 left_loc = {loc.x - 1, loc.y, loc.z};
        const vec3 right_loc = {loc.x + 1, loc.y, loc.z};
        const vec3 up_loc = {loc.x, loc.y - 1, loc.z};
        const vec3 down_loc = {loc.x, loc.y + 1, loc.z};

        tile_t& left = tile_at(left_loc);
        tile_t& right = tile_at(right_loc);
        tile_t& up = tile_at(up_loc);
        tile_t& down = tile_at(down_loc);

        const bool left_walkable = tile_is_walkable(left.get_type());
        const bool right_walkable = tile_is_walkable(right.get_type());
        const bool up_walkable = tile_is_walkable(up.get_type());
        const bool down_walkable = tile_is_walkable(down.get_type());
        const int walkable_neighbors = static_cast<int>(left_walkable) + static_cast<int>(right_walkable) + static_cast<int>(up_walkable) + static_cast<int>(down_walkable);
        if (walkable_neighbors != 2) {
            return false;
        }

        const bool horizontal_passage = left_walkable && right_walkable && !up_walkable && !down_walkable;
        const bool vertical_passage = up_walkable && down_walkable && !left_walkable && !right_walkable;
        if (!horizontal_passage && !vertical_passage) {
            return false;
        }

        if (horizontal_passage) {
            const bool left_has_branch =
                tile_is_walkable(tile_at(vec3{left_loc.x, left_loc.y - 1, left_loc.z}).get_type()) ||
                tile_is_walkable(tile_at(vec3{left_loc.x, left_loc.y + 1, left_loc.z}).get_type());
            const bool right_has_branch =
                tile_is_walkable(tile_at(vec3{right_loc.x, right_loc.y - 1, right_loc.z}).get_type()) ||
                tile_is_walkable(tile_at(vec3{right_loc.x, right_loc.y + 1, right_loc.z}).get_type());
            return left_has_branch != right_has_branch;
        }

        const bool up_has_branch =
            tile_is_walkable(tile_at(vec3{up_loc.x - 1, up_loc.y, up_loc.z}).get_type()) ||
            tile_is_walkable(tile_at(vec3{up_loc.x + 1, up_loc.y, up_loc.z}).get_type());
        const bool down_has_branch =
            tile_is_walkable(tile_at(vec3{down_loc.x - 1, down_loc.y, down_loc.z}).get_type()) ||
            tile_is_walkable(tile_at(vec3{down_loc.x + 1, down_loc.y, down_loc.z}).get_type());
        return up_has_branch != down_has_branch;
    }

    bool tile_is_good_for_upgrade(vec3 loc) {
        auto tile = tile_at(loc);
        if (loc.x >= 1 && loc.y >= 1 && loc.x < width - 1 && loc.y < height - 1) {
            auto t0 = tile_at((vec3){loc.x - 1, loc.y - 1, loc.z});
            auto t1 = tile_at((vec3){loc.x - 1, loc.y, loc.z});
            auto t2 = tile_at((vec3){loc.x - 1, loc.y + 1, loc.z});
            auto t3 = tile_at((vec3){loc.x, loc.y - 1, loc.z});
            auto t4 = tile_at((vec3){loc.x, loc.y + 1, loc.z});
            auto t5 = tile_at((vec3){loc.x + 1, loc.y - 1, loc.z});
            auto t6 = tile_at((vec3){loc.x + 1, loc.y, loc.z});
            auto t7 = tile_at((vec3){loc.x + 1, loc.y + 1, loc.z});
            auto top_row_none = t0.get_type() == TILE_NONE && t3.get_type() == TILE_NONE && t5.get_type() == TILE_NONE;
            auto bottom_row_none = t2.get_type() == TILE_NONE && t4.get_type() == TILE_NONE && t7.get_type() == TILE_NONE;
            auto left_not_none = t1.get_type() != TILE_NONE;
            auto right_not_none = t6.get_type() != TILE_NONE;
            auto case1 = top_row_none && bottom_row_none && left_not_none && right_not_none;
            if (case1) {
                return true;
            }
        }
        return false;
    }

    void df_set_tile(tiletype_t type, int x, int y) {
        tile_t& current = tile_at((vec3){x, y, -1});
        current.set_type(type);
    }

    tiletype_t random_tiletype(tiletype_t a, tiletype_t b) {
        return (tiletype_t)GetRandomValue(a, b);
    }

    void set_area(tiletype_t a, tiletype_t b, Rectangle r) {
        for (int x = r.x; x < r.x + r.width && x < width; x++) {
            for (int y = r.y; y < r.y + r.height && y < height; y++) {
                const tiletype_t t = random_tiletype(a, b);
                df_set_tile(t, x, y);
            }
        }
    }

    /** @brief Clamp a rectangle to the valid bounds of this floor grid. */
    Rectangle df_clamp_area(Rectangle r) const {
        int x0 = static_cast<int>(r.x);
        int y0 = static_cast<int>(r.y);
        int x1 = x0 + static_cast<int>(r.width);
        int y1 = y0 + static_cast<int>(r.height);
        x0 = x0 < 0 ? 0 : x0;
        y0 = y0 < 0 ? 0 : y0;
        x1 = x1 < 0 ? 0 : x1;
        y1 = y1 < 0 ? 0 : y1;
        x1 = x1 > width ? width : x1;
        y1 = y1 > height ? height : y1;
        x1 = x1 < x0 ? x0 : x1;
        y1 = y1 < y0 ? y0 : y1;
        return Rectangle{static_cast<float>(x0), static_cast<float>(y0), static_cast<float>(x1 - x0), static_cast<float>(y1 - y0)};
    }

    /** @brief Return whether a rectangle fits fully within the floor bounds. */
    bool df_area_is_valid(Rectangle r) const {
        return r.width > 0 && r.height > 0 && r.x >= 0 && r.y >= 0 && r.x + r.width <= width && r.y + r.height <= height;
    }

    /**
     * @brief Paint a generic tile area after clamping it to the floor bounds.
     *
     * @return The clamped area that was actually painted.
     */
    Rectangle df_paint_area(tiletype_t a, tiletype_t b, Rectangle r) {
        Rectangle clamped = df_clamp_area(r);
        if (clamped.width <= 0 || clamped.height <= 0) {
            return Rectangle{0, 0, 0, 0};
        }
        set_area(a, b, clamped);
        return clamped;
    }

    /** @brief Paint a floor-material area and return the painted rectangle. */
    Rectangle df_paint_floor_area(Rectangle r) {
        return df_paint_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, r);
    }

    /**
     * @brief Compute a room rectangle adjacent to an origin room in the given direction.
     *
     * @return The proposed adjacent rectangle without validating or painting it.
     */
    Rectangle df_get_adjacent_area(Rectangle origin, direction_t dir, int gap, int area_width, int area_height) const {
        int x = static_cast<int>(origin.x);
        int y = static_cast<int>(origin.y);
        switch (dir) {
        case DIR_LEFT:
            x = static_cast<int>(origin.x) - gap - area_width;
            y = static_cast<int>(origin.y) + (static_cast<int>(origin.height) - area_height) / 2;
            break;
        case DIR_RIGHT:
            x = static_cast<int>(origin.x + origin.width) + gap;
            y = static_cast<int>(origin.y) + (static_cast<int>(origin.height) - area_height) / 2;
            break;
        case DIR_UP:
            x = static_cast<int>(origin.x) + (static_cast<int>(origin.width) - area_width) / 2;
            y = static_cast<int>(origin.y) - gap - area_height;
            break;
        case DIR_DOWN:
            x = static_cast<int>(origin.x) + (static_cast<int>(origin.width) - area_width) / 2;
            y = static_cast<int>(origin.y + origin.height) + gap;
            break;
        default: return Rectangle{0, 0, 0, 0};
        }
        return Rectangle{static_cast<float>(x), static_cast<float>(y), static_cast<float>(area_width), static_cast<float>(area_height)};
    }

    /**
     * @brief Compute the straight connector corridor between two axis-overlapping regions.
     *
     * @return The connector rectangle, or a zero-sized rectangle when no straight connector is possible.
     */
    Rectangle df_get_connector_area(Rectangle a, Rectangle b, int thickness = 1) const {
        const int corridor_thickness = thickness < 1 ? 1 : thickness;
        if (a.x + a.width <= b.x || b.x + b.width <= a.x) {
            const int overlap_start = static_cast<int>(a.y > b.y ? a.y : b.y);
            const int overlap_end = static_cast<int>((a.y + a.height) < (b.y + b.height) ? (a.y + a.height) : (b.y + b.height));
            if (overlap_end <= overlap_start) {
                return Rectangle{0, 0, 0, 0};
            }
            const int center_y = overlap_start + (overlap_end - overlap_start - 1) / 2;
            const int y = center_y - corridor_thickness / 2;
            if (a.x + a.width <= b.x) {
                return df_clamp_area(Rectangle{a.x + a.width, static_cast<float>(y), b.x - (a.x + a.width), static_cast<float>(corridor_thickness)});
            }
            return df_clamp_area(Rectangle{b.x + b.width, static_cast<float>(y), a.x - (b.x + b.width), static_cast<float>(corridor_thickness)});
        }

        if (a.y + a.height <= b.y || b.y + b.height <= a.y) {
            const int overlap_start = static_cast<int>(a.x > b.x ? a.x : b.x);
            const int overlap_end = static_cast<int>((a.x + a.width) < (b.x + b.width) ? (a.x + a.width) : (b.x + b.width));
            if (overlap_end <= overlap_start) {
                return Rectangle{0, 0, 0, 0};
            }
            const int center_x = overlap_start + (overlap_end - overlap_start - 1) / 2;
            const int x = center_x - corridor_thickness / 2;
            if (a.y + a.height <= b.y) {
                return df_clamp_area(Rectangle{static_cast<float>(x), a.y + a.height, static_cast<float>(corridor_thickness), b.y - (a.y + a.height)});
            }
            return df_clamp_area(Rectangle{static_cast<float>(x), b.y + b.height, static_cast<float>(corridor_thickness), a.y - (b.y + b.height)});
        }

        return Rectangle{0, 0, 0, 0};
    }

    /** @brief Paint a connector corridor between two regions. */
    Rectangle df_paint_connector(Rectangle a, Rectangle b, int thickness = 1) {
        Rectangle connector = df_get_connector_area(a, b, thickness);
        if (connector.width <= 0 || connector.height <= 0) {
            return connector;
        }
        return df_paint_floor_area(connector);
    }

    void df_mark_connector_door_candidates(Rectangle connector) {
        if (connector.width <= 0 || connector.height <= 0) {
            return;
        }

        if (connector.width >= connector.height) {
            const int y = static_cast<int>(connector.y + connector.height / 2);
            const int x0 = static_cast<int>(connector.x);
            const int x1 = static_cast<int>(connector.x + connector.width - 1);
            df_set_can_have_door(vec3{x0, y, floor});
            df_set_can_have_door(vec3{x1, y, floor});
            return;
        }

        const int x = static_cast<int>(connector.x + connector.width / 2);
        const int y0 = static_cast<int>(connector.y);
        const int y1 = static_cast<int>(connector.y + connector.height - 1);
        df_set_can_have_door(vec3{x, y0, floor});
        df_set_can_have_door(vec3{x, y1, floor});
    }

    /**
     * @brief Paint a new room adjacent to an origin room and connect it with a corridor.
     *
     * @return The adjacent room rectangle, or a zero-sized rectangle if placement is invalid.
     */
    Rectangle df_paint_adjacent_floor_area(Rectangle origin, direction_t dir, int gap, int area_width, int area_height, int corridor_width = 1) {
        Rectangle adjacent = df_get_adjacent_area(origin, dir, gap, area_width, area_height);
        if (!df_area_is_valid(adjacent)) {
            return Rectangle{0, 0, 0, 0};
        }
        df_paint_floor_area(adjacent);
        Rectangle connector = df_paint_connector(origin, adjacent, corridor_width);
        df_mark_connector_door_candidates(connector);
        return adjacent;
    }

    /** @brief Refresh walkable floor tiles that qualify for upgraded floor visuals. */
    void df_upgrade_floor_tiles() {
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                vec3 loc = {x, y, floor};
                if (!tile_is_good_for_upgrade(loc)) {
                    continue;
                }
                df_set_tile(TILE_FLOOR_STONE_10, x, y);
            }
        }
    }

    /** @brief Recompute which walkable tiles may host generated doors. */
    void df_refresh_door_candidates() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                vec3 loc = {x, y, floor};
                tile_t& tile = tile_at(loc);
                tile.set_can_have_door(false);
            }
        }
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                vec3 loc = {x, y, floor};
                tile_t& tile = tile_at(loc);
                if (!tile_is_walkable(tile.get_type())) {
                    continue;
                }
                if (df_is_good_door_loc(loc)) {
                    tile.set_can_have_door(true);
                }
            }
        }
    }

    void df_set_perimeter(tiletype_t a, tiletype_t b, Rectangle r) {
        for (int x = r.x; x < r.x + r.width; x++) {
            const tiletype_t t = random_tiletype(a, b);
            df_set_tile(t, x, r.y);
            df_set_tile(t, x, r.y + r.height - 1);
        }

        for (int y = r.y; y < r.y + r.height; y++) {
            const tiletype_t t = random_tiletype(a, b);
            df_set_tile(t, r.x, y);
            df_set_tile(t, r.x + r.width - 1, y);
        }
    }

    /**
     * @brief Initialize the floor grid and runtime metadata.
     *
     * @param f Floor index.
     * @param t Biome assigned to the floor.
     */
    void init(int f, biome_t t) {
        massert(f >= 0, "floor must be greater than or equal to zero");
        // creating a new dungeon floor
        // init floor vars
        floor = f;
        biome = t;
        massert(width > 0, "width must be greater than zero");
        massert(height > 0, "height must be greater than zero");
        grid.assign(static_cast<size_t>(width) * static_cast<size_t>(height), tile_t{});
        for (tile_t& tile : grid) {
            tile.set_type(TILE_NONE);
        }
        msuccess2("Created dungeon floor %d with dimensions %dx%d", floor, width, height);
    }

    void df_xform(function<void()> mLambda) {
        mLambda();
    }

    int df_get_possible_downstairs_count_in_area(Rectangle r) {
        massert(r.x >= 0, "x is less than zero");
        massert(r.x < width, "x is out of bounds");
        massert(r.y >= 0, "y is less than zero");
        massert(r.y < height, "y is out of bounds");
        massert(r.width > 0, "w is less than zero");
        massert(r.height > 0, "h is less than zero");
        massert(r.x + r.width <= width, "x + w is out of bounds");
        massert(r.y + r.height <= height, "y + h is out of bounds");
        int count = 0;
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                // check if the tile is possible downstairs
                if (tile_is_possible_downstairs(tile.get_type())) {
                    count++;
                }
            }
        }
        return count;
    }

    int df_get_possible_upstairs_count_in_area(Rectangle r) {
        massert(r.x >= 0, "x is less than zero");
        massert(r.x < width, "x is out of bounds");
        massert(r.y >= 0, "y is less than zero");
        massert(r.y < height, "y is out of bounds");
        massert(r.width > 0, "w is less than zero");
        massert(r.height > 0, "h is less than zero");
        massert(r.x + r.width <= width, "x + w is out of bounds");
        massert(r.y + r.height <= height, "y + h is out of bounds");
        int count = 0;
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                if (tile_is_possible_upstairs(tile.get_type())) {
                    count++;
                }
            }
        }
        return count;
    }

    /** @brief Return all valid upstairs candidate locations inside the given area. */
    shared_ptr<vector<vec3>> df_get_possible_upstairs_locs_in_area(Rectangle r) {
        auto locations = make_shared<vector<vec3>>();
        massert(locations, "failed to make_shared locations");
        // now we can loop thru the dungeon floor again and fill the array with the locations
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                // there wont be any entities yet so do not check for them
                if (tile_is_possible_upstairs(tile.get_type()) && !tile.get_can_have_door() && tile.get_cached_door() == ENTITYID_INVALID) {
                    locations->push_back(vec3{x0, y0, floor});
                }
            }
        }
        return locations;
    }

    /** @brief Return all valid downstairs candidate locations inside the given area. */
    shared_ptr<vector<vec3>> df_get_possible_downstairs_locs_in_area(Rectangle r) {
        massert(r.x >= 0, "x is less than zero");
        massert(r.x < width, "x is out of bounds");
        massert(r.y >= 0, "y is less than zero");
        massert(r.y < height, "y is out of bounds");
        shared_ptr<vector<vec3>> locations = make_shared<vector<vec3>>();
        massert(locations, "failed to make_shared locations");
        // now we can loop thru the dungeon floor again and fill the array with the locations
        for (int y0 = r.y; y0 < height; y0++) {
            for (int x0 = r.x; x0 < width; x0++) {
                tile_t& tile = tile_at(vec3{x0, y0, -1});
                // there wont be any entities yet so do not check for them
                if (tile_is_possible_downstairs(tile.get_type()) && !tile.get_can_have_door() && tile.get_cached_door() == ENTITYID_INVALID) {
                    locations->push_back(vec3{x0, y0, floor});
                }
            }
        }
        return locations;
    }

    int df_get_possible_upstairs_count() {
        return df_get_possible_upstairs_count_in_area((Rectangle){0, 0, (float)width, (float)height});
    }

    int df_get_possible_downstairs_count() {
        return df_get_possible_downstairs_count_in_area((Rectangle){0, 0, (float)width, (float)height});
    }

    shared_ptr<vector<vec3>> df_get_possible_upstairs_locs() {
        auto locs = df_get_possible_upstairs_locs_in_area((Rectangle){0, 0, (float)width, (float)height});
        massert(locs, "failed to get possible upstairs locations");
        return locs;
    }

    shared_ptr<vector<vec3>> df_get_possible_downstairs_locs() {
        auto locs = df_get_possible_downstairs_locs_in_area((Rectangle){0, 0, (float)width, (float)height});
        massert(locs, "failed to get possible downstairs locations");
        return locs;
    }

    void df_free() {
    }

    /**
     * @brief Add an entity to the tile cache at the given location.
     *
     * @param id Entity id to place.
     * @param type Entity category used by the tile cache.
     * @param loc Floor-local placement location.
     * @return The entity id on success, or `ENTITYID_INVALID` on failure.
     */
    entityid df_add_at(entityid id, entitytype_t type, vec3 loc) {
        if (id == ENTITYID_INVALID) {
            minfo("id is invalid");
            return ENTITYID_INVALID;
        }
        massert(!vec3_invalid(loc), "loc is invalid");
        massert(loc.x >= 0 && loc.x < width, "x is out of bounds");
        massert(loc.y >= 0 && loc.y < height, "y is out of bounds");
        tile_t& tile = tile_at(loc);
        const entityid result = tile.tile_add(id, type);
        if (result == ENTITYID_INVALID) {
            minfo("tile_add returned %d", result);
        }
        return result;
    }

    /**
     * @brief Remove an entity from the tile cache at the given location.
     *
     * @return `true` when the expected entity was removed.
     */
    bool df_remove_at(entityid id, vec3 l) {
        massert(id != ENTITYID_INVALID, "id is -1");
        massert(l.x >= 0 && l.x < width, "x is out of bounds");
        massert(l.y >= 0 && l.y < height, "y is out of bounds");
        tile_t& tile = tile_at(l);
        const entityid r = tile.tile_remove(id);
        if (r == ENTITYID_INVALID) {
            merror("df_remove_at: Failed to remove entity %d at (%d, %d)", id, l.x, l.y);
            return false;
        }
        if (r != id) {
            merror("df_remove_at: Removed entity %d but expected to remove %d at (%d, %d)", r, id, l.x, l.y);
            return false;
        }
        return true;
    }

    void df_set_all_tiles(tiletype_t type) {
        set_area(type, type, (Rectangle){0, 0, (float)width, (float)height});
    }

    /**
     * @brief Return a random empty walkable tile suitable for entity placement.
     *
     * @return A valid floor-local location, or `{-1,-1,-1}` if none exists.
     */
    vec3 get_random_loc() {
        vector<vec3> tmp;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                const vec3 loc = {x, y, floor};
                tile_t& tile = tile_at(loc);
                const bool type_invalid = tile.get_type() == TILE_NONE || tile.get_type() == TILE_STONE_WALL_00 || tile.get_type() == TILE_STONE_WALL_01 ||
                                          tile.get_type() == TILE_UPSTAIRS || tile.get_type() == TILE_DOWNSTAIRS;
                if (type_invalid) {
                    continue;
                }
                if (tile.get_cached_player_present() || tile.get_cached_live_npc() != INVALID || tile.get_dead_npc_count() > 0 ||
                    tile.get_cached_item() != INVALID || tile.get_cached_prop() != INVALID || tile.get_cached_box() != INVALID ||
                    tile.get_cached_chest() != INVALID || tile.get_cached_door() != INVALID) {
                    continue;
                }
                tmp.push_back(loc);
            }
        }
        if (tmp.size() == 0) {
            merror2("no locations are suitable. returning (-1, -1, -1)");
            return vec3{-1, -1, -1};
        }
        return tmp[GetRandomValue(0, tmp.size() - 1)];
    }

    /**
     * @brief Return a random location whose tile type matches the requested type.
     *
     * @return A valid floor-local location, or `{-1,-1,-1}` if none exists.
     */
    vec3 df_get_random_loc_of_type(tiletype_t type) {
        vector<vec3> tmp;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                vec3 loc = {x, y, floor};
                tile_t& tile = tile_at(loc);
                if (tile.get_type() != type) {
                    continue;
                }
                tmp.push_back(loc);
            }
        }
        if (tmp.size() == 0) {
            return vec3{-1, -1, -1};
        }
        vec3 loc = tmp[GetRandomValue(0, tmp.size() - 1)];
        return loc;
    }

    /**
     * @brief Construct an uninitialized floor with the requested dimensions.
     *
     * Call `init()` before using the tile grid.
     */
    dungeon_floor(int width = DEFAULT_DUNGEON_FLOOR_WIDTH, int height = DEFAULT_DUNGEON_FLOOR_HEIGHT)
        : floor(0)
        , width(width)
        , height(height)
        , biome(BIOME_NONE)
        , full_light(false) {
        massert(width > 0, "width must be greater than zero");
        massert(height > 0, "height must be greater than zero");
        upstairs_loc = {-1, -1, -1};
        downstairs_loc = {-1, -1, -1};
    }

    ~dungeon_floor() {
        minfo2("dungeon floor destroyed");
    }
};
