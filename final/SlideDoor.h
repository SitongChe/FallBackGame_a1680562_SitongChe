/*
/ This implementation of the Door class is a door which slides open. Pretty simple.
*/

#ifndef SLIDEDOOR_H
#define SLIDEDOOR_H

#include "Door.h"

class SlideDoor : public Door
{
private:
	float openOffset;			// 0.0 = completely closed; width = completely open
	const float SLIDE_SPEED;	// the speed at which we are opening the door

	const int OPEN_PERIOD;	// how long should the door be open before closing?

public:
	SlideDoor(float _x, float _y, float _z, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight, bool _stayOpened = false);

	void update();
	void draw();

	// opens the door if the character is close enough to the door
	void open(Vector3f charPos);

	// closes the door
	void close();

	// different boxes are used depending on the state. If the door is open, it returns an "empty" box
	BoundingBox * getBox();
};

#endif