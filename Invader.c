/*
 Copyright © 2015 Teemu Lindborg
*/

#include "Globals.h"
#include "Invader.h"
#include "Sounds.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Invader.c
/// @brief Functionality for the invaders and the alien
// -----------------------------------------------------------------------------------------------------------------------

void initialiseInvaders(Invader invaders[ROWS][COLS], Invader *alien, SDL_Rect invaderProjectile[COLS], int actInvaderInRow[COLS], int invaderProjectileActive[COLS])
{
  SDL_Rect pos;
  pos.w=SPRITEWIDTH;
  pos.h=SPRITEHEIGHT;
  int ypos=GAP;

  for(int r=0; r<ROWS; ++r)
  {
    int xpos=GAP;
    for(int c=0; c<COLS; ++c)
    {
      pos.x=xpos+SPRITEWIDTH;
      pos.y=ypos+SPRITEHEIGHT+INFOBOXHEIGHT;
      xpos+=(GAP+SPRITEWIDTH);
      invaders[r][c].pos=pos;
      invaders[r][c].active=1;
      invaders[r][c].frame=0;
      if(r==0)
        invaders[r][c].type=TYPE3;
      else if(r==1 || r==2)
        invaders[r][c].type=TYPE2;
      else
        invaders[r][c].type=TYPE1;

    }
    ypos+=(GAP+SPRITEHEIGHT);
  }

  for(int i = 0; i < COLS; ++i)
  {
    invaderProjectile[i].x = 0;
    invaderProjectile[i].y = 0;
    invaderProjectile[i].w = 6;
    invaderProjectile[i].h = 18;
    invaderProjectileActive[i] = 0;
    actInvaderInRow[i] = 5;
  }

  alien->active = 0;
  alien->frame = 0;
  alien->type = 0;
  alien->pos.x=-SPRITEWIDTH;
  alien->pos.y=INFOBOXHEIGHT+1;
  alien->pos.w=SPRITEWIDTH+(SPRITEWIDTH/2);
  alien->pos.h=SPRITEHEIGHT;
}

void updateInvaders(Invader invaders[ROWS][COLS], int *gameSpeed, int *currentFrame, int actInvaderInRow[COLS], int level)
{
  enum DIR{FWD,BWD};
  static int DIRECTION=FWD;
  int yinc=0;
  int increaseY = GAP/2;
  static int actColR = COLS-1;
  static int actColL = 0;
  int stopLoop = 0;
  static int updateSpeed = 1;
  static int animationFrame = 0;
  int whenToUpdate;

  switch(level)
  {
    case 1: whenToUpdate = 8; break;
    case 2: whenToUpdate = 6; break;
    case 3: whenToUpdate = 4; break;
    case 4: whenToUpdate = 2; break;
    case 5: whenToUpdate = 1; break;
    case 6: whenToUpdate = 1; increaseY = GAP; break;
    case 7: whenToUpdate = 1; increaseY = GAP+(GAP/2); break;
    case 8: whenToUpdate = 1; increaseY = GAP*2; break;
    default: whenToUpdate = 1; increaseY = GAP*4; break;
  }

  // Loop through the column from right to find the active outermost column on right
  for(int c = COLS-1; c >= 0; --c)
    {
      // Loop through all the rows in a column
      for(int r = 0; r < ROWS; ++r)
      {
        // If an active invader is found in the current column, stop the loop (no need to run further as this is the outermost active column
        // and set the current column to be the active one
        if(invaders[r][c].active)
        {
          actColR = c;
          stopLoop = 1;
          break;
        }
      }
      if(stopLoop)
      {
        break;
      }
  }

  stopLoop = 0;

  // Loop through the column from left to find the active outermost column on left
  for(int c = 0; c < COLS; ++c)
    {
      // Loop through all the rows in a column
      for(int r = 0; r < ROWS; ++r)
      {
        // If an active invader is found in the current column, stop the loop (no need to run further as this is the outermost active column
        // and set the current column to be the active one
        if(invaders[r][c].active)
        {
          actColL = c;
          stopLoop = 1;
          break;
        }
      }
      if(stopLoop)
      {
        break;
      }
  }

  if(invaders[0][actColR].pos.x>=WIDTH-(SPRITEWIDTH*2-SPRITEWIDTH/2))
  {
    DIRECTION=BWD;
    yinc=increaseY;
    if(updateSpeed%whenToUpdate == 0 && *gameSpeed < 10)
    {
      ++*gameSpeed;
      animationFrame = 0;
      updateSpeed = 0;
    }
    ++updateSpeed;
  }

  else if(invaders[0][actColL].pos.x<=SPRITEWIDTH/2)
  {
    DIRECTION=FWD;
    yinc=increaseY;
    if(updateSpeed%whenToUpdate == 0 && *gameSpeed < 10)
    {
      ++*gameSpeed;
      animationFrame = 0;
      updateSpeed = 0;
    }
    ++updateSpeed;
  }

  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      if(DIRECTION==FWD)
      {
        invaders[r][c].pos.x+=1+*gameSpeed;
      }
      else
      {
        invaders[r][c].pos.x-=1+*gameSpeed;
      }

      // Changing the sprite animation
      ++animationFrame;
      if(animationFrame == ANIMATIONSEQUENCELENGTH/(*gameSpeed))
      {
        if(*currentFrame == 0)
        {
          *currentFrame = 1;
        }
        else
        {
          *currentFrame = 0;
        }
        animationFrame = 0;
      }


      invaders[r][c].pos.y += yinc;

    }
  }
  for(int c = 0; c < COLS; ++c)
  {
    actInvaderInRow[c] = 0;
    for(int r = 0; r < ROWS; ++r)
    {
      if(invaders[r][c].active)
        actInvaderInRow[c] = r+1;
    }
  }
}

void drawInvaders(SDL_Renderer *ren, SDL_Texture *tex, Invader invaders[ROWS][COLS], int currentFrame)
{
  SDL_Rect SrcR;
  static int destroySequence = 0;

  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      if(invaders[r][c].frame == 3)
      {
        SrcR.x=340;
        SrcR.y=616;
        SrcR.w=95;
        SrcR.h=59;
        destroySequence += 1;
      }
      else
      {

        switch(invaders[r][c].type)
        {
          case TYPE3 :
          {
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SrcR.x=292+(117*currentFrame);
            SrcR.y=0;
            SrcR.w=80;
            SrcR.h=80;
            break;
          }
          case TYPE2 :
          {
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            SrcR.x=145*currentFrame;
            SrcR.y=0;
            SrcR.w=111;
            SrcR.h=80;
            break;
          }
          case TYPE1 :
          {
            SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
            SrcR.x=140*currentFrame;
            SrcR.y=120;
            SrcR.w=120;
            SrcR.h=80;
            break;
          }
        }
      }

      if(destroySequence == 10)
      {
        destroySequence = 0;
        invaders[r][c].active = 0;
        invaders[r][c].frame = 0;
      }

      if(invaders[r][c].active)
      {
        SDL_RenderFillRect(ren,&invaders[r][c].pos);
        SDL_RenderCopy(ren, tex,&SrcR,&invaders[r][c].pos);
      }

    }
  }
}

void invaderShootPewPew(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *invaderProjectile)
{
  SDL_Rect projectileSprite;
  projectileSprite.x = 468;
  projectileSprite.y = 379;
  projectileSprite.w = 27;
  projectileSprite.h = 50;

  invaderProjectile->y += 6;

  SDL_RenderCopy(ren, tex, &projectileSprite, invaderProjectile);
}

void moveAlien(SDL_Renderer *ren, SDL_Texture *tex, Invader *alien, int direction, Mix_Chunk *ufosound)
{
  // Set the sprite coordinates for the alien
  SDL_Rect alien_sprite;
  alien_sprite.x = 0;
  alien_sprite.y = 616;
  alien_sprite.w = 125;
  alien_sprite.h = 61;

  static int destroySequence = 0;

  // Checks if the alien has been destroyed and if the destroy sequence should be run
  if(alien->frame == 1)
  {
    alien_sprite.x=340;
    alien_sprite.y=616;
    alien_sprite.w=95;
    alien_sprite.h=59;
    destroySequence += 1;
  }
  else
  {
    // Checks where the alien should be moving, i.e. if it spawned to the left or the right side of the screen
    if(direction == 0)
        alien->pos.x += 5;
    else
        alien->pos.x -= 5;
    // While alien is active play the ufo sound
    playSound(ufosound, 4, -1);

    // Checks if the ufo passed the screen and make it not active and stop the sound
    if(alien->pos.x < -alien->pos.w || alien->pos.x > WIDTH+alien->pos.w)
    {
        alien->active = 0;
        Mix_Pause(4);
    }
  }

  if(destroySequence == 10)
  {
    destroySequence = 0;
    alien->active = 0;
    alien->frame = 0;
  }

  // Render the ufo
  SDL_RenderCopy(ren, tex, &alien_sprite, &alien->pos);
}
