#include <cmath>
#include "Carousel.h"
using namespace std;

Carousel::Carousel(float _x, float _y, float _z, BoundingBox * _box, Material * _mat) : Object(_x, _y, _z, _box, _mat)
{
	rot = 0.0;
	rotSpeed = 1.0;
	friction = 0.003;
	handle = new BezierSpline(Vector3f(0.0, 1.4, 0.0), Vector3f(1.8, 1.4, 0.0), Vector3f(1.8, 1.4, 0.0), Vector3f(1.8, 0.0, 0.0), 5, 0.1, 0.05);
	quadric = gluNewQuadric();
}

Carousel::~Carousel()
{
	delete handle;
	gluDeleteQuadric(quadric);
}

void Carousel::update()
{
	// below, the "* rotSpeed" makes it slow down faster when the rotational speed is high, and slower when it is low. This makes it so that it won't
	// take forever to stop it when the speed is high, but it will spin at a slow speed for quite some time, rather than stopping abruply. However, if
	// we don't use the max-thingy (so that it is multiplied by only 0.5 when the speed is lower than that), then it will basically keep on rotating, slower
	// and slower and slower, for a very long time. This makes it look pretty natural
	rotSpeed -= friction * max<float>(rotSpeed, 0.5);
	if (rotSpeed < 0.0)
		rotSpeed = 0.0;
	rot += rotSpeed;
}

void Carousel::draw()
{
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	glRotatef(rot, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	mat->setMaterial();
	// first, draw the bottom and the middle rod, both made up of one cylinder each
	
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(quadric, 2.0, 2.0, 0.3, 20, 20);	// cylinder for the base
	glRotatef(180, 1, 0, 0);
	gluDisk(quadric, 0.0, 2.0, 20, 20);		// disk for the bottom of the base
	glRotatef(-180, 1, 0, 0);
	glTranslatef(0, 0, 0.3);
	gluDisk(quadric, 0.0, 2.0, 20, 20);		// disk for the top of the base
	gluCylinder(quadric, 0.2, 0.2, 1.5, 20, 10);	// cylinder for the tall rod in the middle
	glTranslatef(0, 0, 1.5);
	gluDisk(quadric, 0.0, 0.2, 10, 10);	// disk at the top of mentioned rod
	glPopMatrix();

	glTranslatef(0, 0.3, 0);
	
	// here, we draw the curved cylinders, each rotated 72 degrees around the center of the carousel
	for (int i = 0; i < 5; i++)
	{
		glRotatef(72, 0, 1, 0);
		handle->draw();
	}

	glPopMatrix();
}

void Carousel::setRotSpeed(float _rotSpeed)
{
	rotSpeed = _rotSpeed;
}

void Carousel::addRotSpeed(float _rotSpeed)
{
	rotSpeed += _rotSpeed;
}

void Carousel::setFriction(float _friction)
{
	friction = _friction;
}