/*
/ This file contains a bunch of variables (used to be constants), as well as some declarations of some functions. Most
/ should be self-explanatory.
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdlib>
#include "Vector3f.h"

extern int FRAMES_PER_SECOND;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern float ASPECT_RATIO;
extern float FIELD_OF_VIEW;

extern float MOVE_SPEED;	// how fast the user can move. Please observe: if the speed is too high, the collision detection breaks down
extern float GRAVITATIONAL_CONSTANT;

// how high/wide are the polygons making up the walls and stuff?
extern float POLYGON_WIDTH;
extern float POLYGON_HEIGHT;

// PLEASE OBSERVE that this function does NOT take the square root of the distance; it gives the
// distance to the power of 2, due to performance optimization
float dist(Vector3f &a, Vector3f &b);

// this is used in several classes, including Wall, PivotDoor, SlideDoor and Table. It draws a surface given the provided width and height, and uses 
// texWidth and texHeight to map any bound texture. Also uses the constants POLYGON_WIDTH and POLYGON_HEIGHT, defined in this file.
void draw_surface(float width, float height, float texWidth, float texHeight);

#endif