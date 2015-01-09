#ifndef SOUNDS_H
#define SOUNDS_H

#include "Globals.h"

void loadSounds(Mix_Chunk *music[4], Mix_Chunk **shoot, Mix_Chunk **invaderkilled, Mix_Chunk **ufo_lp, Mix_Chunk **explosion);
void playSound(Mix_Chunk *sound, int chanToPlay, int loops);
void playMusic(int gameSpeed, Mix_Chunk *music[4]);
void freeSounds(Mix_Chunk *music[4], Mix_Chunk **shoot, Mix_Chunk **invaderkilled, Mix_Chunk **ufo_lowpitch);

#endif
