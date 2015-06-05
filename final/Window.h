/*
/ A simple class which draws a flat surface with a certain alpha value. May use a texture.
*/

#ifndef WINDOW_H
#define WINDOW_H

#include "Object.h"

class Window : public Object
{
private:
	float width, height;
	unsigned int textureID;
public:
	Window(float _x, float _y, float _z, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID = 0);

	void update();
	void draw();
};


#endif