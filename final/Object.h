/*
/ This abstract class represents an Object which can be drawn. Has certain attributes: position,
/ a BoundingBox used for collision detection, material to be applied to the object, and a
/ Transformation object containing transformations to be done on the object before drawing.
*/

#ifndef OBJECT_H
#define OBJECT_H

#include "Constants.h"
#include "Vector3f.h"
#include "BoundingBox.h"
#include "Transformation.h"
#include "Material.h"

class Object
{
protected:
	Vector3f pos;		// each object should have a position
	BoundingBox * box;	// each object may be surrounded by a bounding box, used for collision detection. If no box, then NULL.

	Material * mat;	// the material of the object

	// so this is used to tell how to transform the object. used mostly for rotating the object; 
	// positioning is done with the pos-vector
	Transformation trans;
public:
	Object();
	Object(float _x, float _y, float _z);
	Object(float _x, float _y, float _z, BoundingBox * _box, Material * _mat);
	virtual ~Object() { delete box; }	// leave the deletion of the material to the provider of it; the box, however, is assumed to be individual

	// these methods must be implemented in classes deriving from this class
	virtual void update() = 0;
	virtual void draw() = 0;
	
	// returns a float vector containing the position of the object
	float * getPos() { return pos.getComps(); }

	// returns a pointer to the bounding box about the object
	virtual BoundingBox * getBox() { return box; }

	// adds a transformation to the transformation object
	void rotate(float degrees, float x, float y, float z) { trans.addRotation(degrees, x, y, z); }
	void translate(float x, float y, float z) { trans.addTranslation(x, y, z); }
	void scale(float x, float y, float z) { trans.addScale(x, y, z); }
};


#endif