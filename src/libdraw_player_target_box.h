#pragma once

#include "gamestate.h"
#include "libgame_defines.h"


extern unordered_map<int, Shader> shaders;


static inline bool libdraw_draw_player_target_box(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");

    entityid id = g->hero_id;

    if (id == -1) {
        return false;
    }

    //minfo("getting direction");
    direction_t dir = g->ct.get<direction>(id).value();

    //minfo("getting location");
    vec3 loc = g->ct.get<location>(id).value();

    float x = loc.x + get_x_from_dir(dir);
    float y = loc.y + get_y_from_dir(dir);
    float w = DEFAULT_TILE_SIZE;
    float h = DEFAULT_TILE_SIZE;
    //Color base_c = GREEN;
    float a = 0.75f;
    if (g->player_changing_dir) {
        a = 0.9f;
    }


    //minfo("begin shaders");
    float time = (float)GetTime();
    SetShaderValue(shaders[1], GetShaderLocation(shaders[1], "time"), &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shaders[1], GetShaderLocation(shaders[1], "alpha"), &a, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(shaders[1]);

    DrawRectangleLinesEx((Rectangle){x * w, y * h, w, h}, 1, Fade(GREEN, a));

    //minfo("end shaders");
    EndShaderMode();

    return true;
}
