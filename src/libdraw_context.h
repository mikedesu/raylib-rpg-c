#pragma once

#include "gamestate.h"
#include "libgame_defines.h"
#include "spritegroup.h"
#include "textureinfo.h"
#include <raylib.h>
#include <unordered_map>

using std::unordered_map;

extern unordered_map<entityid, spritegroup*> spritegroups;
extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
extern unordered_map<int, Shader> shaders;
extern RenderTexture2D title_target_texture;
extern RenderTexture2D char_creation_target_texture;
extern RenderTexture2D main_game_target_texture;
extern RenderTexture2D hud_target_texture;
extern RenderTexture2D target;
extern Rectangle target_src;
extern Rectangle target_dest;
extern Rectangle win_dest;
extern Music music;
extern int ANIM_SPEED;
