/*
/ This Table class draws a very simple, rectangular table, with four metallic, cylindrical legs
*/


#ifndef TABLE_H
#define TABLE_H

#include "Constants.h"
#include "Object.h"
#include "Material.h"

class Table : public Object
{
private:
	// width and height of the table/texture, as well as the depth for the table
	float width, height, depth, texWidth, texHeight;
	unsigned int textureID;

	Material * metalMat;
	GLUquadric * quadric;
public:
	Table(float _x, float _y, float _z, float _width, float _height, float _depth, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight);
	~Table();

	void update();
	void draw();
};

#endif