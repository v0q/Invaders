/*
 Copyright © 2015 Teemu Lindborg
*/

#ifndef SHIELDS_H
#define SHIELDS_H

/// @file Shields.h
/// @brief Shield loading/refreshing and destroyal
// Including the global definitions and includes for the use of the functions
#include "Globals.h"

// -----------------------------------------------------------------------------------------------------------------------
/// @file Shields.h
/// @brief The functions that load/refresh the shields when needed and handle the destroyal
///        by pixel manipulation
/// @author Teemu Lindborg
/// @version 1.0
/// @date 09/01/2015
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Loads/refreshes the shield surfaces and textures
/// @param[in] ren Renderer that the render data gets passed to
/// @param[in] shieldSurface Surfaces holding the image data (needed for pixel manipulation)
/// @param[in] shieldTexture Textures that are loaded from the surfaces to be used for the actual rendering
// -----------------------------------------------------------------------------------------------------------------------
void loadShields(SDL_Renderer *ren, SDL_Surface *shieldSurface[4], SDL_Texture *shieldTexture[4]);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief initialiseShields
/// @param shields The rects to which the dimensions and positions of the shields will be stored
// -----------------------------------------------------------------------------------------------------------------------
void initialiseShields(SDL_Rect shields[4]);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Actual pixel manipulation
/// @param[in] shieldSurface The surface that will be manipulated
/// @param[in] x X-coordinate in the surface where the projectile hit
/// @param[in] y Y-coordinate in the surface where the projectile hit
/// @param[in] PlorIn Whether a player or an invader hit the shield (flips and changes the direction of the "explosion")
// -----------------------------------------------------------------------------------------------------------------------
void editPixel(SDL_Surface *shieldSurface, int x, int y, int PlorIn);

// -----------------------------------------------------------------------------------------------------------------------
/// @brief Checks whether the pixel is still "active" i.e. not black
/// @param[in] shieldSurface The surface from which the data will be fetched
/// @param[in] x X-coordinate in the surface where the projectile hit
/// @param[in] y Y-coordinate in the surface where the projectile hit
/// @return The colour of the pixel hit as a Uint32
// -----------------------------------------------------------------------------------------------------------------------
Uint32 pixelActive(SDL_Surface *shieldSurface, int x, int y);

#endif // end SHIELDS_H
