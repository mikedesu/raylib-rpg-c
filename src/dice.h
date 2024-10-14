#pragma once

typedef struct dice_t {
    unsigned int sides;
    unsigned int n;
    unsigned int mod;
} dice_t;


const unsigned int roll_dice(const dice_t dice);
