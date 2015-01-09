#include "Shields.h"
#include "ExplosionPatterns.h"

void loadShields(SDL_Renderer *ren, SDL_Surface *shieldSurface[4], SDL_Texture *shieldTexture[4])
{
  for(int i = 0; i < 4; ++i)
  {
    shieldSurface[i] = IMG_Load("sprites/shieldTexture.png");
    if(!shieldSurface[i])
    {
     printf("IMG_Load: %s\n", IMG_GetError());
     return;
    }
    shieldTexture[i] = SDL_CreateTextureFromSurface(ren, shieldSurface[i]);
  }
}


void editPixel(SDL_Surface *shieldSurface, int x, int y, int PlorIn)
{
  Uint8 *index;
  Uint32 *colour;
  srand(clock());
  int randomPattern = rand()%4;
  index = (Uint8 *)shieldSurface->pixels;

  switch(PlorIn)
  {
    case 0:
    {
      for(int r = 0; r < 12; ++r)
      {
        if((y-r) >= 0)
        {
          for(int c = 0; c < 6; ++c)
          {
            if((x+(c-3)) >= 0 && (x+(c-3)) < shieldSurface->w)
            {
              if(explosionPattern[randomPattern][r][c])
              {
                colour = (Uint32 *)&index[(shieldSurface->pitch*(y-r) + shieldSurface->format->BytesPerPixel*(x+(c-3)))];
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
        if((y+r) < shieldSurface->h)
        {
          for(int c = 0; c < 6; ++c)
          {
            if((x+(c-3)) >= 0 && (x+(c-3)) < shieldSurface->w)
            {
              if(explosionPattern[randomPattern][r][c])
              {
                colour = (Uint32 *)&index[(shieldSurface->pitch*(y+r) + shieldSurface->format->BytesPerPixel*(x+(c-3)))];
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

Uint32 pixelActive(SDL_Surface *shieldSurface, int x, int y)
{
  Uint8 *index;
  Uint32 *value;
  index = (Uint8 *)shieldSurface->pixels;
  value = (Uint32 *)&index[(shieldSurface->pitch*y + shieldSurface->format->BytesPerPixel*x)];
  return *value;
}
