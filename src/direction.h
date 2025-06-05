#pragma once

//#include "location.h"
#include "vec3.h"
typedef enum direction_t
{
    DIR_NONE,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP_LEFT,
    DIR_UP_RIGHT,
    DIR_DOWN_LEFT,
    DIR_DOWN_RIGHT,
    DIR_UNKNOWN,
    DIR_COUNT
} direction_t;

static inline int get_x_from_dir(direction_t dir) {
    switch (dir) {
    case DIR_UP_LEFT:
    case DIR_DOWN_LEFT:
    case DIR_LEFT: return -1;
    case DIR_UP_RIGHT:
    case DIR_DOWN_RIGHT:
    case DIR_RIGHT: return 1;
    case DIR_UP:
    case DIR_DOWN:
    case DIR_NONE:
    case DIR_UNKNOWN:
    default: return 0;
    }
}

static inline int get_y_from_dir(direction_t dir) {
    switch (dir) {
    case DIR_UP:
    case DIR_UP_LEFT:
    case DIR_UP_RIGHT: return -1;
    case DIR_DOWN:
    case DIR_DOWN_LEFT:
    case DIR_DOWN_RIGHT: return 1;
    case DIR_LEFT:
    case DIR_RIGHT:
    case DIR_NONE:
    case DIR_UNKNOWN:
    default: return 0;
    }
}

static inline vec3 get_loc_from_dir(direction_t dir) {
    vec3 loc = {0, 0, 0};
    switch (dir) {
    case DIR_UP: loc = (vec3){0, -1}; break;
    case DIR_DOWN: loc = (vec3){0, 1}; break;
    case DIR_LEFT: loc = (vec3){-1, 0}; break;
    case DIR_RIGHT: loc = (vec3){1, 0}; break;
    case DIR_UP_LEFT: loc = (vec3){-1, -1}; break;
    case DIR_UP_RIGHT: loc = (vec3){1, -1}; break;
    case DIR_DOWN_LEFT: loc = (vec3){-1, 1}; break;
    case DIR_DOWN_RIGHT: loc = (vec3){1, 1}; break;
    case DIR_NONE:
    case DIR_UNKNOWN:
    default: loc = (vec3){0, 0}; break;
    }
    return loc;
}

static inline const direction_t get_dir_from_xy(int x, int y) {
    if (x == 0 && y == 0) return DIR_NONE;
    if (x == 0 && y == -1) return DIR_UP;
    if (x == 0 && y == 1) return DIR_DOWN;
    if (x == -1 && y == 0) return DIR_LEFT;
    if (x == 1 && y == 0) return DIR_RIGHT;
    // also handle diagonals
    if (x == -1 && y == -1) return DIR_UP_LEFT;
    if (x == 1 && y == -1) return DIR_UP_RIGHT;
    if (x == -1 && y == 1) return DIR_DOWN_LEFT;
    if (x == 1 && y == 1) return DIR_DOWN_RIGHT;
    return DIR_UNKNOWN;
}

static inline direction_t get_dir_from_loc(vec3 loc) { return get_dir_from_xy(loc.x, loc.y); }

static inline const char* get_dir_as_string(direction_t dir) {
    switch (dir) {
    case DIR_UP: return "up";
    case DIR_DOWN: return "down";
    case DIR_LEFT: return "left";
    case DIR_RIGHT: return "right";
    case DIR_UP_LEFT: return "up_left";
    case DIR_UP_RIGHT: return "up_right";
    case DIR_DOWN_LEFT: return "down_left";
    case DIR_DOWN_RIGHT: return "down_right";
    default: return "unknown";
    }
}
