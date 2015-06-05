#include "Constants.h"
#include "Key.h"
#include <GLUT.h>

Key::Key(float _x, float _y, float _z) : Object(_x, _y, _z, NULL, NULL)
{
	mat = new Material(GL_FRONT);
	mat->setSpecular(1.0, 1.0, 1.0, 1.0);
	mat->setDiffuse(0.8, 0.8, 0.8, 1.0);
	mat->setAmbient(0.8, 0.8, 0.8, 1.0);
	mat->setShininess(100);

	quadric = gluNewQuadric();
}

Key::~Key()
{
	gluDeleteQuadric(quadric);
}

void Key::draw()
{
	glPushMatrix();
		glTranslatef(pos.getX(), pos.getY(), pos.getZ());
		trans.transform();

		mat->setMaterial();
		glBindTexture(GL_TEXTURE_2D, 0);

		gluCylinder(quadric, 0.03, 0.03, 0.5, 10, 10);
		glRotatef(180, 1, 0, 0);
		gluDisk(quadric, 0.0, 0.03, 10, 10);
		glRotatef(-180, 1, 0, 0);

		glPushMatrix();
			glTranslatef(0, 0, 0.58);
			glRotatef(90, 1, 0, 0);
			glutSolidTorus(0.03, 0.07, 10, 10);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 0, 0.04);
			glRotatef(90, 0, 1, 0);
			gluCylinder(quadric, 0.03, 0.03, 0.12, 10, 10);
			glTranslatef(0, 0, 0.12);
			gluDisk(quadric, 0.0, 0.03, 10, 10);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 0, 0.15);
			glRotatef(90, 0, 1, 0);
			gluCylinder(quadric, 0.03, 0.03, 0.12, 10, 10);
			glTranslatef(0, 0, 0.12);
			gluDisk(quadric, 0.0, 0.03, 10, 10);
		glPopMatrix();

	glPopMatrix();
}

void Key::update()
{
}

bool Key::pickup(Vector3f &playerPos)
{
	if (dist(pos, playerPos) < 0.5)
		return true;
	else
		return false;
}