/*
/ This simple skybox keeps six textured surfaces and the distance these surfaces should have from the player.
/ In each update, the position of these surfaces are updated to be kept at a constant distance from the player,
/ whereas the orientation of them remains the same. Thus, drawing the skybox is done just like any other object.
*/

#ifndef SKYBOX_H
#define SKYBOX_H

#include "TexturedSurface.h"
#include "Vector3f.h"
#include "Material.h"

class SkyBox
{
private:
	TexturedSurface up, down, north, south, east, west;
	float distance;
	Material * mat;
public:
	SkyBox(float _distance, unsigned int _texUp, unsigned int _texDown, unsigned int _texNorth, unsigned int _texSouth, unsigned int _texEast, unsigned int _texWest);
	~SkyBox();

	void update(Vector3f &_playerPos);
	void draw();
};

#endif