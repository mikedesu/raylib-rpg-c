#pragma once
#include "entity_actions.h"
#include "entityid.h"
typedef struct default_action_component_t {
    entityid id;
    entity_action_t action;
} default_action_component;
