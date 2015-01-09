#include "Defender.h"

void initialiseDefender(SDL_Rect spaceShip[2], SDL_Rect projectile[2], SDL_Rect projectileBoom[2])
{
  // Initializing the spaceship "holders", positions and projectiles
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
  }
}

void moveSpaceShip(SDL_Rect *spaceShip, int moveDir)
{
  // Movement stuff
  switch(moveDir)
  {
    case LEFT:
    {
      // Stop the player from beying able to move past the left side of the screen
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
      // Stop the player from beying able to move past the right side of the screen
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

void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *spaceShip, int *playerDead, char *lives, int player)
{
  // initialise the sprite rect, the explosion timer and the explosion frame stuff
  SDL_Rect sS_sprite;
  static int destroySequence = 0;
  static int explosionSpriteTime = 0;
  SDL_RendererFlip flip = SDL_FLIP_NONE;

  if(!*playerDead)
  {
    // Sprite coordinates when player's not dead
    sS_sprite.x = 131;
    sS_sprite.y = 623;
    sS_sprite.w = 73;
    sS_sprite.h = 52;
    if(player)
      SDL_SetTextureColorMod(tex, 255, 0, 0);
  }
  else
  {
    // If player died, start the destroy sequence and set the length of it to be 50 frames
    if(destroySequence%10 == 0)
    {
      // This changes the explosion sprite to "flash" between the two sprites as in the original game
      ++explosionSpriteTime;
      if(explosionSpriteTime%2 == 0)
        explosionSpriteTime = 0;

    }
    // Changing the sprite coordinates to match the two different explosion frames
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
    // As lives is a char, the comparison happens in ASCII values, thus we'll be creating a variable that holds the ascii value of 0
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
