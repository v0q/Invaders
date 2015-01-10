/*
 Copyright © 2015 Teemu Lindborg
 SDAGE 1st year Invaders Assignment
 Initial code © by Jon Macey
*/

#include "Globals.h"
#include "Invader.h"
#include "Sounds.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Invader.c
/// @brief Functionality for the invaders and the alien
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
void initialiseInvaders(Invader o_invaders[ROWS][COLS], Invader *o_alien, SDL_Rect o_invaderProjectile[COLS], int o_actInvaderInRow[COLS], int o_invaderProjectileActive[COLS])
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
      o_invaders[r][c].pos=pos;
      o_invaders[r][c].active=1;
      o_invaders[r][c].frame=0;
      if(r==0)
        o_invaders[r][c].type=TYPE3;
      else if(r==1 || r==2)
        o_invaders[r][c].type=TYPE2;
      else
        o_invaders[r][c].type=TYPE1;

    }
    ypos+=(GAP+SPRITEHEIGHT);
  }

  for(int i = 0; i < COLS; ++i)
  {
    o_invaderProjectile[i].x = 0;
    o_invaderProjectile[i].y = 0;
    o_invaderProjectile[i].w = 6;
    o_invaderProjectile[i].h = 18;
    o_invaderProjectileActive[i] = 0;
    o_actInvaderInRow[i] = 5;
  }

  o_alien->active = 0;
  o_alien->frame = 0;
  o_alien->type = 0;
  o_alien->pos.x=-SPRITEWIDTH;
  o_alien->pos.y=INFOBOXHEIGHT+1;
  o_alien->pos.w=SPRITEWIDTH+(SPRITEWIDTH/2);
  o_alien->pos.h=SPRITEHEIGHT;
}

// -----------------------------------------------------------------------------------------------------------------------
void updateInvaders(Invader io_invaders[ROWS][COLS], int *_gameSpeed, int *o_currentFrame, int o_actInvaderInRow[], int _level)
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

  switch(_level)
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
        if(io_invaders[r][c].active)
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
        if(io_invaders[r][c].active)
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

  if(io_invaders[0][actColR].pos.x>=WIDTH-(SPRITEWIDTH*2-SPRITEWIDTH/2))
  {
    DIRECTION=BWD;
    yinc=increaseY;
    if(updateSpeed%whenToUpdate == 0 && *_gameSpeed < 10)
    {
      ++*_gameSpeed;
      animationFrame = 0;
      updateSpeed = 0;
    }
    ++updateSpeed;
  }

  else if(io_invaders[0][actColL].pos.x<=SPRITEWIDTH/2)
  {
    DIRECTION=FWD;
    yinc=increaseY;
    if(updateSpeed%whenToUpdate == 0 && *_gameSpeed < 10)
    {
      ++*_gameSpeed;
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
        io_invaders[r][c].pos.x+=1+*_gameSpeed;
      }
      else
      {
        io_invaders[r][c].pos.x-=1+*_gameSpeed;
      }

      // Changing the sprite animation
      ++animationFrame;
      if(animationFrame == ANIMATIONSEQUENCELENGTH/(*_gameSpeed))
      {
        if(*o_currentFrame == 0)
        {
          *o_currentFrame = 1;
        }
        else
        {
          *o_currentFrame = 0;
        }
        animationFrame = 0;
      }


      io_invaders[r][c].pos.y += yinc;

    }
  }
  for(int c = 0; c < COLS; ++c)
  {
    o_actInvaderInRow[c] = 0;
    for(int r = 0; r < ROWS; ++r)
    {
      if(io_invaders[r][c].active)
        o_actInvaderInRow[c] = r+1;
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void drawInvaders(SDL_Renderer *_ren, SDL_Texture *_tex, Invader _invaders[ROWS][COLS], int _currentFrame)
{
  SDL_Rect SrcR;
  static int destroySequence = 0;

  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      if(_invaders[r][c].frame == 3)
      {
        SrcR.x=340;
        SrcR.y=616;
        SrcR.w=95;
        SrcR.h=59;
        destroySequence += 1;
      }
      else
      {

        switch(_invaders[r][c].type)
        {
          case TYPE3 :
          {
            SDL_SetRenderDrawColor(_ren, 255, 0, 0, 255);
            SrcR.x=292+(117*_currentFrame);
            SrcR.y=0;
            SrcR.w=80;
            SrcR.h=80;
            break;
          }
          case TYPE2 :
          {
            SDL_SetRenderDrawColor(_ren, 0, 255, 0, 255);
            SrcR.x=145*_currentFrame;
            SrcR.y=0;
            SrcR.w=111;
            SrcR.h=80;
            break;
          }
          case TYPE1 :
          {
            SDL_SetRenderDrawColor(_ren, 0, 0, 255, 255);
            SrcR.x=140*_currentFrame;
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
        _invaders[r][c].active = 0;
        _invaders[r][c].frame = 0;
      }

      if(_invaders[r][c].active)
      {
        SDL_RenderFillRect(_ren, &_invaders[r][c].pos);
        SDL_RenderCopy(_ren, _tex, &SrcR, &_invaders[r][c].pos);
      }

    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void invaderShootPewPew(SDL_Renderer *_ren, SDL_Texture *_tex, SDL_Rect *io_invaderProjectile)
{
  SDL_Rect projectileSprite;
  projectileSprite.x = 468;
  projectileSprite.y = 379;
  projectileSprite.w = 27;
  projectileSprite.h = 50;

  io_invaderProjectile->y += 6;

  SDL_RenderCopy(_ren, _tex, &projectileSprite, io_invaderProjectile);
}

// -----------------------------------------------------------------------------------------------------------------------
void moveAlien(SDL_Renderer *_ren, SDL_Texture *_tex, Invader *io_alien, int _direction, Mix_Chunk *_ufosound)
{
  // Set the sprite coordinates for the alien
  SDL_Rect alien_sprite;
  alien_sprite.x = 0;
  alien_sprite.y = 616;
  alien_sprite.w = 125;
  alien_sprite.h = 61;

  static int destroySequence = 0;

  // Checks if the alien has been destroyed and if the destroy sequence should be run
  if(io_alien->frame == 1)
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
    if(_direction == 0)
        io_alien->pos.x += 5;
    else
        io_alien->pos.x -= 5;
    // While alien is active play the ufo sound
    playSound(_ufosound, 4, -1);

    // Checks if the ufo passed the screen and make it not active and stop the sound
    if(io_alien->pos.x < -io_alien->pos.w || io_alien->pos.x > WIDTH+io_alien->pos.w)
    {
        io_alien->active = 0;
        Mix_Pause(4);
    }
  }

  if(destroySequence == 10)
  {
    destroySequence = 0;
    io_alien->active = 0;
    io_alien->frame = 0;
  }

  // Render the ufo
  SDL_RenderCopy(_ren, _tex, &alien_sprite, &io_alien->pos);
}
