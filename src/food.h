#pragma once

typedef enum { FOOD_NONE, FOOD_BURGER, FOOD_COFFEE, FOOD_ONIGIRI, FOOD_COUNT } food_t;

typedef struct {
    int restore_amount;
} food_attribute_t;
