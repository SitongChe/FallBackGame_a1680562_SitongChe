#include <vector>
#include <cmath>
#include "Camera.h"
#include "AppManager.h"
#include "Constants.h"
#include "Material.h"
#include <GLUT.h>
using namespace std;

extern AppManager * app;

Camera::Camera(float x, float y, float z) : pos(x, y, z), vel(0.0, 0.0, 0.0), SPEED(MOVE_SPEED), CHAR_HEIGHT(1.6), CHAR_WIDTH(0.5), CHAR_DEPTH(0.5)
{
	phi = 0.0f;
	theta = 0.0f;

	for (int i = 0; i < 4; i++)
		buttonPressed[i] = false;

	box = new BoundingBox(pos, CHAR_WIDTH, CHAR_HEIGHT, CHAR_DEPTH);

	roomNo = -1;
}

Camera::~Camera()
{
	delete box;
}

void Camera::update()
{
	// brace yourself... xVel is set to -SPEED, 0 or SPEED. However, this is just the relative
	// speed in the axis which goes perpendicular to the view vector and the y-axis, so to speak.
	// Same reasoning applies to zVel. Thus, the actual change in xPos and zPos needs to take
	// into account these variables, along with the value of theta, in the following rather
	// complex assignments:
	std::vector<Object*> objects = app->getObjects();


	pos.addX(vel.getX() * cos(theta * 3.1416 / 180.0) + vel.getZ() * sin(theta * 3.1416 / 180.0));
	box->setPos(pos.getX(), pos.getY() + CHAR_HEIGHT / 2, pos.getZ());
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (box->collision(objects[i]->getBox()))	// we collided with a box! undo the move
		{
			pos.addX(-(vel.getX() * cos(theta * 3.1416 / 180.0) + vel.getZ() * sin(theta * 3.1416 / 180.0)));
			int no = objects[i]->getBox()->getID();
			if (no != -1)	// if it's a proper room number, assign it to roomNo
				roomNo = no;
			break;
		}
	}


	vel.addY(-GRAVITATIONAL_CONSTANT);
	pos.addY(vel.getY());	// doesn't do anything here, but if we include, say, ability to jump, it'll be important
	box->setPos(pos.getX(), pos.getY() + CHAR_HEIGHT / 2, pos.getZ());
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (box->collision(objects[i]->getBox()))	// we collided with a box! undo the move
		{
			pos.addY(-vel.getY());
			int no = objects[i]->getBox()->getID();
			if (no != -1)	// if it's a proper room number, assign it to roomNo
				roomNo = no;
			vel.setY(0.0);
			break;
		}
	}


	pos.addZ(vel.getX() * sin(theta * 3.1416 / 180.0) - vel.getZ() * cos(theta * 3.1416 / 180.0));
	box->setPos(pos.getX(), pos.getY() + CHAR_HEIGHT / 2, pos.getZ());
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (box->collision(objects[i]->getBox()))	// we collided with a box! undo the move
		{
			pos.addZ(-(vel.getX() * sin(theta * 3.1416 / 180.0) - vel.getZ() * cos(theta * 3.1416 / 180.0)));
			int no = objects[i]->getBox()->getID();
			if (no != -1)	// if it's a proper room number, assign it to roomNo
				roomNo = no;
			break;
		}
	}
}

void Camera::setCamera()
{
	glRotatef(theta, 0, 1, 0);	// rotate about the y-axis
	glRotatef(phi, cos(theta * 3.1416 / 180.0f), 0, sin(theta * 3.1416 / 180.0f));	// rotate about the axis perpendicular to our up-vector and our forward-vector...
	glTranslatef(-pos.getX(), -pos.getY() - CHAR_HEIGHT, -pos.getZ());
}

void Camera::drawCharacter()
{
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	glRotatef(180 - theta, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	Material::setWhiteMaterial();

	// draw the front
	glNormal3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
		glVertex3f(-1.0, 1.0, 0.0);
		glVertex3f(1.0, 1.0, 0.0);
		glVertex3f(0.0, 2.0, 0.0);
	glEnd();

	// draw the back
	glRotatef(180, 0, 1, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-1.0, 1.0, 0.0);
		glVertex3f(1.0, 1.0, 0.0);
		glVertex3f(0.0, 2.0, 0.0);
	glEnd();

	glPopMatrix();
}

void Camera::moveForward(bool mov)
{
	if (buttonPressed[FWD] == mov)	// makes no difference; just return
		return;

	buttonPressed[FWD] = mov;

	if (mov == true)	// if we're set to start moving forward, move forward
		vel.setZ(SPEED);
	else if (buttonPressed[BACK] == true)	// otherwise, if we're still pressing back, move back
		vel.setZ(-SPEED);
	else	// otherwise, stop moving in either direction
		vel.setZ(0.0);
}

void Camera::moveBack(bool mov)
{
	if (buttonPressed[BACK] == mov)
		return;

	buttonPressed[BACK] = mov;

	if (mov == true)	// if we're set to start moving back, move back
		vel.setZ(-SPEED);
	else if (buttonPressed[FWD] == true)	// otherwise, if we're still pressing forward, move forward
		vel.setZ(SPEED);
	else	// otherwise, stop moving in either direction
		vel.setZ(0.0);
}

void Camera::moveLeft(bool mov)
{
	if (buttonPressed[LEFT] == mov)
		return;

	buttonPressed[LEFT] = mov;

	if (mov == true)	// if we're set to start moving left, move left
		vel.setX(-SPEED);
	else if (buttonPressed[RIGHT] == true)	// otherwise, if we're still pressing right, move right
		vel.setX(SPEED);
	else	// otherwise, stop moving in either direction
		vel.setX(0.0);
}

void Camera::moveRight(bool mov)
{
	if (buttonPressed[RIGHT] == mov)
		return;

	buttonPressed[RIGHT] = mov;

	if (mov == true)	// if we're set to start moving right, move right
		vel.setX(SPEED);
	else if (buttonPressed[LEFT] == true)	// otherwise, if we're still pressing left, move left
		vel.setX(-SPEED);
	else	// otherwise, stop moving in either direction
		vel.setX(0.0);
}

void Camera::rotLeftRight(float rot)
{
	theta += rot / 10.0f;

	if (theta > 360)
		theta -= 360;

	else if (theta < 0)
		theta += 360;
}

void Camera::rotUpDown(float rot)
{
	phi += rot / 10.0f;

	if (phi > 90)
		phi = 90;
	else if (phi < -90)
		phi = -90;
}