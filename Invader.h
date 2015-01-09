/*
 Copyright © 2015 Teemu Lindborg
*/

#ifndef INVADER_H
#define INVADER_H

/// @file Invader.h
/// @brief Invader/alien initialisation and functionality
// Including the global definitions and includes for the use of the functions
#include "Globals.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Invader.h
/// @brief Handles the initialisation, movement, animation, shooting and rendering of the invaders.
///        Also moves the ufo across the screen when ever it's triggered and calls the sound of the
///        ufo to be played.
/// @author Teemu Lindborg
/// @version 1.0
/// @date 09/01/2015
/// @todo Tidy the code, especially updateInvaders() and the stuff relating to animation (could probably
///       do in a tidier manner)
// -----------------------------------------------------------------------------------------------------------------------

typedef enum {TYPE1, TYPE2, TYPE3} InvaderType;

typedef struct
{
  SDL_Rect pos;
  int active;
  int frame;
  InvaderType type;
} Invader;

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Sets the dimensions for invader and also assigns the initial positions. Also does it for the
///        projectiles of the invaders and the alien.
/// @param[in] invaders The actual invaders data so that it can be assigned
/// @param[in] alien The datastruct for alien's data
/// @param[in] invaderProjectile Projectile data for every column
/// @param[in] actInvaderInRow Initialises the array holding the bottom-most invader in each column so that
///                        the program knows from which invader a projectile should be shot from in each column
/// @param[in] invaderProjectileActive Initialises the array that keeps track whether a projectile is shot from a certain
///                                column so another one can't be shot from that column before the last one is destroyed
// -----------------------------------------------------------------------------------------------------------------------
void initialiseInvaders(Invader invaders[ROWS][COLS], Invader *alien, SDL_Rect invaderProjectile[COLS], int actInvaderInRow[COLS], int invaderProjectileActive[COLS]);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Handles the movement of the invaders (speed and amount of vertical movement depend on the level)
///        Checks which invader is the bottom-most active one in each column, and which column is the left-/rightmost
///        for the vertical movement. Also handles the calculations for the movement animation
/// @param[in] invaders The invaders data for the movement and checking the active invaders for on rows/cols
/// @param[in] gameSpeed Current game speed to be used with the movement and increased according to the levels
///                      specifications
/// @param[in] currentFrame Calculating which frame should be displayed for the invaders
/// @param[in] actInvaderInRow Updates the array that holds the bottom-most invader in each column
/// @param[in] level Current level so we know when to update the speed and vertical movement
// -----------------------------------------------------------------------------------------------------------------------
void updateInvaders(Invader invaders[ROWS][COLS], int *gameSpeed, int *currentFrame, int actInvaderInRow[COLS], int level);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Renders the invaders depending on the current animation frame
/// @param[in] ren Renderer that the render data gets passed to
/// @param[in] tex Texture with the spritesheet holding all the invader sprites needed
/// @param[in] invaders Invaders data to render correct invaders in correct places
/// @param[in] currentFrame Which sprite of the animation should be rendered
// -----------------------------------------------------------------------------------------------------------------------
void drawInvaders(SDL_Renderer *ren,SDL_Texture *tex, Invader invaders[ROWS][COLS], int currentFrame);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Handle the rendering and movement of a fired invader projectile
/// @param[in] ren Renderer that the render data gets passed to
/// @param[in] tex Texture with the projectile sprite
/// @param[in] invaderProjectile The data of the projectile that needs to be rendered and moved
// -----------------------------------------------------------------------------------------------------------------------
void invaderShootPewPew(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *invaderProjectile);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Handles the movement of the alien to the correct direction and calls the sound function to be played
/// @param[in] ren Renderer that the render data gets passed to
/// @param[in] tex Texture with the aliens sprite
/// @param[in] alien The data holding the position and dimensions of the alien
/// @param[in] direction To which direction the alien should move
/// @param[in] ufosound The sound file to be played
// -----------------------------------------------------------------------------------------------------------------------
void moveAlien(SDL_Renderer *ren, SDL_Texture *tex, Invader *alien, int direction, Mix_Chunk *ufosound);

#endif // end INVADER_H
