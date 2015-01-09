#ifndef INVADER_H__
#define INVADER_H__

#include "Globals.h"

enum InvaderType{TYPE1,TYPE2,TYPE3};

typedef struct
{
  SDL_Rect pos;
  int active;
  int frame;
  enum InvaderType type;
}Invader;

void initialiseInvaders(Invader invaders[ROWS][COLS], Invader *alien, SDL_Rect invaderProjectile[COLS], int actInvaderInRow[COLS], int invaderProjectileActive[COLS]);
void updateInvaders(Invader invaders[ROWS][COLS], int *gameSpeed, int *currentFrame, int actInvaderInRow[COLS], int level);
void drawInvaders(SDL_Renderer *ren,SDL_Texture *tex,Invader invaders[ROWS][COLS], int currentFrame);
void invaderShootPewPew(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *invaderProjectile);
void moveAlien(SDL_Renderer *ren, SDL_Texture *tex, Invader *alien, int direction, Mix_Chunk *ufosound);

#endif
