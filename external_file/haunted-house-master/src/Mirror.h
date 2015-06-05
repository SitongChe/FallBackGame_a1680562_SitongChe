#ifndef MIRROR_H
#define MIRROR_H

#include "Vector3f.h"

class Mirror
{
private:
	Vector3f pos;
	float width, height;
	float yRot;
	unsigned int textureID;

	int texDimension;

	float leftXOffset;
	float rightXOffset;
	float rightYOffset;
	float leftYOffset;
public:
	Mirror(float _x, float _y, float _z, float _width, float _height, float _yRot);
	~Mirror();

	void prepareMirror(Vector3f &charPos);
	void createTexture();

	void draw();
};


#endif