#pragma once

#include "vec3.h"
#include <algorithm>
#include <cstdlib>
#include <vector>




using std::abs;
using std::sort;
using std::vector;




static inline vector<vec3> calculate_path_with_thickness(vec3 start, vec3 end) {
    vector<vec3> path;

    int x1 = start.x;
    int y1 = start.y;
    int x2 = end.x;
    int y2 = end.y;
    int z = start.z;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        // Skip the start point (we don't need to check visibility with self)
        if (x1 != start.x || y1 != start.y) {
            // Add primary point
            path.push_back({x1, y1, z});
            // Add adjacent points for thickness only when we're not at start/end
            // and only when we're moving diagonally
            if (x1 != x2 && y1 != y2 && x1 != start.x && y1 != start.y) {
                // Add perpendicular points for diagonal movement
                if (dx > dy) {
                    path.push_back({x1, y1 + sy, z});
                    path.push_back({x1, y1 - sy, z});
                }
                else {
                    path.push_back({x1 + sx, y1, z});
                    path.push_back({x1 - sx, y1, z});
                }
            }
        }

        // Break if we've reached the end location
        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }

    // Remove duplicate points
    sort(path.begin(), path.end(), [](vec3& a, vec3& b) { return a.x < b.x || (a.x == b.x && a.y < b.y); });

    path.erase(unique(path.begin(), path.end(), [](vec3& a, vec3& b) { return a.x == b.x && a.y == b.y; }), path.end());

    return path;
}
