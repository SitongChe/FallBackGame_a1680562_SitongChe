#include <cmath>
#include "Ghost.h"
#include <GLUT.h>

Ghost::Ghost(float _x, float _y, float _z) : pos(_x, _y, _z)
{
	mat = new Material(GL_FRONT);
	mat->setSpecular(0, 0, 0, 0.7);
	mat->setDiffuse(0.7, 0.7, 0.7, 0.7);
	mat->setAmbient(0.7, 0.7, 0.7, 0.7);
	mat->setShininess(1);

	yRot = 0;

	quadric = gluNewQuadric();
}


Ghost::~Ghost()
{
	delete mat;
	gluDeleteQuadric(quadric);
}

void Ghost::draw()
{
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	glRotatef(yRot, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	mat->setMaterial();

	//gluSphere(quadric, 0.5, 20, 20);

	glBegin(GL_TRIANGLES);
		glNormal3f(0, 0, 1);
		glVertex3f(-0.5, -0.5, 0.0);
		glVertex3f(0.5, -0.5, 0.0);
		glVertex3f(0.0, 0.5, 0.0);
	glEnd();

	glPopMatrix();
}

void Ghost::update(Vector3f &playerPos)
{
	// make sure that creepy ghost is always, ALWAYS facing the player
	yRot = 180 + atan2(pos.getX() - playerPos.getX(), pos.getZ() - playerPos.getZ()) * 180.0 / 3.1416;
}