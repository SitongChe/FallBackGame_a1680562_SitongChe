/*
/ This class represents a creepy snowman. Not only does it have a position and
/ stuff like that, like usual snowmen, but it actually has some behaviour, too.
*/

#ifndef SNOWMAN_H
#define SNOWMAN_H

#include "Vector3f.h"
#include "Material.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Snowman
{
private:
	Vector3f pos;	// this is the position of the center of the bottom of the snowman
	
	Material * bodyMat;
	Material * eyeMat;
	Material * noseMat;

	float yRot;
	
	const float SPEED;	// the speed of the snowman!!

	enum State { IDLE, LOOKING, CHARGING };
	State state;	// the state of the snowman

	GLUquadric * quadric;

	// when the snowman is charging, and it has come close enough to the player, it is considered "done"
	bool done;
public:
	Snowman(float _x, float _y, float _z);
	~Snowman();

	void draw();
	void update(Vector3f &playerPos);

	// sets the snowman to charge at the player
	void charge();

	// sets the snowman to stand and look at the player
	void look();

	// sets the snowman to just stand idle
	void idle();

	// has the snowman come close enough to the player
	bool isDone() { return done; }
};

#endif