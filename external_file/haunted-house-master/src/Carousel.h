/*
/ This class represents a carousel, the kind you might find on a playground, which kids use to spin around in. It uses regular cylinders (with glu) to draw
/ the bottom and the middle, and then uses the BezierSpline-class for the handles.
/
/ Of course, the carousel would be pretty boring if you couldn't rotate it, so you can! The friction (which can be altered) slows down the rotation, and two
/ methods, setRotSpeed and addRotSpeed, are used to change how fast the carousel is rotating.
*/

#ifndef CAROUSEL_H
#define CAROUSEL_H

#include "Object.h"
#include "BezierSpline.h"
#include <GLUT.h>

class Carousel : public Object
{
private:
	BezierSpline * handle;	// this spline makes up the "handle", and we draw it five times with different orientation

	GLUquadric * quadric;	// we use this to draw the bottom and the center cylinder

	float rot;			// how much to rotate the carousel
	float rotSpeed;		// how fast the carousel is spinning
	float friction;		// this slows down the carousel when it is spinning
public:
	Carousel(float _x, float _y, float _z, BoundingBox * _box, Material * _mat);
	~Carousel();

	void update();	// updates the (orientation of) the carousel
	void draw();	// draws the carousel

	// these three methods are used to alter how the carousel spins
	void setRotSpeed(float _rotSpeed);
	void addRotSpeed(float _rotSpeed);
	void setFriction(float _friction);
};

#endif