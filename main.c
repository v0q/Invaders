#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Invader.h"
#include "ExplosionPatterns.h"
// the width of the screen taking into account the maze and block
#define WIDTH 800
// the height of the screen taking into account the maze and block
#define HEIGHT 600
#define ANIMATIONSEQUENCELENGTH 800
#define INFOBOXHEIGHT 40
#define FONTSIZE 16
#define PROJECTILESPEED 12

enum DIRECTION{LEFT,RIGHT};

// Declaring the functions
void initializeInvaders(Invader invaders[ROWS][COLS]);
void updateInvaders(Invader invaders[ROWS][COLS], int *gameSpeed, int *currentFrame, int actInvaderInRow[COLS], int level);
void drawInvaders(SDL_Renderer *ren,SDL_Texture *tex,Invader invaders[ROWS][COLS], int currentFrame);
void invaderShootPewPew(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *invaderProjectile);

void moveSpaceShip(SDL_Rect *spaceShip, int moveDir);
void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *sStexture, SDL_Rect *spaceShip, int *playerDead, char *lives, int *gameover);
void wooAlien(SDL_Renderer *ren, SDL_Texture *tex, Invader *alien, int direction, Mix_Chunk *ufosound);

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile);
void explodeProjectile(SDL_Renderer *ren, SDL_Rect *projectileBoom, SDL_Texture *tex, int *explodeP);

void playSound(Mix_Chunk *sound, int chanToPlay, int loops);
void playMusic(int gameSpeed, Mix_Chunk *music[4]);

void renderLives(SDL_Renderer *ren, SDL_Texture *tex, char *lives);

SDL_Texture *textureFromText(SDL_Renderer *ren, TTF_Font *font, char *textToRender);

void editPixel(SDL_Surface *shieldSurface, int x, int y, int PlorIn);
Uint32 pixelActive(SDL_Surface *shieldSurface, int x, int y);


int main()
{

// -----------------------------------------------------------
// --------------- INITIALIZING STUFF START ------------------
// -----------------------------------------------------------
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

  SDL_Rect bottomLine;
  bottomLine.x = 0;
  bottomLine.y = HEIGHT-30;
  bottomLine.w = WIDTH;
  bottomLine.h = 3;

  // Initializing the spaceship "holder" and position
  SDL_Rect spaceShip;
  spaceShip.x = (WIDTH-SPRITEWIDTH)/2;
  spaceShip.y = HEIGHT-50;
  spaceShip.w = SPRITEWIDTH;
  spaceShip.h = 20;

  // Initializing projectile
  SDL_Rect projectile;
  projectile.x = 0;
  projectile.y = 0;
  projectile.w = 2;
  projectile.h = 8;

  // Initializing the box for projectile explosion
  SDL_Rect projectileBoom;
  projectileBoom.y = +infoLine.y+infoLine.h;

  SDL_Rect invaderProjectile[COLS];

  SDL_Rect screen;
  screen.x = 0;
  screen.y = 0;
  screen.w = WIDTH;
  screen.h = HEIGHT;

  SDL_Rect shield;
  shield.w = 88;
  shield.h = 64;
  shield.y = bottomLine.y - shield.h*2;

  SDL_Rect shields[4] = {shield, shield, shield, shield};

  for(int i = 0; i < 4; ++i)
  {
    shields[i].x = (WIDTH-(shield.w*4))/8 + i*(shield.w+(WIDTH-(shield.w*4))/4);
  }

  // Some variables used all around
  initializeInvaders(invaders);
  int gameSpeed = 1;
  int projectileActive = 0;
  int direction = 0;
  int explodeP = 0;
  int currentFrame = 0;
  int quit=0;
  int actInvaderInRow[COLS];
  int invaderProjectileActive[COLS];
  int gameover = 0;
  int loadNewScreen = 1;
  int playerDead = 0;
  int startgame = 0;
  int colX = 0;
  int colY = 0;
  int newstart = 0;
  int invaderskilled = 0;
  int level = 1;


  char thegameover[] = "Game Over";
  char pressanykey[] = "Press any key to continue";

  for(int i = 0; i < COLS; ++i)
  {
    invaderProjectile[i].x = 0;
    invaderProjectile[i].y = 0;
    invaderProjectile[i].w = 6;
    invaderProjectile[i].h = 18;
    invaderProjectileActive[i] = 0;
    actInvaderInRow[i] = 0;
  }


  // Initializing the stuff for the infobox
  SDL_Rect scoreHolder;
  scoreHolder.x = 5;
  scoreHolder.y = INFOBOXHEIGHT/2 - FONTSIZE/2;
  scoreHolder.w = 0;
  scoreHolder.h = 0;

  SDL_Rect livesHolder;
  livesHolder.x = 5;
  livesHolder.y = bottomLine.y + FONTSIZE/2;
  livesHolder.w = 0;
  livesHolder.h = 0;

  SDL_Rect levelHolder;
  levelHolder.y = INFOBOXHEIGHT/2 - FONTSIZE/2;
  levelHolder.w = 0;
  levelHolder.h = 0;

  int score = 0;
  // Making it possible for the variable to be passed to a string that'll be used for the drawing of the text
  char thescore[12] = {0};
  sprintf(thescore, "Score: %04i", score);

  char thelevel[9] = {0};
  sprintf(thelevel, "Level: %i", level);


  char lives[] = "3";

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
  Mix_Chunk *explosion = NULL;

  music[0] = Mix_LoadWAV("sounds/fastinvader1.wav");
  music[1] = Mix_LoadWAV("sounds/fastinvader2.wav");
  music[2] = Mix_LoadWAV("sounds/fastinvader3.wav");
  music[3] = Mix_LoadWAV("sounds/fastinvader4.wav");
  shoot = Mix_LoadWAV("sounds/shoot.wav");
  invaderkilled = Mix_LoadWAV("sounds/invaderkilled.wav");
  ufo_lowpitch = Mix_LoadWAV("sounds/ufo_lowpitch.wav");
  explosion = Mix_LoadWAV("sounds/explosion.wav");

  if((music[0] == NULL) ||
     (music[1] == NULL) ||
     (music[2] == NULL) ||
     (music[3] == NULL) ||
     (shoot == NULL) ||
     (invaderkilled == NULL) ||
     (ufo_lowpitch == NULL) ||
     (explosion == NULL)) {
      fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
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

  // Initialize the score stuff
  SDL_Texture *scoreTexture;

  scoreTexture = textureFromText(ren, font, thescore);
  SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreHolder.w, &scoreHolder.h);

  // Initialize lives rendering
  SDL_Texture *livesTexture;
  livesTexture = textureFromText(ren, font, lives);
  SDL_QueryTexture(livesTexture, NULL, NULL, &livesHolder.w, &livesHolder.h);

  SDL_Texture *levelTexture;
  levelTexture = textureFromText(ren, font, thelevel);
  SDL_QueryTexture(levelTexture, NULL, NULL, &levelHolder.w, &levelHolder.h);
  levelHolder.x = WIDTH-5-levelHolder.w;

  // Initializing shield textures
  SDL_Surface *shieldSurface[4];
  SDL_Texture *shieldTexture[4];

  for(int i = 0; i < 4; ++i)
  {
    shieldSurface[i] = IMG_Load("shieldTexture.png");
    if(!shieldSurface[i])
    {
     printf("IMG_Load: %s\n", IMG_GetError());
     return EXIT_FAILURE;
    }
    shieldTexture[i] = SDL_CreateTextureFromSurface(ren, shieldSurface[i]);
  }



  // Initializing the rect holders for game over and press any key textures
  SDL_Rect gameoverHolder;
  SDL_Rect anykeyHolder;

  // Initializing the textures
  SDL_Texture *gameoverTexture;
  SDL_Texture *anykeyTexture;

  // Making a font with bigger font size for the game over texture
  TTF_Font* gofont = TTF_OpenFont("fonts/space_invaders.ttf", FONTSIZE*3);

  // Creating the textures by passing the required data to the function that creates textures from text
  gameoverTexture = textureFromText(ren, gofont, thegameover);
  anykeyTexture = textureFromText(ren, font, pressanykey);

  // Getting the width and height for the rect holders from the texture
  SDL_QueryTexture(gameoverTexture, NULL, NULL, &gameoverHolder.w, &gameoverHolder.h);
  SDL_QueryTexture(anykeyTexture, NULL, NULL, &anykeyHolder.w, &anykeyHolder.h);

  // Setting the physical position for the holders
  gameoverHolder.x = (WIDTH-gameoverHolder.w)/2;
  gameoverHolder.y = (HEIGHT-gameoverHolder.h)/2;

  anykeyHolder.x = (WIDTH-anykeyHolder.w)/2;
  anykeyHolder.y = (HEIGHT-gameoverHolder.h)/2;

// ---------------------------------------------------------
// --------------- INITIALIZING STUFF END ------------------
// ---------------------------------------------------------



  while(!startgame)
  {

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      // look for the x of the window being clicked and exit
      if (event.type == SDL_QUIT)
        quit = 1;
      // check for a key down
      if (event.type == SDL_KEYDOWN)
      {
        startgame = 1;
      }
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, anykeyTexture, NULL, &anykeyHolder);
    SDL_RenderPresent(ren);
  }


  // now we are going to loop forever, process the keys then draw

  while (quit !=1 && !gameover)
  {

    if(newstart)
    {
      initializeInvaders(invaders);
      gameSpeed = 1;
      currentFrame = 0;
      playerDead = 0;
      invaderskilled = 0;
      ++level;
      sprintf(thelevel, "Level: %i", level);
      levelTexture = textureFromText(ren, font, thelevel);
      newstart = 0;
    }


    // Checking if LEFT or RIGHT key (optionally A or D) is pressed and running the moveSpaceShip function accordingly.
    // Using the SDL_GetKeyboardState instead so the movement doesn't stop when pressing another key.
    if((keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) && !playerDead)
    {
      moveSpaceShip(&spaceShip, LEFT);
    }
    if((keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) && !playerDead)
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
          if(!projectileActive && !playerDead)
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
        }
      }
    }

  // now we clear the screen (will use the clear colour set previously)
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);

  // Render shields
  for(int i = 0; i < 4; ++i)
    SDL_RenderCopy(ren, shieldTexture[i], NULL, &shields[i]);

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
    shootPewPew(ren, &projectile);

    if(SDL_HasIntersection(&alien.pos, &projectile) && alien.active)
    {
      // If the projectile hits a target, destroy the projectile and change the frame of that specific invader
      // to 3 which equals to the explosion "animation", the actual destroyal of the object happens later when the explosion is complete.
      alien.frame = 1;
      Mix_Pause(4);
      projectileActive = 0;
      score += 10*(alien.type+1);
      sprintf(thescore, "Score: %04i", score);
      if(lives[0] < 9)
      {
        ++lives[0];
        livesTexture = textureFromText(ren, font, lives);
      }
      SDL_QueryTexture(livesTexture, NULL, NULL, &livesHolder.w, &livesHolder.h);
      scoreTexture = textureFromText(ren, font, thescore);
      playSound(invaderkilled, 2, 0);
    }

    // Checks if a shield has been shot
    for(int i = 0; i < 4; ++i)
    {
      // Compensating for the projectile speed, i.e. the "skips" that it does by moving so fast
      for(int pSC = 0; pSC < PROJECTILESPEED+projectile.h; ++pSC)
      {
        // Using this compensated "projectile" that's not rendered as the colliding one and store the coordinates in a separate "collision rect"
        SDL_Rect collision;
        SDL_Rect pSCprojectile = {projectile.x, projectile.y+pSC, projectile.w, projectile.h};
        if(SDL_HasIntersection(&pSCprojectile, &shields[i]))
        {
          // Store the coordinates here and pass on to the function that handles the destruction of the shield, also checks if the part we hit is not destroyed
          SDL_IntersectRect(&pSCprojectile, &shields[i], &collision);
          colX = (collision.x - shields[i].x) / (shields[i].w/shieldSurface[i]->w);
          colY = (collision.y - shields[i].y) / (shields[i].h/shieldSurface[i]->h);
          if(pixelActive(shieldSurface[i], colX, colY) == 0x0000FF00)
          {
            projectileActive = 0;
            editPixel(shieldSurface[i], colX, colY, 0);
            shieldTexture[i] = SDL_CreateTextureFromSurface(ren, shieldSurface[i]);
          }
        }
      }
    }

  }

  // Loop through the invaders to check various things
  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      if(projectileActive)
      {
        // Check if the projectile collides with any of 'em
        if(SDL_HasIntersection(&invaders[r][c].pos, &projectile) && invaders[r][c].active)
        {
          // If the projectile hits a target, destroy the projectile and change the frame of that specific invader
          // to 3 which equals to the explosion "animation", the actual destroyal of the object happens later when the explosion is complete.
          projectileActive = 0;
          invaders[r][c].frame = 3;
          score += 10*(invaders[r][c].type+1);
          sprintf(thescore, "Score: %04i", score);
          scoreTexture = textureFromText(ren, font, thescore);
          playSound(invaderkilled, 2, 0);
          ++invaderskilled;
        }
      }

      // Check if an invader hits the player or reaches the ground => game over
      if((SDL_HasIntersection(&invaders[r][c].pos, &spaceShip) && invaders[r][c].active) ||
         (invaders[r][c].pos.y >= bottomLine.y+SPRITEHEIGHT && invaders[r][c].active))
      {
        gameover = 1;
      }
    }
  }

  // Checks if an explosion sequence should be run
  if(explodeP)
  {
    // If yes, pass the needed stuff to a function handling the explosion
    explodeProjectile(ren, &projectileBoom, tex, &explodeP);
  }

  // Set the a new seed for the random stuff every second, i.e. something random can occur only every second instead of every frame
  srand(time(NULL));

  // Basically set the "probability" of an ufo spawn to be around 5% every second; the rand()%20 can have values of 0..19 thus resulting in a 1/20 chance of it being 0
  // which translates to 5% probability, also checking that there's no alien/ufo active at the given moment
  if(rand()%20 == 0 && !alien.active)
  {
    // Sets the alien/ufo to be active and gives a 50/50 chance for it to spawn on either left or the right side of the screen
    alien.active = 1;
    if(rand()%2 == 0)
    {
        alien.pos.x = -SPRITEWIDTH;
        direction = 0;
    }
    else
    {
        alien.pos.x = WIDTH + SPRITEWIDTH;
        direction = 1;
    }
  }

  // Randomises the event of invaders shooting, gives a 50% chance of a possible invader projectile every second
  if(rand()%2 == 0)
  {
    // Randomises the column from which the projectile will be shot
    int randomColumn = rand()%COLS;
    // Check if a projectile from that column is not active at the given moment and there's actually invaders alive at the given column
    if(actInvaderInRow[randomColumn] != 0 && !invaderProjectileActive[randomColumn])
    {
      // If everything passed this far, set the starting point for a column and set it to be active
      invaderProjectile[randomColumn].x = invaders[actInvaderInRow[randomColumn]-1][randomColumn].pos.x;
      invaderProjectile[randomColumn].y = invaders[actInvaderInRow[randomColumn]-1][randomColumn].pos.y+SPRITEHEIGHT;
      invaderProjectileActive[randomColumn] = 1;
    }
  }

  // Loop through the columns of invaders and search for active projectiles
  for(int i = 0; i < COLS; ++i)
  {
    if(invaderProjectileActive[i])
    {
      //printf("invaderShootPewPew(%d - %d - %d);\n", i, invaderProjectile[i].x, invaderProjectile[i].y);
      // If a shot projectile is found from any given column, send the data to the function that handles its movement and rendering
      invaderShootPewPew(ren, tex, &invaderProjectile[i]);

      // Checks if the projectile has passed the screen and destroys it
      if(invaderProjectile[i].y > bottomLine.y-invaderProjectile[i].h)
        invaderProjectileActive[i] = 0;

      // Checks if the projectile hits the player and the player is not currently "dead" in an explosion sequence
      if(SDL_HasIntersection(&spaceShip, &invaderProjectile[i]) && !playerDead)
      {
        // Destroys the projectile, starts the player explosion sequence, decreases the amount of lives and plays the explosion sound
        invaderProjectileActive[i] = 0;
        playerDead = 1;
        --lives[0];
        livesTexture = textureFromText(ren, font, lives);
        playSound(explosion, 4, 0);
      }

      // Loops through the shields and checks if a projectile shot by an invader hits any one of 'em, if so then record the coordinates and check
      // if that part of the shield is not already destroyed. Continue to the shield destroyal stuff
      for(int s = 0; s < 4; ++s)
      {
        if(SDL_HasIntersection(&invaderProjectile[i], &shields[s]))
        {
          colX = (invaderProjectile[i].x - shields[s].x) / (shields[s].w/shieldSurface[s]->w);
          colY = (invaderProjectile[i].y - shields[s].y) / (shields[s].h/shieldSurface[s]->h);
          if(pixelActive(shieldSurface[s], colX, colY) == 0x0000FF00)
          {
            invaderProjectileActive[i] = 0;
            editPixel(shieldSurface[s], colX, colY, 1);
            shieldTexture[s] = SDL_CreateTextureFromSurface(ren, shieldSurface[s]);
          }
        }
      }
    }
  }

  // If alien has spawned, pass the data to the function that moves and renders the alien (ufo)
  if(alien.active)
  {
    wooAlien(ren, tex, &alien, direction, ufo_lowpitch);
  }

  // Check if all invaders have been killed and start a new game / level by re-initializing some of the variables that define
  // the start of a new game, e.g. invaders, game speed etc.
  if(invaderskilled == COLS*ROWS)
  {
    newstart = 1;
  }

  // Running the stuff that handles invader movement, drawing/rendering and spaceship rendering
  updateInvaders(invaders, &gameSpeed, &currentFrame, actInvaderInRow, level);
  drawInvaders(ren, tex, invaders, currentFrame);

  // Checks if player still has lives left
  if(playerDead != 2)
    drawSpaceShip(ren, tex, &spaceShip, &playerDead, lives, &gameover);

  // Draw a line to separate the info bar from the field
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, &infoLine);

  // Render bottom "separator"
  SDL_SetRenderDrawColor(ren, 35, 255, 0, 255);
  SDL_RenderFillRect(ren, &bottomLine);

  // Render score, lives and level
  SDL_RenderCopy(ren, scoreTexture, NULL, &scoreHolder);
  SDL_RenderCopy(ren, livesTexture, NULL, &livesHolder);
  SDL_RenderCopy(ren, levelTexture, NULL, &levelHolder);
  renderLives(ren, tex, lives);

  // Loop and run the background music based on the current gamespeed
  playMusic(gameSpeed, music);

  // Up until now everything was drawn behind the scenes.
  // This will show the new, red contents of the window.

  SDL_RenderPresent(ren);

  }

  // Set a new position for the "Press any key..." text to be under the game over text
  anykeyHolder.y = gameoverHolder.y+gameoverHolder.h;

  while(gameover)
  {
    SDL_Event event;

    // Start the timer and after 50 frames move on to clearing the screen and rendering the game over text
    if(loadNewScreen <= 50)
      ++loadNewScreen;

    if(loadNewScreen > 50)
    {
      while (SDL_PollEvent(&event))
      {
        // look for the x of the window being clicked and exit
        if (event.type == SDL_QUIT)
          gameover = 0;
        // check for a key down
        if (event.type == SDL_KEYDOWN)
        {
          gameover = 0;
        }
      }
      // Clear the screen and render the game over and press any key... texts
      SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
      SDL_RenderClear(ren);
      SDL_RenderCopy(ren, gameoverTexture, NULL, &gameoverHolder);
      SDL_RenderCopy(ren, anykeyTexture, NULL, &anykeyHolder);
    }

    SDL_RenderPresent(ren);
  }

  // After all else has been lost, i.e. when we finish/quit the game, free the music chunks, close the SDL_Mixer, SDL_ttf and SDL itself
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

void drawSpaceShip(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *spaceShip, int *playerDead, char *lives, int *gameover)
{
  // Initialize the sprite rect, the explosion timer and the explosion frame stuff
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
    // As lives is a char, the comparison happens in ASCII values, thus we'll be creating a variable that holds the ascii value of 1
    int life = '0';
    if(lives[0] > life)
    {
      *playerDead = 0;
      spaceShip->x = (WIDTH-SPRITEWIDTH)/2;
    }
    else
    {
      *playerDead = 2;
      *gameover = 1;
    }
  }

  SDL_RenderCopyEx(ren, tex, &sS_sprite, spaceShip, 0.0, NULL, flip);
}

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile)
{
  projectile->y -= PROJECTILESPEED;

  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, projectile);
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
        ++*currentFrame;

        if(*currentFrame%2 == 0)
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

void renderLives(SDL_Renderer *ren, SDL_Texture *tex, char *lives)
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
    livesTextHolder.x = 25 * ((i%49)+1)+((i%49)*15);
    SDL_RenderCopy(ren, tex, &livesSprite, &livesTextHolder);
  }
  SDL_SetTextureColorMod(tex, 255, 255, 255);
}

void playSound(Mix_Chunk *sound, int chanToPlay, int loops)
{

  // If channel to play the sound on is set to 0, we'll use -1 which checks the first available channel.
  // Should be avoided as it could affect the bg music or anything with a preset channel
    if(chanToPlay == 0)
    {
        chanToPlay = -1;
    }

    // Initializing the channel, the sound and play the as many times as defined in the loops variable passed to the function
    int channel;
    channel = Mix_PlayChannel(chanToPlay, sound, loops);
    if(channel == -1) {
        fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
    }

}
void playMusic(int gameSpeed, Mix_Chunk *music[4])
{
    static int currentSound = 3;
    static int musicFrame = 25;
    int musicSpeed;

    // Check the current gamepeed and adjust the "music speed" or rather the interval between the bg bass noises accordingly
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

    // Check if the channel 1 is active, i.e. the channel that's reserved for the bg music, if not continue to the "interval" calculations
    if(Mix_Playing(1) == 0)
    {
        if(musicFrame < musicSpeed)
        {
         ++musicFrame;
        }
        else
        {
            musicFrame = 1;
        }

        // Plays the next bg sound on every _musicSpeed_ frame
        if(musicFrame%musicSpeed == 0)
        {

            playSound(music[currentSound], 1, 0);
            if(currentSound < 3)
            {
                ++currentSound;
            }
            else
            {
                currentSound = 0;
            }

        }
    }

}

void wooAlien(SDL_Renderer *ren, SDL_Texture *tex, Invader *alien, int direction, Mix_Chunk *ufosound)
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

SDL_Texture *textureFromText(SDL_Renderer *ren, TTF_Font *font, char *textToRender)
{
  SDL_Texture *texture;
  SDL_Color fontColor = {255, 255, 255, 255};

  SDL_Surface *textureSurface = TTF_RenderText_Solid(font, textToRender, fontColor);

  texture = SDL_CreateTextureFromSurface(ren, textureSurface);
  SDL_FreeSurface(textureSurface);

  return texture;
}

void editPixel(SDL_Surface *shieldSurface, int x, int y, int PlorIn)
{
  Uint8 *index;
  Uint32 *colour;
  int randomPattern = rand()%3;
  index = (Uint8 *)shieldSurface->pixels;

  switch(PlorIn)
  {
    case 0:
    {
      for(int r = 0; r < 12; ++r)
      {
        if((y-r) >= 0)
        {
          for(int c = 0; c < 6; ++c)
          {
            if((x+(c-3)) >= 0 && (x+(c-3)) < shieldSurface->w)
            {
              if(explosionPattern[randomPattern][r][c])
              {
                colour = (Uint32 *)&index[(shieldSurface->pitch*(y-r) + shieldSurface->format->BytesPerPixel*(x+(c-3)))];
                *colour = 0x00000000;
              }
            }
            else
              continue;
          }
        }
        else
        {
          break;
        }
      }
      break;
    }
    case 1:
    {
      for(int r = 0; r < 12; ++r)
      {
        if((y+r) < shieldSurface->h)
        {
          for(int c = 0; c < 6; ++c)
          {
            if((x+(c-3)) >= 0 && (x+(c-3)) < shieldSurface->w)
            {
              if(explosionPattern[randomPattern][r][c])
              {
                colour = (Uint32 *)&index[(shieldSurface->pitch*(y+r) + shieldSurface->format->BytesPerPixel*(x+(c-3)))];
                *colour = 0x00000000;
              }
            }
            else
              continue;
          }
        }
        else
        {
          break;
        }
      }
    break;
    }
  }

}

Uint32 pixelActive(SDL_Surface *shieldSurface, int x, int y)
{
  Uint8 *index;
  Uint32 *value;
  index = (Uint8 *)shieldSurface->pixels;
  value = (Uint32 *)&index[(shieldSurface->pitch*y + shieldSurface->format->BytesPerPixel*x)];
  return *value;
}
