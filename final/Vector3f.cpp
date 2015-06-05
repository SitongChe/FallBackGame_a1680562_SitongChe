#include <cmath>
#include "Vector3f.h"

Vector3f::Vector3f()
{
	comp = new float[3];
	comp[0] = comp[1] = comp[2] = 0.0;
}

Vector3f::Vector3f(float _x, float _y, float _z)
{
	comp = new float[3];
	comp[0] = _x;
	comp[1] = _y;
	comp[2] = _z;
}

Vector3f::Vector3f(const Vector3f &arg)
{
	comp = new float[3];
	comp[0] = arg.comp[0];
	comp[1] = arg.comp[1];
	comp[2] = arg.comp[2];
}

Vector3f& Vector3f::operator=(const Vector3f &arg)
{
	if (&arg != this)
	{
		comp = new float[3];
		comp[0] = arg.comp[0];
		comp[1] = arg.comp[1];
		comp[2] = arg.comp[2];
	}

	return *this;
}

Vector3f::~Vector3f()
{
	delete [] comp;
}

float Vector3f::dotProduct(Vector3f &arg)
{
	return (comp[0]*arg.getX()) + (comp[1]*arg.getY()) + (comp[2]*arg.getZ());
}

float Vector3f::length()
{
	return sqrt(dotProduct(*this));
}