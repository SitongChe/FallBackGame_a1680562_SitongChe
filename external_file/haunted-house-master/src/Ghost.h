/*
/ So this creepy class represents a ghost, which is positioned at a certain position, and then...
/ is always looking at the player... more freaky is the fact that it can only be seen through
/ the surveillance camera, but this fact must be enforced in the render()-method of AppManager
*/

#ifndef GHOST_H
#define GHOST_H

#include "Object.h"
#include "Vector3f.h"
#include "Constants.h"
#include "Material.h"

class Ghost
{
private:
	Vector3f pos;
	Material * mat;

	// how much should the ghost rotate about the y-axis? This depends on the position of the
	// player, and is set in each call to update()
	float yRot;

	GLUquadric * quadric;
public:
	Ghost(float _x, float _y, float _z);
	~Ghost();

	void draw();
	void update(Vector3f &playerPos);
};

#endif