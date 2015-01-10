/*
 Copyright © 2015 Teemu Lindborg
 SDAGE 1st year Invaders Assignment
 Initial code © by Jon Macey
*/

#ifndef DEFENDER_H
#define DEFENDER_H

/// @file Defender.h
/// @brief Defender initialisation and functionality
// Including the global definitions and includes for the use of the functions
#include "Globals.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Defender.h
/// @brief Handles the initialisation, movement of the player(s) and projectile(s),
///        shooting, actual rendering and rendering of the lives at the bottom of the screen.
///        Does it for both: one or two players.
/// @author Teemu Lindborg
/// @version 1.0
/// @date 09/01/2015
/// @todo Clean stuff up, check if everything's necessary and if something related to the
///       defenders is missing
// -----------------------------------------------------------------------------------------------------------------------

typedef enum {LEFT,RIGHT} direction;

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Function for assigning initial values for the players' position
/// @param[out] spaceShip an array holding the dimensions and position of two players/defenders
/// @param[out] projectile an array holding the dimensions and position for the projectiles for two players/defenders
/// @param[out] projectileBoom an array holding the dimensions and the initial y-position of the explosion of a projectile
// -----------------------------------------------------------------------------------------------------------------------
void initialiseDefender(SDL_Rect o_spaceShip[2], SDL_Rect o_projectile[2], SDL_Rect o_projectileBoom[2]);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Handles the movement of the defender that's passed in to the function (supports both players)
/// @param[io] spaceShip Holds the defender's data that'll be modified to move the player
/// @param[in] moveDir The direction that the player wants to move the defender
// -----------------------------------------------------------------------------------------------------------------------
void moveSpaceShip(SDL_Rect *io_spaceShip, int _moveDir);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Render the spaceship that's passed on to the function (used to render both players)
/// @param[in] ren Renderer that the render data gets passed to
/// @param[in] sStexture The texture holding the defender
/// @param[in] spaceShip The actual defender that will be rendered
/// @param[io] playerDead A variable that holds the "state" of a players death: 0 = not dead, 1 = died and should explode, 2 = dead, no more lives
/// @param[in] lives Player's lives to check whether the player dies completely or just loses a life
/// @param[in] player Player 1 or 2
// -----------------------------------------------------------------------------------------------------------------------
void drawSpaceShip(SDL_Renderer *_ren, SDL_Texture *_sStexture, SDL_Rect *_spaceShip, int *io_playerDead, char *_lives, int _player);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Handle the player shooting
/// @param[in] ren Renderer that the render data gets passed to
/// @param[io] projectile The data of the projectile used to rendering it and moving it
/// @param[in] level Current level as the speed of the projectile depends on the level
// -----------------------------------------------------------------------------------------------------------------------
void shootPewPew(SDL_Renderer *_ren, SDL_Rect *io_projectile, int _level);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief explodeProjectile
/// @param[in] ren Renderer that the render data gets passed to
/// @param[io] projectileBoom The rect for the explosion
/// @param[in] tex The texture holding the explosion sprite
/// @param[io] explodeP A variable that tells when the projectile should be exploding, will reset after certain amount of frames
// -----------------------------------------------------------------------------------------------------------------------
void explodeProjectile(SDL_Renderer *_ren, SDL_Rect *_projectileBoom, SDL_Texture *_tex, int *o_explodeP);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Render the lives as defenders to the bottom of the screen
/// @param[in] ren Renderer that the render data gets passed to
/// @param[in] tex The texture holding the defender sprite
/// @param[in] lives Amount of lives a player has so a correct amount of defenders can be rendered
/// @param[in] player Player 1 or 2
/// @param[in] players A check whether the game is played in one or two player mode to determine
///            on which side the lives should be rendered
// -----------------------------------------------------------------------------------------------------------------------
void renderLives(SDL_Renderer *_ren, SDL_Texture *_tex, char *_lives, int _player, int _players);

#endif // end DEFENDER_H
