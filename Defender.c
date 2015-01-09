/*
 Copyright © 2015 Teemu Lindborg
*/

#include "Defender.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Defender.c
/// @brief Defender's functionality
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
void initialiseDefender(SDL_Rect spaceShip[2], SDL_Rect projectile[2], SDL_Rect projectileBoom[2])
{
  for(int i = 0; i < 2; ++i)
  {
    spaceShip[i].y = HEIGHT-50;
    spaceShip[i].w = SPRITEWIDTH;
    spaceShip[i].h = 20;
    projectile[i].x = 0;
    projectile[i].y = 0;
    projectile[i].w = 2;
    projectile[i].h = 8;

    projectileBoom[i].y = INFOBOXHEIGHT+5;
    projectileBoom[i].w = SPRITEWIDTH;
    projectileBoom[i].h = 20;
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void moveSpaceShip(SDL_Rect *spaceShip, int moveDir)
{
  switch(moveDir)
  {
    case LEFT:
    {
      if(spaceShip->x > 0)
      {
        spaceShip->x -= 5;
      }
      else
      {
        spaceShip->x = 0;
      }
      break;
    }
    case RIGHT:
    {
      if(spaceShip->x < WIDTH-SPRITEWIDTH)
      {
        spaceShip->x += 5;
      }
      else
      {
        spaceShip->x = WIDTH-SPRITEWIDTH;
      }
      break;
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *spaceShip, int *playerDead, char *lives, int player)
{
  SDL_Rect sS_sprite;
  static int destroySequence = 0;
  static int explosionSpriteTime = 0;
  SDL_RendererFlip flip = SDL_FLIP_NONE;

  if(!*playerDead)
  {
    sS_sprite.x = 131;
    sS_sprite.y = 623;
    sS_sprite.w = 73;
    sS_sprite.h = 52;
    if(player)
      SDL_SetTextureColorMod(tex, 255, 0, 0);
  }
  else
  {
    if(destroySequence%10 == 0)
    {
      ++explosionSpriteTime;
      if(explosionSpriteTime%2 == 0)
        explosionSpriteTime = 0;

    }
    if(explosionSpriteTime)
    {
      sS_sprite.x=340;
      sS_sprite.y=616;
      sS_sprite.w=95;
      sS_sprite.h=59;
    }
    else
    {
      sS_sprite.x = 218;
      sS_sprite.y = 616;
      sS_sprite.w = 105;
      sS_sprite.h = 61;
      flip = SDL_FLIP_VERTICAL;
    }
    destroySequence += 1;
  }

  if(destroySequence == 50)
  {
    destroySequence = 0;
    int life = '0';
    if(lives[0] > life)
    {
      *playerDead = 0;
      spaceShip->x = (WIDTH-SPRITEWIDTH)/2;
    }
    else
    {
      *playerDead = 2;
    }
  }

  SDL_RenderCopyEx(ren, tex, &sS_sprite, spaceShip, 0.0, NULL, flip);
}

// -----------------------------------------------------------------------------------------------------------------------
void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, int level)
{
  int levelAdjustment;
  switch(level)
  {
    case 6: levelAdjustment = PROJECTILESPEED/4; break;
    case 7: levelAdjustment = PROJECTILESPEED/2; break;
    case 8: levelAdjustment = 3*PROJECTILESPEED/4; break;
    default: levelAdjustment = 0; break;
  }

  projectile->y -= PROJECTILESPEED+levelAdjustment;

  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, projectile);
}

// -----------------------------------------------------------------------------------------------------------------------
void explodeProjectile(SDL_Renderer *ren, SDL_Rect *projectileBoom, SDL_Texture *tex, int *explodeP)
{
    static int delay = 0;

    SDL_Rect projectileSprite;
    projectileSprite.x = 218;
    projectileSprite.y = 616;
    projectileSprite.w = 105;
    projectileSprite.h = 61;

    delay += 1;

    if(delay < 5)
    {
      SDL_RenderCopy(ren, tex, &projectileSprite, projectileBoom);
    }
    else
    {
      *explodeP = 0;
      delay = 0;
    }
}

// -----------------------------------------------------------------------------------------------------------------------
void renderLives(SDL_Renderer *ren, SDL_Texture *tex, char *lives, int player, int players)
{
  SDL_Rect livesSprite;
  livesSprite.x = 131;
  livesSprite.y = 623;
  livesSprite.w = 73;
  livesSprite.h = 52;

  SDL_Rect livesTextHolder;
  livesTextHolder.w = SPRITEWIDTH;
  livesTextHolder.h = 20;
  livesTextHolder.y = HEIGHT-25;

  SDL_SetTextureColorMod(tex, 35, 255, 0);
  for(int i = '1'; i < lives[0]; ++i)
  {
    if(players)
    {
      if(player)
        livesTextHolder.x = 25 * ((i%49)+1)+((i%49)*15);
      else
        livesTextHolder.x = WIDTH - (25 * ((i%49)+1)+((i%49)*15) + SPRITEWIDTH);
    }
    else
      livesTextHolder.x = 25 * ((i%49)+1)+((i%49)*15);

    SDL_RenderCopy(ren, tex, &livesSprite, &livesTextHolder);
  }
  SDL_SetTextureColorMod(tex, 255, 255, 255);
}
