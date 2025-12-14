#pragma once


static inline const int get_stat_bonus(int v) {
    if (v == 0 || v == 1)
        return -5;
    if (v == 2 || v == 3)
        return -4;
    if (v == 4 || v == 5)
        return -3;
    if (v == 6 || v == 7)
        return -2;
    if (v == 8 || v == 9)
        return -1;
    return (v - 10) / 2;
}
