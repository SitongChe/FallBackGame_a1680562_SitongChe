/*
/ Represents a key which can be picked up, if the player is close enough to it.
*/

#ifndef KEY_H
#define KEY_H

#include "Object.h"
#include <GLUT.h>

class Key : public Object
{
private:
	GLUquadric * quadric;
public:
	Key(float _x, float _y, float _z);
	~Key();

	void draw();
	void update();

	// the player is trying to pick up the key - is he close enough to do it?!
	// true if he is, false otherwise
	bool pickup(Vector3f &playerPos);
};

#endif