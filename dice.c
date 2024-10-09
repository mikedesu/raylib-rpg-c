#include "dice.h"
#include <stdlib.h>

const unsigned int roll_dice(const dice_t dice) {
    unsigned int result = 0;
    for (unsigned int i = 0; i < dice.n; i++) {
        result += (rand() % dice.sides) + 1;
    }
    return result + dice.mod;
}
