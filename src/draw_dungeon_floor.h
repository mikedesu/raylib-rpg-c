#pragma once

#include "calculate_linear_path.h"
#include "draw_sprite.h"
#include "dungeon_floor.h"
#include "entitytype.h"
#include "gamestate.h"
#include "get_txkey_for_tiletype.h"
#include "libdraw_player_target_box.h"
#include "textureinfo.h"



using std::for_each;
using std::sort;




extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];




static inline bool draw_dungeon_floor_tile(gamestate& g, textureinfo* txinfo, const int x, const int y, const int z)
{
    minfo2("BEGIN draw dungeon floor tile");
    //massert(g, "gamestate is NULL");
    massert(txinfo, "txinfo is null");
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    //auto df = d_get_floor(g.dungeon, z);
    shared_ptr<dungeon_floor> df = g.d.get_floor(z);
    //massert(df, "dungeon_floor is NULL");
    massert(x < df->get_width(), "x is out of bounds");
    massert(y < df->get_height(), "y is out of bounds");
    //massert(df, "dungeon_floor is NULL");


    // this creates a copy of the tile even though `df_tile_at(vec3)` returns a reference!
    //auto tile = df.df_tile_at((vec3){x, y, z});

    // this forces it to be a reference
    shared_ptr<tile_t> tile = df->tile_at((vec3){x, y, z});

    //massert(tile, "tile is NULL");
    //if (tile.type == TILE_NONE || !tile.visible || !tile.explored) {
    if (tile->get_type() == TILE_NONE || !tile->get_visible() || !tile->get_explored())
    {
        minfo2("END draw dungeon floor tile 0");
        return true;
    }
    // Get hero's total light radius
    const int light_dist = g.ct.get<light_radius>(g.hero_id).value_or(1);
    auto maybe_loc = g.ct.get<location>(g.hero_id);
    if (!maybe_loc.has_value())
    {
        minfo2("END draw dungeon floor tile 1");
        return false;
    }
    const vec3 hero_loc = maybe_loc.value();
    // Calculate Manhattan distance from hero to this tile (diamond pattern)
    const int distance = abs(x - hero_loc.x) + abs(y - hero_loc.y);
    // Get tile texture
    const int txkey = get_txkey_for_tiletype(tile->get_type());
    if (txkey < 0)
    {
        minfo2("END draw dungeon floor tile 2");
        return false;
    }
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0)
    {
        minfo2("END draw dungeon floor tile 3");
        return false;
    }
    // Calculate drawing position
    const int offset_x = -12;
    const int offset_y = -12;
    const int px = x * DEFAULT_TILE_SIZE + offset_x;
    const int py = y * DEFAULT_TILE_SIZE + offset_y;
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    const Rectangle dest = {(float)px, (float)py, (float)DEFAULT_TILE_SIZE_SCALED, (float)DEFAULT_TILE_SIZE_SCALED};
    // Draw tile with fade if beyond light dist
    Color draw_color = distance > light_dist ? Fade(WHITE, 0.4f) : WHITE; // Faded for out-of-range tiles
    if (distance > light_dist)
    {
        minfo2("END draw dungeon floor tile 4");
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
        return true;
    }
    // Draw tile with fade ALSO if path between tile and hero is blocked
    auto path = calculate_path_with_thickness({x, y, z}, hero_loc);
    // Check for blocking walls/doors in path
    bool blocking = false;
    for (const auto& v : path)
    {
        shared_ptr<tile_t> tile = df->tile_at(v);
        //if (tile && (tile_is_wall(tile->type))) {

        if (tile_is_none(tile->get_type()))
        {
            blocking = true;
            break;
        }
        else if (tiletype_is_wall(tile->get_type()))
        {
            blocking = true;
            break;
        }




        //if (blocking)
        //{
        //    break;
        //}


        // Check for closed doors
        for (const entityid id : *tile->get_entities())
        {
            if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR && !g.ct.get<door_open>(id).value_or(false))
            {
                blocking = true;
                break;
            }
        }

        if (blocking)
        {
            break;
        }
    }
    // Apply fade if blocked
    draw_color = blocking ? Fade(draw_color, 0.3f) : draw_color;
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
    minfo2("END draw dungeon floor tile 5");
    return true;
}




static inline void libdraw_draw_dungeon_floor_entitytype(gamestate& g, entitytype_t entitytype_0, function<bool(gamestate&, entityid)> additional_check)
{
    shared_ptr<dungeon_floor> df = g.d.get_current_floor();
    //massert(df, "dungeon_floor is NULL");
    const int z = g.d.current_floor;

    for (int y = 0; y < df->get_height(); y++)
    {
        for (int x = 0; x < df->get_width(); x++)
        {
            const vec3 loc = {x, y, z};
            shared_ptr<tile_t> tile = df->tile_at(loc);

            //if (!tile.visible) {
            //    continue;
            //}

            if (tile_is_none(tile->get_type()))
            {
                continue;
            }
            else if (tiletype_is_wall(tile->get_type()))
            {
                continue;
            }

            //if (tile.is_empty) {
            //    continue;
            //}

            // bugfix for tall walls so entities do not draw on top:
            // check to see if the tile directly beneath this tile is a wall
            //const vec3 loc2 = {x, y + 1, z};
            //const auto tile2 = df_tile_at(df, loc2);
            //if (tile2.type == TILE_STONE_WALL_00) {
            //    continue;
            //}

            // Get hero's vision distance and location
            const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
            const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);
            optional<vec3> maybe_hero_loc = g.ct.get<location>(g.hero_id);
            if (!maybe_hero_loc.has_value())
            {
                merror("Hero's location not set");
                continue;
            }

            const vec3 hero_loc = maybe_hero_loc.value();
            const int dist_to_check = std::max(vision_dist, light_rad);
            // Calculate Manhattan distance from hero to this tile (diamond pattern)
            const int dist = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
            // Only draw entities within vision distance
            // we might want to enforce a drawing order with the introduction of doors...
            if (dist > dist_to_check)
            {
                continue;
            }

            // further, we need to step from the hero's location+1 to the tile location-1
            // for each tile in this path, we need to check to see if
            // 1. the tiletype is a WALL
            // 2. the tile contains a DOOR entity
            // if either is true, then there is an object blocking our visibility of any entities
            // it will be eventually possible to "see-thru-walls" in the future to overcome this...

            // Calculate path from entity to hero
            vector<vec3> path = calculate_path_with_thickness(loc, hero_loc);

            // 2. for each item in path
            bool object_blocking = false;

            for (auto v0 : path)
            {
                shared_ptr<tile_t> v0_tile = df->tile_at(v0);
                if (tile_is_none(v0_tile->get_type()))
                {
                    object_blocking = true;
                    break;
                }

                if (tiletype_is_wall(v0_tile->get_type()))
                {
                    object_blocking = true;
                    break;
                }

                // check if tile has a DOOR
                //for_each(
                //    v0_tile.get_entities()->cbegin(),
                //    v0_tile.get_entities()->cend(),
                //    [&g, &object_blocking](const entityid& id)
                //    {
                //        if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR && !g.ct.get<door_open>(id).value_or(false))
                //            object_blocking = true;
                //    });

                for (const entityid id : *v0_tile->get_entities())
                {
                    if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR && !g.ct.get<door_open>(id).value_or(false))
                    {
                        object_blocking = true;
                        break;
                    }
                }

                if (object_blocking)
                {
                    break;
                }
            }

            // render all props
            //for_each(tile.entities->cbegin(), tile.entities->cend(), [&g, entitytype_0](const entityid& id) {
            //    auto type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
            //    if (entitytype_0 == ENTITY_PROP && type == ENTITY_PROP)
            //        draw_sprite_and_shadow(g, id);
            //});

            //if (object_blocking) {
            //    continue;
            //}

            for_each(
                tile->get_entities()->cbegin(),
                tile->get_entities()->cend(),
                [&g, entitytype_0, object_blocking, &additional_check](const entityid& id)
                {
                    const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
                    //auto sm = g.ct.get<spritemove>(id).value_or((Rectangle){0, 0, 0, 0});

                    //spritegroup_t* sg = spritegroups[id];
                    //if (sg->move.x != 0 || sg->move.y != 0) {
                    //    draw_sprite_and_shadow(g, id);
                    //}

                    if (!object_blocking && entitytype_0 == type && additional_check(g, id))
                    {
                        draw_sprite_and_shadow(g, id);
                    }
                    //else if (object_blocking && entitytype_0 == ENTITY_PROP && type == ENTITY_PROP) {
                    //    merror("A prop is being blocked by an object");
                    //} else if (type == ENTITY_PROP) {
                    //    merror("A prop is here");
                    //}
                });
        }
    }
}




const static inline bool draw_dungeon_floor(gamestate& g)
{
    //minfo2("BEGIN draw dungeon floor");
    shared_ptr<dungeon_floor> df = g.d.get_current_floor();
    const int z = g.d.current_floor;

    // render tiles
    //minfo2("BEGIN draw dungeon floor --- rendering tiles");
    //for (int y = 0; y < df.get_height(); y++)
    //{
    //    for (int x = 0; x < df.get_width(); x++)
    //    {
    //        draw_dungeon_floor_tile(g, txinfo, x, y, z);
    //    }
    //}


    for (int i = 0; i < df->get_height() * df->get_width(); i++)
    {
        //minfo2("BEGIN draw tile %d", i);
        const int y = i / df->get_width();
        const int x = i - (y * df->get_width());
        draw_dungeon_floor_tile(g, txinfo, x, y, z);
        //minfo2("END draw tile %d", i);
    }



    //minfo2("END draw dungeon floor --- rendering tiles");

    auto mydefault = [](gamestate& g, entityid id) { return true; };

    auto alive_check = [](gamestate& g, entityid id)
    {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value())
        {
            return !maybe_dead.value();
        }
        return false;
    };

    auto dead_check = [](gamestate& g, entityid id)
    {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value())
        {
            return maybe_dead.value();
        }
        return false;
    };

    //minfo2("BEGIN draw dungeon floor --- rendering entities");
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_DOOR, mydefault);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_PROP, mydefault);
    libdraw_draw_player_target_box(g);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_SPELL, mydefault);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_BOX, mydefault);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_NPC, dead_check);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_ITEM, mydefault);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_NPC, alive_check);
    libdraw_draw_dungeon_floor_entitytype(g, ENTITY_PLAYER, mydefault);
    //minfo2("END draw dungeon floor --- rendering entities");
    //minfo2("END draw dungeon floor");
    return true;
}
