/*
/ This class inherits from Object, but is still abstract; it declares two new virtual methods,
/ open() and close(), which are to be implemented by Door-classes inheriting from this one (like
/ SlideDoor and PivotDoor)
/
*/

#ifndef DOOR_H
#define DOOR_H

#include "Object.h"

class Door : public Object
{
protected:
	// the width and height of the door (and the texture applied to the door)
	float width, height, texWidth, texHeight;
	unsigned int textureID;

	// this enumeration is used to keep track of what the door is doing at the moment
	enum State { CLOSED, OPEN, CLOSING, OPENING };
	State state;	// what is the door doing now?

	bool stayOpened;	// if true; do not close the door after it's been opened

	int timeToClose;		// how much "time" is left until the door should close? when 0 - close.
public:
	Door(float _x, float _y, float _z, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight, bool _stayOpened);

	virtual ~Door() { }

	// opens the door
	virtual void open() { state = OPEN; }

	// opens the door if the character is close enough to the door
	virtual void open(Vector3f charPos) = 0;

	// closes the door
	virtual void close() = 0;

	// is the door open/closed?
	bool isOpen();
	bool isClosed();
};

#endif