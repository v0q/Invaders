#ifndef DEFENDER_H
#define DEFENDER_H

#include "Definitions.h"

typedef enum {LEFT,RIGHT} direction;

void initializeDefender(SDL_Rect spaceShip[2], SDL_Rect projectile[2], SDL_Rect projectileBoom[2]);
void moveSpaceShip(SDL_Rect *spaceShip, int moveDir);
void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *sStexture, SDL_Rect *spaceShip, int *playerDead, char *lives, int player);
void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, int level);

#endif
