#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include "Invader.h"
// include the map for the maze.
// the width of the screen taking into account the maze and block
#define WIDTH 800
// the height of the screen taking into account the maze and block
#define HEIGHT 600
#define ANIMATIONSEQUENCELENGTH 160
#define INFOBOXHEIGHT 40
#define FONTSIZE 16

enum DIRECTION{LEFT,RIGHT};

// Put the sound file to a variable
// static const char *theme = "Space Invaders Soundtrack.mp3";
/// http://www.youtube.com/watch?v=fyTeZn2b46U
/// 'cdave' (April 13, 2014). GitHub Gist
/// Available from: https://gist.github.com/cdave1/10563386 [Accessed 24 October 2014].

// Declaring the functions
void initializeInvaders(Invader invaders[ROWS][COLS]);
void updateInvaders(Invader invaders[ROWS][COLS], int *gameSpeed, int *currentFrame);
void drawInvaders(SDL_Renderer *ren,SDL_Texture *tex,Invader invaders[ROWS][COLS], int currentFrame);

void moveSpaceShip(SDL_Rect *spaceShip, int moveDir);
void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *sStexture, SDL_Rect spaceShip);
void wooAlien(SDL_Renderer *ren, SDL_Rect alien_sprite, SDL_Texture *tex, Invader *alien, int direction, Mix_Chunk *ufosound);

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, SDL_Texture *tex);
void explodeProjectile(SDL_Renderer *ren, SDL_Rect *projectileBoom, SDL_Texture *tex, int *explodeP);

void renderSomeText(TTF_Font *font, SDL_Renderer *ren, char textToRender[17], SDL_Rect *textHolder);
void updateScore(int *score, int alienType, char thescore[17]);
void playSound(Mix_Chunk *sound, int chanToPlay, int loops);
void playMusic();


int main()
{
  Invader invaders[ROWS][COLS];
  initializeInvaders(invaders);

  Invader alien;
  alien.active = 0;
  alien.frame = 0;
  alien.type = 0;
  alien.pos.x=-SPRITEWIDTH;
  alien.pos.y=INFOBOXHEIGHT+1;
  alien.pos.w=SPRITEWIDTH+(SPRITEWIDTH/2);
  alien.pos.h=SPRITEHEIGHT;

  // Create a variable that records the key presses (continuous)
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  // Initializing the "line" that separates the top info box from the play field
  SDL_Rect infoLine;
  infoLine.x = 0;
  infoLine.y = INFOBOXHEIGHT;
  infoLine.w = WIDTH;
  infoLine.h = 5;

  // Initializing the spaceship "holder" and position
  SDL_Rect spaceShip;
  spaceShip.x = (WIDTH-SPRITEWIDTH)/2;
  spaceShip.y = HEIGHT-50;
  spaceShip.w = SPRITEWIDTH;
  spaceShip.h = 20;

  SDL_Rect alien_sprite;
  alien_sprite.x = 0;
  alien_sprite.y = 616;
  alien_sprite.w = 125;
  alien_sprite.h = 61;

  // Initializing projectile
  SDL_Rect projectile;
  projectile.x = 0;
  projectile.y = 0;
  projectile.w = 6;
  projectile.h = 18;

  // Initializing the box for projectile explosion
  SDL_Rect projectileBoom;
  projectileBoom.y = +infoLine.y+infoLine.h;

  // Some variables used all around
  int gameSpeed = 1;
  int projectileActive = 0;
  int direction = 0;
  int explodeP = 0;
  int currentFrame = 0;
  int quit=0;


  // Initializing the stuff for the infobox

  SDL_Rect scoreHolder;
  scoreHolder.x = 0;
  scoreHolder.y = INFOBOXHEIGHT/2 - FONTSIZE/2;
  scoreHolder.w = 0;
  scoreHolder.h = 0;

  int score = 0;
  // Making it possible for the variable to be passed to a string that'll be used for the drawing of the text
  char thescore[17];
  memset(thescore, 0, 17);
  sprintf(thescore, "Score: %04i", score);

  // Initialize TTF
  if(TTF_Init() == -1)
  {
    printf("%s\n", TTF_GetError());
    return EXIT_FAILURE;
  }

  TTF_Font* font = TTF_OpenFont("fonts/space_invaders.ttf", FONTSIZE);

  if(font == NULL)
  {
    printf("BOOM FAILED %s\n", TTF_GetError());
  }


  // initialise SDL and check that it worked otherwise exit
  // see here for details http://wiki.libsdl.org/CategoryAPI
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }

  // Music stuff
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 2;
  int audio_buffers = 4096;

  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
      fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
      exit(1);
  }

  Mix_Chunk *music[4];
  Mix_Chunk *shoot = NULL;
  Mix_Chunk *invaderkilled = NULL;
  Mix_Chunk *ufo_lowpitch = NULL;

  music[0] = Mix_LoadWAV("sounds/fastinvader1.wav");
  music[1] = Mix_LoadWAV("sounds/fastinvader2.wav");
  music[2] = Mix_LoadWAV("sounds/fastinvader3.wav");
  music[3] = Mix_LoadWAV("sounds/fastinvader4.wav");
  shoot = Mix_LoadWAV("sounds/shoot.wav");
  invaderkilled = Mix_LoadWAV("sounds/invaderkilled.wav");
  ufo_lowpitch = Mix_LoadWAV("sounds/ufo_lowpitch.wav");

  if((music[0] == NULL) ||
     (music[1] == NULL) ||
     (music[2] == NULL) ||
     (music[3] == NULL) ||
     (shoot == NULL) ||
     (invaderkilled == NULL) ||
     (ufo_lowpitch == NULL)) {
      fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
  }

/*  int channel;

  channel = Mix_PlayChannel(-1, soundBassLo, -1);
  if(channel == -1) {
      fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
  }
*/
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


  // Loading the texture for the spaceship
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

    // Checking if LEFT or RIGHT key (optionally A or D) is pressed and running the moveSpaceShip function accordingly.
    // Using the SDL_GetKeyboardState instead so the movement doesn't stop when pressing another key.
    if(keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A])
    {
      moveSpaceShip(&spaceShip, LEFT);
    }
    if(keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D])
    {
      moveSpaceShip(&spaceShip, RIGHT);
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

        // Run shooting related stuff if space is presset
        case SDLK_SPACE :
        {
          // Checking if a projectile already exists (is shot and still flying) as we restrict the amount of simultanious projectiles to one.
          if(!projectileActive)
          {
            // If no projectile exists, registers the current x and y position of the spaceship,
            // to get the coordinates from where the projectile should launch
            projectile.x = spaceShip.x+(SPRITEWIDTH/2-1);
            projectile.y = spaceShip.y;

            // Now that the projectile is about to shoot, change the projectileActive to true to prevent more projectiles to be shot
            // at the same time
            projectileActive = 1;
            playSound(shoot, 3, 0);
          }
          break;
        }

        case SDLK_RETURN : updateScore(&score, 1, thescore); break;

       }
    }
  }

  // now we clear the screen (will use the clear colour set previously)
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);

  // Check if projectile has been shot
  if(projectileActive)
  {
    // Check the projectile reaches the top of the screen and destroy it
    if(projectile.y <= 0+infoLine.y+infoLine.h)
    {
      // Assign some variables needed for the "explosion" of the projectile
      // As the width of the explosion if different from the projectiles width, we're gonna move the x coordinate of the explosion
      // half the sprites width to left
      projectileBoom.x = projectile.x - SPRITEWIDTH/2;

      // Activate the variable determining that an explosion should happen
      explodeP = 1;
      projectileActive = 0;
    }

    // Pass the stuff needed to display and move the projectile to the function
    shootPewPew(ren, &projectile, tex);

    // Check if the projectile collides with an invader by running through all the invaders with for loops
    // and checking if the coordinates of the projectile hit an invader.
    // This is done by checking if the projectiles x-coordinate is between the invaders x-coordinate and x-coordinate + SPRITEWIDTH
    // and if the projectiles y-coordinate is between the y-coordinate and y-coordinate + SPRITEHEIGHT of an invader
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
          // If the projectile hits a target, destroy the projectile and change the frame of that specific invader
          // to 3 which equals to the explosion "animation", the actual destroyal of the object happens later when the explosion is complete.
          invaders[r][c].frame = 3;
          projectileActive = 0;
          updateScore(&score, invaders[r][c].type, thescore);
          playSound(invaderkilled, 2, 0);
        }

      }
    }

    // Check if the projectile collides with the alien
    if(alien.pos.x   <=  projectile.x               &&
       projectile.x  <=  alien.pos.x+alien.pos.w    &&
       alien.pos.y   <=  projectile.y               &&
       projectile.y  <=  alien.pos.y+alien.pos.h    &&
       alien.active)
    {
      // If the projectile hits a target, destroy the projectile and change the frame of that specific invader
      // to 3 which equals to the explosion "animation", the actual destroyal of the object happens later when the explosion is complete.
      alien.active = 0;
      Mix_Pause(4);
      projectileActive = 0;
      updateScore(&score, 9, thescore);
      playSound(invaderkilled, 2, 0);
    }

  }

  // Checks if an explosion sequence should be run
  if(explodeP)
  {
    // If yes, pass the needed stuff to a function handling the explosion
    explodeProjectile(ren, &projectileBoom, tex, &explodeP);
  }

  // Running the stuff that handles invader movement, drawing/rendering and spaceship rendering
  updateInvaders(invaders, &gameSpeed, &currentFrame);
  drawInvaders(ren,tex,invaders, currentFrame);
  drawSpaceShip(ren, sStexture, spaceShip);

  if(rand()%1000 == 0 && !alien.active)
  {
    alien.active = 1;
    if(rand()%3 == 0)
    {
        alien.pos.x = -SPRITEWIDTH;
        direction = 0;
    }
    else
    {
        alien.pos.x = WIDTH + SPRITEWIDTH;
        direction = 1;
    }
    //printf("Random occurance\n");
  }

  if(alien.active)
  {
    wooAlien(ren, alien_sprite, tex, &alien, direction, ufo_lowpitch);
  }

  // Draw a line to separate the info bar from the field
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, &infoLine);

  //printf("%s\n", thescore);
  renderSomeText(font, ren, thescore, &scoreHolder);

  playMusic(gameSpeed, music);

  // Up until now everything was drawn behind the scenes.
  // This will show the new, red contents of the window.

  SDL_RenderPresent(ren);

  }


  // Music stuff
 // Mix_FreeMusic(music);
  /// 'cdave' (April 13, 2014). GitHub Gist
  /// Available from: https://gist.github.com/cdave1/10563386 [Accessed 24 October 2014].

  Mix_FreeChunk(music[0]);
  Mix_FreeChunk(music[1]);
  Mix_FreeChunk(music[2]);
  Mix_FreeChunk(music[3]);
  Mix_FreeChunk(shoot);
  Mix_FreeChunk(invaderkilled);
  Mix_FreeChunk(ufo_lowpitch);
  Mix_CloseAudio();
  TTF_Quit();
  SDL_Quit();
  return 0;
}

void moveSpaceShip(SDL_Rect *spaceShip, int moveDir)
{
  // Movement stuff
  switch(moveDir)
  {
    case LEFT:
    {
      // "Boundary collision left"
      if(spaceShip->x > 0+SPRITEWIDTH)
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
      // "Boundary collision right"
      if(spaceShip->x < WIDTH-2*(SPRITEWIDTH))
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

void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *sStexture, SDL_Rect spaceShip)
{
  SDL_Rect sS_sprite;
  sS_sprite.x = 0;
  sS_sprite.y = 0;
  sS_sprite.w = 73;
  sS_sprite.h = 52;

  SDL_RenderCopy(ren, sStexture, &sS_sprite, &spaceShip);
}

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile, SDL_Texture *tex)
{
  SDL_Rect projectileSprite;
  projectileSprite.x = 468;
  projectileSprite.y = 379;
  projectileSprite.w = 27;
  projectileSprite.h = 50;

  projectile->y -= 8;

  SDL_RenderCopy(ren, tex, &projectileSprite, projectile);

}

void explodeProjectile(SDL_Renderer *ren, SDL_Rect *projectileBoom, SDL_Texture *tex, int *explodeP)
{
    static int delay = 0;
    projectileBoom->w = SPRITEWIDTH;
    projectileBoom->h = 20;

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

void updateInvaders(Invader invaders[ROWS][COLS], int *gameSpeed, int *currentFrame)
{
  enum DIR{FWD,BWD};
  static int DIRECTION=FWD;
  int yinc=0;
  static int actColR = COLS-1;
  static int actColL = 0;
  int stopLoop = 0;
  static int updateSpeed = 1;

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

  if(invaders[0][actColR].pos.x>=WIDTH-(SPRITEWIDTH*2-SPRITEWIDTH/2))
  {
    DIRECTION=BWD;
    yinc=GAP/2;
  }

  else if(invaders[0][actColL].pos.x<=SPRITEWIDTH/2)
  {
    DIRECTION=FWD;
    yinc=GAP/2;
    if(updateSpeed%2 == 0 && *gameSpeed < 10)
    {
        ++*gameSpeed;
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

      if(invaders[r][c].pos.x%ANIMATIONSEQUENCELENGTH == 0)
      {
        ++*currentFrame;

        if(*currentFrame%2 == 0)
        {
          *currentFrame = 0;
        }

      }
      invaders[r][c].pos.y += yinc;

    }
  }
}

void renderSomeText(TTF_Font *font, SDL_Renderer *ren, char textToRender[17], SDL_Rect *textHolder)
{
  SDL_Texture *textTexture;
  SDL_Color fontColor = {255, 255, 255, 255};

  SDL_Surface *text = TTF_RenderText_Solid(font, textToRender, fontColor);
  textTexture = SDL_CreateTextureFromSurface(ren, text);

  SDL_QueryTexture(textTexture, NULL, NULL, &textHolder->w, &textHolder->h);

  SDL_RenderCopy(ren, textTexture, NULL, textHolder);
}

void updateScore(int *score, int alienType, char thescore[17])
{
    *score += 10*(alienType+1);
    sprintf(thescore, "Score: %04i", *score);
}

void playSound(Mix_Chunk *sound, int chanToPlay, int loops)
{

    if(chanToPlay == 0)
    {
        chanToPlay = -1;
    }

    // Intializing the channel, sound and play the sound once, check for errors
    int channel;
    channel = Mix_PlayChannel(chanToPlay, sound, loops);
    if(channel == -1) {
        fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
    }

}

void wooAlien(SDL_Renderer *ren, SDL_Rect alien_sprite, SDL_Texture *tex, Invader *alien, int direction, Mix_Chunk *ufosound)
{
    if(direction == 0)
        alien->pos.x += 3;
    else
        alien->pos.x -= 3;

    playSound(ufosound, 4, -1);
    //printf("%d\n", alien->pos.x);

    if(alien->pos.x < -SPRITEWIDTH || alien->pos.x > WIDTH+SPRITEWIDTH)
    {
        alien->active = 0;
        Mix_Pause(4);
    }

    SDL_RenderCopy(ren, tex, &alien_sprite, &alien->pos);
}

void playMusic(int gameSpeed, Mix_Chunk *music[4])
{
    static int currentSound = 0;
    static int musicFrame = 0;
    int musicSpeed;

    switch(gameSpeed)
    {
        case 1: musicSpeed = 50; break;
        case 2: musicSpeed = 45; break;
        case 3: musicSpeed = 40; break;
        case 4: musicSpeed = 35; break;
        case 5: musicSpeed = 30; break;
        case 6: musicSpeed = 25; break;
        case 7: musicSpeed = 20; break;
        case 8: musicSpeed = 15; break;
        case 9: musicSpeed = 10; break;
        case 10: musicSpeed = 5; break;
        default: musicSpeed = 5; break;
    }

    if(Mix_Playing(1) == 0)
    {
        ++musicFrame;

        if(musicFrame%musicSpeed == 0)
        {

            if(currentSound < 3)
            {
                ++currentSound;
            }
            else
            {
                currentSound = 0;
            }

            playSound(music[currentSound], 1, 0);
        }
    }



}
