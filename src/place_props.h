//#pragma once
//#include "create_door.h"
//#include "create_prop.h"
//#include "dungeon_tile_type.h"
//#include "gamestate.h"
//static inline void place_props(gamestate& g) {
//    auto mydefault = [](gamestate& g, entityid id) {};
//    auto set_solid = [](gamestate& g, entityid id) { g.ct.set<solid>(id, true); };
//    auto set_solid_and_pushable = [](gamestate& g, entityid id) {
//        g.ct.set<solid>(id, true);
//        g.ct.set<pushable>(id, true);
//    };
//    for (int z = 0; z < (int)g.dungeon.floors.size(); z++) {
//        auto df = d_get_floor(g.dungeon, z);
//        for (int x = 0; x < df.width; x++) {
//            for (int y = 0; y < df.height; y++) {
//                const vec3 loc = {x, y, z};
//                auto tile = df_tile_at(df, loc);
//                //if (!tile.can_have_door)
//                //    continue;
//                //create_door_at_with(g, loc, [](gamestate& g, entityid id) {});
//                if (tile.type == TILE_UPSTAIRS || tile.type == TILE_DOWNSTAIRS)
//                    continue;
//                if (tile.can_have_door)
//                    continue;
//                if (tile_is_wall(tile)) {
//                    const int flip = GetRandomValue(0, 10);
//                    if (flip == 0) {
//                        const int r = GetRandomValue(1, 3);
//                        switch (r) {
//                        case 1: create_prop_at_with(g, PROP_DUNGEON_BANNER_00, loc, mydefault);
//                        case 2: create_prop_at_with(g, PROP_DUNGEON_BANNER_01, loc, mydefault);
//                        case 3: create_prop_at_with(g, PROP_DUNGEON_BANNER_02, loc, mydefault);
//                        default: break;
//                        }
//                    }
//                } else {
//                    const int flip = GetRandomValue(0, 10);
//                    if (flip == 0)
//                        create_prop_at_with(g, PROP_DUNGEON_WOODEN_TABLE_00, loc, set_solid);
//                    else if (flip == 1)
//                        create_prop_at_with(g, PROP_DUNGEON_WOODEN_TABLE_01, loc, set_solid);
//                    else if (flip == 2)
//                        create_prop_at_with(g, PROP_DUNGEON_WOODEN_CHAIR_00, loc, mydefault);
//                    else if (flip == 3)
//                        create_prop_at_with(g, PROP_DUNGEON_STATUE_00, loc, set_solid_and_pushable);
//                }
//            }
//        }
//    }
//}
