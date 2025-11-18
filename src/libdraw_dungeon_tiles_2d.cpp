#include "libdraw_dungeon_floor_tile.h"
#include "libdraw_dungeon_tiles_2d.h"
#include "textureinfo.h"

extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

bool draw_dungeon_tiles_2d(const shared_ptr<gamestate> g, int z, shared_ptr<dungeon_floor_t> df) {
    massert(g, "gamestate is NULL");
    massert(df, "dungeon_floor is NULL");
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            draw_dungeon_floor_tile(g, txinfo, x, y, z);
        }
    }
    return true;
}
