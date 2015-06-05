#include "Object.h"

Object::Object() : pos() 
{ 
	box = new BoundingBox(); 
	mat = new Material(); 
}

Object::Object(float _x, float _y, float _z) : pos(_x, _y, _z) 
{ 
	box = new BoundingBox(); 
	mat = new Material(); 
}

Object::Object(float _x, float _y, float _z, BoundingBox * _box, Material * _mat) : pos(_x, _y, _z) 
{ 
	box = _box; 
	mat = _mat; 
}
