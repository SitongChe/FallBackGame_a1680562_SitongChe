/*
/ An axis-aligned bounding box. Used for collision detection. Also keeps an ID, which can be set to some
/ number which can be read in case of collision with the bounding box, or to -1 to indicate "no information".
/ Is used by the floors in this project, to indicate which room the player is in.
*/


#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Vector3f.h"

class BoundingBox
{
private:
	Vector3f pos;	// the center position of the box
	float width, height, depth;	// the dimensions of the box

	// so this is an identifier for the bounding box. It is used in collision detection to
	// figure out where the player is. A value of -1 basically says, "ey, no useful information
	// here, I'm just an arbitrary bounding box dood", whereas another value might say "Yes,
	// so I am the bounding box of room no. 2".
	// If no ID is specified in the creation of the box, then -1 is set as default
	int id;
	
	float abs(float arg);	// simply returns the absolute value of the argument
public:
	BoundingBox();

	// takes the position of the center position of the box,
	// as well as the width, height and depth of the box
	BoundingBox(float _x, float _y, float _z, float _width, float _height, float _depth, int _id = -1);
	BoundingBox(Vector3f &_pos, float _width, float _height, float _depth, int _id = -1);
	
	// changes position of the box
	void setPos(float _x, float _y, float _z);
	void setPos(Vector3f &_pos);

	// returns true if this bounding box coincides with the argument box
	bool collision(BoundingBox * box);

	// returns the ID of the box
	int getID();
};


#endif