/*
 Copyright © 2015 Teemu Lindborg
*/

#include "Sounds.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Sounds.c
/// @brief Sound loading, playing and freeing
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
void loadSounds(Mix_Chunk *music[4], Mix_Chunk **shoot, Mix_Chunk **invaderkilled, Mix_Chunk **ufo_lp, Mix_Chunk **explosion)
{
  music[0] = Mix_LoadWAV("sounds/fastinvader1.wav");
  music[1] = Mix_LoadWAV("sounds/fastinvader2.wav");
  music[2] = Mix_LoadWAV("sounds/fastinvader3.wav");
  music[3] = Mix_LoadWAV("sounds/fastinvader4.wav");
  *shoot = Mix_LoadWAV("sounds/shoot.wav");
  *invaderkilled = Mix_LoadWAV("sounds/invaderkilled.wav");
  *ufo_lp = Mix_LoadWAV("sounds/ufo_lowpitch.wav");
  *explosion = Mix_LoadWAV("sounds/explosion.wav");

  if((music[0] == NULL) ||
     (music[1] == NULL) ||
     (music[2] == NULL) ||
     (music[3] == NULL) ||
     (shoot == NULL) ||
     (invaderkilled == NULL) ||
     (ufo_lp == NULL) ||
     (explosion == NULL)) {
      fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void playSound(Mix_Chunk *sound, int chanToPlay, int loops)
{

  // If channel to play the sound on is set to 0, we'll use -1 which checks the first available channel.
  // Should be avoided as it could affect the bg music or anything with a preset channel
    if(chanToPlay == 0)
    {
        chanToPlay = -1;
    }

    // Initializing the channel, the sound and play the as many times as defined in the loops variable passed to the function
    int channel;
    channel = Mix_PlayChannel(chanToPlay, sound, loops);
    if(channel == -1) {
        fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
    }

}

// -----------------------------------------------------------------------------------------------------------------------
void playMusic(int gameSpeed, Mix_Chunk *music[4])
{
    static int currentSound = 3;
    static int musicFrame = 25;
    int musicSpeed;

    // Check the current gamepeed and adjust the "music speed" or rather the interval between the bg bass noises accordingly
    switch(gameSpeed)
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

            playSound(music[currentSound], 1, 0);
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
void freeSounds(Mix_Chunk *music[4], Mix_Chunk **shoot, Mix_Chunk **invaderkilled, Mix_Chunk **ufo_lowpitch, Mix_Chunk **explosion)
{
  Mix_FreeChunk(music[0]);
  Mix_FreeChunk(music[1]);
  Mix_FreeChunk(music[2]);
  Mix_FreeChunk(music[3]);
  Mix_FreeChunk(*shoot);
  Mix_FreeChunk(*invaderkilled);
  Mix_FreeChunk(*ufo_lowpitch);
  Mix_FreeChunk(*explosion);
}
