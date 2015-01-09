/*
 Copyright © 2015 Teemu Lindborg
*/

#include "Shields.h"
#include "ExplosionPatterns.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Shields.c
/// @brief Functions to load/refresh the shields and pixel manipulation functions for destroyal
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
void loadShields(SDL_Renderer *_ren, SDL_Surface *o_shieldSurface[4], SDL_Texture *o_shieldTexture[4])
{
  for(int i = 0; i < 4; ++i)
  {
    o_shieldSurface[i] = IMG_Load("sprites/shieldTexture.png");
    if(!o_shieldSurface[i])
    {
     printf("IMG_Load: %s\n", IMG_GetError());
     return;
    }
    o_shieldTexture[i] = SDL_CreateTextureFromSurface(_ren, o_shieldSurface[i]);
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void initialiseShields(SDL_Rect io_shields[4])
{
  for(int i = 0; i < 4; ++i)
  {
    io_shields[i].w = 88;
    io_shields[i].h = 64;
    io_shields[i].y = HEIGHT - 30 - io_shields[i].h*2;
    io_shields[i].x = (WIDTH-(io_shields[i].w*4))/8 + i*(io_shields[i].w+(WIDTH-(io_shields[i].w*4))/4);
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void editPixel(SDL_Surface *io_shieldSurface, int _x, int _y, int _PlorIn)
{
  Uint8 *index;
  Uint32 *colour;
  srand(clock());
  int randomPattern = rand()%4;
  index = (Uint8 *)io_shieldSurface->pixels;

  switch(_PlorIn)
  {
    case 0:
    {
      for(int r = 0; r < 12; ++r)
      {
        if((_y-r) >= 0)
        {
          for(int c = 0; c < 6; ++c)
          {
            if((_x+(c-3)) >= 0 && (_x+(c-3)) < io_shieldSurface->w)
            {
              if(explosionPattern[randomPattern][r][c])
              {
                colour = (Uint32 *)&index[(io_shieldSurface->pitch*(_y-r) + io_shieldSurface->format->BytesPerPixel*(_x+(c-3)))];
                *colour = 0x00000000;
              }
            }
            else
              continue;
          }
        }
        else
        {
          break;
        }
      }
      break;
    }
    case 1:
    {
      for(int r = 0; r < 12; ++r)
      {
        if((_y+r) < io_shieldSurface->h)
        {
          for(int c = 0; c < 6; ++c)
          {
            if((_x+(c-3)) >= 0 && (_x+(c-3)) < io_shieldSurface->w)
            {
              if(explosionPattern[randomPattern][r][c])
              {
                colour = (Uint32 *)&index[(io_shieldSurface->pitch*(_y+r) + io_shieldSurface->format->BytesPerPixel*(_x+(c-3)))];
                *colour = 0x00000000;
              }
            }
            else
              continue;
          }
        }
        else
        {
          break;
        }
      }
    break;
    }
  }

}

// -----------------------------------------------------------------------------------------------------------------------
Uint32 pixelActive(SDL_Surface *_shieldSurface, int _x, int _y)
{
  Uint8 *index;
  Uint32 *value;
  index = (Uint8 *)_shieldSurface->pixels;
  value = (Uint32 *)&index[(_shieldSurface->pitch*_y + _shieldSurface->format->BytesPerPixel*_x)];
  return *value;
}
