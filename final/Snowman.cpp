#include <cmath>
#include "Constants.h"
#include "Snowman.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Snowman::Snowman(float _x, float _y, float _z) : pos(_x, _y, _z), SPEED(MOVE_SPEED + 0.01)	// slightly faster than the player  >:D
{
	state = IDLE;

	bodyMat = new Material(GL_FRONT);
	bodyMat->setSpecular(0.0, 0.0, 0.0, 0.8);
	bodyMat->setDiffuse(1.0, 1.0, 1.0, 0.8);
	bodyMat->setAmbient(1.0, 1.0, 1.0, 0.8);
	bodyMat->setEmission(0.5, 0.5, 0.5, 1.0);
	bodyMat->setShininess(1.0);

	eyeMat = new Material(GL_FRONT);
	eyeMat->setSpecular(1.0, 0.0, 0.0, 1.0);
	eyeMat->setDiffuse(1.0, 0.0, 0.0, 1.0);
	eyeMat->setAmbient(1.0, 0.0, 0.0, 1.0);
	eyeMat->setEmission(1.0, 0.0, 0.0, 1.0);
	eyeMat->setShininess(100.0);

	noseMat = new Material(GL_FRONT);
	noseMat->setSpecular(0.0, 0.0, 0.0, 1.0);
	noseMat->setDiffuse(1.0, 0.55, 0.0, 1.0);
	noseMat->setAmbient(1.0, 0.55, 0.0, 1.0);
	noseMat->setEmission(0.5, 0.3, 0.0, 1.0);

	quadric = gluNewQuadric();
	yRot = 0.0;

	done = false;
}

Snowman::~Snowman()
{
	delete bodyMat;
	delete eyeMat;
	delete noseMat;
	gluDeleteQuadric(quadric);
}

void Snowman::draw()
{
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY() + 0.5, pos.getZ());
	glRotatef(yRot, 0, 1, 0);

	bodyMat->setMaterial();
	glBindTexture(GL_TEXTURE_2D, 0);

	gluSphere(quadric, 0.5, 20, 20);	// lower part
	glTranslatef(0, 0.73, 0);
	gluSphere(quadric, 0.35, 20, 20);	// middle part
	glTranslatef(0, 0.53, 0);
	gluSphere(quadric, 0.25, 20, 20);	// upper part

	// begin with the nose
	glTranslatef(0.0, 0.0, 0.24);
	noseMat->setMaterial();
	glutSolidCone(0.05, 0.2, 10, 10);

	// begin with the eyes
	eyeMat->setMaterial();
	
	glTranslatef(-0.1, 0.02, 0.0);
	glBegin(GL_POLYGON);
		glNormal3f(0, 0, 1);
		for (float i = 1.5808; i < 6.2832; i += 0.5)	// draw circle from pi/2 to 2*pi
			glVertex3f(0.04*cos(i), 0.04*sin(i), 0.0);
	glEnd();

	glTranslatef(0.2, 0.0, 0.0);
	glBegin(GL_POLYGON);
		glNormal3f(0, 0, 1);
		for (float i = 3.1416; i < 7.854; i += 0.5)	// draw circle from pi/2 to 2*pi
			glVertex3f(0.04*cos(i), 0.04*sin(i), 0.0);
	glEnd();

	glPopMatrix();
}

void Snowman::update(Vector3f &playerPos)
{
	if (state == IDLE)	// do nothing
		return;

	else if (state == LOOKING)	// set yRot so that it points at the player
			yRot = 180 + atan2(pos.getX() - playerPos.getX(), pos.getZ() - playerPos.getZ()) * 180.0 / 3.1416;

	else if (state == CHARGING && !done)	// charge at the player
	{
		yRot = 180 + atan2(pos.getX() - playerPos.getX(), pos.getZ() - playerPos.getZ()) * 180.0 / 3.1416;

		pos.addX(SPEED * cos((90 - yRot) * 3.1416 / 180.0));
		pos.addZ(SPEED * sin((90 - yRot) * 3.1416 / 180.0));

		if (dist(pos, playerPos) < 0.1)
			done = true;
	}
}

void Snowman::charge()
{
	state = CHARGING;
}

void Snowman::look()
{
	state = LOOKING;
}

void Snowman::idle()
{
	state = IDLE;
}