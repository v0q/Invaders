#ifndef SHIELDS_H
#define SHIELDS_H

#include "Globals.h"

void loadShields(SDL_Renderer *ren, SDL_Surface *shieldSurface[4], SDL_Texture *shieldTexture[4]);
void editPixel(SDL_Surface *shieldSurface, int x, int y, int PlorIn);
Uint32 pixelActive(SDL_Surface *shieldSurface, int x, int y);

#endif
