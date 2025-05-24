#pragma once
// bonus_table.h
static inline int bonus_calc(int stat) {
    // Returns a bonus based on the stat value
    // The bonus is calculated as (stat - 10) / 2, rounded down
    // This is a common D&D style bonus calculation
    return (stat - 10) / 2;
}
