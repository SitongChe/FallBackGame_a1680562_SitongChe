/*
/ This class should maybe be called "FlatSurface" or something like that instead, since it is used
/ for more things than just walls (it works just as well for floors and ceilings and stuff like that).
/ Has a width and height, and a texture which is repeatedly applied to the surface. texWidth and
/ texHeight specifies the size of the texture, so that the size does not depend of the number
/ of polygons making up the wall (this number is specified in config.txt)
*/

#ifndef WALL_H
#define WALL_H

#include "Constants.h"
#include "Object.h"
#include "Material.h"

class Wall : public Object
{
private:
	// width and height of the wall/texture
	float width, height, texWidth, texHeight;
	unsigned int textureID;

	// these attributes are used if the wall is two-sided
	bool isTwosided;
	unsigned int backTexID;
	Material * backMat;
	float backTexWidth, backTexHeight;
public:
	Wall(float _x, float _y, float _z, BoundingBox * _box, Material * _mat, float _width, float _height, unsigned int _textureID, float _texWidth, float _texHeight);
	~Wall();

	// if this is called, then the wall will be given a backside to, using the specified texture information
	void setBackside(Material * _backMat, unsigned int _textureID, float _texWidth, float _texHeight);

	void update();
	void draw();
};

#endif