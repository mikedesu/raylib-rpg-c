/** @file create_sg_byid.h
 *  @brief Spritegroup-factory helpers keyed by entity runtime type and subtype.
 */

#pragma once

#include "gamestate.h"

void create_npc_sg_byid(gamestate& g, entityid id);
void create_door_sg_byid(gamestate& g, entityid id);
void create_box_sg_byid(gamestate& g, entityid id);
void create_chest_sg_byid(gamestate& g, entityid id);
void create_potion_sg_byid(gamestate& g, entityid id);
void create_weapon_sg_byid(gamestate& g, entityid id);
void create_shield_sg_byid(gamestate& g, entityid id);
void create_item_sg_byid(gamestate& g, entityid id);
void create_prop_sg_byid(gamestate& g, entityid id);
void create_sg_byid(gamestate& g, entityid id);
