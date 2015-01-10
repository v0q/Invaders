/*
 Copyright © 2015 Teemu Lindborg
 SDAGE 1st year Invaders Assignment
 Initial code © by Jon Macey
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
/// @param[out] music To which our sounds that will be used to create the background music
/// @param[out] shoot To which shooting sound will be loaded
/// @param[out] invaderkilled To which invader death sound will be loaded
/// @param[out] ufo_lp To which the ufo sound will be loaded
/// @param[out] explosion To which the explosion sound will be loaded
// -----------------------------------------------------------------------------------------------------------------------
void loadSounds(Mix_Chunk *o_music[4], Mix_Chunk **o_shoot, Mix_Chunk **o_invaderkilled, Mix_Chunk **o_ufo_lp, Mix_Chunk **o_explosion);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Function that will play the sound chunk that's passed on to it in the desired channel for x loops
/// @param[in] sound Chunk to be played
/// @param[in] chanToPlay In which channel the chunk will be played (different channels are used for different sounds)
/// @param[in] loops How many times the sound should be played
// -----------------------------------------------------------------------------------------------------------------------
void playSound(Mix_Chunk *_sound, int _chanToPlay, int _loops);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief The function that will play the background music/sounds depending on the current gamespeed
/// @param[in] gameSpeed To tell the function how fast/at what intervals should the sounds be played in
/// @param[in] music The actual sound chunks to be played
// -----------------------------------------------------------------------------------------------------------------------
void playMusic(int _gameSpeed, Mix_Chunk *_music[4]);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Frees the chunks from the memory
/// @param[in] music The background music chunks to be freed
/// @param[in] shoot Shooting sound to be freed
/// @param[in] invaderkilled Invader death sound to be freed
/// @param[in] ufo_lowpitch Ufo sound to be freed
/// @param[in] explosion Explosion sound to be freed
// -----------------------------------------------------------------------------------------------------------------------
void freeSounds(Mix_Chunk *_music[4], Mix_Chunk **_shoot, Mix_Chunk **_invaderkilled, Mix_Chunk **_ufo_lowpitch, Mix_Chunk **_explosion);

#endif // end SOUNDS_H
