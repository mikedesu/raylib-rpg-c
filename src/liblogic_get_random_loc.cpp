#include "liblogic_get_random_loc.h"
#include "massert.h"
#include <raylib.h>

vec3 get_random_loc(rect r, int z) {
    massert(r.x >= 0, "rect x is neg");
    massert(r.y >= 0, "rect y is neg");
    massert(r.w > 0, "rect w is lt 1");
    massert(r.h > 0, "rect h is lt 1");
    vec3 loc = {0, 0, z};
    loc.x = GetRandomValue(r.x, r.x + r.w - 1);
    loc.y = GetRandomValue(r.y, r.y + r.h - 1);
    return loc;
}
