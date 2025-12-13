#pragma once

#define play_sound(n) PlaySound(g->sfx->at(n))
#define play_sound_if_heard(n,h) if(h)PlaySound(g->sfx->at(n))
