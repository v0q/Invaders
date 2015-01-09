/*
 Copyright © 2015 Teemu Lindborg
*/

#include "Globals.h"
#include "Invader.h"
#include "Defender.h"
#include "Sounds.h"
#include "Shields.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file main.c
/// @brief Main program
/// \todo Refactoring of the file as the main function is much too big. Find a way proper and tidy way
///       to initialise all the required variables without winding up with functions that take an extensive
///       amount of parameters (initFirstScreen, initialiseScreenStuff)
// -----------------------------------------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------------------------------
/// @brief Initialises the positions and dimensions of some of the stuff that'll be rendered in the game
/// @param[in] infoLine The line that separates the top data from the game, used as the top boundary
/// @param[in] bottomLine Bottom boundary, separates the players' lives from the game
/// @param[in] screen The actual screen dimension
/// @param[in] scoreHolder The rect that holds the score texture
/// @param[in] highscoreHolder The rect that holds the high score texture
/// @param[in] livesHolder The rect that holds the lives
/// @param[in] levelHolder The rect that holds the level
/// @param[in] p1keysHolder The rect that holds the player 1 key instructions
/// @param[in] p2keysHolder The rect that holds the player 2 key instructions
// -----------------------------------------------------------------------------------------------------------------------
void initialiseScreenStuff( SDL_Rect *infoLine, SDL_Rect *bottomLine, SDL_Rect *screen, SDL_Rect *scoreHolder,
                            SDL_Rect *highscoreHolder, SDL_Rect livesHolder[2], SDL_Rect *levelHolder,
                            SDL_Rect *p1keysHolder, SDL_Rect *p2keysHolder);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Initialises SDL, SDL_TTF and SDL_Mixer
/// @return 0 or EXIT_FAILURE on failure
// -----------------------------------------------------------------------------------------------------------------------
int initialiseSDL();

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Initialises the window, renderer and fonts
/// @param[out] win To which the window data will be assigned to
/// @param[out] ren To which the renderer will be assigned to
/// @param[out] font To which the normal font will be assigned to
/// @param[out] bigfont To which the bigger font will be assigned to
/// @return 0 or EXIT_FAILURE on failure
// -----------------------------------------------------------------------------------------------------------------------
int initialiseRenWinFonts(SDL_Window **win, SDL_Renderer **ren, TTF_Font **font, TTF_Font **bigfont);

void explodeProjectile(SDL_Renderer *ren, SDL_Rect *projectileBoom, SDL_Texture *tex, int *explodeP);

SDL_Texture *textureFromText(SDL_Renderer *ren, TTF_Font *font, char *textToRender);
SDL_Texture *loadTexture(SDL_Renderer *ren, char *sprite);

void initFirstScreen(SDL_Rect *mainTextHolder, SDL_Rect *scoreTableHolder, SDL_Rect *alienPtsHolder,
                     SDL_Rect *invT3Holder, SDL_Rect *invT2Holder, SDL_Rect *invT1Holder, SDL_Rect invader[4],
                     SDL_Rect invaderSrcR[4], SDL_Rect *oneplayerHolder, SDL_Rect *twoplayersHolder, SDL_Rect *selectRect,
                     SDL_Rect *p1keysHolder, SDL_Rect *p2keysHolder, SDL_Rect *gameoverHolder, SDL_Rect *anykeyHolder,
                     SDL_Rect *scoreHolder, SDL_Rect *highscoreHolder, SDL_Rect *levelHolder,

                     SDL_Texture *mainTextTexture, SDL_Texture *scoreTableTexture, SDL_Texture *alienPtsTexture,
                     SDL_Texture *invT3Texture, SDL_Texture *invT2Texture, SDL_Texture *invT1Texture,
                     SDL_Texture *onePTexture, SDL_Texture *twoPtexture, SDL_Texture *gameoverTexture, SDL_Texture *anykeyTexture,
                     SDL_Texture *scoreTexture, SDL_Texture *highscoreTexture, SDL_Texture *levelTexture,

                     int currentSelectionCoords[2]);

int main()
{

// -----------------------------------------------------------
// --------------- INITIALIZING STUFF START ------------------
// -----------------------------------------------------------
  Invader invaders[ROWS][COLS], alien;
  SDL_Rect spaceShip[2], projectile[2], projectileBoom[2], shields[4], invaderProjectile[COLS];
  // "Screen" related stuff
  SDL_Rect infoLine, bottomLine, screen, scoreHolder, highscoreHolder, livesHolder[2], levelHolder, p1keysHolder, p2keysHolder;

  FILE *hsFile;

  SDL_Window *win = 0;
  SDL_Renderer *ren = 0;
  TTF_Font *font = NULL, *bigfont = NULL;
  Mix_Chunk *music[4], *shoot, *invaderkilled, *ufo_lowpitch, *explosion_sound;

  // Create a variable that records the key presses (continuous)
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  // Some variables used all around
  int gameSpeed = 1;
  int projectileActive[2] = {0};
  int alienDirection = 0;
  int explodeP[2] = {0};
  int currentFrame = 0;
  int quit=0;
  int gameover = 0;
  int loadNewScreen = 1;
  int playerDead[2] = {0};
  int colX = 0;
  int colY = 0;
  int newstart = 0;
  int fresh = 0;
  int invaderskilled = 0;

  int moveOn = 0;
  int currentSelectionCoords[2];
  int selection = 0;

  int actInvaderInRow[COLS], invaderProjectileActive[COLS];

  initialiseInvaders(invaders, &alien, invaderProjectile, actInvaderInRow, invaderProjectileActive);
  initialiseDefender(spaceShip, projectile, projectileBoom);
  initialiseShields(shields);
  initialiseScreenStuff(&infoLine, &bottomLine, &screen, &scoreHolder, &highscoreHolder, livesHolder, &levelHolder, &p1keysHolder, &p2keysHolder);

  initialiseSDL();
  initialiseRenWinFonts(&win, &ren, &font, &bigfont);
  loadSounds(music, &shoot, &invaderkilled, &ufo_lowpitch, &explosion_sound);

  SDL_Texture *tex, *p1ktex, *p2ktex;
  tex = loadTexture(ren, "sprites");
  p1ktex = loadTexture(ren, "p1keys");
  p2ktex = loadTexture(ren, "p2keys");

  SDL_Surface *shieldSurface[4];
  SDL_Texture *shieldTexture[4];
  loadShields(ren, shieldSurface, shieldTexture);

  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);


  // ---------------------------------------------------------
  // --------------- INIT TEXTURES ETC START -----------------
  // ---------------------------------------------------------

    // Here we're just initializing the "variables" to be used for the start menus etc. (Game Over and Press any key... stuff also initialised and declared here)
    SDL_Rect mainTextHolder, scoreTableHolder, alienPtsHolder, invT3Holder, invT2Holder, invT1Holder,
             invader[4], invaderSrcR[4], oneplayerHolder, twoplayersHolder, selectRect, gameoverHolder, anykeyHolder;
    SDL_Texture *mainTextTexture, *scoreTableTexture, *alienPtsTexture, *invT3Texture, *invT2Texture, *invT1Texture,
                *onePTexture, *twoPtexture, *gameoverTexture, *anykeyTexture, *scoreTexture, *highscoreTexture, *livesTexture[2], *levelTexture;

    // Strings that will be used to create the corresponding textures
    char mainText[] = "Space Invaders";
    char scoreTable[] = "*Score Advance Table*";
    char alienPts[] = "=? Mystery";
    char invT3[] = "=30 Points";
    char invT2[] = "=20 Points";
    char invT1[] = "=10 Points";
    char oneplayer[] = "1  player";
    char twoplayers[] = "2  players";
    char thegameover[] = "Game Over";
    char pressanykey[] = "Press any key to continue";
    int level = 1;
    int score = 0;

    // Game related strings like scores, level and lives
    // Sprintf is used to combine the strings and integers
    char thelevel[9] = {0};
    sprintf(thelevel, "Level: %i", level);
    char thescore[12] = {0};
    sprintf(thescore, "Score: %04i", score);
    char lives[2][2] = {"3", "3"};

    // We're using a simple "encryption" to encrypt the highscores so that editing 'em manually takes a little effort
    // We'll be using encryptkey as a bitwise XOR to "scramble" the highscore a little, also the highscore is stored as a hexadecimal to
    // make it just a tad bit harder to read
    int encryptkey = 123456789;
    char enchighscore[8] = {0};
    int highscore;
    // Opening the file where the highscore is stored, note that as the file is ".xxx" it's handled as a sort-of a system file
    // thus hidden in most machines, this is again done to make it a bit harder to manually edit the high scores.
    hsFile = fopen(".highscore", "r");
    fscanf(hsFile, "%s", enchighscore);
    fclose(hsFile);

    // Here we'll turn the "encrypted" highscore back to a readable integer by reverting it back with yet another XOR using the encryptkey
    highscore = (int)(strtol(enchighscore, NULL, 16)^encryptkey);
    char thehighscore[17] = {0};
    sprintf(thehighscore, "High score: %04i", highscore);

    // Next we're using the function textureFromText() to create textures from the strings declared above
    // Apparently SDL doesn't like to pass the SDL_Textures around, thus we're "forced" to assign/declare the textures
    // inside the function we'll be using them in.
    mainTextTexture = textureFromText(ren, bigfont, mainText);
    scoreTableTexture = textureFromText(ren, font, scoreTable);
    alienPtsTexture = textureFromText(ren, font, alienPts);
    invT3Texture = textureFromText(ren, font, invT3);
    invT2Texture = textureFromText(ren, font, invT2);
    invT1Texture = textureFromText(ren, font, invT1);
    onePTexture = textureFromText(ren, font, oneplayer);
    twoPtexture = textureFromText(ren, font, twoplayers);
    gameoverTexture = textureFromText(ren, bigfont, thegameover);
    anykeyTexture = textureFromText(ren, font, pressanykey);
    scoreTexture = textureFromText(ren, font, thescore);
    levelTexture = textureFromText(ren, font, thelevel);
    highscoreTexture = textureFromText(ren, font, thehighscore);

    // The texture for the lives will be created here (also the position for the holder)
    // as passing textures around and assigning 'em elsewhere has proven tricky so well loop trough the textures
    // and texture holder for both players
    for(int i = 0; i < 2; ++i)
    {
      livesTexture[i] = textureFromText(ren, font, lives[i]);
      SDL_QueryTexture(livesTexture[i], NULL, NULL, &livesHolder[i].w, &livesHolder[i].h);
    }

    // Here we'll pass all the texture holders and the textures to a function that will
    // get the texture dimensions and adjust/assign the positions and dimensions for the texture holders accordingly
    initFirstScreen(&mainTextHolder, &scoreTableHolder, &alienPtsHolder, &invT3Holder, &invT2Holder, &invT1Holder, invader, invaderSrcR,
                    &oneplayerHolder, &twoplayersHolder, &selectRect, &p1keysHolder, &p2keysHolder, &gameoverHolder, &anykeyHolder,
                    &scoreHolder, &highscoreHolder, &levelHolder,
                    mainTextTexture, scoreTableTexture, alienPtsTexture, invT3Texture, invT2Texture, invT1Texture, onePTexture, twoPtexture,
                    gameoverTexture, anykeyTexture, scoreTexture, highscoreTexture, levelTexture,
                    currentSelectionCoords);

  // ---------------------------------------------------------
  // --------------- INIT TEXTURES ETC END -------------------
  // ---------------------------------------------------------


// ---------------------------------------------------------
// --------------- INITIALIZING STUFF END ------------------
// ---------------------------------------------------------

  int startgame = 0;

  while(!startgame)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        quit = 1;
        startgame = 1;
      }
      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_ESCAPE: quit = 1; startgame = 1; break;
          case SDLK_RETURN:
          {
            if(moveOn)
            {
              if(!selection)
              {
                startgame = 1;
                spaceShip[0].x = (WIDTH-SPRITEWIDTH)/2;
                livesHolder[0].x = 5;
              }
              else
              {
                startgame = 1;
                spaceShip[0].x = 3*(WIDTH-SPRITEWIDTH)/4;
                spaceShip[1].x = (WIDTH-SPRITEWIDTH)/4;
                livesHolder[0].x = WIDTH-5-livesHolder[0].w;
                livesHolder[1].x = 5;
              }
            }
            else
            {
            moveOn = 1;
            }
            break;
          }
          case SDLK_RIGHT: case SDLK_LEFT:
          {
            if(moveOn)
            {
              if(!selection)
                ++selection;
              else
                selection = 0;

              selectRect.x = currentSelectionCoords[selection];
            }
            break;
          }
        }
      }
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, mainTextTexture, NULL, &mainTextHolder);
    if(!moveOn)
    {
      SDL_RenderCopy(ren, scoreTableTexture, NULL, &scoreTableHolder);
      SDL_RenderCopy(ren, alienPtsTexture, NULL, &alienPtsHolder);
      SDL_RenderCopy(ren, invT3Texture, NULL, &invT3Holder);
      SDL_RenderCopy(ren, invT2Texture, NULL, &invT2Holder);
      SDL_RenderCopy(ren, invT1Texture, NULL, &invT1Holder);
      for(int i = 0; i < 4; ++i)
      {
        SDL_RenderCopy(ren, tex, &invaderSrcR[i], &invader[i]);
      }
      //SDL_RenderCopy(ren, anykeyTexture, NULL, &anykeyHolder);
    }
    else
    {
      SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
      SDL_RenderFillRect(ren, &selectRect);

      if(selection)
      {
        p1keysHolder.x = WIDTH/2 - p1keysHolder.w - 50;
        SDL_RenderCopy(ren, p2ktex, NULL, &p2keysHolder);
      }
      else
      {
        p1keysHolder.x = (WIDTH-p1keysHolder.w)/2;
      }

      SDL_RenderCopy(ren, p1ktex, NULL, &p1keysHolder);
      SDL_RenderCopy(ren, onePTexture, NULL, &oneplayerHolder);
      SDL_RenderCopy(ren, twoPtexture, NULL, &twoplayersHolder);
    }
    SDL_RenderPresent(ren);
  }



  // now we are going to loop forever, process the keys then draw

  while (quit !=1)
  {

    if(newstart)
    {
      initialiseInvaders(invaders, &alien, invaderProjectile, actInvaderInRow, invaderProjectileActive);
      gameSpeed = 1;
      currentFrame = 0;
      //playerDead[0] = 0;
      //playerDead[1] = 0;
      invaderskilled = 0;
      ++level;

      if(fresh)
      {
        score = 0;
        level = 1;
        spaceShip[0].x = 3*(WIDTH-SPRITEWIDTH)/4;
        spaceShip[1].x = (WIDTH-SPRITEWIDTH)/4;
        for(int i = 0; i < selection+1; ++i)
        {
          lives[i][0] = '3';
          livesTexture[i] = textureFromText(ren, font, lives[i]);
          playerDead[i] = 0;
        }

        loadShields(ren, shieldSurface, shieldTexture);

        score = 0;
        sprintf(thescore, "Score: %04i", score);
        scoreTexture = textureFromText(ren, font, thescore);

        hsFile = fopen(".highscore", "r");
        fscanf(hsFile, "%s", enchighscore);
        fclose(hsFile);

        highscore = (int)(strtol(enchighscore, NULL, 16)^encryptkey);

        sprintf(thehighscore, "High score: %04i", highscore);
        highscoreTexture = textureFromText(ren, font, thehighscore);
        fresh = 0;
      }

      sprintf(thelevel, "Level: %i", level);
      levelTexture = textureFromText(ren, font, thelevel);
      newstart = 0;
    }


    // Checking if LEFT or RIGHT key (optionally A or D) is pressed and running the moveSpaceShip function accordingly.
    // Using the SDL_GetKeyboardState instead so the movement doesn't stop when pressing another key.
    if(keystate[SDL_SCANCODE_LEFT] && !playerDead[0])
    {
      moveSpaceShip(&spaceShip[0], LEFT);
    }
    if(keystate[SDL_SCANCODE_RIGHT] && !playerDead[0])
    {
      moveSpaceShip(&spaceShip[0], RIGHT);
    }
    if(keystate[SDL_SCANCODE_A] && !playerDead[1] && selection)
    {
      moveSpaceShip(&spaceShip[1], LEFT);
    }
    if(keystate[SDL_SCANCODE_D] && !playerDead[1] && selection)
    {
      moveSpaceShip(&spaceShip[1], RIGHT);
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
          if(!projectileActive[0] && !playerDead[0])
          {
            // If no projectile exists, registers the current x and y position of the spaceship,
            // to get the coordinates from where the projectile should launch
            projectile[0].x = spaceShip[0].x+(SPRITEWIDTH/2-1);
            projectile[0].y = spaceShip[0].y;

            // Now that the projectile is about to shoot, change the projectileActive to true to prevent more projectiles to be shot
            // at the same time
            projectileActive[0] = 1;
            playSound(shoot, 3, 0);
          }
          break;
        }
        case SDLK_LSHIFT :
        {
          // Checking if a projectile already exists (is shot and still flying) as we restrict the amount of simultanious projectiles to one.
          if(!projectileActive[1] && !playerDead[1] && selection)
          {
            // If no projectile exists, registers the current x and y position of the spaceship,
            // to get the coordinates from where the projectile should launch
            projectile[1].x = spaceShip[1].x+(SPRITEWIDTH/2-1);
            projectile[1].y = spaceShip[1].y;

            // Now that the projectile is about to shoot, change the projectileActive to true to prevent more projectiles to be shot
            // at the same time
            projectileActive[1] = 1;
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
  for(int p = 0; p < selection+1; ++p)
  {
    if(projectileActive[p])
    {
      // Check the projectile reaches the top of the screen and destroy it
      if(projectile[p].y+projectile[p].h <= infoLine.y+infoLine.h+PROJECTILESPEED)
      {
        // Assign some variables needed for the "explosion" of the projectile
        // As the width of the explosion if different from the projectiles width, we're gonna move the x coordinate of the explosion
        // half the sprites width to left
        projectileBoom[p].x = projectile[p].x - SPRITEWIDTH/2;

        // Activate the variable determining that an explosion should happen
        explodeP[p] = 1;
        projectileActive[p] = 0;
      }

      // Pass the stuff needed to display and move the projectile to the function
      shootPewPew(ren, &projectile[p], level);

      if(SDL_HasIntersection(&alien.pos, &projectile[p]) && alien.active)
      {
        // If the projectile hits a target, destroy the projectile and change the frame of that specific invader
        // to 3 which equals to the explosion "animation", the actual destroyal of the object happens later when the explosion is complete.
        alien.frame = 1;
        Mix_Pause(4);
        projectileActive[p] = 0;
        score += 100;
        sprintf(thescore, "Score: %04i", score);
        if(lives[p][0] < '9')
        {
          ++lives[p][0];
          livesTexture[p] = textureFromText(ren, font, lives[p]);
        }
        else
          score += 500;
        SDL_QueryTexture(livesTexture[p], NULL, NULL, &livesHolder[p].w, &livesHolder[p].h);
        scoreTexture = textureFromText(ren, font, thescore);
        playSound(invaderkilled, 2, 0);
      }

      // Checks if a shield has been shot
      for(int i = 0; i < 4; ++i)
      {
        // Compensating for the projectile speed, i.e. the "skips" that it does by moving so fast
        for(int pSC = projectile[p].h; pSC < PROJECTILESPEED+projectile[p].h; ++pSC)
        {
          // Using this compensated "projectile" that's not rendered as the colliding one and store the coordinates in a separate "collision rect"
          SDL_Rect collision;
          SDL_Rect pSCprojectile = {projectile[p].x, projectile[p].y+pSC, projectile[p].w, projectile[p].h};
          if(SDL_HasIntersection(&pSCprojectile, &shields[i]))
          {
            // Store the coordinates here and pass on to the function that handles the destruction of the shield, also checks if the part we hit is not destroyed
            SDL_IntersectRect(&pSCprojectile, &shields[i], &collision);
            colX = (collision.x - shields[i].x) / (shields[i].w/shieldSurface[i]->w);
            colY = (collision.y - shields[i].y) / (shields[i].h/shieldSurface[i]->h);
            if(pixelActive(shieldSurface[i], colX, colY) == 0x0000FF00)
            {
              projectileActive[p] = 0;
              editPixel(shieldSurface[i], colX, colY, 0);
              shieldTexture[i] = SDL_CreateTextureFromSurface(ren, shieldSurface[i]);
            }
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
      for(int p = 0; p < selection+1; ++p)
      {
        if(projectileActive[p])
        {
          // Check if the projectile collides with any of 'em
          if(SDL_HasIntersection(&invaders[r][c].pos, &projectile[p]) && invaders[r][c].active)
          {
            // If the projectile hits a target, destroy the projectile and change the frame of that specific invader
            // to 3 which equals to the explosion "animation", the actual destroyal of the object happens later when the explosion is complete.
            projectileActive[p] = 0;
            invaders[r][c].frame = 3;
            score += 10*(invaders[r][c].type+1);
            sprintf(thescore, "Score: %04i", score);
            scoreTexture = textureFromText(ren, font, thescore);
            playSound(invaderkilled, 2, 0);
            ++invaderskilled;
          }
        }

        // Check if an invader hits the player or reaches the ground => game over
        if((SDL_HasIntersection(&invaders[r][c].pos, &spaceShip[p]) && invaders[r][c].active) ||
           (invaders[r][c].pos.y >= bottomLine.y-SPRITEHEIGHT && invaders[r][c].active))
        {
          gameover = 1;
          if(highscore < score)
          {
            sprintf(enchighscore, "%x", score^encryptkey);
            hsFile = fopen(".highscore", "w+");
            fprintf(hsFile, "%s", enchighscore);
            fclose(hsFile);
          }
        }
      }
    }
  }

  // Checks if an explosion sequence should be run
  for(int p = 0; p < selection+1; ++p)
  {
    if(explodeP[p])
    {
      // If yes, pass the needed stuff to a function handling the explosion
      explodeProjectile(ren, &projectileBoom[p], tex, &explodeP[p]);
    }
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
        alienDirection = 0;
    }
    else
    {
        alien.pos.x = WIDTH + SPRITEWIDTH;
        alienDirection = 1;
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

      for(int p = 0; p < selection+1; ++p)
      {
        // Checks if the projectile hits the player and the player is not currently "dead" in an explosion sequence
        if(SDL_HasIntersection(&spaceShip[p], &invaderProjectile[i]) && !playerDead[p])
        {
          // Destroys the projectile, starts the player explosion sequence, decreases the amount of lives and plays the explosion sound
          invaderProjectileActive[i] = 0;
          playerDead[p] = 1;
          --lives[p][0];
          livesTexture[p] = textureFromText(ren, font, lives[p]);
          if(lives[0][0] == '0')
          {
            if(selection)
            {
              if(lives[1][0] == '0')
              {
                if(highscore < score)
                {
                  sprintf(enchighscore, "%x", score^encryptkey);
                  hsFile = fopen(".highscore", "w+");
                  fprintf(hsFile, "%s", enchighscore);
                  fclose(hsFile);
                }
              }
            }
            else
            {
              if(highscore < score)
              {
                sprintf(enchighscore, "%x", score^encryptkey);
                hsFile = fopen(".highscore", "w+");
                fprintf(hsFile, "%s", enchighscore);
                fclose(hsFile);
              }
            }
          }
          playSound(explosion_sound, 4, 0);
        }
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
    moveAlien(ren, tex, &alien, alienDirection, ufo_lowpitch);
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

  // Checks if players still have lives left
  if(playerDead[0] != 2)
    drawSpaceShip(ren, tex, &spaceShip[0], &playerDead[0], lives[0], 0);
  if(playerDead[1] != 2 && selection)
    drawSpaceShip(ren, tex, &spaceShip[1], &playerDead[1], lives[1], 1);

  if(playerDead[0] == 2)
  {
    if(selection)
    {
      if(playerDead[1] == 2)
        gameover = 1;
    }
    else
    {
      gameover = 1;
    }
  }

  //drawSpaceShip(ren, tex, &spaceShip2, &playerDead, lives, &gameover);

  // Draw a line to separate the info bar from the field
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, &infoLine);

  // Render bottom "separator"
  SDL_SetRenderDrawColor(ren, 35, 255, 0, 255);
  SDL_RenderFillRect(ren, &bottomLine);

  // Render score, lives and level
  SDL_RenderCopy(ren, scoreTexture, NULL, &scoreHolder);
  SDL_RenderCopy(ren, highscoreTexture, NULL, &highscoreHolder);
  SDL_RenderCopy(ren, levelTexture, NULL, &levelHolder);

  for(int i = 0; i < selection+1; ++i)
  {
    SDL_RenderCopy(ren, livesTexture[i], NULL, &livesHolder[i]);
    renderLives(ren, tex, lives[i], i, selection);
  }

  // Loop and run the background music based on the current gamespeed
  playMusic(gameSpeed, music);

  // Up until now everything was drawn behind the scenes.
  // This will show the new, red contents of the window.

  SDL_RenderPresent(ren);

  while(gameover)
  {
    // Set a new position for the "Press any key..." text to be under the game over text
    anykeyHolder.y = gameoverHolder.y+gameoverHolder.h;

    // Start the timer and after 50 frames move on to clearing the screen and rendering the game over text
    if(loadNewScreen <= 150)
      ++loadNewScreen;

    if(loadNewScreen > 50)
    {
      while (SDL_PollEvent(&event))
      {
        // look for the x of the window being clicked and exit
        if (event.type == SDL_QUIT)
          quit = 1;
        // check for a key down
        if (event.type == SDL_KEYDOWN)
        {
          switch(event.key.keysym.sym)
          {
            case SDLK_ESCAPE: quit = 1; break;
            default:
            {
              if(loadNewScreen >= 150)
              {
                newstart = 1;
                fresh = 1;
              }
            }
          }
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


  }

  // After all else has been lost, i.e. when we finish/quit the game, free the music chunks, close the SDL_Mixer, SDL_ttf and SDL itself

  freeSounds(music, &shoot, &invaderkilled, &ufo_lowpitch, &explosion_sound);
  Mix_CloseAudio();
  TTF_Quit();
  SDL_Quit();
  return 0;
}

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

SDL_Texture *textureFromText(SDL_Renderer *ren, TTF_Font *font, char *textToRender)
{
  SDL_Texture *texture;
  SDL_Color fontColor = {255, 255, 255, 255};

  SDL_Surface *textureSurface = TTF_RenderText_Solid(font, textToRender, fontColor);

  texture = SDL_CreateTextureFromSurface(ren, textureSurface);
  SDL_FreeSurface(textureSurface);

  return texture;
}

SDL_Texture *loadTexture(SDL_Renderer *ren, char *sprite)
{
  SDL_Surface *tmpSurface;
  SDL_Texture *tmpTexture;
  char *baseText = "sprites/%s.png";
  char s[strlen(sprite) + strlen(baseText)];
  sprintf(s, baseText, sprite);
  tmpSurface = IMG_Load(s);

  if(!tmpSurface)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return NULL;
  }

  tmpTexture = SDL_CreateTextureFromSurface(ren, tmpSurface);
  SDL_FreeSurface(tmpSurface);

  return tmpTexture;
}

void initFirstScreen(SDL_Rect *mainTextHolder, SDL_Rect *scoreTableHolder, SDL_Rect *alienPtsHolder,
                     SDL_Rect *invT3Holder, SDL_Rect *invT2Holder, SDL_Rect *invT1Holder, SDL_Rect invader[4],
                     SDL_Rect invaderSrcR[4], SDL_Rect *oneplayerHolder, SDL_Rect *twoplayersHolder, SDL_Rect *selectRect,
                     SDL_Rect *p1keysHolder, SDL_Rect *p2keysHolder, SDL_Rect *gameoverHolder, SDL_Rect *anykeyHolder,
                     SDL_Rect *scoreHolder, SDL_Rect *highscoreHolder, SDL_Rect *levelHolder,

                     SDL_Texture *mainTextTexture, SDL_Texture *scoreTableTexture, SDL_Texture *alienPtsTexture,
                     SDL_Texture *invT3Texture, SDL_Texture *invT2Texture, SDL_Texture *invT1Texture,
                     SDL_Texture *onePTexture, SDL_Texture *twoPtexture, SDL_Texture *gameoverTexture, SDL_Texture *anykeyTexture,
                     SDL_Texture *scoreTexture, SDL_Texture *highscoreTexture, SDL_Texture *levelTexture,

                     int currentSelectionCoords[2])
{

  selectRect->h = 10;
  selectRect->w = 10;

  invaderSrcR[2].x = 292;
  invaderSrcR[2].y = 0;
  invaderSrcR[2].w = 80;
  invaderSrcR[2].h = 80;

  invaderSrcR[1].x = 145;
  invaderSrcR[1].y = 0;
  invaderSrcR[1].w = 111;
  invaderSrcR[1].h = 80;

  invaderSrcR[0].x = 140;
  invaderSrcR[0].y = 120;
  invaderSrcR[0].w = 120;
  invaderSrcR[0].h = 80;

  invaderSrcR[3].x = 0;
  invaderSrcR[3].y = 616;
  invaderSrcR[3].w = 125;
  invaderSrcR[3].h = 61;

  SDL_QueryTexture(mainTextTexture, NULL, NULL, &mainTextHolder->w, &mainTextHolder->h);
  SDL_QueryTexture(scoreTableTexture, NULL, NULL, &scoreTableHolder->w, &scoreTableHolder->h);
  SDL_QueryTexture(alienPtsTexture, NULL, NULL, &alienPtsHolder->w, &alienPtsHolder->h);
  SDL_QueryTexture(invT3Texture, NULL, NULL, &invT3Holder->w, &invT3Holder->h);
  SDL_QueryTexture(invT2Texture, NULL, NULL, &invT2Holder->w, &invT2Holder->h);
  SDL_QueryTexture(invT1Texture, NULL, NULL, &invT1Holder->w, &invT1Holder->h);
  SDL_QueryTexture(onePTexture, NULL, NULL, &oneplayerHolder->w, &oneplayerHolder->h);
  SDL_QueryTexture(twoPtexture, NULL, NULL, &twoplayersHolder->w, &twoplayersHolder->h);
  SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreHolder->w, &scoreHolder->h);
  SDL_QueryTexture(highscoreTexture, NULL, NULL, &highscoreHolder->w, &highscoreHolder->h);
  SDL_QueryTexture(levelTexture, NULL, NULL, &levelHolder->w, &levelHolder->h);
  SDL_QueryTexture(gameoverTexture, NULL, NULL, &gameoverHolder->w, &gameoverHolder->h);
  SDL_QueryTexture(anykeyTexture, NULL, NULL, &anykeyHolder->w, &anykeyHolder->h);

  mainTextHolder->x = (WIDTH-mainTextHolder->w)/2;
  mainTextHolder->y = HEIGHT/4;

  scoreTableHolder->x = (WIDTH-scoreTableHolder->w)/2;
  oneplayerHolder->y = twoplayersHolder->y = scoreTableHolder->y = mainTextHolder->y + mainTextHolder->h + 75;

  alienPtsHolder->x = (WIDTH-alienPtsHolder->w)/2;
  invader[3].y = alienPtsHolder->y = scoreTableHolder->y + scoreTableHolder->h + 5;

  invT3Holder->x = (WIDTH-invT3Holder->w)/2;
  invader[2].y = invT3Holder->y = alienPtsHolder->y + alienPtsHolder->h + 5;

  invT2Holder->x = (WIDTH-invT2Holder->w)/2;
  invader[1].y = invT2Holder->y = invT3Holder->y + invT3Holder->h + 5;

  invT1Holder->x = (WIDTH-invT1Holder->w)/2;
  invader[0].y = invT1Holder->y = invT2Holder->y + invT2Holder->h + 5;

  oneplayerHolder->x = WIDTH/2 - oneplayerHolder->w - oneplayerHolder->w/2;
  twoplayersHolder->x = WIDTH/2 + oneplayerHolder->w/2;

  currentSelectionCoords[0] = oneplayerHolder->x - selectRect->w*2;
  currentSelectionCoords[1] = twoplayersHolder->x - selectRect->w*2;

  selectRect->x = currentSelectionCoords[0];
  selectRect->y = oneplayerHolder->y + oneplayerHolder->h/2 - 2*selectRect->h/3;

  p1keysHolder->y = p2keysHolder->y = oneplayerHolder->y + 75;
  p1keysHolder->x = WIDTH/2 - p1keysHolder->w - 50;
  p2keysHolder->x = WIDTH/2 + 50;

  for(int i = 0; i < 3; ++i)
  {
    invader[i].w = invT1Holder->h*(SPRITEWIDTH/SPRITEHEIGHT);
    invader[i].h = invT1Holder->h;
  }

  invader[3].w = 3*invT1Holder->h*(invaderSrcR[3].w/invaderSrcR[3].h)/4;
  invader[3].h = 3*invT1Holder->h/4;

  invader[0].x = invT1Holder->x - invader[0].w - 5;
  invader[1].x = invT1Holder->x - invader[1].w - 5;
  invader[2].x = invT1Holder->x - invader[2].w - 5;
  invader[3].x = alienPtsHolder->x - invader[3].w - 5;

  levelHolder->x = WIDTH-5-levelHolder->w;
  highscoreHolder->x = (WIDTH-highscoreHolder->w)/2;

  // Setting the physical position for the holders
  gameoverHolder->x = (WIDTH-gameoverHolder->w)/2;
  gameoverHolder->y = (HEIGHT-gameoverHolder->h)/2;

  anykeyHolder->x = (WIDTH-anykeyHolder->w)/2;
  anykeyHolder->y = (HEIGHT-gameoverHolder->h)/2;

}

void initialiseScreenStuff( SDL_Rect *infoLine, SDL_Rect *bottomLine, SDL_Rect *screen, SDL_Rect *scoreHolder,
                            SDL_Rect *highscoreHolder, SDL_Rect livesHolder[2], SDL_Rect *levelHolder,
                            SDL_Rect *p1keysHolder, SDL_Rect *p2keysHolder)
{
  infoLine->x = 0;
  infoLine->y = INFOBOXHEIGHT;
  infoLine->w = WIDTH;
  infoLine->h = 5;

  bottomLine->x = 0;
  bottomLine->y = HEIGHT-30;
  bottomLine->w = WIDTH;
  bottomLine->h = 3;

  screen->x = 0;
  screen->y = 0;
  screen->w = WIDTH;
  screen->h = HEIGHT;

  scoreHolder->x = 5;
  scoreHolder->y = INFOBOXHEIGHT/2 - FONTSIZE/2;
  scoreHolder->w = 0;
  scoreHolder->h = 0;

  highscoreHolder->x = 0;
  highscoreHolder->y = INFOBOXHEIGHT/2 - FONTSIZE/2;
  highscoreHolder->w = 0;
  highscoreHolder->h = 0;

  for(int i = 0; i < 2; ++i)
  {
    livesHolder[i].w = 0;
    livesHolder[i].h = 0;
    livesHolder[i].y = bottomLine->y + FONTSIZE/2;
  }

  levelHolder->y = INFOBOXHEIGHT/2 - FONTSIZE/2;
  levelHolder->w = 0;
  levelHolder->h = 0;

  p1keysHolder->w = 2*390/3;
  p1keysHolder->h = 2*87/3;

  p2keysHolder->w = 2*226/3;
  p2keysHolder->h = 2*137/3;
}

int initialiseSDL()
{
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 2;
  int audio_buffers = 4096;

  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }

  if(TTF_Init() == -1)
  {
    printf("%s\n", TTF_GetError());
    return EXIT_FAILURE;
  }

  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
      fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
      exit(1);
  }

  return 0;
}

int initialiseRenWinFonts(SDL_Window **win, SDL_Renderer **ren, TTF_Font **font, TTF_Font **bigfont)
{
  *win = SDL_CreateWindow("Invaders", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (*win == 0)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }
  *ren = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (*ren == 0)
  {
    printf("%s\n",SDL_GetError() );
    return EXIT_FAILURE;
  }

  *font = TTF_OpenFont("fonts/space_invaders.ttf", FONTSIZE);
  *bigfont = TTF_OpenFont("fonts/space_invaders.ttf", FONTSIZE*4);

  if(*font == NULL || *bigfont == NULL)
  {
    printf("BOOM FAILED %s\n", TTF_GetError());
    return EXIT_FAILURE;
  }

  return 0;
}
