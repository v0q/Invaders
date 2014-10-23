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
// an enumeration for direction to move USE more enums!
enum DIRECTION{UP,DOWN,LEFT,RIGHT,NONE};

void initializeInvaders(Invader invaders[ROWS][COLS]);
void updateInvaders(Invader invaders[ROWS][COLS]);
void drawInvaders(SDL_Renderer *ren,SDL_Texture *tex,Invader invaders[ROWS][COLS]);

void moveSpaceShip(SDL_Rect spaceShip, int moveDir);

int main()
{
  Invader invaders[ROWS][COLS];
  initializeInvaders(invaders);

  // Initialize the spaceship & sprite position
  SDL_Rect spaceShip;
  spaceShip.x = (WIDTH+SPRITEWIDTH)/2;
  spaceShip.y = HEIGHT-50;
  spaceShip.w = SPRITEWIDTH;
  spaceShip.h = 20;

  SDL_Rect sS_sprite;
  sS_sprite.x = 0;
  sS_sprite.y = 0;
  sS_sprite.w = 73;
  sS_sprite.h = 52;



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



  int quit=0;
  // now we are going to loop forever, process the keys then draw

  while (quit !=1)
  {
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
        case SDLK_LEFT : moveSpaceShip(spaceShip, LEFT); break;
        case SDLK_RIGHT : moveSpaceShip(spaceShip, RIGHT); break;

       }
    }
  }

  // now we clear the screen (will use the clear colour set previously)
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);
  updateInvaders(invaders);
  drawInvaders(ren,tex,invaders);
  // Up until now everything was drawn behind the scenes.
  // This will show the new, red contents of the window.
  SDL_RenderPresent(ren);

  }

  SDL_Quit();
  return 0;
}

void moveSpaceShip(int *spaceShip_x, int moveDir)
{
    switch(moveDir)
    {
        case LEFT:
        {
            spaceShip.x -= 1;
            break;
        }
        case RIGHT:
        {
            spaceShip.x += 1;
            break;
        }

    }
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
      SDL_RenderFillRect(ren,&invaders[r][c].pos);
      SDL_RenderCopy(ren, tex,&SrcR,&invaders[r][c].pos);


    }
  }
}

void updateInvaders(Invader invaders[ROWS][COLS])
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
        invaders[r][c].pos.x+=1;
      else
        invaders[r][c].pos.x-=1;
      invaders[r][c].pos.y+=yinc;

    }
  }
}
