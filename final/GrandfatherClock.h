/*
/ This class represents a simple grandfather's clock. The modeling is extremely simple, since I felt the point was more about the modeling of the movement
/ of the pendulum and the clock hands, rather than having a nice looking clock (it wouldn't have been difficult to make it, but very tedious). 
/ It actually does keep track of the time, but it assumes that if the FRAMES_PER_SECOND variable is 60, then it will
/ definitely be 60 frames in one second. This is not the case in this project - it actually takes one 60th of a second between updates - so this clock is
/ slower than real life... but on my machine, it's pretty close to the real thing  ;)
*/



#ifndef GRANDFATHERCLOCK_H
#define GRANDFATHERCLOCK_H

#include "Object.h"
#include "Material.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class GrandfatherClock : public Object
{
private:

	// the clock shows the seconds, the minutes and the hours. The corresponding fields keep track of the time. For each field,
	// we've got an extra field - secondAngle, minuteAngle, hourAngle - which stores the angle to use for each clock hand (so
	// that we don't have to calculate these angles in each frame).
	// Finally, milliseconds is incremented in each update. This does not store how many milliseconds we are into the second,
	// but rather how many frames we are. So when milliseconds == FRAMES_PER_SECOND, it's been one second (not really, but..).
	int milliseconds;
	int seconds;
	float secondAngle;
	int minutes;
	float minuteAngle;
	int hours;
	float hourAngle;

	// since each second consists of FRAMES_PER_SECOND frames, we only need to calculate that many angles for the pendulum
	// (instead of doing it in each update), and we only need to do it once, then use the value of milliseconds to index into
	// this array
	float * pendulumAngles;

	void calcAngles();	// calculates the above array

	int coeff;	// either 1 or -1. Updated each second, and multiplied with the values in pendulumAngles when rotating the pendulum

	const float PENDULUM_MAX_ANGLE;	// the maximum angle of the pendulum
	const float WIDTH;	// the width of the clock
	const float HEIGHT;	// the height of the clock
	const float DEPTH;	// the depth of the clock

	// texture and material for the clock face
	GLuint faceTexID;
	Material * faceMat;

	// texture and material for the base stuff (the sides and so on)
	GLuint baseTexID;
	Material * baseMat;

	Material * pendMat;

	GLUquadric * quadric;
public:
	GrandfatherClock(float _x, float _y, float _z, BoundingBox * _box, Material * _mat);
	~GrandfatherClock();

	void update();
	void draw();
};

#endif