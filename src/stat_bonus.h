#pragma once


static inline const int get_stat_bonus(int v) {
    if (v == 0 || v == 1) return -5;
    if (v == 2 || v == 3) return -4;
    if (v == 4 || v == 5) return -3;
    if (v == 6 || v == 7) return -2;
    if (v == 8 || v == 9) return -1;
    if (v == 10 || v == 11) return 0;
    if (v == 11 || v == 12) return 1;
    if (v == 13 || v == 14) return 2;
    if (v == 15 || v == 16) return 3;
    if (v == 17 || v == 18) return 4;

    return 0;
}
