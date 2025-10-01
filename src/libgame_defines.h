#pragma once

//#define GAME_VERSION "v0.0.3.19-20250704"
#define WINDOW_TITLE "@evildojo666 presents: project.rpg"
#define GAME_VERSION "v0.0.3.30-2025-09-27"

//#define DEFAULT_MUSIC_VOLUME 0.5f
//#define DEFAULT_WIN_WIDTH 800
//#define DEFAULT_WIN_HEIGHT 480
//#define DEFAULT_WIN_WIDTH 640
//#define DEFAULT_WIN_HEIGHT 360
#define DEFAULT_WIN_WIDTH 1920
#define DEFAULT_WIN_HEIGHT 1080
//#define DEFAULT_WIN_WIDTH 1280
//#define DEFAULT_WIN_HEIGHT 720
//#define DEFAULT_WIN_WIDTH 960
//#define DEFAULT_WIN_HEIGHT 540

//#define DEFAULT_TARGET_WIDTH 1920
//#define DEFAULT_TARGET_HEIGHT 1080
//#define DEFAULT_TARGET_WIDTH 1280
//#define DEFAULT_TARGET_HEIGHT 720
#define DEFAULT_TARGET_WIDTH 960
#define DEFAULT_TARGET_HEIGHT 540

//#define DEFAULT_TARGET_WIDTH 800
//#define DEFAULT_TARGET_HEIGHT 480
//#define DEFAULT_TARGET_WIDTH 640
//#define DEFAULT_TARGET_HEIGHT 360
//#define SPRITEGROUP_DEFAULT_SIZE 32
#define DEFAULT_TILE_SIZE_SCALED 32


#define DEFAULT_TARGET_FPS 60
#define FRAMEINTERVAL 10
#define DEFAULT_LOCK 30
#define DEFAULT_TILE_SIZE 8
#define DEFAULT_ZOOM_INCR 1.0f

#define TX_HUMAN_IDLE 1
#define TX_HUMAN_IDLE_SHADOW 2
#define TX_HUMAN_WALK 3
#define TX_HUMAN_WALK_SHADOW 4
#define TX_HUMAN_ATTACK 5
#define TX_HUMAN_ATTACK_SHADOW 6
#define TX_HUMAN_JUMP 7
#define TX_HUMAN_JUMP_SHADOW 8
#define TX_HUMAN_SPIN_DIE 9
#define TX_HUMAN_SPIN_DIE_SHADOW 10
#define TX_HUMAN_SOUL_DIE 11
#define TX_HUMAN_SOUL_DIE_SHADOW 12
#define TX_HUMAN_DMG 75
#define TX_HUMAN_DMG_SHADOW 76
#define TX_HUMAN_GUARD 66
#define TX_HUMAN_GUARD_SHADOW 67
#define TX_HUMAN_GUARD_SUCCESS 68
#define TX_HUMAN_GUARD_SUCCESS_SHADOW 69

#define TX_ORC_IDLE 13
#define TX_ORC_IDLE_SHADOW 14
#define TX_ORC_WALK 15
#define TX_ORC_WALK_SHADOW 16
#define TX_ORC_ATTACK 17
#define TX_ORC_ATTACK_SHADOW 18
#define TX_ORC_CHARGED_ATTACK 19
#define TX_ORC_CHARGED_ATTACK_SHADOW 20
#define TX_ORC_JUMP 21
#define TX_ORC_JUMP_SHADOW 22
#define TX_ORC_DIE 23
#define TX_ORC_DIE_SHADOW 24
#define TX_ORC_DMG 25
#define TX_ORC_DMG_SHADOW 26
//213 4 4  0  img/characters/orc_guard.png
//214 4 4  0  img/characters/shadows/orc_guard_shadow.png
//215 4 3  0  img/characters/orc_guard_success.png
//216 4 3  0  img/characters/shadows/orc_guard_success_shadow.png
#define TX_ORC_GUARD 213
#define TX_ORC_GUARD_SHADOW 214
#define TX_ORC_GUARD_SUCCESS 215
#define TX_ORC_GUARD_SUCCESS_SHADOW 216

#define TX_DIRT_00 0
#define TX_STONE_00 27
#define TX_STONE_01 28
#define TX_STONE_02 29
#define TX_STONE_03 30
#define TX_STONE_04 31
#define TX_STONE_05 32
#define TX_STONE_06 33
#define TX_STONE_07 34
#define TX_STONE_08 35
#define TX_STONE_09 36
#define TX_STONE_10 37
#define TX_STONE_11 38
#define TX_STONE_12 39
//#define TX_STONE_13 40
#define TX_STONEWALL_00 41
#define TX_STONEWALL_01 42
#define TX_STONEWALL_02 43
#define TX_STONEWALL_03 44
//#define TX_STONEWALL_04 45
//#define TX_STONEWALL_05 46
//#define TX_STONEWALL_06 47
//#define TX_STONEWALL_07 48
//#define TX_STONEWALL_08 49
//#define TX_STONEWALL_09 50
//#define TX_STONEWALL_10 51
//#define TX_STONEWALL_11 52
//#define TX_STONEWALL_12 53
//#define TX_STONEWALL_13 54
//#define TX_STONEWALL_14 55

//#define TX_STONEWALL_15 217
//#define TX_STONEWALL_16 218
//#define TX_STONEWALL_17 219
//#define TX_STONEWALL_18 220
//#define TX_STONEWALL_19 221
//#define TX_STONEWALL_20 222
//#define TX_STONEWALL_21 223
//#define TX_STONEWALL_22 224
//#define TX_STONEWALL_23 225
//#define TX_STONEWALL_24 226
//#define TX_STONEWALL_25 227
//#define TX_STONEWALL_26 228
//#define TX_STONEWALL_27 229
//#define TX_STONEWALL_28 230
//#define TX_STONEWALL_29 231
//#define TX_STONEWALL_30 232
//#define TX_STONEWALL_31 233
//#define TX_STONEWALL_32 234
//#define TX_STONEWALL_33 235
//#define TX_STONEWALL_34 236
//#define TX_STONEWALL_35 237

#define TX_STONE_DIRT_UL_00 238
#define TX_STONE_DIRT_U_00 239
#define TX_STONE_DIRT_UR_00 240
#define TX_STONE_DIRT_L_00 241
#define TX_STONE_DIRT_C_00 242
#define TX_STONE_DIRT_R_00 243
#define TX_STONE_DIRT_DL_00 244
#define TX_STONE_DIRT_D_00 245
#define TX_STONE_DIRT_DR_00 246

#define TX_STONE_DIRT_UL_01 247
#define TX_STONE_DIRT_U_01 248
#define TX_STONE_DIRT_UR_01 249
#define TX_STONE_DIRT_L_01 250
#define TX_STONE_DIRT_C_01 251
#define TX_STONE_DIRT_R_01 252
#define TX_STONE_DIRT_DL_01 253
#define TX_STONE_DIRT_D_01 254
#define TX_STONE_DIRT_DR_01 255

#define TX_STONE_DIRT_UL_02 256
#define TX_STONE_DIRT_U_02 257
#define TX_STONE_DIRT_UR_02 258
#define TX_STONE_DIRT_L_02 259
#define TX_STONE_DIRT_C_02 260
#define TX_STONE_DIRT_R_02 261
#define TX_STONE_DIRT_DL_02 262
#define TX_STONE_DIRT_D_02 263
#define TX_STONE_DIRT_DR_02 264

#define TX_STONE_DIRT_UL_03 265
#define TX_STONE_DIRT_U_03 266
#define TX_STONE_DIRT_UR_03 267
#define TX_STONE_DIRT_DL_03 268
#define TX_STONE_DIRT_D_03 269
#define TX_STONE_DIRT_DR_03 270

#define TX_STONE_DIRT_UL_04 271
#define TX_STONE_DIRT_U_04 272
#define TX_STONE_DIRT_UR_04 273
#define TX_STONE_DIRT_DL_04 274
#define TX_STONE_DIRT_D_04 275
#define TX_STONE_DIRT_DR_04 276

#define TX_STONE_DIRT_UL_05 277
#define TX_STONE_DIRT_U_05 278
#define TX_STONE_DIRT_UR_05 279
#define TX_STONE_DIRT_DL_05 280
#define TX_STONE_DIRT_D_05 281
#define TX_STONE_DIRT_DR_05 282

#define TX_GRASS_00 56
#define TX_GRASS_01 57
#define TX_GRASS_02 58
#define TX_GRASS_03 59
#define TX_GRASS_04 60
#define TX_DIRT_01 61
#define TX_DIRT_02 62
#define TX_DIRT_03 63
#define TX_DIRT_04 64
#define TX_DIRT_05 65

//#define TX_GUARD_BUCKLER_FRONT 70
//#define TX_GUARD_BUCKLER_BACK 71
#define TX_GUARD_BUCKLER_SUCCESS_FRONT 72
#define TX_GUARD_BUCKLER_SUCCESS_BACK 73

#define TX_BUCKLER 74

#define TX_GRASS_05 77
#define TX_GRASS_06 78
#define TX_GRASS_07 79
#define TX_GRASS_08 80
#define TX_GRASS_09 81
#define TX_GRASS_10 82
#define TX_GRASS_11 83
#define TX_GRASS_12 84
#define TX_GRASS_13 85
#define TX_GRASS_14 86
#define TX_GRASS_15 87
#define TX_GRASS_16 88
#define TX_GRASS_17 89
#define TX_GRASS_18 90
#define TX_GRASS_19 91
#define TX_DIRT_GRASS_00 92
#define TX_DIRT_GRASS_01 93
#define TX_DIRT_GRASS_02 94
#define TX_DIRT_GRASS_03 95
#define TX_DIRT_GRASS_04 96
#define TX_DIRT_GRASS_05 97
#define TX_DIRT_GRASS_06 98
#define TX_DIRT_GRASS_07 99
#define TX_DIRT_GRASS_08 100
#define TX_DIRT_GRASS_09 101
#define TX_DIRT_GRASS_10 102
#define TX_DIRT_GRASS_11 103
#define TX_DIRT_GRASS_12 104
#define TX_DIRT_GRASS_13 105
#define TX_DIRT_GRASS_14 106
#define TX_WATER_00 107
#define TX_WATER_01 108
#define TX_DIRT_06 109
#define TX_DIRT_07 110
#define TX_DIRT_08 111

#define TX_PRESSURE_PLATE_UP_00 112
#define TX_PRESSURE_PLATE_DOWN_00 113
#define TX_STONE_TRAP_OFF_00 114
#define TX_STONE_TRAP_ON_00 115
#define TX_WALL_SWITCH_UP_00 116
#define TX_WALL_SWITCH_DOWN_00 117

#define TX_UPSTAIRS_00 118
#define TX_DOWNSTAIRS_00 119

#define TX_SWORD_00 120

#define TX_DWARF_IDLE 121
#define TX_DWARF_IDLE_SHADOW 122
#define TX_DWARF_WALK 123
#define TX_DWARF_WALK_SHADOW 124
#define TX_DWARF_ATTACK 125
#define TX_DWARF_ATTACK_SHADOW 126
#define TX_DWARF_JUMP 127
#define TX_DWARF_JUMP_SHADOW 128
#define TX_DWARF_SPIN_DIE 129
#define TX_DWARF_SPIN_DIE_SHADOW 130
//#define TX_DWARF_SOUL_DIE 131
//#define TX_DWARF_SOUL_DIE_SHADOW 132
#define TX_DWARF_DMG 133
#define TX_DWARF_DMG_SHADOW 134
//#define TX_DWARF_GUARD 66
//#define TX_DWARF_GUARD_SHADOW 67
//#define TX_DWARF_GUARD_SUCCESS 68
//#define TX_DWARF_GUARD_SUCCESS_SHADOW 69

#define TX_ELF_IDLE 135
#define TX_ELF_IDLE_SHADOW 136
#define TX_ELF_WALK 137
#define TX_ELF_WALK_SHADOW 138
#define TX_ELF_ATTACK 139
#define TX_ELF_ATTACK_SHADOW 140
#define TX_ELF_JUMP 141
#define TX_ELF_JUMP_SHADOW 142
#define TX_ELF_SPIN_DIE 143
#define TX_ELF_SPIN_DIE_SHADOW 144
//#define TX_ELF_SOUL_DIE 145
//#define TX_ELF_SOUL_DIE_SHADOW 146
#define TX_ELF_DMG 147
#define TX_ELF_DMG_SHADOW 148
//#define TX_ELF_GUARD 66
//#define TX_ELF_GUARD_SHADOW 67
//#define TX_ELF_GUARD_SUCCESS 68
//#define TX_ELF_GUARD_SUCCESS_SHADOW 69

#define TX_HALFLING_IDLE 149
#define TX_HALFLING_IDLE_SHADOW 150
#define TX_HALFLING_WALK 151
#define TX_HALFLING_WALK_SHADOW 152
#define TX_HALFLING_ATTACK 153
#define TX_HALFLING_ATTACK_SHADOW 154
#define TX_HALFLING_JUMP 155
#define TX_HALFLING_JUMP_SHADOW 156
#define TX_HALFLING_DMG 157
#define TX_HALFLING_DMG_SHADOW 158
#define TX_HALFLING_SPIN_DIE 159
#define TX_HALFLING_SPIN_DIE_SHADOW 160
//#define TX_HALFLING_SOUL_DIE 159
//#define TX_HALFLING_SOUL_DIE_SHADOW 160
//#define TX_HALFLING_GUARD 66
//#define TX_HALFLING_GUARD_SHADOW 67
//#define TX_HALFLING_GUARD_SUCCESS 68
//#define TX_HALFLING_GUARD_SUCCESS_SHADOW 69

#define TX_GOBLIN_IDLE 163
#define TX_GOBLIN_IDLE_SHADOW 164
#define TX_GOBLIN_WALK 165
#define TX_GOBLIN_WALK_SHADOW 166
#define TX_GOBLIN_ATTACK 167
#define TX_GOBLIN_ATTACK_SHADOW 168
#define TX_GOBLIN_JUMP 169
#define TX_GOBLIN_JUMP_SHADOW 170
#define TX_GOBLIN_DMG 171
#define TX_GOBLIN_DMG_SHADOW 172
#define TX_GOBLIN_SPIN_DIE 173
#define TX_GOBLIN_SPIN_DIE_SHADOW 174
//#define TX_GOBLIN_SOUL_DIE 173
//#define TX_GOBLIN_SOUL_DIE_SHADOW 174
//#define TX_GOBLIN_GUARD 66
//#define TX_GOBLIN_GUARD_SHADOW 67
//#define TX_GOBLIN_GUARD_SUCCESS 68
//#define TX_GOBLIN_GUARD_SUCCESS_SHADOW 69

#define TX_BAT_IDLE 177
#define TX_BAT_IDLE_SHADOW 178
#define TX_BAT_ATTACK 179
#define TX_BAT_ATTACK_SHADOW 180
#define TX_BAT_DMG 181
#define TX_BAT_DMG_SHADOW 182
#define TX_BAT_SLEEP 183
#define TX_BAT_SLEEP_SHADOW 184
#define TX_BAT_DIE 316
#define TX_BAT_DIE_SHADOW 317

#define TX_WOLF_IDLE 185
#define TX_WOLF_IDLE_SHADOW 186
#define TX_WOLF_WALK 187
#define TX_WOLF_WALK_SHADOW 188
#define TX_WOLF_ATTACK 189
#define TX_WOLF_ATTACK_SHADOW 190
#define TX_WOLF_DMG 191
#define TX_WOLF_DMG_SHADOW 192
#define TX_WOLF_JUMP 193
#define TX_WOLF_JUMP_SHADOW 194
#define TX_WOLF_DIE 195
#define TX_WOLF_DIE_SHADOW 196

#define TX_DAGGER 197
#define TX_TWO_HANDED_SWORD 198

#define TX_IRON_SHIELD 199
#define TX_WOODEN_TOWER_SHIELD 200

#define TX_POTION_HP_SMALL 201
#define TX_POTION_HP_MEDIUM 202
#define TX_POTION_HP_LARGE 203

#define TX_POTION_MP_SMALL 204
#define TX_POTION_MP_MEDIUM 205
#define TX_POTION_MP_LARGE 206

#define TX_ACTION_SLASH_AXE_B 207
#define TX_ACTION_SLASH_AXE_F 208

#define TX_ACTION_SLASH_DAGGER_B 209
#define TX_ACTION_SLASH_DAGGER_F 210

#define TX_ACTION_SLASH_SWORD_B 211
#define TX_ACTION_SLASH_SWORD_F 212

#define TX_WOODEN_DOOR_CLOSED 283
#define TX_WOODEN_DOOR_OPEN 284

#define TX_KEY_BASIC 285

//286 1 4  0  img/weapons/axe.png
#define TX_AXE 286

//287 4 4  0  img/characters/dwarf_guard_success.png
//288 4 4  0  img/characters/shadows/dwarf_guard_success_shadow.png
//289 4 4  0  img/characters/elf_guard_success.png
//290 4 4  0  img/characters/shadows/elf_guard_success_shadow.png
#define TX_DWARF_GUARD_SUCCESS 287
#define TX_DWARF_GUARD_SUCCESS_SHADOW 288
#define TX_ELF_GUARD_SUCCESS 289
#define TX_ELF_GUARD_SUCCESS_SHADOW 290
#define TX_HALFLING_GUARD_SUCCESS 291
#define TX_HALFLING_GUARD_SUCCESS_SHADOW 292
#define TX_GOBLIN_GUARD_SUCCESS 293
#define TX_GOBLIN_GUARD_SUCCESS_SHADOW 294

//295 4 3 0 img/actions/guard_buckler_success_front.png
//296 4 3 0 img/actions/guard_buckler_success_back.png
//297 4 3 0 img/actions/guard_iron_shield_success_front.png
//298 4 3 0 img/actions/guard_iron_shield_success_back.png
#define TX_GUARD_WOODEN_SHIELD_SUCCESS_FRONT 295
#define TX_GUARD_WOODEN_SHIELD_SUCCESS_BACK 296
#define TX_GUARD_IRON_SHIELD_SUCCESS_FRONT 297
#define TX_GUARD_IRON_SHIELD_SUCCESS_BACK 298

//299 1 4 0 img/weapons/bow.png
//#
//300 4 8 0 img/characters/human_shot.png
//301 4 8 0 img/characters/shadows/human_shot_shadow.png
//302 4 8 0 img/characters/orc_shot.png
//303 4 8 0 img/characters/shadows/orc_shot_shadow.png
//304 4 8 0 img/characters/dwarf_shot.png
//305 4 8 0 img/characters/shadows/dwarf_shot_shadow.png
//306 4 8 0 img/characters/elf_shot.png
//307 4 8 0 img/characters/shadows/elf_shot_shadow.png
//308 4 8 0 img/characters/halfling_shot.png
//309 4 8 0 img/characters/shadows/halfling_shot_shadow.png
//310 4 8 0 img/characters/goblin_shot.png
//311 4 8 0 img/characters/shadows/goblin_shot_shadow.png
#define TX_BOW 299

#define TX_HUMAN_SHOT 300
#define TX_HUMAN_SHOT_SHADOW 301
//#define TX_ORC_SHOT 302
//#define TX_ORC_SHOT_SHADOW 303
#define TX_DWARF_SHOT 304
#define TX_DWARF_SHOT_SHADOW 305
#define TX_ELF_SHOT 306
#define TX_ELF_SHOT_SHADOW 307
#define TX_HALFLING_SHOT 308
#define TX_HALFLING_SHOT_SHADOW 309
#define TX_GOBLIN_SHOT 310
#define TX_GOBLIN_SHOT_SHADOW 311

//312 4 8 0 img/actions/shot_bow_f.png
//313 4 8 0 img/actions/shot_bow_b.png
//314 4 8 0 img/actions/shot_bow_b_halfling_goblin_dwarf.png
#define TX_ACTION_SHOT_BOW_F 312
#define TX_ACTION_SHOT_BOW_B 313
#define TX_ACTION_SHOT_BOW_B_HALFLING_GOBLIN_DWARF 314

//315 1 4 0 img/items/arrow.png
#define TX_ARROW 315

//318 4 12 0  img/monsters/warg/warg_idle.png
//319 4 12 0  img/monsters/warg/shadows/warg_idle_shadow.png
//320 4  4 0  img/monsters/warg/warg_walk.png
//321 4  4 0  img/monsters/warg/shadows/warg_walk_shadow.png
//322 4  4 0  img/monsters/warg/warg_attack.png
//323 4  4 0  img/monsters/warg/shadows/warg_attack_shadow.png
//324 4  4 0  img/monsters/warg/warg_dmg.png
//325 4  4 0  img/monsters/warg/shadows/warg_dmg_shadow.png
//326 4  4 0  img/monsters/warg/warg_jump.png
//327 4  4 0  img/monsters/warg/shadows/warg_jump_shadow.png
//328 1  5 0  img/monsters/warg/warg_die.png
//329 1  5 0  img/monsters/warg/shadows/warg_die_shadow.png
#define TX_WARG_IDLE 318
#define TX_WARG_IDLE_SHADOW 319
#define TX_WARG_WALK 320
#define TX_WARG_WALK_SHADOW 321
#define TX_WARG_ATTACK 322
#define TX_WARG_ATTACK_SHADOW 323
#define TX_WARG_DMG 324
#define TX_WARG_DMG_SHADOW 325
#define TX_WARG_JUMP 326
#define TX_WARG_JUMP_SHADOW 327
#define TX_WARG_DIE 328
#define TX_WARG_DIE_SHADOW 329

//330 4  8 0  img/monsters/green_slime/SlimeGreenIdle.png
//331 4  8 0  img/monsters/green_slime/shadows/ShadowSlimeGreenIdle.png
//332 4  8 0  img/monsters/green_slime/SlimeGreenJumpAttack.png
//333 4  8 0  img/monsters/green_slime/shadows/ShadowSlimeGreenJumpAttack.png
//334 4  8 0  img/monsters/green_slime/SlimeGreenDmg.png
//335 4  8 0  img/monsters/green_slime/shadows/ShadowSlimeGreenDmg.png
//336 4  8 0  img/monsters/green_slime/SlimeGreenDie.png
//337 4  8 0  img/monsters/green_slime/shadows/ShadowSlimeGreenDie.png
#define TX_GREEN_SLIME_IDLE 330
#define TX_GREEN_SLIME_JUMP_ATTACK 332
#define TX_GREEN_SLIME_DMG 334
#define TX_GREEN_SLIME_DIE 336
#define TX_GREEN_SLIME_IDLE_SHADOW 331
#define TX_GREEN_SLIME_JUMP_ATTACK_SHADOW 333
#define TX_GREEN_SLIME_DMG_SHADOW 335
#define TX_GREEN_SLIME_DIE_SHADOW 337
//--------------------------------
//#define TX_WAND_BASIC 338
//--------------------------------
//
//
//
// spells (unimplemented)
//
//339 1 24 0  img/spells/full_fire.png
//340 1  8 0  img/spells/cast_fire.png
//341 1  8 0  img/spells/persist_fire.png
//342 1  8 0  img/spells/end_fire.png
//
//--------------------------------

#define TX_SPELL_FIRE_FULL 339
#define TX_SPELL_FIRE_CAST 340
#define TX_SPELL_FIRE_PERSIST 341
#define TX_SPELL_FIRE_END 342

//--------------------------------
//--------------------------------
//--------------------------------
// rings
//--------------------------------
//--------------------------------
//--------------------------------
#define TX_RING_GOLD 343
#define TX_RING_SILVER 344
#define TX_RING_IRON 345
//--------------------------------
#define TX_WOODEN_BOX 346
//--------------------------------
#define TX_WARHAMMER 347
#define TX_WHIP 348

//--------------------------------
#define COMPANYSCENE 0
#define TITLESCENE 1
#define GAMEPLAYSCENE 2

#define COMPANYNAME "@evildojo666"
#define COMPANYFILL "   x  x x   "

//#define DEFAULT_FONT_PATH "fonts/hack.ttf"

#define DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE 16

#define DEFAULT_DUNGEONFLOOR_WIDTH 8
#define DEFAULT_DUNGEONFLOOR_HEIGHT 8

#define SPRITEGROUP_DEFAULT_SIZE 32

#define DEFAULT_CAMERA_ZOOM 10.0f

#define DEFAULT_ANIM_SPEED 10


#define DEFAULT_SPRITEGROUPS_SIZE 128
