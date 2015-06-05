#include "BoundingBox.h"
#include "Constants.h"

BoundingBox::BoundingBox() : pos(Vector3f())
{
	width = 0.0;
	height = 0.0;
	depth = 0.0;
	id = -1;
}

BoundingBox::BoundingBox(float _x, float _y, float _z, float _width, float _height, float _depth, int _id) : pos(Vector3f(_x, _y, _z))
{
	width = _width;
	height = _height;
	depth = _depth;
	id = _id;
}

BoundingBox::BoundingBox(Vector3f &_pos, float _width, float _height, float _depth, int _id) : pos(_pos)
{
	width = _width;
	height = _height;
	depth = _depth;
	id = _id;
}

void BoundingBox::setPos(float _x, float _y, float _z)
{
	pos.setComps(_x, _y, _z);
}
	
void BoundingBox::setPos(Vector3f &_pos)
{
	pos.setComps(_pos.getComps());
}

float BoundingBox::abs(float arg)
{
	if (arg < 0.0)
		return float(arg * -1.0);
	else
		return arg;
}

bool BoundingBox::collision(BoundingBox * box)
{	
	if (box == NULL)
		return false;

	if ( abs(pos.getX() - box->pos.getX()) > ((width + box->width) / 2.0) )
		return false;

	if ( abs(pos.getY() - box->pos.getY()) > ((height + box->height) / 2.0) )
		return false;

	if ( abs(pos.getZ() - box->pos.getZ()) > ( (depth + box->depth) / 2.0) )
		return false;

	return true;
}

int BoundingBox::getID()
{
	return id;
}