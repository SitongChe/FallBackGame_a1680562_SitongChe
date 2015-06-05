#include "Door.h"

Door::Door(float _x, float _y, float _z, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight, bool _stayOpened) : Object(_x, _y, _z, _box, _mat)
{
	width = _width;
	height = _height;
	texWidth = _texWidth;
	texHeight = _texHeight;
	textureID = _textureID;

	state = CLOSED;
	timeToClose = 0;
	stayOpened = _stayOpened;
}

bool Door::isOpen()
{
	if (state == OPEN)
		return true;
	else
		return false;
}

bool Door::isClosed()
{
	if (state == CLOSED)
		return true;
	else
		return false;
}