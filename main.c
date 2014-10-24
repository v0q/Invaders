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

enum DIRECTION{LEFT,RIGHT};

void initializeInvaders(Invader invaders[ROWS][COLS]);
void updateInvaders(Invader invaders[ROWS][COLS], int moveSpeed);
void drawInvaders(SDL_Renderer *ren,SDL_Texture *tex,Invader invaders[ROWS][COLS]);

void moveSpaceShip(SDL_Rect *spaceShip, int moveDir, int moveSpeed);
void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *sStexture, SDL_Rect spaceShip);

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, int *projectileActive, int moveSpeed);

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
  projectile.w = 2;
  projectile.h = 8;

  int moveSpeed = 1;

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
  image=IMG_Load("InvaderA2.bmp");
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

  int projectileActive = 0;
  int quit=0;
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
    // Run the stuff to move the projectile
    shootPewPew(ren, &projectile, &projectileActive, moveSpeed);

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
          invaders[r][c].active = 0;
          projectileActive = 0;
        }

      }
    }

    // If the projectile reaches the top, destroy it
    if(projectile.y <= 0)
    {
      projectileActive = 0;
    }
  }

  updateInvaders(invaders, moveSpeed);
  drawInvaders(ren,tex,invaders);
  drawSpaceShip(ren, sStexture, spaceShip);

  //SDL_RenderFillRect(ren, &spaceShip);
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

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, int *projectileActive, int moveSpeed)
{
  *projectileActive = 1;
  projectile->y -= 4*moveSpeed;
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, projectile);
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
  SrcR.x=0;
  SrcR.y=0;
  SrcR.w=88;
  SrcR.h=64;
  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      switch(invaders[r][c].type)
      {
      case TYPE1 : SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); break;
      case TYPE2 : SDL_SetRenderDrawColor(ren, 0, 255, 0, 255); break;
      case TYPE3 : SDL_SetRenderDrawColor(ren, 0, 0, 255, 255); break;
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
  if(invaders[0][COLS].pos.x>=(WIDTH-SPRITEWIDTH)-(COLS*(SPRITEWIDTH+GAP)))
  {
    DIRECTION=BWD;
    yinc=GAP;

  }
  else if(invaders[0][0].pos.x<=SPRITEWIDTH)
  {
    DIRECTION=FWD;
    yinc=GAP;

  }

  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      if(DIRECTION==FWD)
        invaders[r][c].pos.x+=1*moveSpeed;
      else
        invaders[r][c].pos.x-=1*moveSpeed;
      invaders[r][c].pos.y+=yinc;

    }
  }
}
