#include "SkyBox.h"

SkyBox::SkyBox(float _distance, unsigned int texUp, unsigned int texDown, unsigned int texNorth, unsigned int texSouth, unsigned int texEast, unsigned int texWest) :
		up(0, _distance, 0, _distance*2, _distance*2, NULL, texUp),
		down(0, -_distance, 0, _distance*2, _distance*2, NULL, texDown),
		north(0, 0, -_distance, _distance*2, _distance*2, NULL, texNorth),
		south(0, 0, _distance, _distance*2, _distance*2, NULL, texSouth),
		east(_distance, 0, 0, _distance*2, _distance*2, NULL, texEast),
		west(-_distance, 0, 0, _distance*2, _distance*2, NULL, texWest)
{
	up.rotate(90, 1, 0, 0);
	down.rotate(-90, 1, 0, 0);
	south.rotate(180, 0, 1, 0);
	east.rotate(-90, 0, 1, 0);
	west.rotate(90, 0, 1, 0);
	distance = _distance;

	mat = new Material(GL_FRONT);
	mat->setSpecular(0, 0, 0, 1);
	mat->setDiffuse(0, 0, 0, 1);
	mat->setAmbient(0, 0, 0, 1);
	mat->setShininess(0);
	mat->setEmission(0.5, 0.5, 0.5, 1);
}

SkyBox::~SkyBox()
{
	delete mat;
}

void SkyBox::update(Vector3f &_playerPos)
{
	float * playerPos = _playerPos.getComps();
	float * position;

	position = up.getPos();
	position[0] = playerPos[0];
	position[1] = playerPos[1] + distance;
	position[2] = playerPos[2];

	position = down.getPos();
	position[0] = playerPos[0];
	position[1] = playerPos[1] - distance;
	position[2] = playerPos[2];

	position = north.getPos();
	position[0] = playerPos[0];
	position[1] = playerPos[1];
	position[2] = playerPos[2] - distance;

	position = south.getPos();
	position[0] = playerPos[0];
	position[1] = playerPos[1];
	position[2] = playerPos[2] + distance;

	position = east.getPos();
	position[0] = playerPos[0] + distance;
	position[1] = playerPos[1];
	position[2] = playerPos[2];

	position = west.getPos();
	position[0] = playerPos[0] - distance;
	position[1] = playerPos[1];
	position[2] = playerPos[2];
}

void SkyBox::draw()
{
	mat->setMaterial();
	up.draw();
	down.draw();
	north.draw();
	south.draw();
	east.draw();
	west.draw();
}