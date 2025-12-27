#pragma once

typedef enum
{
    GAMESTATE_FLAG_NONE = 0,
    GAMESTATE_FLAG_PLAYER_INPUT = 1,
    GAMESTATE_FLAG_PLAYER_ANIM = 2,
    GAMESTATE_FLAG_NPC_TURN = 3,
    GAMESTATE_FLAG_NPC_ANIM = 4,
    GAMESTATE_FLAG_NPC_BATCH = 5,
    GAMESTATE_FLAG_COUNT = 6
} gamestate_flag_t;


//#define GAMESTATE_FLAG_TOSTR(n)                                                                                                                                \
//    (n == 0   ? "GAMESTATE_FLAG_NONE"                                                                                                                          \
//     : n == 1 ? "GAMESTATE_FLAG_PLAYER_INPUT"                                                                                                                  \
//     : n == 2 ? "GAMESTATE_FLAG_PLAYER_ANIM"                                                                                                                   \
//     : n == 3 ? "GAMESTATE_FLAG_NPC_TURN"                                                                                                                      \
//     : n == 4 ? "GAMESTATE_FLAG_NPC_ANIM"                                                                                                                      \
//     : n == 5 ? "GAMESTATE_FLAG_NPC_BATCH"                                                                                                                     \
//     : n == 6 ? "GAMESTATE_FLAG_COUNT"                                                                                                                         \
//              : "ERROR_UNKNOWN")
