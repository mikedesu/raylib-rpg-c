#pragma once


typedef enum {
    RACETYPE_NONE,
    RACETYPE_HUMAN,
    RACETYPE_ORC,
    RACETYPE_HALF_ORC,
} racetype_t;


typedef struct {
    racetype_t primary;
    racetype_t secondary;
} race_t;
