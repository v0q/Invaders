/*
 Copyright © 2015 Teemu Lindborg
*/

#include "Defender.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Defender.c
/// @brief Defender's functionality
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
void initialiseDefender(SDL_Rect o_spaceShip[2], SDL_Rect o_projectile[2], SDL_Rect o_projectileBoom[2])
{
  for(int i = 0; i < 2; ++i)
  {
    o_spaceShip[i].y = HEIGHT-50;
    o_spaceShip[i].w = SPRITEWIDTH;
    o_spaceShip[i].h = 20;
    o_projectile[i].x = 0;
    o_projectile[i].y = 0;
    o_projectile[i].w = 2;
    o_projectile[i].h = 8;

    o_projectileBoom[i].y = INFOBOXHEIGHT+5;
    o_projectileBoom[i].w = SPRITEWIDTH;
    o_projectileBoom[i].h = 20;
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void moveSpaceShip(SDL_Rect *io_spaceShip, int _moveDir)
{
  switch(_moveDir)
  {
    case LEFT:
    {
      if(io_spaceShip->x > 0)
      {
        io_spaceShip->x -= 5;
      }
      else
      {
        io_spaceShip->x = 0;
      }
      break;
    }
    case RIGHT:
    {
      if(io_spaceShip->x < WIDTH-SPRITEWIDTH)
      {
        io_spaceShip->x += 5;
      }
      else
      {
        io_spaceShip->x = WIDTH-SPRITEWIDTH;
      }
      break;
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------
void drawSpaceShip(SDL_Renderer *_ren, SDL_Texture *_sStexture, SDL_Rect *_spaceShip, int *io_playerDead, char *_lives, int _player)
{
  SDL_Rect sS_sprite;
  static int destroySequence = 0;
  static int explosionSpriteTime = 0;
  SDL_RendererFlip flip = SDL_FLIP_NONE;

  if(!*io_playerDead)
  {
    sS_sprite.x = 131;
    sS_sprite.y = 623;
    sS_sprite.w = 73;
    sS_sprite.h = 52;
    if(_player)
      SDL_SetTextureColorMod(_sStexture, 255, 0, 0);
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
    if(_lives[0] > life)
    {
      *io_playerDead = 0;
      _spaceShip->x = (WIDTH-SPRITEWIDTH)/2;
    }
    else
    {
      *io_playerDead = 2;
    }
  }

  SDL_RenderCopyEx(_ren, _sStexture, &sS_sprite, _spaceShip, 0.0, NULL, flip);
}

// -----------------------------------------------------------------------------------------------------------------------
void shootPewPew(SDL_Renderer *_ren, SDL_Rect *io_projectile, int _level)
{
  int levelAdjustment;
  switch(_level)
  {
    case 6: levelAdjustment = PROJECTILESPEED/4; break;
    case 7: levelAdjustment = PROJECTILESPEED/2; break;
    case 8: levelAdjustment = 3*PROJECTILESPEED/4; break;
    default: levelAdjustment = 0; break;
  }

  io_projectile->y -= PROJECTILESPEED+levelAdjustment;

  SDL_SetRenderDrawColor(_ren, 255, 255, 255, 255);
  SDL_RenderFillRect(_ren, io_projectile);
}

// -----------------------------------------------------------------------------------------------------------------------
void explodeProjectile(SDL_Renderer *_ren, SDL_Rect *_projectileBoom, SDL_Texture *_tex, int *o_explodeP)
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
      SDL_RenderCopy(_ren, _tex, &projectileSprite, _projectileBoom);
    }
    else
    {
      *o_explodeP = 0;
      delay = 0;
    }
}

// -----------------------------------------------------------------------------------------------------------------------
void renderLives(SDL_Renderer *_ren, SDL_Texture *_tex, char *_lives, int _player, int _players)
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

  SDL_SetTextureColorMod(_tex, 35, 255, 0);
  for(int i = '1'; i < _lives[0]; ++i)
  {
    if(_players)
    {
      if(_player)
        livesTextHolder.x = 25 * ((i%49)+1)+((i%49)*15);
      else
        livesTextHolder.x = WIDTH - (25 * ((i%49)+1)+((i%49)*15) + SPRITEWIDTH);
    }
    else
      livesTextHolder.x = 25 * ((i%49)+1)+((i%49)*15);

    SDL_RenderCopy(_ren, _tex, &livesSprite, &livesTextHolder);
  }
  SDL_SetTextureColorMod(_tex, 255, 255, 255);
}
