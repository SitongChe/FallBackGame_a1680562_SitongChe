#include "Transformation.h"

Transformation::Transformation()
{}
	
void Transformation::addRotation(float degrees, float x, float y, float z)
{
	transformations.push_back(Trans(x, y, z, degrees, ROTATION));	
}
	
void Transformation::addTranslation(float x, float y, float z)
{
	transformations.push_back(Trans(x, y, z, 0, TRANSLATION));
}

void Transformation::addScale(float x, float y, float z)
{
	transformations.push_back(Trans(x, y, z, 0, SCALE));
}

void Transformation::transform()
{
	for (unsigned int i = 0; i < transformations.size(); i++)
		transformations[i].transform();
}