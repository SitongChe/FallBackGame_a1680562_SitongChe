/*
/ This implementation of the Door class is a door which pivots around an axis when it opens/closes,
/ just like a regular door. In creating an instance of this class, the position of the midpoint of
/ the door, as well as the point of pivot, is needed. (please observe that the door will pivot about
/ the y-axis)
*/

#ifndef PIVOTDOOR_H
#define PIVOTDOOR_H

#include "Door.h"

class PivotDoor : public Door
{
private:
	Vector3f pivot;		// pos from Object stores the midpoint of the door; this is the pivot point

	const float MAX_ANGLE;	// how far can the door be opened?
	const float ROTATION_SPEED;	// how fast is the door opening/closing?
	float angle;	// 0.0 is closed; MAX_ANGLE is wide open
	float doorAngle;	// this is the angle made between pos and pivot

	const int OPEN_PERIOD;	// how long should the door be open before closing?

public:
	PivotDoor(float _x, float _y, float _z, float _xPivot, float _yPivot, float _zPivot, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight, bool _stayOpened = false);

	void update();
	void draw();

	// opens the door if the character is close enough to the door
	void open(Vector3f charPos);

	// closes the door
	void close();

	// different boxes are used depending on the state. If the door is not closed, then the box is "empty"
	BoundingBox * getBox();
};

#endif