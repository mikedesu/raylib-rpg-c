#pragma once
#include "entityid.h"
#include "roll.h"
typedef struct base_attack_damage_component {
    entityid id;
    roll damage; // The base attack damage of the entity
} base_attack_damage_component;
