/*
/ Last minute solution. Just a surface with a texture applied to it
*/

#ifndef TEXTUREDSURFACE_H
#define TEXTUREDSURFACE_H

#include "Object.h"

class TexturedSurface : public Object
{
private:
	float width, height;
	unsigned int textureID;
public:
	TexturedSurface();
	TexturedSurface(float _x, float _y, float _z, float _width, float _height, Material * _mat, unsigned int _texID);
	~TexturedSurface() {}

	void draw();
	void update();
};

#endif