/*
/ The Transformation class contains a number of transformations (Trans objects), which are all 
/ executed by calling the transform() method 
*/

#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <vector>
#include <GLUT.h>

enum TransType { ROTATION, TRANSLATION, SCALE };

// This structure is used to store the information of a transformation
struct Trans
{
	float x, y, z, degrees;
	TransType type;

	Trans(float _x, float _y, float _z, float _degrees, TransType _type)
	{
		x = _x;
		y = _y;
		z = _z;
		degrees = _degrees;
		type = _type;
	}

	void transform()
	{
		switch (type)
		{
		case ROTATION: glRotatef(degrees, x, y, z); break;
		case TRANSLATION: glTranslatef(x, y, z); break;
		case SCALE: glScalef(x, y, z); break;
		}
	}
};

class Transformation
{
private:	
	std::vector<Trans> transformations;
public:
	Transformation();
	
	void addRotation(float degrees, float x, float y, float z);
	void addTranslation(float x, float y, float z);
	void addScale(float x, float y, float z);

	void transform();
};

#endif