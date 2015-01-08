#ifndef SOUNDS_H
#define SOUNDS_H

#include "Definitions.h"

void playSound(Mix_Chunk *sound, int chanToPlay, int loops);
void playMusic(int gameSpeed, Mix_Chunk *music[4]);

#endif
