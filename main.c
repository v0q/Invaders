#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "Invader.h"
// include the map for the maze.
// the width of the screen taking into account the maze and block
#define WIDTH 800
// the height of the screen taking into account the maze and block
#define HEIGHT 600
#define ANIMATIONSTEP 2500

enum DIRECTION{LEFT,RIGHT};

void initializeInvaders(Invader invaders[ROWS][COLS]);
void updateInvaders(Invader invaders[ROWS][COLS], int moveSpeed);
void drawInvaders(SDL_Renderer *ren,SDL_Texture *tex,Invader invaders[ROWS][COLS]);

void moveSpaceShip(SDL_Rect *spaceShip, int moveDir, int moveSpeed);
void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *sStexture, SDL_Rect spaceShip);

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, SDL_Texture *tex,  int *projectileActive, int moveSpeed, int *projectileExplosion);

int main()
{
  Invader invaders[ROWS][COLS];
  initializeInvaders(invaders);
  Uint8 *keystate = SDL_GetKeyboardState(NULL);

  // Initialize the spaceship & sprite position
  SDL_Rect spaceShip;
  spaceShip.x = (WIDTH-SPRITEWIDTH)/2;
  spaceShip.y = HEIGHT-50;
  spaceShip.w = SPRITEWIDTH;
  spaceShip.h = 20;

  SDL_Rect projectile;
  projectile.x = 0;
  projectile.y = spaceShip.y;
  projectile.w = 6;
  projectile.h = 18;

  int moveSpeed = 1;
  int projectileExplosion = 0;
  int projectileActive = 0;
  int quit=0;

  // initialise SDL and check that it worked otherwise exit
  // see here for details http://wiki.libsdl.org/CategoryAPI
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }
  // we are now going to create an SDL window

  SDL_Window *win = 0;
  win = SDL_CreateWindow("Invaders", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (win == 0)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }
  // the renderer is the core element we need to draw, each call to SDL for drawing will need the
  // renderer pointer
  SDL_Renderer *ren = 0;
  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  // check to see if this worked
  if (ren == 0)
  {
    printf("%s\n",SDL_GetError() );
    return EXIT_FAILURE;
  }
  // this will set the background colour to white.
  // however we will overdraw all of this so only for reference
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

//  // SDL image is an abstraction for all images
  SDL_Surface *image;
//  // we are going to use the extension SDL_image library to load a png, documentation can be found here
//  // http://www.libsdl.org/projects/SDL_image/
  image=IMG_Load("sprites.png");
  if(!image)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }
//  // SDL texture converts the image to a texture suitable for SDL rendering  / blitting
//  // once we have the texture it will be store in hardware and we don't need the image data anymore

  SDL_Texture *tex = 0;
  tex = SDL_CreateTextureFromSurface(ren, image);
  // free the image
  SDL_FreeSurface(image);


  // Stuff for initializing the spaceship

  // Load the spaceships textures

  SDL_Surface *sShip;
  sShip=IMG_Load("Ship.png");
  if(!sShip)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }

  SDL_Texture *sStexture = 0;
  sStexture = SDL_CreateTextureFromSurface(ren, sShip);

  SDL_FreeSurface(sShip);

  // now we are going to loop forever, process the keys then draw

  while (quit !=1)
  {

    if(keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A])
    {
      moveSpaceShip(&spaceShip, LEFT, moveSpeed);
    }
    if(keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D])
    {
      moveSpaceShip(&spaceShip, RIGHT, moveSpeed);
    }

    SDL_Event event;
    // grab the SDL even (this will be keys etc)
    while (SDL_PollEvent(&event))
    {
      // look for the x of the window being clicked and exit
      if (event.type == SDL_QUIT)
        quit = 1;
      // check for a key down
      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym)
        {

        // if we have an escape quit
        case SDLK_ESCAPE : quit=1; break;
        case SDLK_SPACE :
        {
          if(!projectileActive)
          {
            projectile.x = spaceShip.x+(SPRITEWIDTH/2-1);
            projectile.y = spaceShip.y;
            projectileActive = 1;
          }
          break;
        }
        case SDLK_KP_PLUS :
        {
          if(moveSpeed < 9)
          {
            moveSpeed += 2;
          }
          break;
        }
        case SDLK_KP_MINUS :
        {
          if(moveSpeed > 1)
          {
            moveSpeed -= 2;
          }
          break;
        }

       }
    }
  }

  // now we clear the screen (will use the clear colour set previously)
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);

  // Check if projectile has been shot
  if(projectileActive)
  {
    // If the projectile reaches the top, destroy it
    if(projectile.y <= 0)
    {
      projectileExplosion = 1;
    }

    // Run the stuff to move the projectile
    shootPewPew(ren, &projectile, tex, &projectileActive, moveSpeed, &projectileExplosion);
    // Run through the invaders
    for(int r=0; r<ROWS; ++r)
    {
      for(int c=0; c<COLS; ++c)
      {

        // Check if the projectile collides with any of 'em
        if(invaders[r][c].pos.x   <=  projectile.x                      &&
           projectile.x           <=  invaders[r][c].pos.x+SPRITEWIDTH  &&
           invaders[r][c].pos.y   <=  projectile.y                      &&
           projectile.y           <=  invaders[r][c].pos.y+SPRITEHEIGHT &&
           invaders[r][c].active)
        {
          // If the projectile hits a target, "deactive"/destroy it and destroy the projectile
          invaders[r][c].frame = 3;
          projectileActive = 0;
        }

      }
    }

  }

  updateInvaders(invaders, moveSpeed);
  drawInvaders(ren,tex,invaders);
  drawSpaceShip(ren, sStexture, spaceShip);

  // Up until now everything was drawn behind the scenes.
  // This will show the new, red contents of the window.

  SDL_RenderPresent(ren);

  }

  SDL_Quit();
  return 0;
}

void moveSpaceShip(SDL_Rect *spaceShip, int moveDir, int moveSpeed)
{
  // Movement stuff
  switch(moveDir)
  {
    case LEFT:
    {
      // "Boundary collision left"
      if(spaceShip->x > 0+SPRITEWIDTH)
      {
        spaceShip->x -= 5*moveSpeed;
      }
      else
      {
        spaceShip->x = 0;
      }
      break;
    }
    case RIGHT:
    {
      // "Boundary collision right"
      if(spaceShip->x < WIDTH-2*(SPRITEWIDTH))
      {
        spaceShip->x += 5*moveSpeed;
      }
      else
      {
        spaceShip->x = WIDTH-SPRITEWIDTH;
      }

      break;
    }
  }
}

void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *sStexture, SDL_Rect spaceShip)
{
  SDL_Rect sS_sprite;
  sS_sprite.x = 0;
  sS_sprite.y = 0;
  sS_sprite.w = 73;
  sS_sprite.h = 52;

  SDL_RenderCopy(ren, sStexture, &sS_sprite, &spaceShip);
}

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, SDL_Texture *tex, int *projectileActive, int moveSpeed, int *projectileExplosion)
{
  SDL_Rect projectileSprite;

  SDL_Rect projectileBoom;
  projectileBoom.x = projectile->x - SPRITEWIDTH/2;
  projectileBoom.y = projectile->y;
  projectileBoom.w = SPRITEWIDTH;
  projectileBoom.h = 20;

  if(!*projectileExplosion)
  {
    projectile->y -= 8*moveSpeed;
    projectileSprite.x = 468;
    projectileSprite.y = 379;
    projectileSprite.w = 27;
    projectileSprite.h = 50;
    SDL_RenderCopy(ren, tex, &projectileSprite, projectile);
  }
  else
  {
    *projectileExplosion = 0;
    *projectileActive = 0;
    projectileSprite.x = 218;
    projectileSprite.y = 616;
    projectileSprite.w = 105;
    projectileSprite.h = 61;
    SDL_RenderCopy(ren, tex, &projectileSprite, &projectileBoom);

  }

  //SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

  //SDL_RenderFillRect(ren, projectile);
}

void initializeInvaders(Invader invaders[ROWS][COLS])
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
      pos.y=ypos+SPRITEHEIGHT;
      xpos+=(GAP+SPRITEWIDTH);
      invaders[r][c].pos=pos;
      invaders[r][c].active=1;
      invaders[r][c].frame=0;
      if(r==0)
        invaders[r][c].type=TYPE1;
      else if(r==1 || r==2)
        invaders[r][c].type=TYPE2;
      else
        invaders[r][c].type=TYPE3;

    }
    ypos+=(GAP+SPRITEHEIGHT);
  }
}


void drawInvaders(SDL_Renderer *ren, SDL_Texture *tex, Invader invaders[ROWS][COLS])
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
          case TYPE1 :
          {
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SrcR.x=292+(117*invaders[r][c].frame);
            SrcR.y=0;
            SrcR.w=80;
            SrcR.h=80;
            break;
          }
          case TYPE2 :
          {
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            SrcR.x=145*invaders[r][c].frame;
            SrcR.y=0;
            SrcR.w=111;
            SrcR.h=80;
            break;
          }
          case TYPE3 :
          {
            SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
            SrcR.x=140*invaders[r][c].frame;
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

void updateInvaders(Invader invaders[ROWS][COLS], int moveSpeed)
{
  enum DIR{FWD,BWD};
  static int DIRECTION=FWD;
  int yinc=0;
  static int actColR = COLS-1;
  static int actColL = 0;
  int stopLoop = 0;

  // Loop through the column from right to find the active outermost column on right
  for(int c = COLS-1; c >= 0; --c)
    {
      // Loop through all the rows in a column
      for(int r = 0; r < ROWS; ++r)
      {
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

  if(invaders[0][actColR].pos.x>=WIDTH-(SPRITEWIDTH*2))
  {
    DIRECTION=BWD;
    yinc=GAP;
  }

  else if(invaders[0][actColL].pos.x<=SPRITEWIDTH)
  {
    DIRECTION=FWD;
    yinc=GAP;

  }

  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      if(DIRECTION==FWD)
      {
        invaders[r][c].pos.x+=1*moveSpeed;
      }
      else
      {
        invaders[r][c].pos.x-=1*moveSpeed;
      }

      if(invaders[r][c].pos.x%40 == 0 && invaders[r][c].frame != 3)
      {
        invaders[r][c].frame += 1;
        if(invaders[r][c].frame%2 == 0)
        {
          invaders[r][c].frame = 0;
        }
      }
      invaders[r][c].pos.y += yinc;

    }
  }
}
