/*
 Copyright © 2015 Teemu Lindborg
*/

#include "Sounds.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Sounds.c
/// @brief Sound loading, playing and freeing
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
void loadSounds(Mix_Chunk *o_music[4], Mix_Chunk **o_shoot, Mix_Chunk **o_invaderkilled, Mix_Chunk **o_ufo_lp, Mix_Chunk **o_explosion)
{
  o_music[0] = Mix_LoadWAV("sounds/fastinvader1.wav");
  o_music[1] = Mix_LoadWAV("sounds/fastinvader2.wav");
  o_music[2] = Mix_LoadWAV("sounds/fastinvader3.wav");
  o_music[3] = Mix_LoadWAV("sounds/fastinvader4.wav");
  *o_shoot = Mix_LoadWAV("sounds/shoot.wav");
  *o_invaderkilled = Mix_LoadWAV("sounds/invaderkilled.wav");
  *o_ufo_lp = Mix_LoadWAV("sounds/ufo_lowpitch.wav");
  *o_explosion = Mix_LoadWAV("sounds/explosion.wav");

  if((o_music[0] == NULL) ||
     (o_music[1] == NULL) ||
     (o_music[2] == NULL) ||
     (o_music[3] == NULL) ||
     (o_shoot == NULL) ||
     (o_invaderkilled == NULL) ||
     (o_ufo_lp == NULL) ||
     (o_explosion == NULL)) {
      fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void playSound(Mix_Chunk *_sound, int _chanToPlay, int _loops)
{

  // If channel to play the sound on is set to 0, we'll use -1 which checks the first available channel.
  // Should be avoided as it could affect the bg music or anything with a preset channel
    if(_chanToPlay == 0)
    {
        _chanToPlay = -1;
    }

    // Initializing the channel, the sound and play the as many times as defined in the loops variable passed to the function
    int channel;
    channel = Mix_PlayChannel(_chanToPlay, _sound, _loops);
    if(channel == -1) {
        fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
    }

}

// -----------------------------------------------------------------------------------------------------------------------
void playMusic(int _gameSpeed, Mix_Chunk *_music[4])
{
    static int currentSound = 3;
    static int musicFrame = 25;
    int musicSpeed;

    // Check the current gamepeed and adjust the "music speed" or rather the interval between the bg bass noises accordingly
    switch(_gameSpeed)
    {
        case 1: musicSpeed = 50; break;
        case 2: musicSpeed = 45; break;
        case 3: musicSpeed = 40; break;
        case 4: musicSpeed = 35; break;
        case 5: musicSpeed = 30; break;
        case 6: musicSpeed = 25; break;
        case 7: musicSpeed = 20; break;
        case 8: musicSpeed = 15; break;
        case 9: musicSpeed = 10; break;
        case 10: musicSpeed = 5; break;
        default: musicSpeed = 5; break;
    }

    // Check if the channel 1 is active, i.e. the channel that's reserved for the bg music, if not continue to the "interval" calculations
    if(Mix_Playing(1) == 0)
    {
        if(musicFrame < musicSpeed)
        {
         ++musicFrame;
        }
        else
        {
            musicFrame = 1;
        }

        // Plays the next bg sound on every _musicSpeed_ frame
        if(musicFrame%musicSpeed == 0)
        {

            playSound(_music[currentSound], 1, 0);
            if(currentSound < 3)
            {
                ++currentSound;
            }
            else
            {
                currentSound = 0;
            }

        }
    }

}

// -----------------------------------------------------------------------------------------------------------------------
void freeSounds(Mix_Chunk *_music[4], Mix_Chunk **_shoot, Mix_Chunk **_invaderkilled, Mix_Chunk **_ufo_lowpitch, Mix_Chunk **_explosion)
{
  Mix_FreeChunk(_music[0]);
  Mix_FreeChunk(_music[1]);
  Mix_FreeChunk(_music[2]);
  Mix_FreeChunk(_music[3]);
  Mix_FreeChunk(*_shoot);
  Mix_FreeChunk(*_invaderkilled);
  Mix_FreeChunk(*_ufo_lowpitch);
  Mix_FreeChunk(*_explosion);
}
