/*
 Copyright © 2015 Teemu Lindborg
*/

#ifndef SOUNDS_H
#define SOUNDS_H

/// @file Sounds.h
/// @brief Sound loading, playing and memory freeing
// Including the global definitions and includes for the use of the functions
#include "Globals.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Sounds.h
/// @brief Sound related functions that handle the loading, playing of sounds and the memory freeing
///        in the end
/// @author Teemu Lindborg
/// @version 1.0
/// @date 09/01/2015
/// @todo On some machines the sound doesn't seem to keep up (delay) and need to figure out why and fix that
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Loads all the needed sound files to the memory
/// @param[in] music To which our sounds that will be used to create the background music
/// @param[in] shoot To which shooting sound will be loaded
/// @param[in] invaderkilled To which invader death sound will be loaded
/// @param[in] ufo_lp To which the ufo sound will be loaded
/// @param[in] explosion To which the explosion sound will be loaded
// -----------------------------------------------------------------------------------------------------------------------
void loadSounds(Mix_Chunk *music[4], Mix_Chunk **shoot, Mix_Chunk **invaderkilled, Mix_Chunk **ufo_lp, Mix_Chunk **explosion);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Function that will play the sound chunk that's passed on to it in the desired channel for x loops
/// @param[in] sound Chunk to be played
/// @param[in] chanToPlay In which channel the chunk will be played (different channels are used for different sounds)
/// @param[in] loops How many times the sound should be played
// -----------------------------------------------------------------------------------------------------------------------
void playSound(Mix_Chunk *sound, int chanToPlay, int loops);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief The function that will play the background music/sounds depending on the current gamespeed
/// @param[in] gameSpeed To tell the function how fast/at what intervals should the sounds be played in
/// @param[in] music The actual sound chunks to be played
// -----------------------------------------------------------------------------------------------------------------------
void playMusic(int gameSpeed, Mix_Chunk *music[4]);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Frees the chunks from the memory
/// @param[in] music The background music chunks to be freed
/// @param[in] shoot Shooting sound to be freed
/// @param[in] invaderkilled Invader death sound to be freed
/// @param[in] ufo_lowpitch Ufo sound to be freed
/// @param[in] explosion Explosion sound to be freed
// -----------------------------------------------------------------------------------------------------------------------
void freeSounds(Mix_Chunk *music[4], Mix_Chunk **shoot, Mix_Chunk **invaderkilled, Mix_Chunk **ufo_lowpitch, Mix_Chunk **explosion);

#endif // end SOUNDS_H
